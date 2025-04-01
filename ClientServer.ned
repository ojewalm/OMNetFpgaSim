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
        double streamInterval = default(0.000001);   // Interval between streams
        double packetInterval=  default(0.000001);   // Interval between packets within a stream
        int packetsPerStream = default(2);   // Number of packets in each stream
        int packetSize = default(200);  
        int streamCount = 0;  
        int streamId = default(0); 
        int source = default(0);
        int destination = default(-1);
        int type = default(0);
        string protocol = default("tcp");
        
        //simtime_t sendTime; //= default(0.0);
        
        @display("i=block/app");
   		//@display("i=misc/sensor3;bgb=418,317");
   		//@display("i=block/transport");
   		//@display("i=device/accesspoint;bgb=418,317");
   		
         // Size of each packet (in bytes)
    gates:
        input in;
        output out;
    
}

simple Mobile extends Client
{
    parameters:
       
        //@display("i=block/app");
   		//@display("i=misc/sensor2");
   		//@display("i=block/transport");
   		//@display("i=device/accesspoint;bgb=418,317");
   		//@display("i=block/fork");
   		@display("i=device/palm2;bgb=418,317");
   		//@display("i=misc/train");
   		//@display("i=device/wifilaptop");
   		
   		
         // Size of each packet (in bytes)
}

simple Laptop extends Client
{
    parameters:
       
        //@display("i=block/app");
   		//@display("i=misc/sensor2");
   		//@display("i=block/transport");
   		//@display("i=device/accesspoint;bgb=418,317");
   		//@display("i=block/fork");
   		//@display("i=device/palm2;bgb=418,317");
   		//@display("i=misc/train");
   		@display("i=device/wifilaptop");
   		
   		
         // Size of each packet (in bytes)
}

simple Accesspoint extends Client
{
    parameters:
       
        //@display("i=block/app");
   		//@display("i=misc/sensor2");
   		//@display("i=block/transport");
   		@display("i=device/accesspoint;bgb=418,317");
   		//@display("i=block/fork");
   		//@display("i=device/palm2;bgb=418,317");
   		//@display("i=misc/train");
   		//@display("i=device/wifilaptop");
   		
   		
         // Size of each packet (in bytes)
}



simple Server
{
    parameters:
        double processingRate = default(0.1);
        int ports = default(4);   // Rate at which streams are processed
        int numBuffers = default(4)
        //int packetsPerStream = default(2);    // Number of packets per stream (can be 1 for single-packet streams)
       // @display("i=device/pc2;bgb=418,317");
       @display("i=device/server;bgb=418,317");
       //i=device/cellphone
        
        //@signal[dropped](type="long");
        //@statistic[droppedPacket](title="dropped Packet"; source="dropped"; record=vector,stats; interpolationmode=none);
        @signal[delay](type="long");
        @statistic[endtoEndDelay](title="end to end delay"; source="delay"; record=vector,stats; interpolationmode=none);
        
    gates:
        input in[ports];
        output out[ports];
}


network ClientServer
{
    submodules:
        client1: Client;
        client2: Accesspoint;
        client3: Laptop;
        client4: Mobile;
        server: Server
        {
        	ports =4;
            numBuffers=4;
            @display("i=device/server;bgb=418,317");
        }
    connections:
        client1.out --> {  delay = 100ns; } --> server.in[0];
        client1.in <-- {  delay = 100ns; } <-- server.out[0];
        
        client2.out --> {  delay = 100ns; } --> server.in[1];
        client2.in <-- {  delay = 100ns; } <-- server.out[1];
        
        client3.out --> {  delay = 100ns; } --> server.in[2];
        client3.in <-- {  delay = 100ns; } <-- server.out[2];
        
        client4.out --> {  delay = 100ns; } --> server.in[3];
        client4.in <-- {  delay = 100ns; } <-- server.out[3];
}

//
// ClientServer-mid with 50 clients and 1 server
//
network ClientServerMid
{
    submodules:
        // Create 50 clients, which could be of different types
        client[8]: Client {
            //@display("p=100,100");
            packetsPerStream = 1 + (index % 3);
            packetInterval = 0.0001;// millionPersecond
            streamInterval =0.001;// 1 + (index % 3);
            packetSize = intuniform(200, 300);
            streamId = index;
            source = index;
            destination = 50; // Server ID
            type = index % 4;  // Cycle through audio (0), video (1), signals (2)
            protocol = "tcp";
                
        }
        server: Server {
            ports =8;
            numBuffers=4;
            @display("p=200,100");
            @display("i=device/server;bgb=418,317");
        }
    connections allowunconnected:
        // Loop to connect each client to the server with a delay
        for i=0..7 {
            client[i].out --> { delay = 100ns; } --> server.in[i];
            client[i].in <-- { delay = 100ns; } <-- server.out[i];
        }
}

//
// ClientServer-large with 500 clients and 1 server
//
network ClientServerLarge
{
    submodules:
        // Create 500 clients
        client[500]: Client {
           // @display("p=100,100");
            
     
                packetsPerStream = 1 + (index % 3);
                packetInterval = 0.00001;// millionPersecond
                streamInterval = 0.001;//1 + (index % 3);
                packetSize = intuniform(200, 300);
                streamId = index;
                source = index;
                destination = 5; // Server ID
                type = index % 4;  // Cycle through audio (0), video (1), signals (2)
                protocol = "tcp";
                
        }
        server: Server {
            ports =500;
            numBuffers=4;
            @display("p=200,100");
            @display("i=device/server;bgb=418,317");
        }
    connections allowunconnected:
        // Loop to connect each client to the server with a delay
        for i=0..499 {
            client[i].out --> { delay = 100ns; } --> server.in[i];
            client[i].in <-- { delay = 100ns; } <-- server.out[i];
        }
}

