#!/usr/bin/env python3
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

""" @file main.py
    @brief Entry point to initialize connector and run CLI tools
    @author Dan Stover - Adobe
    @date 20221207
"""

import logging
import sys
import threading
import os.path
import json
import argparse
import pathlib
import shlex

from substanceconnector.framework.system import SystemApplication
from substanceconnector.framework.instance import ConnectorInstance
from substanceconnector.framework.features.sbsarimport import SBSARImportApplication as SbsarApplication, SBSARImportMessageSchema
from substanceconnector.framework.features.rawdata import RawDataApplication as DataApplication
from substanceconnector.framework.features.meshimport import MeshImportApplication as MeshApplication, MeshImportMessageSchema, MeshConfigRequestSchema, SendAssetMessageSchema, ColorFormat


class CliApplication(SystemApplication):
    """ Class for overwriting connection callbacks where we can react to them
    but they are still called and tracked within thee system. """
    @classmethod
    def recv_connection_established(cls, context, message_type, message):
        """ Callback for handling incoming connection """
        if message not in SystemApplication.get_application_map().keys():
            logging.info("Connection established with: %s", (message))
        else:
            logging.info("Connection re-established with: %s", (message))
        super().recv_connection_established(context, message_type, message)

    @classmethod
    def recv_connection_closed(cls, context, message_type, message):
        """ override for logging when a connection has ended """
        logging.info("Connection closed with: %s", (message))
        super().recv_connection_closed(context, message_type, message)

    @classmethod
    def recv_context(cls, context, message_type, message):
        """ Callback for handling incoming connection """
        try:
            message_as_json = json.loads(message)
            if message_as_json["id_name"] in SystemApplication.get_application_map().keys():
                logging.info("Context Received from: %s", (message_as_json["id_name"]))
                super().recv_context(context, message_type, message)
        except Exception as error:
            logging.info(error)

class CliSbsarApplication(SbsarApplication):
    """ Override of import to log events """
    @classmethod
    def recv_import_sbsar(cls, context, message_type, message):
        """ New schema import asset event handler """
        print("New import asset event received.")
        super().recv_import_sbsar(context, message_type, message)

class CliRawDataApplication(DataApplication):
    """ Override of import to log events """
    @classmethod
    def recv_raw_data(cls, context, message_type, message):
        """ New schema import asset event handler """
        print("New import asset event received.")
        super().recv_raw_data(context, message_type, message)

class CliMeshApplication(MeshApplication):
    """ Override of import to log events """
    @classmethod
    def recv_load_mesh(cls, context, message_type, message):
        """ Mesh load event handler """
        super().recv_load_mesh(context, message_type, message)

    @classmethod
    def recv_update_mesh(cls, context, message_type, message):
        """ Mesh update event handler """
        super().recv_update_mesh(context, message_type, message)

    @classmethod
    def recv_mesh_config_req(cls, context, message_type, message):
        """ Event to then fill out export format context to send back to
        the sending app """
        super().recv_mesh_config_req(context, message_type, message)



        try:
            stringobject = """{
                "assetName": "",
                "assetUuid": "",
                "supported_formats": ["usd", "fbx"],
                "colorFormat": "Vertex",
                "colorEncoding": "SRGB",
                "allowNegativeTransforms": true,
                "allowInstances": false,
                "flattenHierarchy": false,
                "axisConvention": "ForceZUp_XRight_YForward",
                "unit": "Centimeters",
                "topology": "Triangles",
                "requestUv": false,
                "enableUdims": false
            }"""

            json_filler = json.loads(stringobject)
            message_json = json.loads(message)
            json_filler["assetName"] = message_json["assetName"]
        except Exception as err:
            print("Error json: {0}".format(err))

        try:
            schema_object = MeshImportMessageSchema(json_filler)
        except Exception as err:
            print("Error schema: {0}".format(err))
        MeshApplication.send_mesh_config(context, schema_object)
        print("Send Mesh Config")

    @classmethod
    def recv_mesh_config(cls, context, message_type, message):
        """ Event to then fill out export format context to send back to
        the sending app """
        super().recv_mesh_config(context, message_type, message)

def send_message(context, schema, send_function):
    """ Sends a given message to the connected context """
    if(context == None):
        # If user just specifies asset, send to all connections
        for connection in SystemApplication.get_application_map():
            context = SystemApplication.get_application_map()[connection].get("context")
            if send_function(context, schema):
                print("Sending {0} to connection: {1}".format(type(schema).__name__, connection))
                return True
    elif context in SystemApplication.get_application_map():
        # If the user specifies dest send to a specific location
        context = SystemApplication.get_application_map()[context].get("context")
        if send_function(context, schema):
            return True

    return False

#Asset import feature functions
def send_entry_sbsar(context, schema):
    """ Entry callback for schema send to"""
    return SbsarApplication.send_import_sbsar(context, schema)

def send_entry_sbsar_string(context, string):
    """ Entry callback for schema send to"""
    return SbsarApplication.send_import_sbsar_string(context, string)

def send_sbsar(context, schema):
    """ Sends an asset using the new schema format """
    send_message(context, schema, send_entry_sbsar)

def send_sbsar_string(context, string):
    """ Sends an asset using a schema formatted string"""
    send_message(context, string, send_entry_sbsar_string)

#Raw data feature functions
def send_entry_raw_data(context, string):
    """ Entry callback for schema send to"""
    return DataApplication.send_raw_data(context, string)

def send_raw_data(context, string):
    """ Sends an asset using the new schema format """
    send_message(context, string, send_entry_raw_data)

#mesh import feature functions
def send_entry_load_mesh(context, schema):
    return MeshApplication.send_load_mesh(context, schema)

def send_entry_load_mesh_string(context, string):
    return MeshApplication.send_load_mesh_string(context, string)

def send_load_mesh(context, schema):
    send_message(context, schema, send_entry_load_mesh)

def send_load_mesh_string(context, string):
    send_message(context, string, send_entry_load_mesh_string)

def send_entry_mesh_config_request(context, schema):
    return MeshApplication.send_mesh_config_request(context, schema)

def send_entry_mesh_config_request_string(context, string):
    return MeshApplication.send_mesh_config_request_string(context, string)

def send_mesh_config_request(context, schema):
    send_message(context, schema, send_entry_mesh_config_request)

def send_mesh_config_request_string(context, string):
    send_message(context, string, send_entry_mesh_config_request_string)

def list_connections(arg_list):
    """ Prints the list of all current connections """
    for con in SystemApplication.get_application_map():
        if SystemApplication.get_application_map()[con].get("schema"):
            print(con, ": ", SystemApplication.get_application_map()[con].get("schema").to_json())
        else:
            print(con, ": No context found." )

# Map of the command type to the function to perform
COMMAND_MAP = {
    'sendsbsar': send_sbsar,
    'sendsbsarstring': send_sbsar_string,
    'sendrawdata': send_raw_data,
    'sendmesh' : send_load_mesh,
    'sendmeshstring' : send_load_mesh_string,
    'sendmeshconfigrequest' : send_mesh_config_request,
    'sendmeshconfigrequeststring' : send_mesh_config_request_string,
    'list': list_connections,
    'exit': None,
    'quit': None,
}

# Map of the classes to use for each command input
CLASS_MAP = {
    'sendsbsar': SBSARImportMessageSchema,
    'sendsbsarstring': str,
    'sendrawdata': str,
    'sendmesh': SendAssetMessageSchema,
    'sendmeshstring': str,
    'sendmeshconfigrequest': MeshConfigRequestSchema,
    'sendmeshconfigrequeststring': str,
    'list': None,
    'exit': None,
    'quit': None,
}

class ErrorCatchingArgumentParser(argparse.ArgumentParser):
    def exit(self, status=0, message=None):
        if status:
            print('Command parse error: ', message)

def _run_loop():
    """ Runs until the user exits """
    to_exit = False
    # Listen on the main thread for a command to be sent, then
    # send it to the other driver
    while not to_exit:
        # Handle interactive commands. Make sure to not look like the Python
        # console as that could be very confusing
        command = input('>> ')
         # Split based on command style regex
        command_split = shlex.split(command)

        # Skip this iteration if the command was empty
        if not command_split:
            continue

         # Received the exit command - exit
        if command_split[0] == 'exit' or command_split[0] == 'quit':
            to_exit = True
        else:
            parser = argparse.ArgumentParser(exit_on_error=False, usage="", add_help=True)
            parser.add_argument("command",
                                help=None)
            isstringcommand = False
            match command_split[0]:
                case "sendsbsar":
                    parser.add_argument("-d", "--destination",
                                help="Name of connection target for send message.", required=False)
                    parser.add_argument("-p", "--path",
                                help="Path to asset file : REQUIRED", required=True)
                    parser.add_argument("-i", "--uuid",
                                help="Unique ID for the asset, if not set will be generated on schema object creation", required=False, default="")
                    parser.add_argument("-n", "--name",
                                help="Name of the asset", required=False, default="")
                    parser.add_argument("-t", "--type",
                                help="Type of asset (material, preset, etc.)", required=False, default="material")
                case "sendsbsarstring":
                    parser.add_argument("-d", "--destination",
                                help="Name of connection target for send message.", required=False)
                    parser.add_argument("-s", "--string",
                                help="JSON formatted string for SBSARImportMessageSchema object. : REQUIRED", required=False)
                    isstringcommand = True

                case "sendrawdata":
                    parser.add_argument("-d", "--destination",
                                help="Name of connection target for send message.", required=False)
                    parser.add_argument("-s", "--string",
                                help="Raw data string to be sent. : REQUIRED", required=True)

                case "sendmesh":
                    parser.add_argument("-d", "--destination",
                                help="Name of connection target for send message.", required=False)
                    parser.add_argument("-p", "--path",
                                help="Path to mesh file : REQUIRED", required=True)
                    parser.add_argument("-i", "--uuid",
                                help="Unique ID for the asset, if not set will be generated on schema object creation", required=False, default="")
                    parser.add_argument("-n", "--name",
                                help="Name of the asset", required=False, default="")
                    parser.add_argument("-t", "--type",
                                help="Type of asset (mesh)", required=False, default="mesh")
                case "sendmeshstring":
                    parser.add_argument("-d", "--destination",
                                help="Name of connection target for send message.", required=False)
                    parser.add_argument("-s", "--string",
                                help="JSON formatted string for MeshImportMessageSchema object. : REQUIRED", required=False)
                    isstringcommand = True
                case "sendmeshconfigrequest":
                    parser.add_argument("-d", "--destination",
                                help="Name of connection target for send message.", required=False)
                    parser.add_argument("-a", "--assetName",
                                help="Name of the asset", required=True)
                    parser.add_argument("-i", "--uuid",
                                help="Unique ID for the asset, if not set will be generated on schema object creation", required=False, default="")
                case "sendmeshconfigrequeststring":
                    parser.add_argument("-d", "--destination",
                                help="Name of connection target for send message.", required=False)
                    parser.add_argument("-s", "--string",
                                help="JSON formatted string for MESH_IMPORT_SCHEMA object. : REQUIRED", required=False)
                    isstringcommand = True

                case "list":
                    COMMAND_MAP[command_split[0]](None)
                    continue
                case "commands":
                    print("Valid commands: ", list(COMMAND_MAP.keys()))
                    continue
                case _:
                    print('Error: Command not found. Enter "commands" for valid commands')
                    continue
            try:
                args = parser.parse_args(command_split)
            except (argparse.ArgumentError, SystemExit):
                print("Error parsing command: ", command_split[0])
                parser.print_help()
                continue

            arguments = vars(args)
            print(arguments)
            if isstringcommand:
                #string command
                COMMAND_MAP[command_split[0]](arguments["destination"], arguments["string"])
            else:
                #create a schema instance based on the command sent, arguments are contained in a map that matches the schema object
                schema_instance = CLASS_MAP[command_split[0]](arguments)

                #send command passing the schema instance
                COMMAND_MAP[command_split[0]](arguments["destination"], schema_instance)

def main(argv):
    """ Main function for the connector test program """
    # Print out the version
    print('Connector Version: {}'.format(ConnectorInstance.get_version()))

    # Register are custom application
    current_base = CliApplication()
    current_sbsar = CliSbsarApplication()
    current_data = CliRawDataApplication()
    current_mesh = CliMeshApplication()
    ConnectorInstance.register_application(current_base)
    ConnectorInstance.register_application(current_sbsar)
    ConnectorInstance.register_application(current_data)
    ConnectorInstance.register_application(current_mesh)

    # Init the connector module
    if len(argv) > 1:
        ConnectorInstance.initialize(argv[1])
        logging.basicConfig(
            filename='{}.log'.format(argv[1]),
            level=logging.DEBUG)
    else:
        ConnectorInstance.initialize("ConnectorCLI")
        logging.basicConfig(filename='ConnectorCLI.log', level=logging.DEBUG)

    logging.getLogger().addHandler(logging.StreamHandler(sys.stdout))
    logging.info("Started")

    # Loop process until user exit
    input_thread = threading.Thread(target=_run_loop)
    input_thread.daemon = True
    input_thread.start()
    input_thread.join()

    # Shut down the connector module
    ConnectorInstance.shutdown()

    logging.info("Shutdown")

    # On quit, exit the program
    sys.exit(0)


if __name__ == '__main__':
    main(sys.argv)
