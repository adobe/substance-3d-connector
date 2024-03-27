/** @file Application.java
    @brief Contains a generic application interface that all interfaces to
           Connector can derive from.
    @author Galen Helfter - Allegorithmic
    @date 20190104
    @copyright Allegorithmic. All rights reserved.
*/

package com.alg.connector;

public interface Application
{
    /* Method expected for the Application/client to register any callbacks
     * that it exports with the Core. */
    public void registerCallbacks();

    /* Called by the Core in the pre-init on all registered applications
     * before beginning its initialization. */
    public void preInit();

    /* Called by the Core in the post-init phase on all registered applications
     * after completing the initialization process. */
    public void postInit();

    /* Called by the Core during its pre shutdown phase, which is prior to it
     * beginning the internal shutdown. */
    public void preShutdown();

    /* Called at the end of the shutdown, after the core systems have been
     * completed their shutdown phase. */
    public void postShutdown();

    /* Initialization method that the actual application/implementation will
     * use to register the application with the Core. */
    public void init();
};
