import re
import os
import sys
import struct

def is_bs2x(target_name):
    keywords = {
        'bs21a': 0xE0,
        'bs21e': 0xD0,
        'bs20': 0xE,
        'bs21': 0xD,
        'bs22': 0xB,
        'bs26': 0x7
    }
    for key in keywords:
        if key in target_name:
            print(target_name)
            return keywords[key]
    return 0xFF

def get_user_version(output_path):
    menuconfig_path = os.path.join(output_path, f"mconfig.h")
    user_version = 0
    if os.path.exists(menuconfig_path):
        with open(menuconfig_path, 'r') as file:
            lines = file.readlines()
        for line in lines:
            if line.startswith('#define CONFIG_BS2X_USER_PRODUCT_ID'):
                user_version = line.split()[2]
                return user_version

def insert_key_version(target_name, output_path, sign_path):
    bs2x_target = is_bs2x(target_name)
    if bs2x_target == 0xFF:
        return
    user_version = get_user_version(output_path)
    final_version = (hex((bs2x_target))[2:].zfill(2)) + (user_version[2:].zfill(6))
    if os.path.exists(sign_path):
        with open(sign_path, 'r') as file:
            content = file.read()
        content = re.sub(r'(KeyVersion=0x)[0-9A-Fa-f]+', r'\g<1>' + final_version, content)
        with open(sign_path, 'w')as file:
            file.write(content)

def insert_key_version_fota(target_name, output_path):
    bs2x_target = is_bs2x(target_name)
    if bs2x_target == 0xFF:
        return
    fota_path = os.path.join(output_path, f"fota.fwpkg")
    user_version_int = int(get_user_version(output_path), 16)
    final_version_int = (bs2x_target << 24) | user_version_int
    with open(fota_path, 'r+b') as f:
        f.seek(0x40)
        f.write(struct.pack("I", final_version_int))
        f.flush()

arg1 = sys.argv[1]
arg2 = sys.argv[2]
arg3 = sys.argv[3]
if arg1 == 'insert_key_version_fota':
    insert_key_version_fota(arg2, arg3)
if arg1 == 'insert_key_version':
    arg4 = sys.argv[4]
    insert_key_version(arg2, arg3, arg4)