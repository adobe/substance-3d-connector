/** @file qml_connector.hpp
    @brief Binds the functions from the liveconnector framework to Qml functions
    @author Galen Helfter - Allegorithmic
    @date 20181025
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_QML_CONNECTOR_HPP
#define _SUBSTANCE_CONNECTOR_QML_CONNECTOR_HPP

#include <QtCore/QObject>
#include <QtCore/QString>

#include <QtQml/QQmlExtensionPlugin>

/** @brief Class acting as a proxy from Qml to the Connector C API */
class SubstanceConnector : public QObject
{
    Q_OBJECT

public:
    explicit SubstanceConnector(QObject *parent = nullptr);

    /* Bind all of the C functionality to the Q object as Q_INVOKABLE methods */
    Q_INVOKABLE QString version() const;

    /* Initialize the Connector module - performs an internal initialization of the
     * C library */
    Q_INVOKABLE unsigned int init();

    Q_INVOKABLE unsigned int shutdown();

    Q_INVOKABLE unsigned int write_message(unsigned int context,
                                           const QString &message_type,
                                           const QString &message);

    Q_INVOKABLE unsigned int open_tcp(unsigned int port);

    Q_INVOKABLE unsigned int open_unix(const QString &context);

    Q_INVOKABLE unsigned int close_context(unsigned int context);

    Q_INVOKABLE unsigned int connect_tcp(unsigned int port);

    Q_INVOKABLE unsigned int connect_unix(const QString &filepath);
};

/** @brief Class representing the Qml extension to load the module into
 *      the Qml engine. */
class ConnectorPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri) override;
};

#endif /* _SUBSTANCE_CONNECTOR_QML_CONNECTOR_HPP */
