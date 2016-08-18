"""Reference taken from L3 learning switch"""
from pox.core import core
import pox.lib.packet as pckt
import pox.openflow.libopenflow_01 as of
from pox.lib.addresses import IPAddr, EthAddr
from pox.lib.packet.arp import arp
from pox.lib.packet.ethernet import ethernet, ETHER_BROADCAST
from pox.lib.util import str_to_bool, dpidToStr
import pox
import time
log = core.getLogger()
from pox.lib.recoco import Timer
from pox.lib.revent import EventHalt, Event, EventMixin
from pox.lib.packet.ipv4 import ipv4

class Entry (object):
  def __init__ (self, port, mac):
    self.port = port
    self.mac = mac
  def __eq__ (self, other):
    if type(other) == tuple:
      return (self.port,self.mac)==other
    else:
      return (self.port,self.mac)==(other.port,other.mac)
  def __ne__ (self, other):
    return not self.__eq__(other)

def dpid_to_mac (dpid):
    return EthAddr("%012x" % (dpid & 0xffFFffFFffFF,))


class router (EventMixin):
  def __init__ (self, unknownARP = []):
    self.unknownARP = unknownARP
    self.arpTable = {}
    self.ip_to_port = {}
    self.buffer_queue = {}
    self.routing_table = [ ['10.0.1.100', '10.0.1.100', 's1-eth1', '10.0.1.1', 1],
                           ['10.0.2.100', '10.0.2.100', 's1-eth2', '10.0.2.1', 2],
                           ['10.0.3.100', '10.0.3.100', 's1-eth3', '10.0.3.1', 3]]
    self.listenTo(core)

  def processWaitingPackets (self, dpid, ipaddr, macaddr, port):
    if (dpid,ipaddr) in self.buffer_queue:

      bucket = self.buffer_queue[(dpid,ipaddr)]
      del self.buffer_queue[(dpid,ipaddr)]
      log.debug("Sending %i buffered packets to %s from %s"
                % (len(bucket),ipaddr,dpidToStr(dpid)))
      for _,buffer_id,in_port in bucket:
        po = of.ofp_packet_out(buffer_id=buffer_id,in_port=in_port)
        po.actions.append(of.ofp_action_dl_addr.set_dst(macaddr))
        po.actions.append(of.ofp_action_output(port = port))
        core.openflow.sendToDPID(dpid, po)

  def _handle_GoingUpEvent (self, event):
    self.listenTo(core.openflow)
    log.debug("Connection is Up...")

  def _arp_request(self, packet,dstaddr, inport, dpid, event):
      r = arp()
      r.hwtype = r.HW_TYPE_ETHERNET
      r.prototype = r.PROTO_TYPE_IP
      r.hwlen = 6
      r.protolen = r.protolen
      r.opcode = r.REQUEST
      r.hwdst = ETHER_BROADCAST
      r.protodst = dstaddr
      r.hwsrc = dpid_to_mac(dpid)
      r.protosrc = packet.next.srcip
      e = ethernet(type=ethernet.ARP_TYPE, src=packet.src,
                     dst=ETHER_BROADCAST)
      e.set_payload(r)
      log.debug("DPID %i, INPORT %i, sending ARP Request for %s on behalf of %s" % (dpid, inport, str(r.protodst), str(r.protosrc)))
      msg = of.ofp_packet_out()
      msg.data = e.pack()
      msg.actions.append(of.ofp_action_output(port = of.OFPP_FLOOD))
      msg.in_port = inport
      #self.connections[dpid].send(msg)
      event.connection.send(msg)

  def _arp_response(self, a,packet, inport, dpid, event):
      r = arp()
      r.hwtype = a.hwtype
      r.prototype = a.prototype
      r.hwlen = a.hwlen
      r.protolen = a.protolen
      r.opcode = arp.REPLY
      r.hwdst = a.hwsrc
      r.protodst = a.protosrc
      r.protosrc = a.protodst
      r.hwsrc = self.arpTable[dpid][a.protodst].mac

      e = ethernet(type=packet.type,src=dpid_to_mac(dpid), dst=a.hwsrc)
      e.set_payload(r)
      msg = of.ofp_packet_out()
      msg.data = e.pack()
      msg.actions.append(of.ofp_action_output(port = of.OFPP_IN_PORT))
      msg.in_port = inport

      log.debug("DPID %d, INPORT %d, answering for arp from %s: MAC for %s is %s", dpid, inport, str(a.protosrc), str(r.protosrc), str(r.hwsrc))
      #self.connections[dpid].send(msg)
      event.connection.send(msg)


  def _handle_PacketIn (self, event):
    dpid = event.connection.dpid
    inport = event.port
    packet = event.parsed
    if not packet.parsed:
      log.warning("%i %i ignoring unparsed packet", dpid, inport)
      return

    if dpid not in self.arpTable:
      self.arpTable[dpid] = {}

    if packet.type == ethernet.LLDP_TYPE:
      return

    if isinstance(packet.next, ipv4):
      log.debug("%i %i IP %s => %s", dpid,inport,packet.next.srcip,packet.next.dstip)
      self.processWaitingPackets (dpid, packet.next.srcip, packet.src, inport)
      if packet.next.srcip in self.arpTable[dpid]:
        if self.arpTable[dpid][packet.next.srcip] != (inport, packet.src):
          log.info("%i %i RE-learned %s", dpid,inport,packet.next.srcip)
      else:
        log.debug("%i %i learned %s", dpid,inport,str(packet.next.srcip))
      self.arpTable[dpid][packet.next.srcip] = Entry(inport, packet.src)

      prt = 0
      dstaddr = packet.next.dstip

      prt = self.ip_to_port.get(dstaddr)

      if prt != None:
        log.debug("Got port %i from dictionary",prt)
        mac = None
        if dstaddr in self.arpTable[dpid]:
          mac = self.arpTable[dpid][dstaddr].mac
        if mac!=None:
          actions = []
          actions.append(of.ofp_action_dl_addr.set_dst(mac))
          actions.append(of.ofp_action_dl_addr.set_src(dpid_to_mac(dpid)))
          actions.append(of.ofp_action_output(port = prt))
          match = of.ofp_match.from_packet(packet, inport)
          match.dl_src = None

          msg = of.ofp_flow_mod(command=of.OFPFC_ADD,buffer_id=event.ofp.buffer_id,actions=actions,match=of.ofp_match.from_packet(packet,inport))
          event.connection.send(msg.pack())
          return
        else:
        # Add to tracked buffers
          if (dpid,dstaddr) not in self.buffer_queue:
            self.buffer_queue[(dpid,dstaddr)] = []
          bucket = self.buffer_queue[(dpid,dstaddr)]
          entry = (None,event.ofp.buffer_id,inport)
          bucket.append(entry)
          self._arp_request(packet,dstaddr,inport,dpid,event)

      else:
        icmpflag = 0
        for list in self.routing_table:
          if list[0] == str(dstaddr):
            log.debug("Searching Routing Table for IP address: %s ",str(dstaddr))
            prt = list[4]
          if list[3] == str(dstaddr):
            prt = list[4]
            icmpflag = 1
            self.arpTable[dpid][dstaddr] = Entry(prt, dpid_to_mac(dpid))

        if icmpflag == 1:
          icmp = pckt.icmp()
          icmp.type = pckt.TYPE_ECHO_REPLY
          icmp.payload = packet.find("icmp").payload

          # Make the IP packet around it
          ipp = pckt.ipv4()
          ipp.protocol = ipp.ICMP_PROTOCOL
          ipp.srcip = packet.find("ipv4").dstip
          ipp.dstip = packet.find("ipv4").srcip

          # Ethernet around that...
          e = pckt.ethernet()
          e.src = packet.dst
          e.dst = packet.src
          e.type = e.IP_TYPE

          # Hook them up...
          ipp.payload = icmp
          e.payload = ipp

          # Send it back to the input port
          msg = of.ofp_packet_out()
          msg.actions.append(of.ofp_action_output(port = of.OFPP_IN_PORT))
          msg.data = e.pack()
          msg.in_port = event.port
          event.connection.send(msg)
          return
        if prt == None:
          log.debug("Unreachable IP Address : %s",str(dstaddr))
          icmp = pckt.icmp()
          icmp.type = 3
          #icmp.payload = packet.find("icmp").payload

          # Make the IP packet around it
          ipp = pckt.ipv4()
          ipp.protocol = ipp.ICMP_PROTOCOL
          ipp.srcip = packet.find("ipv4").dstip
          ipp.dstip = packet.find("ipv4").srcip

          # Ethernet around that...
          e = pckt.ethernet()
          e.src = packet.dst
          e.dst = packet.src
          e.type = e.IP_TYPE

          # Hook them up...
          ipp.payload = icmp
          e.payload = ipp

          # Send it back to the input port
          msg = of.ofp_packet_out()
          msg.actions.append(of.ofp_action_output(port = of.OFPP_IN_PORT))
          msg.data = e.pack()
          msg.in_port = event.port
          event.connection.send(msg)
          return


        log.debug("Got Port Number : %s from routing table",prt)
        self.ip_to_port[dstaddr] = prt
        mac = None
        if dstaddr in self.arpTable[dpid]:
          mac = self.arpTable[dpid][dstaddr].mac
        if mac!=None:
          actions = []
          actions.append(of.ofp_action_dl_addr.set_dst(mac))
          actions.append(of.ofp_action_dl_addr.set_src(dpid_to_mac(dpid)))
          actions.append(of.ofp_action_output(port = prt))
          match = of.ofp_match.from_packet(packet, inport)
          match.dl_src = None # Wildcard source MAC

          msg = of.ofp_flow_mod(command=of.OFPFC_ADD,buffer_id=event.ofp.buffer_id,actions=actions,match=of.ofp_match.from_packet(packet,inport))
          event.connection.send(msg.pack())
          return
        else:
          if (dpid,dstaddr) not in self.buffer_queue:
            self.buffer_queue[(dpid,dstaddr)] = []
          bucket = self.buffer_queue[(dpid,dstaddr)]
          entry = (None,event.ofp.buffer_id,inport)
          bucket.append(entry)
          self._arp_request(packet,dstaddr,inport,dpid,event)

    elif isinstance(packet.next, arp):
      a = packet.next
      log.debug("%i %i ARP %s %s => %s", dpid, inport,
       {arp.REQUEST:"request",arp.REPLY:"reply"}.get(a.opcode,
       'op:%i' % (a.opcode,)), str(a.protosrc), str(a.protodst))

      dstaddr = a.protodst
      prt = None
      for list in self.routing_table:
        if list[0] == str(dstaddr):
          log.debug("Searching Routing Table for IP address: %s ",str(dstaddr))
          prt = list[4]
        if list[3] == str(dstaddr):
          prt = list[4]
          #icmpflag = 1
          self.arpTable[dpid][dstaddr] = Entry(prt, dpid_to_mac(dpid))

      if prt == None:
        log.debug("Unreachable IP Address : %s",str(dstaddr))
        return

      if a.prototype == arp.PROTO_TYPE_IP:
        if a.hwtype == arp.HW_TYPE_ETHERNET:
          if a.protosrc != 0:

            # Learn or update port/MAC info
            if a.protosrc in self.arpTable[dpid]:
              if self.arpTable[dpid][a.protosrc] != (inport, packet.src):
                log.info("%i %i RE-learned %s", dpid,inport,str(a.protosrc))
            else:
              log.debug("%i %i learned %s", dpid,inport,str(a.protosrc))
            self.arpTable[dpid][a.protosrc] = Entry(inport, packet.src)

            # Send any waiting packets...
            self.processWaitingPackets(dpid, a.protosrc, packet.src, inport)

            if a.opcode == arp.REQUEST:
              # Maybe we can answer

              if a.protodst in self.arpTable[dpid]:
                # We have an answer...
                self._arp_response(a,packet, inport, dpid,event)
                return
            if a.opcode == arp.REPLY:
              for list in self.routing_table:
                if list[0] == str(a.protodst):
                  log.debug("Searching Routing Table for IP address: %s ",str(a.protodst))
                  prt = list[4]
                  msg = of.ofp_packet_out(in_port = inport, action = of.ofp_action_output(port = prt))

      log.debug("%i %i flooding ARP %s %s => %s" % (dpid, inport,
       {arp.REQUEST:"request",arp.REPLY:"reply"}.get(a.opcode,
       'op:%i' % (a.opcode,)), str(a.protosrc), str(a.protodst)))

      msg = of.ofp_packet_out(in_port = inport, action = of.ofp_action_output(port = of.OFPP_FLOOD))
      if event.ofp.buffer_id is of.NO_BUFFER:
        # Try sending the (probably incomplete) raw data
        msg.data = event.data
      else:
        msg.buffer_id = event.ofp.buffer_id
      event.connection.send(msg.pack())


def launch ( a=None):
  a = str_to_bool(a)
  core.registerNew(router, a)

