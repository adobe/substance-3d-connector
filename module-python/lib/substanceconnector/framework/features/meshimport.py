"""
Copyright 2023 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
"""

""" Application type to handle all importing of mesh file types """

import uuid
import json
import jsonschema
from enum import Enum

from ..application import BaseApplication
from ..instance import ConnectorInstance

class FileFormat(Enum):
    fbx = 0
    glb = 1
    gltf = 2
    igs = 3
    jt = 4
    obj = 5
    step = 6
    stl = 7
    stp = 8
    usd = 9
    usda = 10
    usdz = 11
    usdc = 12

class ColorFormat(Enum):
    NoColorFormat = 0
    Vertex = 1
    Texture = 2

class ColorEncoding(Enum):
	Linear = 0
	SRGB = 1

class AxisConvention(Enum):
    ForceYUp_XRight_mZForward = 0
    ForceZUp_XRight_YForward = 1

class Unit(Enum):
    Millimeters = 0
    Centimeters = 1
    Meters = 2
    Kilometers = 3
    Inches = 4
    Feet = 5
    Yards = 6
    Miles = 7

class ExportTopology(Enum):
    Triangles = 0
    Quads = 1
print([fileformat.name for fileformat in FileFormat])

MESH_IMPORT_SCHEMA = {
    "type": "object",
    "properties": {
        "assetName": {"type": "string"},
        "assetUuid": {"type": "string"},
        "supportedFormats": {"type": "array", "items": {"type": "string", "enum": [fileformat.name for fileformat in FileFormat]}},
        "colorFormat": {"type": "string", "enum": [colorformat.name for colorformat in ColorFormat]},
        "colorEncoding": {"type": "string", "enum": [colorencoding.value for colorencoding in ColorEncoding]},
        "allowNegativeTransforms": {"type": "boolean"},
        "allowInstances": {"type": "boolean"},
        "flattenHierarchy": {"type": "boolean"},
        "axisConvention": {"type": "string", "enum": [axisconvention.name for axisconvention in AxisConvention]},
        "unit": {"type": "string", "enum": [unit.name for unit in Unit]},
        "topology": {"type": "string", "enum": [exporttopology.name for exporttopology in ExportTopology]},
        # "targetFaceCount": {"type": "integer"},
        "requestUv": {"type": "boolean"},
        "enableUdims": {"type": "boolean"},
    },
    "required": ["assetName"],
}

class MeshImportMessageSchema:
    """ Message schema data structure """
    def __init__(self, json_object):
        try:
            self.assetName = json_object.get("assetName", None)
            self.assetUuid = json_object.get("assetUuid")
            if json_object.get("supportedFormats") != None:
                self.supportedFormats = [FileFormat[formatinput].name for formatinput in json_object.get("supportedFormats")]
            self.colorFormat = ColorFormat[json_object.get("colorFormat")].name
            self.colorEncoding = ColorEncoding[json_object.get("colorEncoding")].name
            self.allowNegativeTransforms = json_object.get("allowNegativeTransforms")
            self.allowInstances = json_object.get("allowInstances")
            self.flattenHierarchy = json_object.get("flattenHierarchy")
            self.axisConvention = AxisConvention[json_object.get("axisConvention")].name
            self.unit = Unit[json_object.get("unit")].name
            self.topology = ExportTopology[json_object.get("topology")].name
            # self.targetFaceCount = json_object.get("targetFaceCount")
            self.requestUv = json_object.get("requestUv")
            self.enableUdims = json_object.get("enableUdims")
        except Exception as err:
            print("Error: {0}".format(err))

        self.is_valid()

    def to_json(self):
        """ Returns a json string of this object"""
        return json.dumps(self, default=lambda o: o.__dict__, sort_keys=True,
                          indent=4)

    @classmethod
    def from_string(self, string):
        json_filler = json.loads(string)
        #None must be passed to required fields for validation failures
        return MeshImportMessageSchema(json_filler)

    def is_valid(self):
        """ Returns a boolean if the json is valid """
        try:
            json_filler = json.loads(self.to_json())
            jsonschema.validate(instance=json_filler, schema=MESH_IMPORT_SCHEMA)
            return True
        except jsonschema.exceptions.ValidationError as err:
            print("Json error: {}".format(err))

        return False

MESH_CONFIG_REQUEST_SCHEMA = {
    "type": "object",
    "properties": {
        "assetName": {"type": "string"},
        "assetUuid": {"type": "string"},
    },
    "required": ["assetName"],
}    

class MeshConfigRequestSchema:
    """ Message schema data structure """
    def __init__(self, json_object):
        self.assetName = json_object.get("assetName", None)
        self.assetUuid = json_object.get("assetUuid", "")
        
        self.is_valid()

    def to_json(self):
        """ Returns a json string of this object"""
        return json.dumps(self, default=lambda o: o.__dict__, sort_keys=True,
                          indent=4)

    @classmethod
    def from_string(self, string):
        json_filler = json.loads(string)
        #None must be passed to required fields for validation failures
        return MeshImportMessageSchema(json_filler)

    def is_valid(self):
        """ Returns a boolean if the json is valid """
        try:
            json_filler = json.loads(self.to_json())
            jsonschema.validate(instance=json_filler, schema=MESH_CONFIG_REQUEST_SCHEMA)
            return True
        except jsonschema.exceptions.ValidationError as err:
            print("Json error: {}".format(err))

        return False


SEND_ASSET_SCHEMA = {
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

class SendAssetMessageSchema:
    """ Message schema data structure """
    def __init__(self, json_object):
        self.path = json_object.get("path", None)
        self.name = json_object.get("name", "")
        self.uuid = json_object.get("id", "")
        if self.uuid == "":
            self.uuid = str(uuid.uuid4())
        self.type = json_object.get("type", "mesh")
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
        return SendAssetMessageSchema(json_filler.get("path", None), json_filler.get("id", ""), json_filler.get("name", ""), json_filler.get("type", None))

    def is_valid(self):
        """ Returns a boolean if the json is valid """
        try:
            json_filler = json.loads(self.to_json())
            jsonschema.validate(instance=json_filler, schema=SEND_ASSET_SCHEMA)
            return True
        except jsonschema.exceptions.ValidationError as err:
            print("Json error: {}".format(err))

        return False

class MeshImportApplication(BaseApplication):
    """ Handles all importing and exporting of mesh files"""
    _LOAD_MESH_UUID = uuid.UUID('fad67b5b-e141-4be7-b27a-d6f6a4f89424')
    _UPDATE_MESH_UUID = uuid.UUID('7c3e3c65-0c16-41d5-808b-953759930ffe')
    _REQUEST_MESH_CONFIG_UUID = uuid.UUID('685dd700-9d31-455d-98a4-431bb039819a')
    _RECEIVE_MESH_CONFIG_UUID = uuid.UUID('0b6d7b54-a90b-4c05-b0f9-359271b2efbc')

    @classmethod
    def get_callback_list(cls):
        """ Returns the callback list for mesh file imports """
        return [(cls._LOAD_MESH_UUID, cls.recv_load_mesh),
                (cls._UPDATE_MESH_UUID, cls.recv_update_mesh),
                (cls._REQUEST_MESH_CONFIG_UUID, cls.recv_mesh_config_req),
                (cls._RECEIVE_MESH_CONFIG_UUID, cls.recv_mesh_config)]
    
    @classmethod
    def get_feature_ids(cls):
        """ Method called at application registration to return the feature
         id set for the application to build the connection context """
        return [
            cls._LOAD_MESH_UUID,
            cls._UPDATE_MESH_UUID,
            cls._REQUEST_MESH_CONFIG_UUID,
            cls._RECEIVE_MESH_CONFIG_UUID
        ]

    @classmethod
    def recv_load_mesh(cls, context, message_type, message):
        """ Mesh load event handler """
        print("Load Mesh:")
        print(message)

    @classmethod
    def recv_update_mesh(cls, context, message_type, message):
        """ Mesh update event handler """
        print("Update Mesh:")
        print(message)

    @classmethod
    def recv_mesh_config_req(cls, context, message_type, message):
        """ Event to then fill out export format context to send back to
        the sending app """
        print("Receive Mesh Config Request:")
        print(message)

    @classmethod
    def recv_mesh_config(cls, context, message_type, message):
        """ Event to then fill out export format context to send back to
        the sending app """
        print("Receive Mesh Config:")
        print(message)
   
    @classmethod
    def send_load_mesh(cls, context, schema_instance):
        """ Send a mesh load event to another application """
        ConnectorInstance.write_message(context, cls._LOAD_MESH_UUID, schema_instance.to_json())

    @classmethod
    def send_load_mesh_string(cls, context, string):
        schema_instance = SendAssetMessageSchema.from_string(string)
        return MeshImportApplication.send_load_mesh(cls, context, schema_instance)

    @classmethod
    def send_update_mesh(cls, context, schema_instance):
        """ Send a mesh update event to another application """
        ConnectorInstance.write_message(context, cls._UPDATE_MESH_UUID, schema_instance.to_json())

    @classmethod
    def send_update_mesh_string(cls, context, string):
        schema_instance = SendAssetMessageSchema.from_string(string)
        return MeshImportApplication.send_update_mesh(cls, context, schema_instance)

    @classmethod
    def send_mesh_config_request(cls, context, schema_instance):
        """ Send a message to a receiving mesh importing application to get
        the expected mesh configuration """
        ConnectorInstance.write_message(context, cls._REQUEST_MESH_CONFIG_UUID, schema_instance.to_json())
    
    @classmethod
    def send_mesh_config_request_string(cls, context, string):
        schema_instance = MeshConfigRequestSchema.from_string(string)
        return MeshImportApplication.send_load_mesh(cls, context, schema_instance)

    @classmethod
    def send_mesh_config(cls, context, schema_instance):
        """ Send a message to a receiving mesh importing application with the 
        expected mesh configuration """
        ConnectorInstance.write_message(context, cls._RECEIVE_MESH_CONFIG_UUID, schema_instance.to_json())

    @classmethod
    def send_mesh_config_string(cls, context, string):
        schema_instance = MeshImportMessageSchema.from_string(string)
        return MeshImportApplication.send_mesh_config(cls, context, schema_instance)
