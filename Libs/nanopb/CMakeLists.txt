#****************************************************************************
#*
#*   Copyright (C) 2016 Shilo_XyZ_. All rights reserved.
#*   Author:  Shilo_XyZ_ <Shilo_XyZ_<at>mail.ru>
#*
#* Redistribution and use in source and binary forms, with or without
#* modification, are permitted provided that the following conditions
#* are met:
#*
#* 1. Redistributions of source code must retain the above copyright
#*    notice, this list of conditions and the following disclaimer.
#* 2. Redistributions in binary form must reproduce the above copyright
#*    notice, this list of conditions and the following disclaimer in
#*    the documentation and/or other materials provided with the
#*    distribution.
#*
#* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
#* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
#* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
#* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
#* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
#* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
#* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
#* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#* POSSIBILITY OF SUCH DAMAGE.
#*
#****************************************************************************/

cmake_minimum_required(VERSION 2.8.11)

set(NANOPB_DIST             ${CMAKE_CURRENT_SOURCE_DIR}/nanopb-dist)

set(CMAKE_MODULE_PATH       ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")

set(NANOPB_GENERATE_CPP_APPEND_PATH FALSE)

find_package(Nanopb REQUIRED)
include_directories(${NANOPB_INCLUDE_DIRS})

set(PROTOBUF_PROTO_FILE     protocol.proto)

# fake target
add_custom_target(protobuf_proto_def SOURCES ${PROTOBUF_PROTO_FILE})

nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS
    ${PROTOBUF_PROTO_FILE}
)

set_source_files_properties(${PROTO_SRCS} ${PROTO_HDRS}
    PROPERTIES GENERATED TRUE)

add_library(nanopb STATIC ${NANOPB_SRCS} ${PROTO_SRCS})

target_compile_definitions(nanopb
    PUBLIC
        -DPB_FIELD_16BIT
        -DPB_NO_ERRMSG
)

target_include_directories(nanopb
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        ${NANOPB_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PROTOBUF_PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${PROTOBUF_PROTO_FILE} PARENT_SCOPE)
