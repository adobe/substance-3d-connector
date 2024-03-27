""" Contains the message object for representing incoming or outgoing messages
    from or to the connector library """

from .callbacks import CallbackManager
from ..core import connector

class Message:
    """ Object to store the message information from the core connector library. """
    def __init__(self, context, message_type, message):
        """ Constructor, expecting the context that it was received from, a
            message type as a uuid and a message as a string """
        self.context = context
        self.message_type = message_type
        self.message = message

    def process(self):
        """ Processes the message, calling the appropriate callback """
        callback = CallbackManager.find_callback(self.message_type)

        if callback:
            callback(self.context, self.message_type, self.message)

    def send(self):
        """ Sends this message to the context assigned to it """
        connector.write_message(self.context, self.message_type, self.message)
