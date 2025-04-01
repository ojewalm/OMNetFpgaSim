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

//
// TODO generated message class
//
message ClientMsg{
    int streamId;
    int type; //= 0; //0 audio, 1=video; 2=signals, 3=others
    int size;
    int source;
    int destination;
    int seqNum;
    int totNumPackets;
    string protocol;
    simtime_t sendTime;
    int64   byteLength;
    
    //string anotherField;
    //double arrayField1[];
    //double arrayField2[10];
}
