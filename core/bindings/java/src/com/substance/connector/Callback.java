/** @file Callback.java
    @brief Contains the callback interface for the Java bindings.
    @author Galen Helfter - Allegorithmic
    @date 20190104
    @copyright Allegorithmic. All rights reserved.
*/

package com.alg.connector;

public interface Callback
{
    public String getMessageType();

    public void callback(String message);
};
