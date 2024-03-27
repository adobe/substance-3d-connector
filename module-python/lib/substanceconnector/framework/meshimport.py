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

from .application import BaseApplication
from .instance import ConnectorInstance


class MeshImportApplication(BaseApplication):
    """ Handles all importing and exporting of mesh files"""
    _IMPORT_MESH_UUID = uuid.UUID('42394c4d-04dc-4e7e-a00f-58e947f903ac')
    _SEND_EXPORT_FORMAT = uuid.UUID('afd2dfd8-d028-4098-9366-43f31db9b36a')
    _EXPORT_FORMAT_REQUEST = uuid.UUID('e07c6691-d0f8-46af-b1d0-9d9fecc12559')

    @classmethod
    def get_callback_list(cls):
        """ Returns the callback list for mesh file imports """
        return [(cls._IMPORT_MESH_UUID, cls.recv_import_mesh),
                (cls._EXPORT_FORMAT_REQUEST, cls.recv_export_format_req)]

    @classmethod
    def recv_export_format_req(cls, context, message_type, message):
        """ Event to then fill out export format context to send back to
        the sending app """

    @classmethod
    def recv_import_mesh(cls, context, message_type, message):
        """ Import the mesh event handler """

    @classmethod
    def send_request_mesh_format(cls, context, message):
        """ Send a message to a recieveing mesh importing application to get
        the expected export format """
        ConnectorInstance.write_message(context, cls._IMPORT_MESH_UUID, message)

    @classmethod
    def send_import_mesh(cls, context, message):
        """ Send a mesh file to another application """
        ConnectorInstance.write_message(context, cls._IMPORT_MESH_UUID, message)
