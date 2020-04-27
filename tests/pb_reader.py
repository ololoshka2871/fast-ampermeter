#!/usr/bin/env python
# -*- coding: utf-8 -*-


import argparse
import os
import sys
import time
import libfast_ampermeter
import protocol_pb2


def print_hader(stream):
    stream.write('Номер измерения;Время обработки запроса[мс];Нарпяжение[В];Ток[A]\n')


def gen_pattern():
    res = '{number};{1};{voltage};{current}'
    return res


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--interval', '-i', type=float, help="Интервал опроса [c]", default=0.01)
    parser.add_argument('--port', '-p', type=str, help='Serial port', default='/dev/ttyACM0')

    args = parser.parse_args()

    device = libfast_ampermeter.Fast_ampermater_io(args.port, args.interval)

    request = libfast_ampermeter.Fast_ampermeter_requestBuilder.build_last_measure_request()

    print_hader(sys.stdout)
    pattern = gen_pattern()

    while True:


        try:
            start = time.time()
            response = device.process_request_sync(request)
            req_time = time.time()
        except:
            continue


        if (not response) or response.Global_status != protocol_pb2.STATUS.Value('OK'):
            raise RuntimeError('Error {} during read values'.format(response.Global_status))

        res = {
            'number' : response.lastMeasure.number,
            'voltage': response.lastMeasure.voltage,
            'current': response.lastMeasure.current,
        }

        pr_time = time.time()
        print(pattern.format(int(pr_time * 1000),
                             int((req_time - start) * 1000),
                             **res))

        # sleep if needed
        processed = time.time() - start
        if processed < args.interval:
            time.sleep(args.interval - processed)

# чтобы при импорте не выполнялся код автоматом
if __name__ == '__main__':
    main()
