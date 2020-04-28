#!/usr/bin/env python
# -*- coding: utf-8 -*-


import argparse
import libfast_ampermeter
import protocol_pb2
from matplotlib.lines import Line2D
import matplotlib.pyplot as plt
import matplotlib.animation as animation

class Scope(object):
    maxpoints = 1000

    def __init__(self, ax):
        self.ax = ax
        self.tdata = []
        self.v_data = []
        self.I_data = []
        self.voltage_line = Line2D(self.tdata, self.v_data, color='r')
        self.current_line = Line2D(self.tdata, self.I_data, color='b')

        self.ax2 = ax.twinx()

        self.ax.add_line(self.voltage_line)
        self.ax2.add_line(self.current_line)
        self.ax.autoscale_view(True,True,True)
        self.ax2.autoscale_view(True, True, True)

        self.points_count = 0
        self.start_point = 0

    def update(self, v):
        he = v.HistoryElements

        if self.points_count > Scope.maxpoints - len(he):
            to_remove = self.points_count - Scope.maxpoints + len(he)
            self.points_count = Scope.maxpoints
            self.tdata = self.tdata[to_remove:]
            self.v_data = self.v_data[to_remove:]
            self.I_data = self.I_data[to_remove:]
        else:
            self.points_count += len(he)


        for item in he:
            self.tdata.append(item.number)
            self.v_data.append(item.voltage)
            self.I_data.append(item.current)

        self.start_point = he[-1].number

        self.voltage_line.set_data(self.tdata, self.v_data)
        self.current_line.set_data(self.tdata, self.I_data)

        self.ax.relim()
        self.ax.autoscale_view(True,True,True)

        self.ax2.relim()
        self.ax2.autoscale_view(True, True, True)

        return self.voltage_line, self.current_line


def reader(device, scope):
    def _rdr():
        while True:
            request = libfast_ampermeter.Fast_ampermeter_requestBuilder.build_measure_history_request(scope.start_point)

            try:
                response = device.process_request_sync(request)
            except:
                continue

            if (not response) or response.Global_status != protocol_pb2.STATUS.OK:
                raise RuntimeError('Error {} during read values'.format(response.Global_status))

            yield response.measureHistory

    return _rdr


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--history-size', '-s', type=int, help="размер истории", default=32)
    parser.add_argument('--period', '-d', type=int, help="Период опроса в девайсе [мс]", default=1)
    parser.add_argument('--port', '-p', type=str, help='Serial port', default='/dev/ttyACM0')

    args = parser.parse_args()

    device = libfast_ampermeter.Fast_ampermater_io(args.port, (args.history_size * args.period / 2) / 1000.0)

    fig, ax = plt.subplots()
    scope = Scope(ax)

    ani = animation.FuncAnimation(fig, scope.update, reader(device, scope),
                                  interval=args.history_size * args.period / 2,
                                  blit=False)
    plt.show()


# чтобы при импорте не выполнялся код автоматом
if __name__ == '__main__':
    main()
