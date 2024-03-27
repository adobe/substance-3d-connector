/** @file connector_qml.cpp
    @brief Binds the functions from the connector framework to Qml functions
    @author Galen Helfter - Allegorithmic
    @date 20181025
    @copyright Allegorithmic. All rights reserved.
*/

#include "connector_qml.hpp"

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/connector.h>

#include <QtQml/QQmlEngine>

#include <cstdio>
#include <memory>

SubstanceConnector::SubstanceConnector(QObject *parent)
    : QObject(parent)
{
}

QString SubstanceConnector::version() const
{
    return QString(substance_connector_version());
}

unsigned int SubstanceConnector::init()
{
    return substance_connector_init();
}

unsigned int SubstanceConnector::shutdown()
{
    return substance_connector_shutdown();
}

unsigned int SubstanceConnector::write_message(unsigned int context,
                                          const QString &message_type,
                                          const QString &message)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    /* Connector API expects a UTF-8 encoded string. Convert UTF-16 strings from
     * Qt into something that Connector can handle.*/
    QByteArray utf8_message_type = message_type.toUtf8();
    QByteArray utf8_message = message.toUtf8();

    retcode = substance_connector_write_message(context, utf8_message_type.data(),
                                           utf8_message.data());

    return retcode;
}

unsigned int SubstanceConnector::open_tcp(unsigned int port)
{
    unsigned int context = UINT_MAX;
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    retcode = substance_connector_open_tcp(port, &context);

    if (retcode != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        context = UINT_MAX;
    }

    return context;
}

unsigned int SubstanceConnector::open_unix(const QString &filepath)
{
    unsigned int context = UINT_MAX;
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    QByteArray utf8_filepath = filepath.toUtf8();

    retcode = substance_connector_open_unix(utf8_filepath.data(), &context);

    if (retcode != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        context = UINT_MAX;
    }

    return context;
}

unsigned int SubstanceConnector::close_context(unsigned int context)
{
    return substance_connector_close_context(context);
}

unsigned int SubstanceConnector::connect_tcp(unsigned int port)
{
    unsigned int context = UINT_MAX;
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    retcode = substance_connector_connect_tcp(port, &context);

    if (retcode != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        context = UINT_MAX;
    }

    return context;
}

unsigned int SubstanceConnector::connect_unix(const QString &filepath)
{
    unsigned int context = UINT_MAX;
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    QByteArray utf8_filepath = filepath.toUtf8();

    retcode = substance_connector_connect_unix(utf8_filepath.data(), &context);

    if (retcode != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        context = UINT_MAX;
    }

    return context;
}

void ConnectorPlugin::registerTypes(const char *uri)
{
    // @uri com.allegorithmic.liveconnector
    int register_result = qmlRegisterType<SubstanceConnector>("SubstanceConnector", 1, 0, "Connector");
}
