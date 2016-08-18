Part 1: 
As a Switch:
1.Switch on your VM and login to Mininet with username and password as ( mininet, mininet)
2.To enable network access in the VM type in " sudo dhclient eth1 " in Mininet terminal
3.Start Xming.
4.Using Putty with X11 forwarding enabled open 2 SSH terminals and login using same credentials
5.Type in "sudo mn --topo single,3 --mac --switch ovsk --controller remote" in one of the terminals and this would create the topology
6.After typing the above command mininet will get it prompt which is shown as mininet> prompt
7.Change to folder mininet/pox/pox/misc and copy rupa_of_tutorial_switch.py file into folder
8.Name the file as of_tutorial_switch.py
9.In another terminal, change to folder mininet/pox directory.
10.Run " ./pox.py log.level --DEBUG misc.of_tutorial_switch " to start the controller
11.Type in "xterm h1 h2 h3" to create three terminals for hosts 1,2 and 3 in the terminal which has mininet> prompt
12.Type "tcpdump -XX -n -i h2-eth0" in xterm for h2
13.Type "tcpdump -XX -n -i h3-eth0" in xterm for h3 
14.Type "ping -c1 10.0.0.2" in xterm for h1, here h1 pings h2
15.In h3's xterm only an ARP request has been received but h3 does not give any reply
16.In h2's xterm, the ARP request is got, and sends a reply to h1.
17.In h1's xterm, the ping is successful
17.Close all the three xterms and check the bandwidth using the command iperf

As a Router:
1.Login to Mininet with username and password as ( mininet, mininet)
2.To enable network access in the VM type in " sudo dhclient eth1 " in Mininet terminal
3.Start Xming.
4.Using Putty with X11 forwarding enabled open 2 SSH terminals and login using same credentials
5.Change to folder mininet/pox/pox/misc and copy rupa_partA_topo.py file into folder
6.Name the file as mytopo.py and change directory to mininet/pox/pox/misc.
7.Run "sudo mn --custom mytopo.py --topo mytopo --mac --controller remote" to create the topology
8.After typing the above command mininet will get it prompt which is shown as mininet> prompt
9.Copy the rupa_of_tutorial_routerA.py file in the folder mininet/pox/pox/misc
10.Name the file as of_tutorial_router1.py
11.Change to mininet/pox directory
10.In another terminal run " ./pox.py log.level --DEBUG misc.of_tutorial_router1 " to start the controller
11.Execute "h1 ping -c1 h2" to ping host h2 from h1 in the terminal which has mininet> prompt in the shell, 
12.In the controller terminal, the output shows the flow of the packet.
13.The mininet terminal shows the ping statistics
14.In the topology terminal which has mininet> prompt in the shell, execute "pingall" to ping every host from every other host.
15.In the controller terminal, the output shows the flow of the packets. 
16.The mininet terminal shows the ping statistics.
17.In the topology terminal which has mininet> prompt in the shell, execute "h1 ping -c1 10.0.1.1" to ping the router from h1.
17.In the controller terminal, the output shows the flow of the packets, which shows the router sends an ICMP reply
18.The mininet terminal shows the ping statistics.
19.In the topology terminal which has mininet> prompt in the shell, execute "h1 ping -c1 10.99.0.1 " to ping an unreachable host from h1.
19.In the controller terminal, the output shows the flow of the packets, which shows the the unreachable destination message
20.The mininet terminal shows the ping statistics, which says that the packet has been lost as host is unreachable
21.Check the bandwidth using the command iperf


