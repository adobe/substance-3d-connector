""" Application type to Handle all importing of various file types, such
    as sbsar, sbs and sbsprs files."""

import uuid
import json
import jsonschema
import os.path

from ..instance import ConnectorInstance
from ..application import BaseApplication


SBSARIMPORTSCHEMA = {
    "type": "object",
    "properties": {
        "path": {"type": "string"},
        "name": {"type": "string"},
        "uuid": {"type": "string"},
        "type": {"type": "string"},
        "take_file_ownership": {"type": "boolean"},
    },
    "required": ["path", "type", "uuid"],
}

class SBSARImportMessageSchema:
    """ Message schema data structure """
    def __init__(self, json_object):
        self.path = json_object.get("path", None)
        self.name = json_object.get("name", "")
        self.uuid = json_object.get("id", "")
        if self.uuid == "":
            self.uuid = str(uuid.uuid4())
        self.type = json_object.get("type", "material")
        self.take_file_ownership = json_object.get("take_file_ownership", False)
        self.is_valid()

    def to_json(self):
        """ Returns a json string of this object"""
        return json.dumps(self, default=lambda o: o.__dict__, sort_keys=True,
                          indent=4)

    @classmethod
    def from_string(self, string):
        json_filler = json.loads(string)
        #None must be passed to required fields for validation failures
        return SBSARImportMessageSchema(json_filler.get("path", None), json_filler.get("id", ""), json_filler.get("name", ""), json_filler.get("type", None))

    def is_valid(self):
        """ Returns a boolean if the json is valid """
        try:
            json_filler = json.loads(self.to_json())
            jsonschema.validate(instance=json_filler, schema=SBSARIMPORTSCHEMA)
            return True
        except jsonschema.exceptions.ValidationError as err:
            print("Json error: {}".format(err))

        return False


class SBSARImportApplication(BaseApplication):
    """ Handles all importing and exporting of substance asset files """
    _IMPORT_SBSAR_UUID = uuid.UUID('91e3dfbc-80b8-4b1a-92d5-63ec09ac641a')
    _UPDATE_SBSAR_UUID = uuid.UUID('1643e0bf-7344-4314-9cbf-6107e30e5ed7')

    @classmethod
    def get_callback_list(cls):
        """ Returns the callback list for substance file imports """
        return [(cls._IMPORT_SBSAR_UUID, cls.recv_import_sbsar)]\
            + super().get_callback_list()

    @classmethod
    def recv_import_sbsar(cls, context, message_type, message):
        """ Import the asset and parse the schema"""
        json.loads(message)
        print(message)

    @classmethod
    def send_import_sbsar(cls, context, schema_instance):
        """ Send an sbs/sbsar file to another application """
        if not isinstance(schema_instance, SBSARImportMessageSchema):
            print("Invalid SBSARImportMessageSchema object parameter")
            return False
        if not os.path.isfile(schema_instance.path):
            print("Send asset requires a valid file path.")
            return False
        if schema_instance.is_valid() is False:
            print("Failed to send connector message, schema is not valid.")
            return False

        print("\n")
        print(schema_instance.to_json())
        print("\n")

        ConnectorInstance.write_message(
            context, cls._IMPORT_SBSAR_UUID, schema_instance.to_json())
        return True
    
    @classmethod
    def send_import_sbsar_string(cls, context, string):
        schema_instance = SBSARImportMessageSchema.from_string(string)
        return SBSARImportApplication.send_import_sbsar(cls, context, schema_instance)

    @classmethod
    def get_feature_ids(cls):
        """ Method called at application registration to return the feature
         id set for the application to build the connection context """
        return [
            cls._IMPORT_SBSAR_UUID
        ]
