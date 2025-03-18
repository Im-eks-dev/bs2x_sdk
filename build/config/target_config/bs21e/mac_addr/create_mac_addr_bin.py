#!/usr/bin/env python3
# coding=utf-8
import subprocess
from ctypes import *
import csv
import struct
import os
import sys
from sys import version_info
import configparser

file_dir = os.path.dirname(os.path.realpath(__file__))
g_root = os.path.realpath(os.path.join(file_dir, "..", "..", "..", "..", ".."))
sys.path.append(os.path.join(g_root, 'tools', 'pkg'))

from packet_create import packet_bin, update_bin

def create_mac_addr_bin(addr, path):
    bin_path = path
    cur_addr = str(hex(addr))[2:]
    addr_list = []
    for i in range(6):
        tmp = int(cur_addr[i * 2 : (i + 1) * 2], 16)
        addr_list.append(tmp)
    with open(bin_path, "wb+") as mac:
        for i in addr_list:
            value = struct.pack("B", i)
            mac.write(value)
        mac.close()

def packet_all_bin(source_path, update_bin_path, addr, size):
    update_bin_bx = update_bin_path + "|" + addr + "|" + size + "|1"
    packet_post_agvs = list()
    packet_post_agvs.append(update_bin_bx)
    update_bin(source_path, packet_post_agvs)

if __name__ == "__main__":
    # 需确保mac_addr.bin烧录地址不与其它分区重叠，具体参考分区表
    if len(sys.argv) > 2:
        print("Param error!")
        sys.exit(0)
    target = sys.argv[1]
    conf = configparser.RawConfigParser()
    conf.optionxform = lambda option: option
    config_path = os.path.join(file_dir, 'config.ini')
    bin_path = os.path.join(g_root,  'interim_binary', 'bs21e', 'bin', 'mac_bin', target)
    mac_bin_path = os.path.join(bin_path, 'mac_addr.bin')
    source_fwpkg_path = os.path.join(g_root,  'output', 'bs21e', 'fwpkg', target, 'bs21e_all_in_one.fwpkg')
    conf.read(config_path, encoding='UTF-8')
    os.makedirs(bin_path, exist_ok=True)

    mac_addr = conf['MacAddrConfig']['MAC_ADDR']
    load_addr = conf['PartitionConfig']['LOAD_START_ADDR']
    load_size = conf['PartitionConfig']['LOAD_SIZE']
    print("Mac address: ", mac_addr)
    print("Load address: ", load_addr)
    print("Load size: ", load_size)
    cur_addr = int(mac_addr, 16)
    create_mac_addr_bin(cur_addr, mac_bin_path)
    packet_all_bin(source_fwpkg_path, mac_bin_path, load_addr, load_size)