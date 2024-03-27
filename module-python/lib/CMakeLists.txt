# If the user has set an install location, install everything to
# that folder in the proper layout
if (DEFINED CONNECTOR_PYTHON_INSTALL_LOCATION)
    install(
        FILES
        substanceconnector/__init__.py

        DESTINATION
        ${CONNECTOR_PYTHON_INSTALL_LOCATION}/substanceconnector
    )

    install(
        FILES
        substanceconnector/core/__init__.py

        DESTINATION
        ${CONNECTOR_PYTHON_INSTALL_LOCATION}/substanceconnector/core
    )

    install(
        FILES
        substanceconnector/framework/__init__.py
        substanceconnector/framework/application.py
        substanceconnector/framework/assetimport.py
        substanceconnector/framework/callbacks.py
        substanceconnector/framework/connectionschema.py
        substanceconnector/framework/exception.py
        substanceconnector/framework/instance.py
        substanceconnector/framework/message.py
        substanceconnector/framework/substanceimport.py
        substanceconnector/framework/system.py
        substanceconnector/framework/trampoline.py
        substanceconnector/framework/uuid_utils.py

        DESTINATION
        ${CONNECTOR_PYTHON_INSTALL_LOCATION}/substanceconnector/framework
    )
endif ()
