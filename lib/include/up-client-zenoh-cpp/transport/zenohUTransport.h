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

#ifndef _ZENOH_UTRANSPORT_
#define _ZENOH_UTRANSPORT_

#include <cstddef>
#include <unordered_map>
#include <atomic>
#include <zenoh.h>
#include <up-cpp/transport/datamodel/UPayload.h>
#include <up-cpp/transport/datamodel/UMessage.h>
#include <up-client-zenoh-cpp/session/zenohSessionManager.h>

#include <up-cpp/transport/UTransport.h>

namespace uprotocol::utransport {

    class ListenerContainer {
        public:
            std::vector<z_owned_subscriber_t> subVector_;
            std::vector<z_owned_queryable_t> queryVector_;
            std::vector<uprotocol::utransport::UListener*> listenerVector_;
    };

    class ZenohUTransport : public uprotocol::utransport::UTransport {

        public:

            ZenohUTransport(const ZenohUTransport&) = delete;
            ZenohUTransport& operator=(const ZenohUTransport&) = delete;

            /**
            * Transmit UPayload to the topic using the attributes defined in UTransportAttributes.
            * @param message message to be sent
            * @return Returns OKSTATUS if the payload has been successfully sent (ACK'ed), otherwise it
            * returns FAILSTATUS with the appropriate failure.
            */
            uprotocol::v1::UStatus send(const uprotocol::utransport::UMessage &message) noexcept;

            /**
            * Register listener to be called when UPayload is received for the specific topic.
            * @param topic Resolved UUri for where the message arrived via the underlying transport technology.
            * @param listener The method to execute to process the date for the topic.
            * @return Returns OKSTATUS if the listener is unregistered correctly, otherwise it returns FAILSTATUS
            * with the appropriate failure.
            */ 
            uprotocol::v1::UStatus registerListener(const uprotocol::v1::UUri &uri,
                                                    uprotocol::utransport::UListener &listener) noexcept;

            /**
            * Unregister a listener for a given topic. Messages arriving on this topic will no longer be processed
            * by this listener.
            * @param topic Resolved UUri for where the listener was registered to receive messages from.
            * @param listener The method to execute to process the date for the topic.
            * @return Returns OKSTATUS if the listener is unregistered correctly, otherwise it returns FAILSTATUS
            * with the appropriate failure.
            */
            uprotocol::v1::UStatus unregisterListener(const uprotocol::v1::UUri &uri, 
                                                      uprotocol::utransport::UListener &listener) noexcept;

        protected:
            /* Initialization success/failure */
            uprotocol::v1::UStatus uSuccess_;
            
           // ZenohUTransport() noexcept;
            ZenohUTransport(ZenohSessionManagerConfig &config) noexcept;
            //ZenohUTransport(ZenohSessionManagerConfig config) noexcept;
            ~ZenohUTransport() noexcept;

        private:

            static void OnSubscriberClose(void *arg);

            static void OnQueryClose(void *arg);

            static void QueryHandler(const z_query_t *query, 
                                    void *context);

            static void SubHandler(const z_sample_t* sample,
                                   void* arg);

            uprotocol::v1::UCode sendPublish(const uprotocol::utransport::UMessage &message) noexcept;

            uprotocol::v1::UCode sendQueryable(const uprotocol::utransport::UMessage &message) noexcept;

            uprotocol::v1::UCode mapEncoding(const uprotocol::utransport::UPayloadFormat &payloadFormat, 
                                             z_encoding_t &encoding) noexcept;

            /* zenoh session handle*/
            z_owned_session_t session_;
            /* indicate that termination is pending so no new transactions are allowed*/
            std::atomic_bool termPending_ = false;
            /* how many send transactions are currently in progress*/
            std::atomic_uint32_t pendingSendRefCnt_ = 0;
            /* how many times uTransport was initialized*/
            std::atomic_uint32_t refCount_ = 0;
            
            std::mutex mutex_;

            using uuriKey = std::string;     
            using uuidStr = std::string;

            std::unordered_map<uuriKey, z_owned_publisher_t> pubHandleMap_;
            std::unordered_map<uuriKey, std::shared_ptr<ListenerContainer>> listenerMap_;  
            std::unordered_map<uuriKey, bool> authorized_;
            std::unordered_map<uuidStr, z_owned_query_t> queryMap_;

            std::mutex pubInitMutex_;
            std::mutex subInitMutex_;

            static constexpr auto termMaxRetries_ = size_t(10);
            static constexpr auto termRetryTimeout_ = std::chrono::milliseconds(100);

            using cbArgumentType = std::tuple<std::shared_ptr<uprotocol::v1::UUri>, ZenohUTransport*, uprotocol::utransport::UListener&>;
    };
}
#endif /*_ZENOH_UTRANSPORT_*/
