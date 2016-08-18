PART 2:
As Router:

1.Switch on your VM and login to Mininet with username and password as ( mininet, mininet)
2.To enable network access in the VM type in " sudo dhclient eth1 " in Mininet terminal
3.Start Xming.
4.Using Putty with X11 forwarding enabled open 2 SSH terminals and login using same credentials
5.Copy the "topo.part_2.py" file into folder mininet/pox/pox/misc
5.Name the file as mytopo2.py and do cd to mininet/pox/pox/misc.
6.Run "sudo mn --custom mytopo2.py --topo mytopo --mac --controller remote" which will create the topology
8.Copy the router_exc_part2.py file into folder mininet/pox/pox/misc
9.Name the file as of_tutorial_rtr_2.py and change to mininet/pox directory
10.Run " ./pox.py log.level --DEBUG misc.of_tutorial_rtr_2 " to start the controller in other terminal
11.Execute "h1 ping -c1 h2" to ping host h2 from h1 in the topology terminal
12.In the controller terminal, the output shows the flow of the packet.
13.The mininet terminal shows the ping statistics
14.In the topology terminal which has mininet> prompt in the shell, execute "pingall" to ping every host from every other host.
15.In the controller terminal, the output shows the flow of the packets. 
16.The mininet terminal shows the ping statistics
In In the topology terminal execute "h1 ping -c1 10.0.1.1" to ping the router from h1.
17.In the controller terminal, the output shows the flow of the packets, which shows the router sends an ICMP reply
18.The mininet terminal shows the ping statistics 
In the topology terminal which has mininet> prompt in the shell, execute "h1 ping -c1 10.99.0.1 " to ping an unreachable host from h1.
19.In the controller terminal, the output shows the flow of the packets, which shows the the unreachable destination message
20.The mininet terminal shows the ping statistics, which says that the packet has been lost as host is unreachable
21.Again in the topology terminal execute "h1 ping -c1 h3" to ping host h3 from h1.
22.In the controller terminal, the output shows the flow of the packet.
23.The mininet terminal shows that the packet has been lost as the destination is unreachable
24.Check the bandwidth using the command iperf