/** @file LiveLink.java
    @brief Contains a generic livelink framework to use to wrap around the
           Connector module.
    @author Galen Helfter - Allegorithmic
    @date 20190104
    @copyright Allegorithmic. All rights reserved.
*/

package com.alg.connector;

import java.util.ArrayList;

import com.alg.connector.Application;
import com.alg.connector.Callback;
import com.alg.connector.Core;

/* Generic livelink implementation program */
public abstract class LiveLink implements Application
{
    protected ArrayList<Callback> mCallbacks;
    protected boolean mCreatedCallbacks;

    public LiveLink()
    {
        mCallbacks = new ArrayList<Callback>();
        mCreatedCallbacks = false;
    }

    public void registerCallbacks()
    {
        Core core = Core.getInstance();

        for (Callback callback : mCallbacks)
        {
            String message_type = callback.getMessageType();
            core.addCallback(message_type, callback);
        }
    }

    public void preInit()
    {
        /* Pre-initialize call will be a good time to set up callbacks */
        if (!mCreatedCallbacks)
        {
            /* Import texture callback */
            Callback callback = getImportTexture();
            mCallbacks.add(callback);

            /* Reload texture callback */
            callback = getReloadTexture();
            mCallbacks.add(callback);

            mCreatedCallbacks = true;
        }
    }

    public abstract void postInit();

    public abstract void preShutdown();

    public abstract void postShutdown();

    /* Acquire callback for importing a texture */
    public abstract Callback getImportTexture();

    /* Acquire callback for reloading a texture */
    public abstract Callback getReloadTexture();
};
