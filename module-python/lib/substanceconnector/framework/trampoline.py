""" Holds the trampoline manager, which manages the behavior for all
    incoming messages from the native connector library """

from .message import Message

class TrampolineManager:
    """ Manages the current trampoline, which is the function that handles
        all incoming messages from the native connector library """
    _current_trampoline = None

    @classmethod
    def _default_trampoline(cls, context, message_type, message):
        """ Default trampoline. It attempts to find the callback in the
            callback map and invokes it if found """
        message_struct = Message(context, message_type, message)
        message_struct.process()

    @classmethod
    def initialize(cls):
        """ Initializes the trampoline manager to its default state """
        if cls._current_trampoline is None:
            cls._current_trampoline = cls._default_trampoline

    @classmethod
    def set_custom_trampoline(cls, function):
        """ Sets a new trampoline to override the default behavior for when a
            message is received """
        if function:
            cls._current_trampoline = function

    @classmethod
    def reset_trampoline(cls):
        """ Resets the trampoline manager's internal state for shutdown """
        cls._current_trampoline = None

    @classmethod
    def call_trampoline(cls, context, message_type, message):
        """ Invokes the trampoline currently set with the passed in
            parameters """
        if cls._current_trampoline:
            cls._current_trampoline(context, message_type, message)
