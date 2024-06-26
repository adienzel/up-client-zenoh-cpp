# Copyright (c) 2024 General Motors GTO LLC
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
cmake_minimum_required(VERSION 3.20)
project(up-client-zenoh-cpp VERSION 0.1.0 LANGUAGES CXX)

find_package(Protobuf REQUIRED)
find_package(spdlog REQUIRED)

if(BUILD_UNBUNDLED)
#    find_package(up-cpp REQUIRED)
    find_package(zenohc REQUIRED)
else()
    if(CROSS_COMPILE)
        find_package(zenohc REQUIRED)
    else()
        find_library(ZENOH_LIB zenohc)
    endif()
#    find_package(up-cpp QUIET)
endif()

# Support pulling headers outside of /usr/local
include_directories(${CMAKE_INSTALL_PREFIX}/include)

add_definitions(-DSPDLOG_FMT_EXTERNAL)

set(SRC_FILES         
    src/zenohUri.cpp
    src/zenohUTransport.cpp
    src/zenohRpcClient.cpp
    src/upZenohClient.cpp
    src/zenohSessionManager.cpp)

add_library(${PROJECT_NAME} ${SRC_FILES})
add_library(up-client-zenoh-cpp::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} 
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> 
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}> 
        $ENV{CMAKE_ZENOH_INCLUDE_PATH}
        ${HOME}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src)

if ((BUILD_UNBUNDLED) OR (CROSS_COMPILE))
    target_link_libraries(${PROJECT_NAME} 
        PUBLIC 
            up-cpp::up-cpp
            zenohc::lib)
else()
    target_link_libraries(${PROJECT_NAME} 
        PUBLIC 
            up-cpp::up-cpp 
            ${ZENOH_LIB}) 
endif()

INSTALL(TARGETS ${PROJECT_NAME})
INSTALL(DIRECTORY include DESTINATION .)
