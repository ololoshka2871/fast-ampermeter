#!/usr/bin/env python
# -*- coding: utf-8 -*-

import pytest
import protocol_pb2
import libfast_ampermeter
import os

def print_help():
    print("Возможно вы пытаитесь запустить этот файл напрямую?\n"
    "Запстите: $ PORT=/dev/ttyACMx make pb_basic_test.run\n")


@pytest.fixture
def device(request):
    if not ('PORT' in os.environ.keys()):
        print("Не указан COM-порт для теста!\n")
        print_help()
        assert 0

    d = libfast_ampermeter.Fast_ampermater_io(os.environ['PORT'])

    def fin():
        d.disconnect()

    request.addfinalizer(fin)
    return d

def basic_check(resp):
    assert resp is not None
    assert resp.deviceID == protocol_pb2.INFO.FAST_AMPERMETER_ID
    assert resp.Global_status == protocol_pb2.STATUS.OK


def test_ping(device):
    request = libfast_ampermeter.Fast_ampermeter_requestBuilder.build_ping_request()

    resp = device.process_request_sync(request)

    basic_check(resp)


def test_read_last_measure(device):
    request = libfast_ampermeter.Fast_ampermeter_requestBuilder.build_last_measure_request()

    resp = device.process_request_sync(request)
    basic_check(resp)

    assert resp.HasField('lastMeasure')
    lm = resp.lastMeasure
    assert lm.number > 0


def test_read_all_fistory(device):
    request = libfast_ampermeter.Fast_ampermeter_requestBuilder.build_measure_history_request()

    r = protocol_pb2.Request()
    r.ParseFromString(request.SerializeToString())

    resp = device.process_request_sync(request)
    basic_check(resp)

    assert resp.HasField('measureHistory')
