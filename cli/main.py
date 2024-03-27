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

from substanceconnector.framework.system import SystemApplication
from substanceconnector.framework.instance import ConnectorInstance
from substanceconnector.framework.assetimport import AssetImportApplication as ImportApplication


class CliApplication(SystemApplication):
    """ Class for overwriting connection callbacks where we can react to them
    but they are still called and tracked within thee system. """
    @classmethod
    def recv_connection_established(cls, context, message_type, message):
        """ Callback for handling incoming connection """
        if message not in SystemApplication.get_application_map().keys():
            logging.info("Connection established with: %s", (message))
            super().recv_connection_established(context, message_type, message)

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

class CliImportApplication(ImportApplication):
    """ Override of import to log events """
    @classmethod
    def original_recv_import_sbsar(cls, context, message_type, message):
        """ Callback for when we receive an import sbsar message """
        print("receive message for importing an sbsar: {}".format(message))
        super().original_recv_import_sbsar(context, message_type, message)

    @classmethod
    def recv_import_asset(cls, context, message_type, message):
        """ New schema import asset event handler """
        print("New import asset event received")
        super().recv_import_asset(context, message_type, message)


def send_message(arg_list, send_function):
    """ Sends a given message to the connected context """
    # If the user specifies dest and path
    if len(arg_list) > 1 and \
        arg_list[0] in SystemApplication.get_application_map() and \
            os.path.isfile(arg_list[1]):

        # Send to specific location
        context = SystemApplication.get_application_map()[arg_list[0]]
        send_function(context, arg_list[1])
        return True

    # If user just specifies asset, send to all connections
    print(arg_list)
    if len(arg_list) > 0 and os.path.isfile(arg_list[0]):
        for connection in SystemApplication.get_application_map():
            context = SystemApplication.get_application_map()[connection]
            send_function(context, arg_list[0])
            print("Sending {0} to connection: {1}".format(arg_list[0],
                                                         connection))
        return True

    return False


def send_entry_asset(context, arg_list):
    """ Entry callback for schema send to"""
    ImportApplication.send_import_asset(context, arg_list)


def send_entry_legacy(context, arg_list):
    """ Entry callback for legacy send to """
    ImportApplication.original_send_to(context, arg_list)


def send_legacy(arg_list):
    """ Sends the legacy file path instead of the new schema  """
    send_message(arg_list, send_entry_legacy)


def send_asset(arg_list):
    """ Sends an asset using the new schema format """
    send_message(arg_list, send_entry_asset)


def list_connections(arg_list):
    """ Prints the list of all current connections """
    for con in SystemApplication.get_application_map():
        print(con, ": ", SystemApplication.get_application_map()[con].get("schema").to_json())


# Map of the command type to the function to perform
COMMAND_MAP = {
    'send': send_asset,
    'send_legacy': send_legacy,
    'list': list_connections,
}


def _run_loop():
    """ Runs until the user exits """
    to_exit = False
    # Listen on the main thread for a command to be sent, then
    # send it to the other driver
    while not to_exit:
        # Handle interactive commands. Make sure to not look like the Python
        # console as that could be very confusing
        command = input('>> ')

        # Split on whitespace with default split behavior
        command_split = command.split()

        # Skip this iteration if the command was empty
        if not command_split:
            continue

        # Received the exit command - exit
        if command_split[0] == 'exit' or command_split[0] == 'quit':
            to_exit = True
        else:
            # Check whether the element is in the map
            if command_split[0] in COMMAND_MAP:
                COMMAND_MAP[command_split[0]](command_split[1:])
            else:
                print('Error: Command not found')


def main(argv):
    """ Main function for the connector test program """
    # Print out the version
    print('Connector Version: {}'.format(ConnectorInstance.get_version()))

    # Register are custom application
    current_base = CliApplication()
    current_import = CliImportApplication()
    ConnectorInstance.register_application(current_base)
    ConnectorInstance.register_application(current_import)

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
