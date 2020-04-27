# -*- coding: utf-8 -*-

import protocol_pb2
import random
import serial
from google.protobuf.internal.encoder import _VarintBytes
from google.protobuf.internal.decoder import _DecodeVarint32


class TimeoutError(RuntimeError):
    pass


class Fast_ampermeter_requestBuilder:
    @staticmethod
    def build_request():
        """
        Создаёт заготовку запроса

        :return: объект типа protocol_pb2.Request c заполнениыми полями id и version
        """
        req = protocol_pb2.Request()
        req.id = random.randrange(0xffffffff)
        req.protocolVersion = protocol_pb2.INFO.PROTOCOL_VERSION
        req.deviceID = protocol_pb2.INFO.ID_DISCOVER
        return req

    @staticmethod
    def build_ping_request():
        """
        Создаёт запрос проверки соединения

        :return: объект типа protocol_pb2.Request
        """
        return Fast_ampermeter_requestBuilder.build_request()

    @staticmethod
    def build_last_measure_request():
        """
        Создаёт запрос чтения последнего измерения

        :return: объект типа protocol_pb2.Request
        """
        req = Fast_ampermeter_requestBuilder.build_request()

        req.lastMeasureRequest.CopyFrom(protocol_pb2.LastMeasureRequest())

        return req

    @staticmethod
    def build_measure_history_request(start=None, max_count=None):
        """
        Создаёт запрос чтения истории измерений

        :return: объект типа protocol_pb2.Request
        """
        req = Fast_ampermeter_requestBuilder.build_request()

        rq = protocol_pb2.GetMeasureHistory()

        if start:
            rq.from_element = start

        if max_count:
            rq.count = max_count

        req.getMeasureHistory.CopyFrom(rq)

        return req



class Fast_ampermater_io:
    """Класс для простого доступа к Fast-ampermeter использованием google protocol buffers"""
    def __init__(self, port, timeout=1):
        """
        Конструктор

        :param port: COM-Порт к которому будет произведено подключение
        """
        self.port = port
        self.base_timeout = timeout  # сек

        self.ser = serial.Serial(port, timeout=timeout, write_timeout=timeout)
        self.isConnected = True

    def __str__(self):
        """
        Выводит краткую информацию о состоянии драйвера

        :return: Строка с краткой информацией ос состоянии устройства и соединения
        """
        return 'Fast ampermeter на порту "{}"'.format(self.port)

    def connect(self):
        """
        Инициирует подключение к устройству

        :return: None
        """

        self.ser.open()

        self.isConnected = True

    def disconnect(self):
        """
        Инициирует отключение от устройства

        :return: None
        """
        if not self.isConnected:
            return

        self.ser.close()

    def process_request_sync(self, request, timeout_sec=-1):
        """
        Синхронный обработчик запроса (блокирет вызвавший поток до получения ответа или до истечения таймаута)

        :param request: объект типа protocol_pb2.Request
        :param timeout_sec: Таймаут ожидания ответа от устройства
        :return:
        """
        if not (type(request) is protocol_pb2.Request):
            raise TypeError('"request" mast be instance of "protocol_pb2.Request"')

        return self.process_request_common(request, timeout_sec if timeout_sec > 0 else self.base_timeout)

    def process_request_common(self, request, timeout_sec):
        self.ser.timeout = timeout_sec

        m = chr(protocol_pb2.INFO.MAGICK).encode()

        self.ser.write(m)
        self.ser.write(_VarintBytes(request.ByteSize()))
        self.ser.write(request.SerializeToString())


        response = protocol_pb2.Response()

        resp = self.ser.read(1024)
        if len(resp) < 5:
            raise RuntimeError('Empty response')

        _m, new_pos = _DecodeVarint32(resp, 0)

        if _m != protocol_pb2.INFO.MAGICK:
            return RuntimeError('Protocol error')

        msg_len, msg_offset = _DecodeVarint32(resp, new_pos)

        resp = resp[msg_offset:msg_offset+msg_len]
        response.ParseFromString(resp)

        if request.id != response.id:
            raise RuntimeError('Message sequence corrupt')

        return response


