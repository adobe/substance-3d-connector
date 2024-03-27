""" Contains the ConnectorInstance class, which is a high level mechanism for
    working with Substance Connector """

import sys
if sys.version_info[0] < 3:
    def enum(**enums):
        return type('Enum', (), enums)
else:
    from enum import Enum

from .callbacks import CallbackManager
from .trampoline import TrampolineManager
from .uuid_utils import convert_uuid_to_integers, convert_integers_to_uuid
from .exception import ConnectorException
from .connectionschema import ConnectionSchema
from ..core import connector


def _trampoline(context, uuid_ints, message):
    """ Trampoline registered with the native Python module to
        call back out into the framework """
    message_type = convert_integers_to_uuid(uuid_ints)

    TrampolineManager.call_trampoline(context, message_type, message)

class _ApplicationManager:
    """ Manager for all applications, which stores the application list for
        the current running instance """
    def __init__(self):
        self._application_list = []

    def register_application(self, application):
        """ Registers an application with the application manager, which will
            then invoke state callbacks on it """
        self._application_list.append(application)

    def call_pre_init(self):
        """ Iterates over all applications and calls their pre_init methods """
        for application in self._application_list:
            application.pre_init()

    def call_post_init(self):
        """ Iterates over all applications and calls their post_init methods """
        for application in self._application_list:
            application.post_init()

    def call_pre_shutdown(self):
        """ Iterates over all applications and calls their pre_shutdown methods """
        for application in self._application_list:
            application.pre_shutdown()

    def call_post_shutdown(self):
        """ Iterates over all applications and calls their post_shutdown methods """
        for application in self._application_list:
            application.post_shutdown()

    def clear_applications(self):
        """ Clears the internal application list. This should only be called
            after they have all been successfully shut down """
        self._application_list = []

class ConnectorInstance:
    """ High level manager object for interacting with the connector framework
        and underlying native library """

    if sys.version_info[0] >= 3:
        class _State(Enum):
            """ Enumeration of the current state of the connector framework """
            SHUTDOWN = 0
            INITIALIZED_STARTED = 1
            INITIALIZED = 2
            SHUTDOWN_STARTED = 3
            ERROR = 4
    else:
        _State = enum(SHUTDOWN=0, INITIALIZED_STARTED=1, INITIALIZED=2, SHUTDOWN_STARTED=3, ERROR=4)

    _application_name = None
    _state = _State.SHUTDOWN
    _application_manager = _ApplicationManager()
    _open_context = None
    _feature_level = []

    @classmethod
    def register_application(cls, application):
        """ Registers an application class with the connector Framework """
        # Applications may only be registered in a shutdown state
        if cls._state != cls._State.SHUTDOWN:
            raise ConnectorException('Error - all Connector applications must be '
                                'registered prior to initialization')
        
        cls._feature_level += application.get_feature_ids()

        cls._application_manager.register_application(application)

    @classmethod
    def initialize(cls, application_name):
        """ Performs the initialization of the connector library and framework. """
        # Only continue with initialization if the framework is currently
        # uninitialized
        if cls._state != cls._State.SHUTDOWN:
            raise ConnectorException('Initialization must be called from '
                                'an uninitialized state')

        cls._state = cls._State.INITIALIZED_STARTED

        # Call the pre_init method of all registered applications before any
        # internal initialization is started
        cls._application_manager.call_pre_init()

        # Initialize the trampoline system. This will not alter the trampoline
        # if it has been set to a custom function
        TrampolineManager.initialize()

        native_return_code = connector.init(application_name)

        if native_return_code != 0:
            # Set to an error state and return an exception
            cls._state = cls._State.ERROR
            raise ConnectorException('Failed to initialize connector library, state now '
                                'set to error.')

        # Register our trampoline with the native code
        connector.register_trampoline(_trampoline)

        # Set up automatic connections and broadcast to all open instances
        cls._open_context = connector.open_default_tcp()
        connector.broadcast_tcp()

        # Set our application name to the one passed in
        cls._application_name = application_name

        # Call the post-init functions of all applications prior to setting the
        # state to initialized
        cls._application_manager.call_post_init()

        # Set the state to initialized now that everything is completed
        cls._state = cls._State.INITIALIZED

    @classmethod
    def shutdown(cls):
        """ Performs a full shutdown of all internal framework systems and the
            native connector library """
        if cls._state != cls._State.INITIALIZED:
            raise ConnectorException('Shutdown must be called from a fully '
                                'initialized state.')

        cls._state = cls._State.SHUTDOWN_STARTED

        # Iterate over all registered applications and call the pre shutdown
        # method before shutting down internal systems
        cls._application_manager.call_pre_shutdown()

        # Clear all callbacks
        CallbackManager.clear_callbacks()

        native_return_code = connector.shutdown()

        if native_return_code != 0:
            # Set to an error state and return an exception
            cls._state = cls._State.ERROR
            raise ConnectorException('Failed to properly shutdown connector library, '
                                'state now set to error.')

        # Prior to clearing all applications and setting the shutdown state,
        # call the post shutdown method of all registered applications
        cls._application_name = ''
        cls._open_context = None
        cls._application_manager.call_post_shutdown()
        cls._application_manager.clear_applications()

        # On completion, set the state back to shutdown
        cls._state = cls._State.SHUTDOWN

    @classmethod
    def write_message(cls, context, message_type, message):
        """ Writes the message to the connection represented by the context """
        uuid_ints = convert_uuid_to_integers(message_type)

        connector.write_message(context, uuid_ints, message)

    @classmethod
    def get_version(cls):
        """ Returns the version of connector core """
        return connector.version()

    @classmethod
    def get_connection_context(cls):
        currentSchema = ConnectionSchema(
            cls._application_name,
            cls._application_name,
            cls.get_version(),
            [str(feature_id) for feature_id in cls._feature_level])
        return currentSchema
