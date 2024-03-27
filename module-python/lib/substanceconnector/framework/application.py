""" Base application type for Python bindings. This will be inherited
    by any applications that register callbacks for message types with
    the connector framework """

from .callbacks import CallbackManager

class BaseApplication:
    """ Base application type that all other application classes will
        inherit from """
    @classmethod
    def get_callback_list(cls):
        """ Returns the callback list for this application, which
            is an empty list by default. The callback list should be
            a list of tuples, of which each has the message type UUID as
            the first element and the callback function as the second """
        return []

    @classmethod
    def pre_init(cls):
        """ Method called at the beginning of startup, after the state has been
            changed to initialization started, but before any startup routines
            have been invoked invoked """
        callback_list = cls.get_callback_list()
        for message_type, callback in callback_list:
            CallbackManager.register_callback(message_type, callback)

    @classmethod
    def post_init(cls):
        """ Method called after the completion of all startup procedures, but
            before the state has been finalized to initialized """

    @classmethod
    def pre_shutdown(cls):
        """ Method called at the beginning of shutdown, after the state has
            been changed to shutdown started, but before any shutdown routines
            have been invoked """

    @classmethod
    def post_shutdown(cls):
        """ Method called after all shutdown routines have been called, but
            before the state has been finalized as shutdown """

    @classmethod
    def get_feature_ids(cls):
        """ Method called at application registration to return the feature
         id set for the application to build the connection context """
