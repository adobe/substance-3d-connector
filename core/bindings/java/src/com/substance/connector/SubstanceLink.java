/** @file SubstanceLink.java
    @brief Contains a generic livelink framework to use to wrap around the
           Connector module.
    @author Galen Helfter - Allegorithmic
    @date 20190104
    @copyright Allegorithmic. All rights reserved.
*/

package com.alg.connector;

import com.alg.connector.Core;
import com.alg.connector.Connector;

public class SubstanceLink
{
    public static void main(String[] args)
    {
        Core core = Core.getInstance();

        /* core.initialize(); */

        System.out.println(Connector.version());
    }
};
