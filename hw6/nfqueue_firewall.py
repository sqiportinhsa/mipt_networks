import argparse
import socket

from netfilterqueue import NetfilterQueue
from scapy.all import *

class Filter:
    flags = {'QR', 'name', 'type', 'class', 'ttl', 'length'}
    
    def __init__(self):
        self.default_drop = bool
        self.rules = list()
    
    def add_rule(self, rule : dict) -> None:
        self.rules.append(rule)

    def set_default(self, drop : bool) -> None:
        self.default_drop = drop

    def match(self, packet, rule) -> bool:
        for key in packet.keys():
            if key in rule.keys():
                if key == 'name':
                    if not (packet[key].find(rule[key]) != -1):
                        return False
                elif not rule[key] == packet[key]:
                    return False
        
        return True
                
    def drop(self, packet) -> bool:
        for rule in self.rules:
            if self.match(packet, rule): 
                print(f"Match with rule {rule}")
                return rule['drop']
        
        print("No match with rules, use default.")
        return self.default_drop
    
    def print_rules(self):
        print(f"default drop: {self.default_drop}")
        print(f"rules: {self.rules}")

    def apply(self, packet):
        scp_packet = IP(packet.get_payload())

        if not scp_packet.haslayer(DNS):
            print("Got not DNS package, skip.")
            packet.accept()
        else:
            packet = get_dns_packet(scp_packet)
            print("Got DNS packet:")
            print_packet(packet)

            if self.drop(packet):
                print("Drop after filter\n")
                packet.drop()
            else:
                print("Pass after filter\n")        
                packet.accept()


def get_filter_from_file(path : str) -> Filter:
    with open(path, 'r') as file:
        first_line = True
        rules_list = Filter()

        for line in file:
            if len(line) == 0:
                continue

            if first_line:
                first_line = False
                if line.find("default") == -1:
                    raise Exception('Default action with a packet must be specified')
                
                default_drop = bool(int(line[line.find("default") + len("default=")]))
                rules_list.set_default(default_drop)
                continue

            rule = dict()
            rule['drop'] = not default_drop

            flags = line.split(sep=" ")
            for flag in flags:
                flag_info = flag.split(sep="=")
                if len(flag_info) != 2:
                    raise Exception(f'flag must be in format \'flag=value\', got: {flag}')
                    
                if flag_info[0] not in Filter.flags and flag_info[0] != "drop":
                    raise Exception(f'Unknown flag: {flag_info[0]}')
                
                if flag_info[0] == 'name':
                    rule['name'] = flag_info[1]
                elif flag_info[0] == 'drop':
                    rule['drop'] = bool(int(flag_info[1]))
                else:
                    rule[flag_info[0]] = int(flag_info[1])

            rules_list.add_rule(rule)    
        
    return rules_list



def print_packet(packet) -> None:
    for key, val in packet.items():
        print(f"{key}: {val}")
    
def get_dns_packet(packet) -> dict:
    if packet.haslayer(DNSRR):
        rr = packet.getlayer(DNSRR)
        return {"QR"    : 1,
                "name"  : rr.get_field('rrname').i2repr(rr, rr.rrname),
                "type"  : rr.type, 
                "class" : rr.rclass, 
                "ttl"   : rr.ttl,
                "length"   : rr.rdlen}
    
    elif packet.haslayer(DNSQR):
        qr = packet.getlayer(DNSQR)
        return { "QR"    : 0,
                 "name"  : qr.get_field('qname').i2repr(qr, qr.qname),
                 "type"  : qr.qtype,
                 "class" : qr.qclass}
        
def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("rules_path", type=str)
    parser.add_argument("queue_num", type=int)
    return parser.parse_args()

def main(args):
    filter = get_filter_from_file(args.rules_path)
    
    nfqueue = NetfilterQueue()
    
    nfqueue.bind(int(args.queue_num), filter.apply)
    sock = socket.fromfd(nfqueue.get_fd(), socket.AF_UNIX, socket.SOCK_STREAM)
    
    try:
        nfqueue.run_socket(sock)
    except KeyboardInterrupt:
        pass

    sock.close()
    nfqueue.unbind()

if __name__ == "__main__":
    args = parse_args()
    main(args)
