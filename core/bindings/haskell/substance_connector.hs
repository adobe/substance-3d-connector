-- @file substance_connector.hs
-- @brief Contains the Substance.Connector module with the bindings to the
--        Substance Connector C library
-- @author Galen Helfter - Allegorithmic
-- @copyright Allegorithmic. All rights reserved.

module Substance.Connector
(
    version,
    init,
    shutdown
) where

import Foreign.C.Types
import Foreign.C.String

foreign import ccall "substance_connector_version" version :: CString
foreign import ccall "substance_connector_init" init :: CUInt
foreign import ccall "substance_connector_shutdown" shutdown :: CUInt
