/*
 * client.cc
 *
 *  Created on: Sep 24, 2024
 *      Author: OJEWALM
 */

#include <string.h>
#include <omnetpp.h>
#include "ClientMsg_m.h"
#include <string>
using namespace omnetpp;

/**
 * Client module that sends streams of packets to a server.
 * The number of packets in each stream, the size of the packets, and the interval between streams are parameterized.
 */
class Client : public cSimpleModule
{
  private:
    int streamCount;           // Number of streams to send
    int packetsPerStream;      // Number of packets per stream
    int packetSize;            // Size of each packet in bits
    double streamInterval;     // Interval between streams
    int currentStream;         // The current stream number
    int packetsSentInStream;   // Number of packets sent in the current stream
    int streamId;
    double packetInterval;
    int source;
    int destination;
    int type;
    bool openLoop;
    std::string protocol;
    cMessage *sendNextPacket;  // Self-message for sending the next packet in a stream
    cMessage *sendNext;
    ClientMsg *lastPacketSent;
    long numPacketsSent;
    long numStreamsSent;
    //long numReceived;
    long droppedPackets;
    //simtime_t sendTime;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void sendStream();  // Function to send a stream of packets
    virtual void sendStreamOpenLoop();
    virtual void sendPacket(int streamId, int PacketNum, int packetsPerStream);  // Function to send a single packet
    virtual ClientMsg *generateMessage(int packetNum);
    //virtual void finish() override;
    virtual void sendNPacket(int streamId);
};

Define_Module(Client);

void Client::initialize()
{
    // Initialize parameters from the NED file
    streamCount = par("streamCount");
    packetsPerStream = par("packetsPerStream");
    packetSize = par("packetSize");
    streamInterval = par("streamInterval");
    packetInterval = par("packetInterval");
    streamId = par("streamId");
    currentStream = 0;
    packetsSentInStream = 0;
    source = par("source");
    destination = par("destination");
    type = par("type");
    openLoop= false;
    //sendTime = par("sendTime");
    protocol = std::string(par("protocol").stringValue());

    // Schedule the first stream to be sent
    sendNextPacket = new cMessage("sendNextPacket");
    scheduleAt(simTime(), sendNextPacket);
    EV << "Client initialized\n";
}

void Client::handleMessage(cMessage *msg)
{
    const char* msgName = msg->getName();

    if (msg == sendNextPacket) {
        // If it's time to send the next packet in the current stream
        if (!openLoop){
        sendStream();
        }else{
            sendStreamOpenLoop();
        }
    }
    else if (strcmp(msgName, "sendNextStream") == 0) {  // send next stream based on feedback from server
        EV << "Server Feedback Received: " << msgName << "\n";
        numStreamsSent++;
        if (!openLoop){
//            sendStreamOpenLoop();
//        }else{

        sendStream();
        }
    }
    else  if (strcmp(msgName, "sendNext") == 0) {
        // If it's time to send the next packet in the current stream

        packetsSentInStream+=1;
       // sendPacket(streamId, packetsSentInStream, packetsPerStream);


        // Check if we've completed the current stream
          if (packetsSentInStream >= packetsPerStream) {
              currentStream++;          // Move to next stream
       // Reset packet counter
              numPacketsSent++;
              packetsSentInStream = 0;    // Reset stream packet counter
          } else {

              sendPacket(streamId, packetsSentInStream, packetsPerStream);
          }


    }
    else  if (strcmp(msgName, "packetDropped") == 0) {
            // If server notifies that a packet is dropped.
        droppedPackets++;
        EV<< "Packet " << packetsSentInStream << " of stream " <<streamId<<" will be retransmitted\n";
        //don't increase packet count, just resent last packet
        sendPacket(streamId, packetsSentInStream, packetsPerStream);

        }

    else {
        // Handle any other incoming messages (like acknowledgments from the server)
        EV << "Received acknowledgment or unknown message: " << msgName << "\n";
        delete msg;  // Clean up the message
    }

}

void Client::sendStream()
{

    packetsSentInStream = 0; //reset count
    // Send first packet
    sendPacket(streamId, packetsSentInStream, packetsPerStream);

}

void Client::sendStreamOpenLoop()
{

    packetsSentInStream = 0; //reset count
    // Send first packet
    ClientMsg *custMsg = generateMessage(packetsSentInStream);
    custMsg->setByteLength(packetSize);
    // Send the packet through the "out" gate
     lastPacketSent = custMsg;
     send(custMsg, "out");
     numPacketsSent++;

     scheduleAt(simTime() +  streamInterval, sendNextPacket);

}


void Client::sendNPacket(int streamId)
{
    sendNext = new cMessage("sendNext");
    scheduleAt(simTime() +  packetInterval, sendNext);

}

void Client::sendPacket(int streamId, int packetNum, int packetsPerStream)
{
    // Create a new packet (cPacket supports packet size)

    /**
     *
     * char pktname[40];
    sprintf(pktname, "client-%d-packet-%d-packetsPerStream-%d", streamId, packetNum, packetsPerStream);

    cPacket *pkt = new cPacket(pktname);

    //Set the packet size in bits
    pkt->setByteLength(packetSize);
    **/

    //Generate custom packet
    ClientMsg *custMsg = generateMessage(packetNum);
    custMsg->setByteLength(packetSize);
    // Send the packet through the "out" gate
    lastPacketSent = custMsg;
    send(custMsg, "out");
    numPacketsSent++;
}

ClientMsg *Client::generateMessage(int packetNum)
{



    char msgname[40];
    sprintf(msgname, "client-%d-to-server-%d", source, destination);

    // Create message object and set source and destination field.
    ClientMsg *msg = new ClientMsg(msgname);
    msg->setSource(source);
    msg->setDestination(destination);
    msg->setType(type);
    msg->setSeqNum(packetNum);
    msg->setTotNumPackets(packetsPerStream);
    msg->setProtocol(par("protocol"));
    msg->setStreamId(streamId);
    msg->setSize(packetSize);
    msg->setSendTime(simTime());
    return msg;


}
//void Client::finish()
//{
//    // This function is called by OMNeT++ at the end of the simulation.
//    EV << "Sent:     " << numPacketsSent << endl;
//    EV << "Received: " << numStreamsSent << endl;
//    EV <<droppedPackets << " Packets Dropped." << endl;
//   // EV << "Hop count, min:    " << hopCountStats.getMin() << endl;
//   // EV << "Hop count, max:    " << hopCountStats.getMax() << endl;
//   // EV << "Hop count, mean:   " << hopCountStats.getMean() << endl;
//    //EV << "Hop count, stddev: " << hopCountStats.getStddev() << endl;
//
//    recordScalar("#sent", numPacketsSent);
//    recordScalar("#received", numStreamsSent);
//    recordScalar("#dropped", droppedPackets);
//
//    //hopCountStats.recordAs("hop count");
//}




