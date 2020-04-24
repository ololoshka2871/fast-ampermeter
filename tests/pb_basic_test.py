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

def test_sdfsd(device):
    request = libfast_ampermeter.Fast_ampermeter_requestBuilder.build_ping_request()

    resp = device.process_request_sync(request)

    assert resp is not None
    assert resp.deviceID == protocol_pb2.INFO.FAST_AMPERMETER_ID
