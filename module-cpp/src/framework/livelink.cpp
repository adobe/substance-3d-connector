/*
Copyright 2019 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

//! @file livelink.cpp
//! @brief Base application type for livelink component
//! @author Galen Helfter - Adobe
//! @date 20190411
//! @copyright Adobe. All rights reserved.

#include <substance/connector/framework/livelink.h>

namespace Substance
{
namespace Connector
{
namespace Framework
{
static const substance_connector_uuid_t updateImageId = {
	// fc6491a3-5d59-4a59-aa08-89f526958f17
	0xfc6491a3u,
	0x5d594a59u,
	0xaa0889f5u,
	0x26958f17u};

static const substance_connector_uuid_t importMeshId = {
	// bccb1197-8740-47b3-bb9f-d39952465297
	0xbccb1197u,
	0x874047b3u,
	0xbb9fd399u,
	0x52465297u};

Livelink::~Livelink()
{
}

void Livelink::preInit()
{
	// Initialize the callbacks
	mCallbacks.push_back(CallbackPair(updateImageId, {mUpdateImage}));

	Application::preInit();
}

void Livelink::postInit()
{
}

void Livelink::postShutdown()
{
}
} // namespace Framework
} // namespace Connector
} // namespace Substance
