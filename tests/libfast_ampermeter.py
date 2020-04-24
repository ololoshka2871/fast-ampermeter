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
        req.protocolVersion = protocol_pb2.INFO.Value('PROTOCOL_VERSION')
        req.deviceID = protocol_pb2.INFO.Value('ID_DISCOVER')
        return req

    @staticmethod
    def build_ping_request():
        """
        Создаёт запрос проверки соединения

        :return: объект типа protocol_pb2.Request
        """
        return Fast_ampermeter_requestBuilder.build_request()



class Fast_ampermater_io:
    """Класс для простого доступа к Fast-ampermeter использованием google protocol buffers"""
    def __init__(self, port):
        """
        Конструктор

        :param port: COM-Порт к которому будет произведено подключение
        """
        self.port = port
        self.base_timeout = 1  # сек

        self.ser = serial.Serial(port)
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

    def process_request_sync(self, request, timeout_sec=1):
        """
        Синхронный обработчик запроса (блокирет вызвавший поток до получения ответа или до истечения таймаута)

        :param request: объект типа protocol_pb2.Request
        :param timeout_sec: Таймаут ожидания ответа от устройства
        :return:
        """
        if not (type(request) is protocol_pb2.Request):
            raise TypeError('"request" mast be instance of "protocol_pb2.Request"')

        return self.process_request_common(request, timeout_sec)

    def process_request_common(self, request, timeout_sec):
        self.ser.timeout = timeout_sec

        m = chr(protocol_pb2.INFO.MAGICK).encode()

        self.ser.write(m)
        self.ser.write(_VarintBytes(request.ByteSize()))
        self.ser.write(request.SerializeToString())


        response = protocol_pb2.Response()

        resp = self.ser.read(1024)
        try:
            _m, new_pos = _DecodeVarint32(resp, 0)

            if _m != protocol_pb2.INFO.MAGICK:
                return None

            msg_len, msg_offset = _DecodeVarint32(resp, new_pos)

            resp = resp[msg_offset:msg_offset+msg_len]
            response.ParseFromString(resp)
        except Exception:
            raise TimeoutError('Timeout')

        if request.id != response.id:
            return None

        return response


