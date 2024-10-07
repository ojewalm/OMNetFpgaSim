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
    int source;
    int destination;
    int type;
    std::string protocol;
    cMessage *sendNextPacket;  // Self-message for sending the next packet in a stream


  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void sendStream();  // Function to send a stream of packets
    virtual void sendPacket(int streamId, int PacketNum, int packetsPerStream);  // Function to send a single packet
    virtual ClientMsg *generateMessage(int packetNum);
};

Define_Module(Client);

void Client::initialize()
{
    // Initialize parameters from the NED file
    streamCount = par("streamCount");
    packetsPerStream = par("packetsPerStream");
    packetSize = par("packetSize");
    streamInterval = par("streamInterval");
    streamId = par("streamId");
    currentStream = 0;
    packetsSentInStream = 0;
    source = par("source");
    destination = par("destination");
    type = par("type");
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
        sendStream();
    }
    else if (strcmp(msgName, "sendNextPacket") == 0) {  // send next stream based on feedback from server
        EV << "Server Feedback Received: " << msgName << "\n";
        sendStream();
    }
    else {
        // Handle any other incoming messages (like acknowledgments from the server)
        EV << "Received acknowledgment or unknown message: " << msgName << "\n";
        delete msg;  // Clean up the message
    }

}

void Client::sendStream()
{
    if (packetsSentInStream <= streamCount) {
        // Send packets in the current stream
        for (int i = 0; i < packetsPerStream; i++) {
            sendPacket(streamId, i, packetsPerStream);
        }

        currentStream++;  // Move to the next stream
        packetsSentInStream = 0;  // Reset the packet counter

        // Schedule the next stream to be sent after the specified interval
        //scheduleAt(simTime() + streamInterval, sendNextPacket); //Open loop instance
    }
}

void Client::sendPacket(int streamId, int packetNum, int packetsPerStream)
{
    // Create a new packet (cPacket supports packet size)

    char pktname[40];
    sprintf(pktname, "client-%d-packet-%d-packetsPerStream-%d", streamId, packetNum, packetsPerStream);

    cPacket *pkt = new cPacket(pktname);

    // Set the packet size in bits
    pkt->setByteLength(packetSize);
    ClientMsg *custMsg = generateMessage(packetNum);
    // Send the packet through the "out" gate
    send(custMsg, "out");
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
    return msg;


}

