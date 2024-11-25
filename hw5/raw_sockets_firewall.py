import argparse
import dpkt
import os
import socket

from socket import AF_INET, AF_PACKET, ETH_P_ALL, SOCK_RAW

Buffer_size = 1024

class Filter:
    flags = {'protocol', 'src_ip', 'dst_ip', 'src_port', 'dst_port'}

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
                if not rule[key] == packet[key]:
                    return False
        
        return True
                
    def drop(self, pkt) -> bool:
        for rule in self.rules:
            if self.match(pkt, rule): 
                print(f"Match with rule {rule}")
                return rule['drop']
        
        print("No match with rules, use default.")
        return self.default_drop
    
    def apply(self, input : socket, output : socket) -> bool:
        data = input.recv(Buffer_size)

        eth = dpkt.ethernet.Ethernet(data)
        if not isinstance(eth.data, dpkt.ip.IP):
            print("Got not tcp/udp/icmp packet, pass.\n");
        else:
            pkt = packet_info(eth)
            print("Got packet to filter:")
            print_packet(pkt)

            if not self.drop(pkt):
                print("Pass after filter.\n")
            else:
                print("Drop after filter.\n")
                return False

        output.sendall(data)
        return True
    
    def print_rules(self):
        print(f"default drop: {self.default_drop}")
        print(f"rules: {self.rules}")


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
                
                if flag_info[0] == 'dst_port' or flag_info[0] == 'src_port':
                    rule[flag_info[0]] = socket.htons(int(flag_info[1]))
                elif flag_info[0] == 'drop':
                    rule['drop'] = bool(int(flag_info[1]))
                else:
                    rule[flag_info[0]] = flag_info[1]

            rules_list.add_rule(rule)    
        
    return rules_list    

def create_socket(interface : str) -> socket:
    sock = socket.socket(AF_PACKET, SOCK_RAW)
    sock.bind((interface, ETH_P_ALL))
    return sock

def print_packet(packet) -> None:
    for key, val in packet.items():
        print(f"\t{key}: {val}")

def set_tcp_udp_packet(protocol : str, packet):
    return {'protocol' : protocol, 
            'src_ip'   : str(socket.inet_ntop(AF_INET, packet.src)), 
            'src_port' : socket.ntohs(packet.data.sport),
            'dst_ip'   : str(socket.inet_ntop(AF_INET, packet.dst)),
            'dst_port' : socket.ntohs(packet.data.dport)}

        
def packet_info(iface : dpkt.ethernet.Ethernet) -> dict:
    ip = iface.data
    
    if isinstance(ip.data, dpkt.udp.UDP):
        packet = set_tcp_udp_packet("udp", ip)
        return packet
        
    if isinstance(ip.data, dpkt.tcp.TCP):
        packet = set_tcp_udp_packet("tcp", ip)
        return packet
        
    if isinstance(ip.data, dpkt.icmp.ICMP):
        packet = {'protocol' : 'icmp', 
                  'src_ip'   : str(socket.inet_ntop(AF_INET, ip.src)), 
                  'dst_ip'   : str(socket.inet_ntop(AF_INET, ip.dst))}
        return packet

    raise Exception(f"Unknown instance {ip.data}")  


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("rules_path", type=str, help="Path of file with rules")
    parser.add_argument("interface1", type=str, help="First interface number")
    parser.add_argument("interface2", type=str, help="Second interface number")
    return parser.parse_args()
        

def main():
    args = parse_args()

    filter = get_filter_from_file(args.rules_path)
    filter.print_rules()
    
    input  = create_socket(args.interface1)
    output = create_socket(args.interface2)
    
    if os.fork():
        while(True):
            filter.apply(input, output)
    else:
        while(True):
            filter.apply(output, input)

if __name__ == "__main__":
    main()