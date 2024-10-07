//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

simple Client
{
    parameters:
        double streamInterval = default(5.0);   // Interval between streams
        double packetInterval=  default(1.0);   // Interval between packets within a stream
        int packetsPerStream = default(2);   // Number of packets in each stream
        int packetSize = default(200);  
        int streamCount = 0;  
        int streamId = default(0); 
        int source = default(0);
        int destination = default(-1);
        int type = default(0);
        string protocol = default("tcp");
        
   
         // Size of each packet (in bytes)
    gates:
        input in;
        output out;
}

simple Server
{
    parameters:
        double processingRate = default(1.0);   // Rate at which streams are processed
        //int packetsPerStream = default(2);    // Number of packets per stream (can be 1 for single-packet streams)
    gates:
        input in[4];
        output out[4];
}


network ClientServer
{
    submodules:
        client1: Client;
        client2: Client;
        client3: Client;
        client4: Client;
        server: Server;
    connections:
        client1.out --> {  delay = 100ms; } --> server.in[0];
        client1.in <-- {  delay = 100ms; } <-- server.out[0];
        
        client2.out --> {  delay = 100ms; } --> server.in[1];
        client2.in <-- {  delay = 100ms; } <-- server.out[1];
        
        client3.out --> {  delay = 100ms; } --> server.in[2];
        client3.in <-- {  delay = 100ms; } <-- server.out[2];
        
        client4.out --> {  delay = 100ms; } --> server.in[3];
        client4.in <-- {  delay = 100ms; } <-- server.out[3];
}
