#!/usr/bin/env python

import sys
import hid
import struct

vendor_id = 0x0483
product_id = 0x5750

usage_page = 0xFF00
usage = 0x1


def get_raw_hid_interface():
    device_interfaces = hid.enumerate(vendor_id, product_id)
    raw_hid_interfaces = [i for i in device_interfaces if i['usage_page'] == usage_page and i['usage'] == usage]

    if len(raw_hid_interfaces) == 0:
        raise IOError('No device found')

    interface = hid.device()
    interface.open_path(raw_hid_interfaces[0]['path'])

    # print("Manufacturer: %s" % interface.get_manufacturer_string())
    # print("Product: %s" % interface.get_product_string())

    return interface


def send_raw_packet(data):
    interface = get_raw_hid_interface()

    if interface is None:
        print("No device found")
        sys.exit(1)

    request_packet = [0x00] * 33 # First byte is Report ID
    request_packet[1:len(data) + 1] = data

    print("Request:")
    print(request_packet)

    try:
        interface.write(request_packet)

        response_packet = interface.read(32, timeout_ms=1000)

        print("Response:")
        print(response_packet)
    finally:
        interface.close()


def read_data(interface):

    r = 0
    while True:
        d = interface.read(32)
        if d:
            res = struct.unpack('<ffff', bytearray(d))
            print(';'.join(map(str, res)))


def main():
    interface = get_raw_hid_interface()
    read_data(interface)


if __name__ == '__main__':
    main()
