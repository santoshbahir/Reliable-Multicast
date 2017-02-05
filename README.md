Construction of a reliable multicast tool using UDP/IP.  The reliable multicast will 
assume no network partitions or processor crashes, but it WILL handle all kinds  of
of message omissions over a local area network.

All packets are delivered reliably according to AGREED order that respects FIFO order as well.

The designed protocol should not assume knowing the number of packets each 
A tool that reliably multicasts packets between N machines that reside on the same 
network segment,so that one multicast message may get to all of them.

The tool consists of two programs: 
	mcast - the tool's process. 
	start_mcast - a process signaling the mcast processes to start.
The mcast process should be run as follows:

mcast <num_of_packets> <machine_index> <number of machines> <loss rate>

The <number of machines> indicates how many mcast machines will be run. 
The maximal number of <number of machines> should be a constant. In a particular 
execution there may be less than or equal <number of machines>.  <loss rate> is the 
percentage loss rate for this execution. Mmachine_index will be in the 
range [1..<number of machines>] and exactly <number of machines> mcast programs, 
each with a different index, will be ready to run before start_mcast is executed.

The num_of_packets indicates how many packets THIS mcast has to initiate. 
This number may be on the order of 200,000 packets and even more. Each packet 
should contain the creating machine_index, a packet_index and a random number, each
stored in a 4 byte integer. The random number is an integer, picked at the time of 
the creation of the packet, randomly selected from the range 1-1,000,000. 
In addition, each packet will include 1300 additional payload bytes. If your 
protocol requires additional information, that can be added to the packet. 
Therefore, each packet should include at least 1312 bytes, and possibly more, 
depending on the information used by the protocol. 

After all the mcast processes have been started, the start_mcast process is 
executed, signaling the mcast processes to start the protocol by sending a single 
special multicast message.

Output:
-------
On delivery of a packet, the mcast should write to a file, in ASCII format, 
the packet data as follows:
  
fprintf(fd, "%2d, %8d, %8d\n", pack->machine_index, pack->index, pack->random_data);

The output file name will be <machine_index>.out

