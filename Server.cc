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

std::ostringstream oss;

using namespace omnetpp;

class Server : public cSimpleModule
{
  private:
    double processingRate;       // Rate at which streams are processed
    int maxBufferCount;          // Maximum number of concurrent streams (buffers) handled by the server
    std::map<int, std::vector<cMessage*>> streamBuffers; // Map of buffers to store packets for each stream
    std::map<int, int> streamPacketCount; // Map to keep track of packet counts per stream
    cMessage *processingEvent;   // Event to trigger stream processing
    cMessage *sendNextPacket;

    //information from recieved packets

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
;


  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void processStream(int streamId); // Function to process a stream
    virtual void sendAcknowledgment(int streamId); // Send acknowledgment to client

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
    maxBufferCount = 100; // Maximum 100 concurrent stream buffers

    //processingEvent = new cMessage("processingEvent");

    // Initialize empty stream buffers
    streamBuffers.clear();
    streamPacketCount.clear();
    EV << "Server initialized\n";
}

void Server::handleMessage(cMessage *msg)
{
    // Extract stream ID and packet number from the message name (assuming the name format is "stream-<id>-packet-<num>")
    const char* msgName = msg->getName();
    ClientMsg *rmsg = check_and_cast<ClientMsg *>(msg);
    streamId = rmsg->getStreamId();
    source = rmsg->getSource();
    type = rmsg->getType();
    packetNum =  rmsg->getSeqNum();
    packetsPerStream =  rmsg->getTotNumPackets();
    protocol = rmsg->getProtocol();
    packetSize =  rmsg->getSize();


    //EV << "Received stream " << msgName ;
    //int streamId, packetNum, packetsPerStream;

    //sscanf(msgName, "client-%d-packet-%d-packetsPerStream-%d", &streamId, &packetNum, &packetsPerStream );


    EV << "Received stream " << streamId << " with " << packetsPerStream << " packets.\n";

    //if (streamPacketCount.find(streamId) == streamPacketCount.end()) {
      if (packetNum == 0) {
        // If this is the first packet of a stream, initialize the stream's packet count and buffer
        streamPacketCount[streamId] = 1; // Start with the first packet
        streamBuffers[streamId].push_back(rmsg); // Add the first packet to the buffer
    } else {
        // If this is an existing stream, buffer the packet
        streamPacketCount[streamId]++;
        streamBuffers[streamId].push_back(rmsg);
    }

    // Process the stream if all packets have been received (assuming that packetsPerStream is a fixed value from the NED file)


    if (streamPacketCount[streamId] == packetsPerStream) {
        // All packets for this stream have been received, schedule processing
        scheduleAt(simTime() + processingRate, processingEvent);
        processStream(streamId);
    } else if (packetsPerStream == 1) {
        // If it's a single-packet stream, process immediately without buffering
       processStream(streamId);

    }
}

void Server::processStream(int streamId)
{


    // Combine the packets into a single stream and simulate processing
    std::vector<cMessage*>& buffer = streamBuffers[streamId];

    // Simulate stream processing (e.g., combining the packets)
    for (cMessage* packet : buffer) {
        delete packet;  // Clean up memory for the processed packet
    }

    // Clear the buffer for this stream after processing
    streamBuffers.erase(streamId);
    streamPacketCount.erase(streamId);

    EV << "Finished processing stream " << streamId << ". Sending acknowledgment.\n";
    sendAcknowledgment(streamId);
}

void Server::sendAcknowledgment(int streamId)
{
    // Send an acknowledgment message back to the client
    char ackName[30];
    //sprintf(ackName, "ack-stream-%d", streamId);
    cMessage *ackMsg = new cMessage(ackName);
    sendNextPacket = new cMessage("sendNextPacket");



    send(sendNextPacket, "out", streamId);

    EV << "Acknowledged stream " << streamId << "\n";  //<< " with " << streamPacketCount[streamId] << " packets.\n";

}
