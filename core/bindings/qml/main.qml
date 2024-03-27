import QtQuick 2.7
import Painter 1.0

import "./substanceconnector" as SubstanceConnector

PainterPlugin
{
    SubstanceConnector.Connector {
        id: connector
    }
    function printVersion() {
        if (SubstanceConnector === undefined)
        {
            alg.log.info("No good.!");
        }
        else if (SubstanceConnector.Connector === undefined)
        {
            alg.log.info("Damn!");
        }
        else if (SubstanceConnector.Test === undefined)
        {
            alg.log.info("This is fine.");
        }
    }

    onConfigure:
    {
        printVersion();
    }
}
