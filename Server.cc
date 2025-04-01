/*
 * txc1.cc
 *
 *  Created on: Sep 24, 2024
 *      Author: OJEWALM
 */

/*
 * server.cc
 *
 *  Created on: Sep 25, 2024
 *      Author: OJEWALM
 */

#include <string.h>
#include <omnetpp.h>
#include <map>
#include <vector>
#include <sstream>
#include "ClientMsg_m.h"
#include <string>
#include <iostream>
#include <cmath>
#include<algorithm>
#include <fstream>
#include <iomanip>  // For controlling output formatting
std::ostringstream oss;

using namespace omnetpp;

enum FunctionStatus {
    NOT_BUSY = 0,  // Function is not busy
    BUSY = 1       // Function is busy
};

enum QueueModel{
    DIRECT = 0,  // No queueing
    PER_FUNCTION = 1 , //each function with its own queue, and packets are queued by type
    REASSEMBLY_BUFFER =2 // Reassembly buffers are used and packets can choose from a pool of buffers
};

enum LoadBalancingAlgorithm { NAIVE_RR=0,
    RR=1,
    JSQ=2,
    Powof2Choices=3
};

class Server : public cSimpleModule
{
  private:
    double processingRate;       // Rate at which streams are processed
    int MAX_NUM_BUFFERS;          // Maximum number of concurrent streams (buffers) handled by the server
    std::map<int, std::vector<cMessage*>> streamBuffers; // Map of buffers to store packets for each stream
    std::map<int, int> streamPacketCount; // Map to keep track of packet counts per stream
    std::map<int, FunctionStatus> functionStatus; // Map to track if each function type is busy or not (0, 1, 2 -> status)
    std::map<int, double> latency;
    std::map<int, double> Throughput;
    std::map<int, long> processedPackets;
    std::map<int, long> droppedPackets;
    std::map<int, std::vector<double>> droppedPacketsTime;
    std::map<int, std::vector<double>> Delay;
    QueueModel queuemodel;
    cMessage *processingEvent;   // Event to trigger stream processing
    cMessage *sendNextStream;
    cMessage *sendNext; //sendNextPacket
    cMessage *packetDropped;
    long numSent;
    long numReceived;
    cHistogram droppedPacketsStats;
    cOutVector droppedPacketsVector;
    cHistogram endtoEndDelayStats;
    cOutVector endtoEndDelayVector;
    std::map<int, int> acceleratorInstancesByType = {
        {0, 1}, // Type 0 (e.g., audio)
        {1, 1}, // Type 1 (e.g., video)
        {2, 1},  // Type 2 (e.g., signals)
        {3, 1}  // Type 3
    };

    std::map<int, double> nextScheduledEventTime = {
        {0, 0}, // Instance 0 (default event time)
        {1, 0}, // Instance 1
        {2, 0},  // Instance 2
        {3, 0}  // Instance 3
    };

    //information from recieved packets
    LoadBalancingAlgorithm lbAlgorithm;
    int streamId;
    int source;
    int type;
    int packetNum;
    std::string protocol;
    int packetsPerStream;      // Number of packets per stream
    int packetSize;            // Size of each packet in bits
    double streamInterval;     // Interval between streams
    int currentStream;         // The current stream number
    int packetsSentInStream;   // Number of packets sent in the current stream
    long droppedPacketSignal;
    simtime_t completionSignal;
    int roundRobinIndex;
    simtime_t sendTime;
    const int types = 4;    ///types of functions/traffic
    const int MAX_BUFFER_SIZE = 10; //maximum num of packets in a buffer
    int function_types;
;


  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void processStream(int streamId); // Function to process a stream
    virtual void processStreamReassembly(int streamId, int streamQueue); // Function to process a stream in reassembly buffer mode
    virtual void sendAcknowledgment(int streamId); // Send acknowledgment to client
    virtual void finish() override;

    void queueMessage(cMessage *msg);
    void notifyClient  (int source);//(int streamId, int reason);
    bool isBufferAvailable();
    bool isBufferAvailableForStream(int streamId);
    std::vector<int> getEligibleBuffersForStream(int packetsPerStream_, int packetNum_);

    void dequeueMessages(int streamId, int n);
    void enqueueMessages(ClientMsg *topMessage);
    void enqueueMessagesBuffer(ClientMsg *rmsg);

    void setFunctionBusy(int functionType);
    void setFunctionNotBusy(int functionType);
    bool isFunctionBusy(int functionType);

    int naiveRoundRobin(std::vector<int> eligibleBuffers);
    int roundRobin(std::vector<int> eligibleBuffers);
    int powerOfTwoChoices(std::vector<int> eligibleBuffers);
    int joinShortestQueue(std::vector<int> eligibleBuffers, int packetsPerStream_, int packetNum_);



  public:
    Server();
    virtual ~Server();
};

Define_Module(Server);

Server::Server() {
    processingEvent = nullptr;
}

Server::~Server() {
    // Clean up memory for buffered packets
    for (auto& streamBuffer : streamBuffers) {
        for (cMessage* msg : streamBuffer.second) {
            delete msg;
        }
    }
    cancelAndDelete(processingEvent);
}

void Server::initialize()
{
    // Initialize parameters from NED file
    processingRate = par("processingRate").doubleValue();
    MAX_NUM_BUFFERS = 4; // Maximum 100 concurrent stream buffers
    droppedPacketSignal = registerSignal("dropped");
    completionSignal = registerSignal("delay");
    //processingEvent = new cMessage("processingEvent");

    // Initialize empty stream buffers

    queuemodel = REASSEMBLY_BUFFER; //PER_FUNCTION;
    streamBuffers.clear();
    streamPacketCount.clear();
    for (int bufferId = 0; bufferId < MAX_NUM_BUFFERS; ++bufferId) {
            streamBuffers[bufferId] = std::vector<cMessage*>();
            streamPacketCount[bufferId]=0;
        }

    lbAlgorithm = JSQ;  // Set your desired algorithm
    roundRobinIndex = 0;
    //initialize function status

    function_types = 4;
    for (int i=0; i<function_types; i++){
        functionStatus[i] = NOT_BUSY;
        latency[i]=0.0;
        Throughput[i]=0.0;
        Delay[i].push_back(0.0);
        processedPackets[i]=0;
        droppedPackets[i]=0;

    }
    //functionStatus[0] = NOT_BUSY;
    //functionStatus[1] = NOT_BUSY;
    //functionStatus[2] = NOT_BUSY;
    //functionStatus[3] = NOT_BUSY;




    EV << "Server initialized\n";
}

void Server::handleMessage(cMessage *msg)
{
    const char* msgName = msg->getName();
    const char* word = "process";
    if (strstr(msgName, word) != nullptr){ //(msg == processingEvent){

        int procId;
        int streamQueue;
        sscanf(msgName, "process-%d-%d", &procId, &streamQueue);


        if (queuemodel == REASSEMBLY_BUFFER){
            processStreamReassembly(procId, streamQueue);
        } else{

            processStream(procId);
        }

    }else {

        if (queuemodel == PER_FUNCTION){

        // Extract stream ID and packet number from the message name (assuming the name format is "stream-<id>-packet-<num>")
            //const char* msgName = msg->getName();
            ClientMsg *rmsg = check_and_cast<ClientMsg *>(msg);
            enqueueMessages(rmsg);

        } else if (queuemodel == REASSEMBLY_BUFFER){
            ClientMsg *rmsg = check_and_cast<ClientMsg *>(msg);

            enqueueMessagesBuffer(rmsg);


        } else if (queuemodel == DIRECT){

            ClientMsg *rmsg = check_and_cast<ClientMsg *>(msg);

        }
    }


}


void Server::processStream(int streamId_)
{


    // Combine the packets into a single stream and simulate processing
    std::vector<cMessage*>& buffer = streamBuffers[streamId_];

    // Simulate stream processing (e.g., combining the packets)
    //for (cMessage* packet : buffer) {
      //  delete packet;  // Clean up memory for the processed packet
       // emit(droppedPacketSignal, streamId);
   // }

    // Clear the buffer for this stream after processing
    //streamBuffers.erase(streamId);
    //streamPacketCount.erase(streamId);

    cMessage *topMessage = streamBuffers[streamId_].front();
    ClientMsg *ttopMessage = check_and_cast<ClientMsg *>(topMessage);
    int packetsPerStream_ =  ttopMessage->getTotNumPackets();
    simtime_t sendTime_ = ttopMessage->getSendTime();

    int func_type = ttopMessage->getType();
    dequeueMessages(streamId_, packetsPerStream_);


    simtime_t delay  = simTime()-sendTime_;

    //emit(completionSignal, double (delay)); //need to have msg->arrivalTime so that completion is simTime() - arrivalTime


    EV << "Finished processing stream " << streamId_ << ". Sending acknowledgment.\n";
    sendAcknowledgment(streamId_);

    setFunctionNotBusy(func_type); //functionStatus[streamId] = NOT_BUSY;
}


void Server::processStreamReassembly(int streamId_, int streamQueue)
{


    std::vector<cMessage*>& buffer = streamBuffers[streamQueue];

    cMessage *topMessage = buffer.front();
    ClientMsg *ttopMessage = check_and_cast<ClientMsg *>(topMessage);
    int packetsPerStream_ =  ttopMessage->getTotNumPackets();
    simtime_t sendTime_ = ttopMessage->getSendTime();
    streamId_ = ttopMessage->getStreamId();

    int func_type = ttopMessage->getType();
    dequeueMessages(streamQueue, packetsPerStream_);

    simtime_t delay  = simTime()-sendTime_;

    //emit(completionSignal,delay ); //need to have msg->arrivalTime so that completion is simTime() - arrivalTime

    EV << "Finished processing stream " << streamId_ << ". Sending acknowledgment.\n";
    sendAcknowledgment(streamId_);
    if (static_cast<double>(nextScheduledEventTime[func_type]) - simTime().dbl() <=0){
        setFunctionNotBusy(func_type); //functionStatus[streamId] = NOT_BUSY;
    }

}

void Server::sendAcknowledgment(int streamId)
{
    // Send an acknowledgment message back to the client
    char ackName[30];
    //sprintf(ackName, "ack-stream-%d", streamId);
    cMessage *ackMsg = new cMessage(ackName);
    sendNextStream = new cMessage("sendNextStream");

    send(sendNextStream, "out", streamId);

    EV << "Requesting next packet(s) of stream " << streamId << "\n";  //<< " with " << streamPacketCount[streamId] << " packets.\n";
}


bool Server::isBufferAvailable() {
    // Check if any buffer has enough space to accept a new request
    for (const auto& pair : streamBuffers) {
        if (pair.second.size() < MAX_BUFFER_SIZE) {
            return true; // At least one buffer has space
        }
    }
    return false; // All buffers are full
}

bool Server::isBufferAvailableForStream(int streamId) {
    // Check if a buffer is available for a specific stream
    auto it = streamBuffers.find(streamId);
    if (it != streamBuffers.end()) {
        // If the stream exists and its buffer is not full, return true
        int remaining_space = MAX_BUFFER_SIZE - it->second.size();

        return remaining_space <= (packetsPerStream - packetNum);
    }
    return true; // If the stream does not exist, we can start a new buffer
}

std::vector<int> Server::getEligibleBuffersForStream(int packetsPerStream_, int packetNum_) {
    std::vector<int> eligibleBuffers;  // To store indices of eligible buffers

    // Iterate over all stream buffers
    for (const auto& pair : streamBuffers) {
        int streamQueue = pair.first;
        const std::vector<cMessage*>& buffer = pair.second;

        // Check the remaining space in the current buffer
        int remaining_space = MAX_BUFFER_SIZE - buffer.size();

        //If buffer is not empty, check that there are no ongoing reception in the buffer

        if (buffer.size()>0){
            cMessage *lastMessage = buffer.back();
                ClientMsg *ttopMessage = check_and_cast<ClientMsg *>(lastMessage);

                if (ttopMessage->getSeqNum()+1 == ttopMessage->getTotNumPackets() && remaining_space >= (packetsPerStream_ - packetNum_)){
                    eligibleBuffers.push_back(streamQueue);
                }

        } else if (remaining_space >= (packetsPerStream_ - packetNum_)) { // If the buffer has enough space for the remaining packets in the stream
            eligibleBuffers.push_back(streamQueue);  // Add this buffer index to the list
        }
    }

    return eligibleBuffers;  // Return all eligible buffers
}

void Server::queueMessage(cMessage *msg) {
    ClientMsg *rmsg = dynamic_cast<ClientMsg *>(msg); // Type cast to ClientMsg
    if (!rmsg) return; // Handle case where casting fails

    int streamId = rmsg->getStreamId();
    int type = rmsg->getType(); // 0 = audio, 1 = video, 2 = signals
    int packetNum = rmsg->getSeqNum();
    int packetsPerStream = rmsg->getTotNumPackets();

    // Ongoing Request Concatenation: If the packet belongs to an ongoing request
    if (streamBuffers.find(streamId) != streamBuffers.end()) {
        streamBuffers[streamId].push_back(msg);
        streamPacketCount[streamId]++;

        // Check if all packets for the request have been received
        if (streamPacketCount[streamId] == packetsPerStream) {
            EV << "Reassembled complete request for stream: " << streamId<< "\n";
        }
        return;
    }

    // New Request Initiation: If a new request arrives
    if (isBufferAvailableForStream(streamId)) {
        // Assign this new request to an available buffer
        streamBuffers[streamId].push_back(msg);
        streamPacketCount[streamId] = 1; // Start counting packets for this stream
        std::cout << "New request initiated for stream: " << streamId << std::endl;
    } else {
        // Overflow Handling: If no buffer is available, drop the packet
        notifyClient(streamId); // Reason 1: No available buffers
    }
}

void Server::notifyClient(int source)
{
    packetDropped = new cMessage("packetDropped");
    send(packetDropped, "out", source);
    EV << "Stream " << source << " dropped\n";  //<< " with " << streamPacketCount[streamId] << " packets.\n";

}

void Server::dequeueMessages(int streamQueue, int n) {
    std::vector<cMessage*> dequeuedMessages;
    std::vector<cMessage*>& buffer = streamBuffers[streamQueue];

    cMessage *topMessage = buffer.front();
    ClientMsg *ttopMessage = check_and_cast<ClientMsg *>(topMessage);
    int packetsPerStream_ =  ttopMessage->getTotNumPackets();
    simtime_t sendTime_ = ttopMessage->getSendTime();
    int streamId_ = ttopMessage->getStreamId();

    int func_type = ttopMessage->getType();

    // Check if the stream exists in streamBuffers
    if (streamBuffers.find(streamQueue) != streamBuffers.end()) {
        std::vector<cMessage*>& buffer = streamBuffers[streamQueue];



        // Make sure we are not dequeuing more messages than available
        int numToDequeue = std::min(n, static_cast<int>(buffer.size()));

        // Dequeue the first `numToDequeue` messages
        dequeuedMessages.insert(dequeuedMessages.end(), buffer.begin(), buffer.begin() + numToDequeue);

        // Erase the dequeued messages from the buffer
        buffer.erase(buffer.begin(), buffer.begin() + numToDequeue);
        streamPacketCount[streamQueue] -= numToDequeue;

        EV << numToDequeue << "Packets of Stream " << streamId_ << " dequeued from buffer " << streamQueue<<". New length is: " << buffer.size() << "\n";
        // If the buffer is empty after dequeuing, you may want to remove the stream from the map
        if (buffer.empty()) {
            //streamBuffers.erase(streamQueue);
            //streamPacketCount.erase(streamQueue); // Clean up packet count for this stream as well
        }


    } else {
        EV<< "Stream " << streamId << " not found!" << std::endl;
    }

    //return dequeuedMessages;
}


void Server::setFunctionBusy(int functionType) {
       if (functionStatus.find(functionType) != functionStatus.end()) {
           functionStatus[functionType] = BUSY;
           EV<< "Function type " << functionType << " is now BUSY.\n";
       }
   }

   // Method to set a function as not busy
void Server::setFunctionNotBusy(int functionType) {
       if (functionStatus.find(functionType) != functionStatus.end()) {
           functionStatus[functionType] = NOT_BUSY;
           EV << "Function type " << functionType << " is now NOT_BUSY.\n";
       }
   }

   // Method to check if a function is busy
bool Server::isFunctionBusy(int functionType) {
       if (functionStatus.find(functionType) != functionStatus.end()) {
           return functionStatus[functionType] == BUSY;
       }
       return false; // Return false if the function type does not exist
   }

void Server::enqueueMessages(ClientMsg *rmsg) {

       int  streamId_ = rmsg->getStreamId();
       int source_ = rmsg->getSource();
       int func_type = rmsg->getType();
       double sendTime_ = rmsg->getSendTime().dbl();
       int packetNum_ =  rmsg->getSeqNum();
       int packetsPerStream_ =  rmsg->getTotNumPackets();
       std::string protocol_ = rmsg->getProtocol();
       int packetSize_ =  rmsg->getSize();
       //simtime_t sendTime_ = rmsg->getSendTime();

      if (packetNum_ == 0) {
          // If this is the first packet of a stream, initialize the stream's packet count and buffer
          streamPacketCount[streamId_] = 1; // Start with the first packet
          streamBuffers[streamId_].push_back(rmsg); // Add the first packet to the buffer
       } else {
           // If this is an existing stream, buffer the packet
           streamPacketCount[streamId_]++;
           streamBuffers[streamId_].push_back(rmsg);
       }

       // Process the stream if all packets have been received (assuming that packetsPerStream is a fixed value from the NED file)

       double processingRate_;
       if (streamPacketCount[streamId_] == packetsPerStream_ or packetsPerStream_ == 1) {
           EV << "Received stream " << streamId_ << " with " << packetsPerStream_ << " packets.\n";
           // All packets for this stream have been received, schedule processing

           char processInst[40];
           sprintf(processInst, "process-%d-%d", streamId_, streamId_);
           processingEvent =  new cMessage(processInst);

           if (type == 0){
              processingRate_ = packetSize_*packetsPerStream_/1000; //audio rate
           }else if (type == 1){

              processingRate_ = packetSize_*packetsPerStream_/1000; //video rate
           }else if (type == 2){

              processingRate_ = packetSize_*packetsPerStream_/1000; //signals rate
           }
           else if (type == 3){

              processingRate_ = packetSize_*packetsPerStream_/1000; //signals rate
           }

           double remainint_exec;

           remainint_exec = std::max(0.0, static_cast<double>(nextScheduledEventTime[func_type]) - simTime().dbl());

           setFunctionBusy(func_type); // functionStatus[streamId] = BUSY;

           remainint_exec+=processingRate_;
           nextScheduledEventTime[func_type] = simTime().dbl() + remainint_exec;
           processedPackets[func_type]+=packetsPerStream_;
           EV << processedPackets[func_type] << " packets of stream  "<< streamId_ <<"now processed\n";
           latency[func_type]+=  nextScheduledEventTime[func_type] - sendTime_;
           //Throughput[func_type] = (Throughput[func_type]+ (packetSize_*packetsPerStream_/()));
            Delay[func_type].push_back(nextScheduledEventTime[func_type]-sendTime_);

            scheduleAt(nextScheduledEventTime[func_type], processingEvent);
           //processStream(streamId);
          }
}

void Server::enqueueMessagesBuffer(ClientMsg *rmsg) {
    int streamId_ = rmsg->getStreamId();
    int source_ = rmsg->getSource();
    int func_type = rmsg->getType();
    int packetNum_ = rmsg->getSeqNum();
    int packetsPerStream_ = rmsg->getTotNumPackets();
    std::string protocol_ = rmsg->getProtocol();
    int packetSize_ = rmsg->getSize();
    //simtime_t sendTime_ = rmsg->getSendTime();
    double sendTime_ = rmsg->getSendTime().dbl();
    double processingRate_;

    int eligibleQueue = -1;  // Start with an invalid queue

    if (packetNum_ == 0) {  // First packet, initialize

        std::vector<int> eligibleBuffers = getEligibleBuffersForStream(packetsPerStream_, packetNum_);

        if (eligibleBuffers.size() == 0){

                EV << "No Queue found for Stream " << streamId_ << ", Packet: "<< packetNum_ <<", Packet will be dropped\n";
                droppedPackets[func_type]++;
                droppedPacketsTime[func_type].push_back(simTime().dbl());
                notifyClient(source_);
                delete rmsg;
                return;
            }

        switch (lbAlgorithm) {
            case NAIVE_RR:
                eligibleQueue = naiveRoundRobin(eligibleBuffers);
                break;
            case RR:
                eligibleQueue = roundRobin(eligibleBuffers);
                break;
            case JSQ:
                eligibleQueue = joinShortestQueue(eligibleBuffers, packetsPerStream_, packetNum_);
                break;
            case Powof2Choices:
                eligibleQueue = powerOfTwoChoices(eligibleBuffers);
                break;
        }

        if (eligibleQueue == -1) {  // No eligible queue found
            EV << "No Queue found for Stream " << streamId_ << " Packet: " << packetNum_ << ", Packet will be dropped\n";
            droppedPacketsTime[func_type].push_back(simTime().dbl());
            droppedPackets[func_type]++;
            notifyClient(source_);
            delete rmsg;
            return;
        } else {
            streamPacketCount[eligibleQueue] += 1;  // Start with the first packet
            streamBuffers[eligibleQueue].push_back(rmsg);  // Add packet to buffer
            EV << "Stream " << streamId_ << " packet " << packetNum_ << " queued in buffer " << eligibleQueue << "\n";

        }
    } else {  // For subsequent packets


        for (const auto& pair : streamBuffers) {
            if (!pair.second.empty()) {
                ClientMsg *rlastPacket = check_and_cast<ClientMsg *>(pair.second.back());
                if (rlastPacket->getSeqNum() == packetNum_ - 1 && rlastPacket->getSource() == source_) {
                    eligibleQueue = pair.first;
                    sendTime_=rlastPacket->getSendTime().dbl();
                    EV << "Stream " << streamId_ << " packet " << packetNum_ << " queued in buffer " << eligibleQueue << "\n";
                    break;
                }
            }
        }

        if (eligibleQueue == -1) {  // No queue found
            EV << "No Queue found for Stream " << streamId_ << ", Packet"<< packetNum_ <<", Packet will be dropped\n";
            droppedPackets[func_type]++;
            droppedPacketsTime[func_type].push_back(simTime().dbl());
            notifyClient(source_);
            delete rmsg;
            return;
        } else {
            streamPacketCount[eligibleQueue] += 1;
            streamBuffers[eligibleQueue].push_back(rmsg);
        }
    }

    if (eligibleQueue > -1) {
    // Process the stream if all packets have been received
    if (packetNum_ + 1 == packetsPerStream_) {
        EV << "Received stream " << streamId_ << " with " << packetsPerStream_ << " packets.\n";

        char processInst[40];
        sprintf(processInst, "process-%d-%d", streamId_, eligibleQueue);
        processingEvent = new cMessage(processInst);

        if (func_type == 0) {
            processingRate_ =  0.001; //packetSize_ * packetsPerStream_ *8/ 10000;  // audio rate 1kbps
        } else if (func_type == 1) {
            processingRate_ =  0.002;//packetSize_ * packetsPerStream_ *8/10000;  // video rate
        } else if (func_type == 2) {
            processingRate_ = 0.003; //packetSize_ * packetsPerStream_ *8/ 10000;  // signals rate
        }else if (func_type == 3){

            processingRate_ = 0.004;//packetSize_ * packetsPerStream_ *8/ 10000; //signals rate
         }

        //setFunctionBusy(func_type);  // Mark the function type as busy
        //scheduleAt(simTime() + processingRate, processingEvent);

        double remainint_exec;

        remainint_exec = std::max(0.0, static_cast<double>(nextScheduledEventTime[func_type]) - simTime().dbl());

        setFunctionBusy(func_type); // functionStatus[streamId] = BUSY;

        remainint_exec+=processingRate_;
        nextScheduledEventTime[func_type] = simTime().dbl() + remainint_exec;
        processedPackets[func_type]+=packetsPerStream_;
        EV << processedPackets[func_type] << " packets of stream  "<< streamId_ <<"now processed\n";
        EV << nextScheduledEventTime[func_type] - sendTime_<< " taken to process  "<< streamId_ <<" sent at time " << sendTime_<<"\n";
        latency[func_type]+=  nextScheduledEventTime[func_type] - sendTime_;
        //Throughput[func_type] = (Throughput[func_type]+ (packetSize_*packetsPerStream_/()));
        Delay[func_type].push_back(nextScheduledEventTime[func_type]-sendTime_);

        scheduleAt(nextScheduledEventTime[func_type], processingEvent);

        //processStream(streamId);


    } else { //request next packet

        sendNext = new cMessage("sendNext");
        send(sendNext, "out", source_);

    }

    }
}

// NAIVE Round Robin algorithm: rotates without considering queue size
int Server::naiveRoundRobin(std::vector<int> eligibleBuffers) {
    int eligibleQueue = roundRobinIndex % streamBuffers.size();
    roundRobinIndex++;
    return eligibleQueue;
}

// Round Robin algorithm considering availability
int Server::roundRobin(std::vector<int> eligibleBuffers) {
    // Ensure the round-robin index stays within the eligible buffers range
    int initialIndex = roundRobinIndex % eligibleBuffers.size();
    int eligibleQueue = -1;
    int index = initialIndex;

    // Iterate over the eligible buffers to find the first available one
    do {
        int bufferIndex = eligibleBuffers[index];
        if (streamBuffers[bufferIndex].size() < MAX_BUFFER_SIZE) {
            eligibleQueue = bufferIndex;
            break;
        }
        index = (index + 1) % eligibleBuffers.size();
    } while (index != initialIndex);

    // Update roundRobinIndex for the next invocation
    roundRobinIndex = (index + 1) % eligibleBuffers.size();
    return eligibleQueue;
}

// Join Shortest Queue (JSQ) algorithm: finds the queue with the fewest packets
int Server::joinShortestQueue(std::vector<int> eligibleBuffers, int packetsPerStream_, int packetNum_) {
    int eligibleQueue = -1;
    int minQueueSize = MAX_BUFFER_SIZE + 1;

    // Iterate over the eligibleBuffers vector
    for (int bufferIndex : eligibleBuffers) {
        // Check the corresponding buffer size in streamBuffers
        if (streamBuffers[bufferIndex].size() <= MAX_BUFFER_SIZE - packetsPerStream_ &&
            streamBuffers[bufferIndex].size() < minQueueSize) {
            eligibleQueue = bufferIndex;
            minQueueSize = streamBuffers[bufferIndex].size(); // Update the minimum size found
        }
    }

    return eligibleQueue;
}

// Power of Two Choices algorithm: picks two random queues and selects the least loaded one
int Server::powerOfTwoChoices(std::vector<int> eligibleBuffers) {
    if (eligibleBuffers.size() < 2) {
        // If there are less than two eligible buffers, return the only available one or an error
        return eligibleBuffers.empty() ? -1 : eligibleBuffers[0];
    }

    // Select two random indices from the eligibleBuffers vector
    int idx1 = rand() % eligibleBuffers.size();
    int idx2 = rand() % eligibleBuffers.size();

    // Get the actual stream IDs from the eligibleBuffers vector
    int queue1 = eligibleBuffers[idx1];
    int queue2 = eligibleBuffers[idx2];

    // Compare the sizes of the corresponding buffers and return the index of the smaller one
    if (streamBuffers[queue1].size() < streamBuffers[queue2].size()) {
        return queue1;
    } else {
        return queue2;
    }
}


void Server::finish()
{


    for (int i=0; i<function_types; i++){

     EV << "Average latency for Stream: " << i <<    " is: "<< latency[i]/(processedPackets[i]/(1+i))<< "s; " << processedPackets[i] <<" packets processed; "<< droppedPackets[i]<< " packet(s) dropped\n";
    }
     // Open output files for summary and delay results
        std::ofstream summaryFile("summaryOfResults.csv");
        std::ofstream delayFile("delayResults.csv");
        std::ofstream dropTimeFile("dropTimeStampResults.csv");

        // Check if files opened successfully
        if (!summaryFile.is_open() || !delayFile.is_open()|| !dropTimeFile.is_open()) {
            EV << "Error opening file for writing results!" << std::endl;
            return;
        }

        // Write the headers for the summary CSV file
        summaryFile << "streamId,averageLatency,processedPackets,droppedPackets\n";

        // Write the header for the delay CSV file (streamId + delay values)
        delayFile << "streamId,delayValues\n";

        // Write the header for the delay CSV file (streamId + delay values)
        dropTimeFile << "streamId,delayValues\n";

        // Process each stream
        for (const auto& entry : latency) {
            int streamId = entry.first;
            double avgLatency = processedPackets[streamId] > 0 ? latency[streamId] / (processedPackets[streamId]/(1+streamId) ): 0;

            // Write the summary results in CSV format
            summaryFile << streamId << ","
                        << std::fixed << std::setprecision(6) << avgLatency << ","
                        << processedPackets[streamId] << ","
                        << droppedPackets[streamId] << "\n";

            // Write the delay results for the stream
            delayFile << streamId << ",";
            const auto& delays = Delay[streamId];

            for (size_t i = 0; i < delays.size(); ++i) {
                delayFile << delays[i];
                if (i != delays.size() - 1) {
                    delayFile << ",";  // Add comma between delay values
                }
            }
            delayFile << "\n";  // End of the line for this stream

            // Write the drop timestampt results for the stream
            dropTimeFile << streamId << ",";
             const auto& drops = droppedPacketsTime[streamId];

             for (size_t i = 0; i < drops.size(); ++i) {
                 dropTimeFile << drops[i];
                  if (i != drops.size() - 1) {
                      dropTimeFile << ",";  // Add comma between delay values
                   }
               }
             dropTimeFile << "\n";  // End of the line for this stream
        }

        // Close the files after writing
        summaryFile.close();
        delayFile.close();
        dropTimeFile.close();


    //EV << "Sent:     " << numPacketsSent << endl;
    //EV << "Received: " << numStreamsSent << endl;
   // EV <<droppedPackets << " Packets Dropped." << endl;
   // EV << "Hop count, min:    " << hopCountStats.getMin() << endl;
   // EV << "Hop count, max:    " << hopCountStats.getMax() << endl;
   // EV << "Hop count, mean:   " << hopCountStats.getMean() << endl;
    //EV << "Hop count, stddev: " << hopCountStats.getStddev() << endl;

   // recordScalar("#sent", numPacketsSent);
   // recordScalar("#received", numStreamsSent);
    //recordScalar("#dropped", droppedPackets);

    //hopCountStats.recordAs("hop count");
}
