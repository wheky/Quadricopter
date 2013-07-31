#!/bin/bash
ip addr add 10.0.0.2/24 dev enp0s25
ip link set up dev enp0s25

sysctl -w net.ipv4.ip_forward=1
iptables -t nat -A POSTROUTING -o wlo1 -j MASQUERADE
iptables-save > /etc/iptables/iptables.rules
systemctl start iptables

