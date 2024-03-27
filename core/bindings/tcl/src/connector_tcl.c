/** @file connector_tcl.c
    @brief Defines the Tcl bindings for the connector API
    @author Galen Helfter - Allegorithmic
    @date 20181111
    @copyright Allegorithmic. All rights reserved.
*/

#include <tcl.h>

#if TCL_MAJOR_VERSION < 8
#error "Substance Connector framework requires Tcl 8.0 or greater"
#endif

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/connector.h>

#define NS "substance"

static int tcl_connector_version(ClientData cdata,
                            Tcl_Interp *interp,
                            int objc,
                            Tcl_Obj *const objv[])
{
    const char *version = substance_connector_version();
    Tcl_SetObjResult(interp, Tcl_NewStringObj(version, -1));
    return TCL_OK;
}

static int tcl_connector_init(ClientData cdata,
                         Tcl_Interp *interp,
                         int objc,
                         Tcl_Obj *const objv[])
{
    unsigned int retcode = substance_connector_init();
    Tcl_SetObjResult(interp, Tcl_NewIntObj((int) retcode));
    return TCL_OK;
}

static tcl_connector_shutdown(ClientData cdata,
                         Tcl_Interp *interp,
                         int objc,
                         Tcl_Obj *const objv[])
{
    unsigned int retcode = substance_connector_shutdown();
    Tcl_SetObjResult(interp, Tcl_NewIntObj((int) retcode));
    return TCL_OK;
}

SUBSTANCE_CONNECTOR_EXPORT
int Connector_Init(Tcl_Interp *interp)
{
    Tcl_Namespace *namespace = NULL;

    if (Tcl_InitStubs(interp, TCL_VERSION, 0) == NULL)
    {
        return TCL_ERROR;
    }
    if (Tcl_PkgProvide(interp, "SubstanceConnector", CONNECTOR_TCL_VERSION) == TCL_ERROR)
    {
        return TCL_ERROR;
    }

    /* Create the Tcl namespace for the library to reside within */
    namespace = Tcl_CreateNamespace(interp, NS, NULL, NULL);
    if (namespace == NULL)
    {
        return TCL_ERROR;
    }

    /* Register functions with the Tcl interpreter */
    Tcl_CreateObjCommand(interp, NS "::version", tcl_connector_version, NULL, NULL);
    Tcl_CreateObjCommand(interp, NS "::init", tcl_connector_init, NULL, NULL);
    Tcl_CreateObjCommand(interp, NS "::shutdown",
                         tcl_connector_shutdown, NULL, NULL);

    return TCL_OK;
}
