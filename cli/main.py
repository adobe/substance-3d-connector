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
    def recv_connection_closed(cls, context, message_type, message):
        """ override for logging when a connection has ended """
        logging.info("Connection closed with: %s", (message))
        super.recv_connetion_closed(context, message_type, message)

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
        print("Receive message for importing an sbsar: {}".format(message))
        super().original_recv_import_sbsar(context, message_type, message)

    @classmethod
    def recv_import_asset(cls, context, message_type, message):
        """ New schema import asset event handler """
        print("New import asset event received.")
        super().recv_import_asset(context, message_type, message)


def send_message(arg_list, send_function):
    """ Sends a given message to the connected context """
    if(arg_list[0] == None):
        # If user just specifies asset, send to all connections
        for connection in SystemApplication.get_application_map():
            context = SystemApplication.get_application_map()[connection].get("context")
            if send_function(context, arg_list[1:]):
                print("Sending {0} to connection: {1}".format(arg_list[1], connection))
                return True
    elif arg_list[0] in SystemApplication.get_application_map():
        # If the user specifies dest send to a specific location
        context = SystemApplication.get_application_map()[arg_list[0]].get("context")
        if send_function(context, arg_list[1:]):
            return True

    return False


def send_entry_asset(context, arg_list):
    """ Entry callback for schema send to"""
    return ImportApplication.send_import_asset(context, arg_list)


def send_entry_legacy(context, arg_list):
    """ Entry callback for legacy send to """
    return ImportApplication.original_send_to(context, arg_list)


def send_legacy(arg_list):
    """ Sends the legacy file path instead of the new schema  """
    send_message(arg_list, send_entry_legacy)


def send_asset(arg_list):
    """ Sends an asset using the new schema format """
    send_message(arg_list, send_entry_asset)


def list_connections(arg_list):
    """ Prints the list of all current connections """
    for con in SystemApplication.get_application_map():
        if SystemApplication.get_application_map()[con].get("schema"):
            print(con, ": ", SystemApplication.get_application_map()[con].get("schema").to_json())
        else:
            print(con, ": No context found." )

# Map of the command type to the function to perform
COMMAND_MAP = {
    'send': send_asset,
    'send_legacy': send_legacy,
    'list': list_connections,
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
            parser = argparse.ArgumentParser(exit_on_error=False, usage="", add_help=False)
            parser.add_argument("command",
                                help=None)
            match command_split[0]:
                case "send":
                    parser.add_argument("-d", "--destination",
                                help="Name of connection target for send message.", required=False)
                    parser.add_argument("-s", "--source",
                                help="Path to asset file : REQUIRED", required=True)
                    parser.add_argument("-t", "--type",
                                help="Type of asset (sbsar, material, preset, etc.)", required=False, default="sbsar")
                case "send_legacy":
                    parser.add_argument("-d", "--destination",
                                help="Name of connection target for send message.", required=False)
                    parser.add_argument("-s", "--source",
                                help="Path to asset file", required=True)
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

            #send list of all value defined by parser arguments excluding the command name
            COMMAND_MAP[command_split[0]](list(arguments.values())[1:])

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
