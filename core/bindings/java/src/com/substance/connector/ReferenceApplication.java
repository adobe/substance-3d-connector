/** @file ReferenceApplication.java
    @brief Contains a generic application interface that all interfaces to
           Connector can derive from.
    @author Galen Helfter - Allegorithmic
    @date 20190104
    @copyright Allegorithmic. All rights reserved.
*/

package com.alg.connector;

import java.util.ArrayList;
import com.alg.connector.Application;
import com.alg.connector.Core;

public abstract class ReferenceApplication implements Application
{
    private ArrayList<Callback> mCallbacks;

    /* Abstract methods */
    public abstract void preInit();
    public abstract void postInit();

    /* Constructors */
    public ReferenceApplication(ArrayList<Callback> callbacks)
    {
        mCallbacks = new ArrayList<Callback>(callbacks);
    }

    private void registerCallbacks()
    {
        Core core = Core.getInstance();

        /* Register all callbacks with the core systems */
        for (Callback callback : mCallbacks)
        {
            String messageType = callback.getMessageType();

            core.addCallback(messageType, callback);
        }
    }

    public void init()
    {
        Core core = Core.getInstance();

        core.registerApplication(this);

        registerCallbacks();
    }
};
