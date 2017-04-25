# ns3-sim
An ns3 simulation script to compare various TCP variants under congestion

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
