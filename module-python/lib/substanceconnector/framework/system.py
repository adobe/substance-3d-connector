""" Defines the basic, functioning system application module for interacting
    with Substance Connector """

import uuid
import logging
import json

from .application import BaseApplication
from substanceconnector.framework.instance import ConnectorInstance
from substanceconnector.framework.connectionschema import ConnectionSchema

class SystemApplication(BaseApplication):
    """ Application type for handling all system defined messages, such as
        connections being established or closed. """
    _CONNECTION_ESTABLISHED_UUID = uuid.UUID('02572bc5-2d84-450a-9e01-d22c66b1abb1')
    _CONNECTION_CLOSED_UUID = uuid.UUID('04705ddf-16d4-4489-af6c-6e3a93f1959d')
    _CONNECTION_UPDATE_CONTEXT_UUID = uuid.UUID('39b07a67-4996-4b62-a340-908a81f66562')
    _application_map = {}

    @classmethod
    def get_callback_list(cls):
        """ Returns the callback list, including the callbacks for when
            a connection is opened and when one is closed """
        return [
            (cls._CONNECTION_ESTABLISHED_UUID, cls.recv_connection_established),
            (cls._CONNECTION_CLOSED_UUID, cls.recv_connection_closed),
            (cls._CONNECTION_UPDATE_CONTEXT_UUID, cls.recv_context),
        ]

    @classmethod
    def get_feature_ids(cls):
        """ Method called at application registration to return the feature
         id set for the application to build the connection context """
        return [
            cls._CONNECTION_ESTABLISHED_UUID,
            cls._CONNECTION_CLOSED_UUID,
            cls._CONNECTION_UPDATE_CONTEXT_UUID
        ]

    @classmethod
    def get_application_map(cls):
        """ Acquires the current application map """
        return cls._application_map

    @classmethod
    def post_shutdown(cls):
        """ Post shutdown override - Clears the application map """
        cls._application_map = {}

    @classmethod
    def recv_connection_established(cls, context, message_type, message):
        """ Callback for handling incoming connection """
        if message_type == cls._CONNECTION_ESTABLISHED_UUID:
            cls._application_map[message] = {"context": context, "schema": {}}
            cls.send_context(context)

    @classmethod
    def recv_connection_closed(cls, context, message_type, message):
        """ Callback for when connections are closed """
        if message_type == cls._CONNECTION_CLOSED_UUID:
            cls._application_map.pop(message)

    @classmethod
    def send_context(cls, context):
        """ Callback for handling incoming connection """
        try:
            currentSchema = ConnectorInstance.get_connection_context()
            ConnectorInstance.write_message(context, cls._CONNECTION_UPDATE_CONTEXT_UUID, currentSchema.to_json())
        except Exception as error:
            print(error)

    @classmethod
    def recv_context(cls, context, message_type, message):
        try:
            """ Callback for handling incoming connection """
            if message_type == cls._CONNECTION_UPDATE_CONTEXT_UUID:
                message_object = json.loads(message)
                message_as_schema = ConnectionSchema(message_object["display_name"],
                                                    message_object["id_name"],
                                                    message_object["connector_version"],
                                                    message_object["available_features"])
                cls._application_map[message_as_schema.id_name]["schema"] = message_as_schema
        except Exception as error:
            print(error)
        
            