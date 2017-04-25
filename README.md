# ns3-sim
An ns3 simulation script to compare various TCP variants under congestion

### Install Instructions:
Requirements:
 * ns3(3.26)
Steps:
 * Install ns3 and put ns3.cc in the scratch directory.
 * Run the script using ```./waf --run "ns3"```
 * For command line options and help, ```./waf --run "ns3 --PrintHelp"```

### Topology
    n0 ----------- n1
    
Topology consists of two nodes N0 and N1 connected  by a link  of bandwidth 1Mbps and link delay 10ms. We are using a drop-tail queue at the link.


### Simulation
There is a TCP agent at N0 which creates FTP traffic destined for N1. There are 5 CBR traffic agents of rate 300 Kbps each at N0 destined for N1.
The timing of the flows is as follows:

 * FTP starts at 0 sec and continues till the end of simulation
 * CBR1 starts at 200 ms and continues till end
 * CBR2 starts at 400 ms and continues till end
 * CBR3 starts at 600 ms and stops at 1200 ms
 * CBR4 starts at 800 ms and stops at 1400 ms
 * CBR5 starts at 1000 ms and stops at 1600 ms
 * Simulation runs for 1800 ms

### Plots
![Congestion Window vs Time](https://github.com/samj1912/ns3-sim/blob/master/plots/congestion.png)

![Total Bytes vs Time](https://github.com/samj1912/ns3-sim/blob/master/plots/total_bytes.png)

![Packet Drops vs Time](https://github.com/samj1912/ns3-sim/blob/master/plots/drops.png)
