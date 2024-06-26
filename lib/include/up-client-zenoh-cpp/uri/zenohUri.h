/*
 * Copyright (c) 2024 General Motors GTO LLC
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2024 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ZENOH_URI_
#define _ZENOH_URI_

#include <string>
#include <up-core-api/uri.pb.h>

namespace uprotocol::uri {

/// @returns Empty string if unable to serialize the UUri, otherwise UUri
///          converted to a Zenoh key string
[[ nodiscard ]] std::string toZenohKeyString(const uprotocol::v1::UUri &u_uri);

} // namespace uprotocol::tools

#endif /*_ZENOH_URI_*/
