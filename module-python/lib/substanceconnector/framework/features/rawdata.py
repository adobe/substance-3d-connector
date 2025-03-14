""" Application type to Handle sending raw data strings between connections"""

import uuid
import json
import jsonschema
import os.path

from ..instance import ConnectorInstance
from ..application import BaseApplication

class RawDataApplication(BaseApplication):
    """ Handles all sending and recieving of raw data """
    _RAW_DATA_UUID = uuid.UUID('7276449c-5220-4e21-b07f-f86d169cc6de')

    @classmethod
    def get_callback_list(cls):
        """ Returns the callback list for raw data messages """
        return [(cls._RAW_DATA_UUID, cls.recv_raw_data)]\
            + super().get_callback_list()

    @classmethod
    def recv_raw_data(cls, context, message_type, message):
        """ load the raw data message schema """
        print(message)

    @classmethod
    def send_raw_data(cls, context, data_string):
        ConnectorInstance.write_message(
            context, cls._RAW_DATA_UUID, data_string)
        return True

    @classmethod
    def get_feature_ids(cls):
        """ Method called at application registration to return the feature
         id set for the application to build the connection context """
        return [
            cls._RAW_DATA_UUID
        ]
