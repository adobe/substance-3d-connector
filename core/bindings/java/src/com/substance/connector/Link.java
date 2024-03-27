/** @file Connector.java
    @brief Contains the Java language bindings for the Substance Connector library.
    @author Galen Helfter - Allegorithmic
    @date 20190104
    @copyright Allegorithmic. All rights reserved.
*/

package com.alg.connector;

import java.util.HashMap;
import com.alg.connector.Callback;

/* Exception to the Java coding style - Native methods will use the C Connector
 * standard of snake case (such as add_callback) to match the underlying
 * C interface. This may end up as determined style for the project that
 * all native methods use this distinction for clarity. */

/* This class will not be public outside of the module */
class Connector
{
    private static HashMap<String, Callback> sCallbacks;

    static
    {
        /* Load the compiled Java library to bind it */
        System.loadLibrary("link_java");
    }

    public static boolean initialize(String application_name)
    {
        /* Native code will add our callback on its own */
        return (native_init(application_name) == 0);
    }

    public static boolean shutdown()
    {
        return (native_shutdown() == 0);
    }

    public static String version()
    {
        return native_version();
    }

    public static void addCallback(String message_type, Callback callback)
    {
        /* Check if the message type is already bound */
        if (sCallbacks.containsKey(message_type))
        {
            sCallbacks.put(message_type, callback);
        }
    }

    public static void externalCallback(int context, String message_type)
    {
        /* Find the callback object in the map and invoke it */
        Callback callback = sCallbacks.get(message_type);

        if (callback != null)
        {
            callback.callback(message_type);
        }
    }

    /* Internal language bindings for the C library */

    /* Initialize the Connector module */
    private static native int native_init(String application_name);

    /* Shuts down the Connector module */
    private static native int native_shutdown();

    /* Returns the current version of the library */
    private static native String native_version();
};
