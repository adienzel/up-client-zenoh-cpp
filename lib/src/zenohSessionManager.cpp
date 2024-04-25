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
 
#include "up-client-zenoh-cpp/session/zenohSessionManager.h"
#include <spdlog/spdlog.h>

using namespace uprotocol::v1;

ZenohSessionManager& ZenohSessionManager::instance(void) noexcept {

	static ZenohSessionManager sessionManager;

	return sessionManager;
}

UCode ZenohSessionManager::init(ZenohSessionManagerConfig &sessionConfig) noexcept {

    if (0 == refCount_) {

        std::lock_guard<std::mutex> lock(mutex_);

        if (0 == refCount_) {

            z_owned_config_t config = z_config_default();
        
            if (!sessionConfig.connectKey.empty()) {

                if (0 > zc_config_insert_json(z_loan(config), Z_CONFIG_CONNECT_KEY, sessionConfig.connectKey.c_str())) {
                    spdlog::error("zc_config_insert_json (Z_CONFIG_CONNECT_KEY) failed");
                    return UCode::INTERNAL;
                }
            }

            if (!sessionConfig.listenKey.empty()) {

                if (0 > zc_config_insert_json(z_loan(config), Z_CONFIG_LISTEN_KEY, sessionConfig.listenKey.c_str())) {
                    spdlog::error("zc_config_insert_json (Z_CONFIG_LISTEN_KEY) failed");
                    return UCode::INTERNAL;
                }
            }
            if (!sessionConfig.qosEnabled.empty() && (sessionConfig.qosEnabled == "false" || sessionConfig.qosEnabled =="true")) {
                if (0 > zc_config_insert_json(z_loan(config), "transport/unicast/qos/enabled", "false")) {
                    spdlog::error("zc_config_insert_json (transport/unicast/qos/enabled) failed");
                    return UCode::INTERNAL;
                }
            }

            if (!sessionConfig.lowLatency.empty() && (sessionConfig.lowLatency == "false" || sessionConfig.lowLatency =="true")) {
                if (0 > zc_config_insert_json(z_loan(config), "transport/unicast/lowlatency", "true")) {
                    spdlog::error("zc_config_insert_json (transport/unicast/lowlatency) failed");
                    return UCode::INTERNAL;
                }
            }
            
            if (sessionConfig.scouting_delay == 0) {
                if (0 > zc_config_insert_json(z_loan(config), "scouting/delay", "0")) {
                    spdlog::error("zc_config_insert_json (scouting/delay) failed");
                    return UCode::INTERNAL;
                }
                
            }
//            if (0 > zc_config_insert_json(z_loan(config), Z_CONFIG_MULTICAST_SCOUTING_KEY, "false")) {
//                spdlog::error("zc_config_insert_json (transport/unicast/lowlatency) failed");
//                return UCode::INTERNAL;
//            }

           

            session_ = z_open(z_move(config));
            if (false == z_check(session_)) {
                spdlog::error("z_open failed");
                return UCode::INTERNAL;
            }
        }

        refCount_.fetch_add(1);
    } else {
        refCount_.fetch_add(1);
    }
    
    spdlog::info("ZenohSessionManager::init done , ref count {}", refCount_.load());
    
    return UCode::OK;
}

UCode ZenohSessionManager::term() noexcept {
    
    std::lock_guard<std::mutex> lock(mutex_);

    if (0 < refCount_) {

        refCount_.fetch_sub(1);

        if (0 == refCount_) {
            z_close(z_move(session_));
            spdlog::info("ZenohSessionManager::term() done");
        }
    }

    return UCode::OK;
}

std::optional<z_owned_session_t> ZenohSessionManager::getSession() noexcept {

    std::lock_guard<std::mutex> lock(mutex_);

    if (0 < refCount_) {
        return session_;
    }

    return std::nullopt;
}