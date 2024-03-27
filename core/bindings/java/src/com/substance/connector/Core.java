/** @file Core.java
    @brief Defines the core for the Java bindings.
    @author Galen Helfter - Allegorithmic
    @date 20190104
    @copyright Allegorithmic. All rights reserved.
*/

package com.alg.connector;

import java.util.ArrayList;
import java.util.concurrent.atomic.AtomicInteger;

/* Allegorithmic Connector imports */
import com.alg.connector.Application;
import com.alg.connector.Connector;

public class Core
{
    /* Class variables */
    private static Core sCoreSingleton = new Core();

    private static class LinkState
    {
        static int Uninitialized = 0;
        static int InitStarted = 1;
        static int Initialized = 2;
        static int ShutdownStarted = 3;
    };

    /* Instance variables */
    private ArrayList<Application> mRegisteredApplications;
    private AtomicInteger mLinkState;

    /* Class methods */
    public static Core getInstance()
    {
        return sCoreSingleton;
    }

    /* Instance methods */
    public boolean initialize(String application_name)
    {
        boolean result = false;

        boolean change_state = mLinkState.compareAndSet(LinkState.Uninitialized,
                                                        LinkState.InitStarted);

        /* Initialize the Connector interface */
        if (change_state)
        {
            /* Begin initialization - call pre-initialization on all
             * registered applications. */
            callPreInit();

            /* Perform internal initialization */
            result = Connector.initialize(application_name);

            if (result)
            {
                /* Initialization is successful and complete - call post
                 * initialization on all applications before state change */
                callPostInit();

                mLinkState.compareAndSet(LinkState.InitStarted,
                                         LinkState.Initialized);
            }
        }

        return result;
    }

    public boolean shutdown()
    {
        boolean result = false;

        boolean change_state = mLinkState.compareAndSet(LinkState.Initialized,
                                                        LinkState.ShutdownStarted);

        if (change_state)
        {
            /* Begin shutdown - call pre-shutdown on all registered
             * applications. Will be called regardless of success */
            callPreShutdown();

            result = Connector.shutdown();

            if (result)
            {
                callPostShutdown();

                /* Clear registered applications to have a clean state */
                mRegisteredApplications.clear();

                mLinkState.compareAndSet(LinkState.ShutdownStarted,
                                         LinkState.Uninitialized);
            }
        }

        return result; 
    }

    /* Registers the given application internally, so that it may call
     * the expected functions at specific state changes. */
    public void registerApplication(Application app)
    {
        mRegisteredApplications.add(app);
    }

    /* Registers the given callback class internally */
    public void addCallback(String message_type, Callback callback)
    {
        Connector.addCallback(message_type, callback);
    }

    private Core()
    {
        mRegisteredApplications = new ArrayList<Application>();
        mLinkState = new AtomicInteger(0);
    }

    private void callPreInit()
    {
        for (Application application : mRegisteredApplications)
        {
            application.preInit();
        }
    }

    private void callPostInit()
    {
        for (Application application : mRegisteredApplications)
        {
            application.postInit();
        }
    }

    private void callPreShutdown()
    {
        for (Application application : mRegisteredApplications)
        {
            application.preShutdown();
        }
    }

    private void callPostShutdown()
    {
        for (Application application : mRegisteredApplications)
        {
            application.postShutdown();
        }
    }
};
