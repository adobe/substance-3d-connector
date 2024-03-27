""" Manages the callbacks that are mapped to a message type """

class CallbackManager:
    """ Internal manager for managing the callbacks currently registered with
        the connector framework """
    _callback_map = {}

    @classmethod
    def find_callback(cls, message_type):
        """ Attempts to locate the callback in the callbacks that are currently
            registered. Returns the callback on success, or None if not
            found """
        result = None

        if message_type in cls._callback_map:
            result = cls._callback_map[message_type]

        return result

    @classmethod
    def clear_callbacks(cls):
        """ Resets the internal callback map to a clear state """
        cls._callback_map = {}

    @classmethod
    def register_callback(cls, message_type, callback):
        """ Registers the callback and maps it to the message type, which
            should be a python UUID object from the uuid module. """
        cls._callback_map[message_type] = callback

    @classmethod
    def remove_callback(cls, message_type):
        """ If there is a callback registered to the message type, unregisters
            and removes it from the callback map """
        cls._callback_map.pop(message_type)
