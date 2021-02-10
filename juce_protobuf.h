// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/*
  ==============================================================================
  BEGIN_JUCE_MODULE_DECLARATION
 
  ID:            juce_protobuf
  vendor:        Segfault16
  version:       3.13.0
  name:          Protobuf for Juce
  description:   Protobuf for Juce
  dependencies:  
  website:       www.github.com/segfault16/juce_protobuf
  searchpaths:   ./ ./protobuf/src
  license:       MIT
  END_JUCE_MODULE_DECLARATION
 
 ==============================================================================
 */

#include "config.h"

#if PROTO_MODULE_FULL || PROTO_MODULE_LIGHT
#include "protobuf/src/google/protobuf/arena.h"
#include "protobuf/src/google/protobuf/arenastring.h"
#include "protobuf/src/google/protobuf/extension_set.h"
#include "protobuf/src/google/protobuf/generated_message_util.h"
#include "protobuf/src/google/protobuf/implicit_weak_message.h"
#include "protobuf/src/google/protobuf/parse_context.h"
#include "protobuf/src/google/protobuf/io/coded_stream.h"
#include "protobuf/src/google/protobuf/io/strtod.h"
#include "protobuf/src/google/protobuf/io/zero_copy_stream.h"
#include "protobuf/src/google/protobuf/io/zero_copy_stream_impl.h"
#include "protobuf/src/google/protobuf/io/zero_copy_stream_impl_lite.h"
#include "protobuf/src/google/protobuf/message_lite.h"
#include "protobuf/src/google/protobuf/repeated_field.h"
#include "protobuf/src/google/protobuf/stubs/bytestream.h"
#include "protobuf/src/google/protobuf/stubs/common.h"
#include "protobuf/src/google/protobuf/stubs/int128.h"
#include "protobuf/src/google/protobuf/stubs/once.h"
#include "protobuf/src/google/protobuf/stubs/status.h"
#include "protobuf/src/google/protobuf/stubs/statusor.h"
#include "protobuf/src/google/protobuf/stubs/stringpiece.h"
#include "protobuf/src/google/protobuf/stubs/stringprintf.h"
#include "protobuf/src/google/protobuf/stubs/strutil.h"
#include "protobuf/src/google/protobuf/stubs/time.h"
#include "protobuf/src/google/protobuf/wire_format_lite.h"
#endif

#if PROTO_MODULE_FULL
#include "protobuf/src/google/protobuf/any.h"
#include "protobuf/src/google/protobuf/any.pb.h"
#include "protobuf/src/google/protobuf/api.pb.h"
#include "protobuf/src/google/protobuf/compiler/importer.h"
#include "protobuf/src/google/protobuf/compiler/parser.h"
#include "protobuf/src/google/protobuf/descriptor.h"
#include "protobuf/src/google/protobuf/descriptor.pb.h"
#include "protobuf/src/google/protobuf/descriptor_database.h"
#include "protobuf/src/google/protobuf/duration.pb.h"
#include "protobuf/src/google/protobuf/dynamic_message.h"
#include "protobuf/src/google/protobuf/empty.pb.h"
#include "protobuf/src/google/protobuf/field_mask.pb.h"
#include "protobuf/src/google/protobuf/generated_message_reflection.h"
#include "protobuf/src/google/protobuf/io/gzip_stream.h"
#include "protobuf/src/google/protobuf/io/printer.h"
#include "protobuf/src/google/protobuf/io/tokenizer.h"
#include "protobuf/src/google/protobuf/map_field.h"
#include "protobuf/src/google/protobuf/message.h"
#include "protobuf/src/google/protobuf/reflection_ops.h"
#include "protobuf/src/google/protobuf/service.h"
#include "protobuf/src/google/protobuf/source_context.pb.h"
#include "protobuf/src/google/protobuf/struct.pb.h"
#include "protobuf/src/google/protobuf/stubs/substitute.h"
#include "protobuf/src/google/protobuf/text_format.h"
#include "protobuf/src/google/protobuf/timestamp.pb.h"
#include "protobuf/src/google/protobuf/type.pb.h"
#include "protobuf/src/google/protobuf/unknown_field_set.h"
#include "protobuf/src/google/protobuf/util/delimited_message_util.h"
#include "protobuf/src/google/protobuf/util/field_comparator.h"
#include "protobuf/src/google/protobuf/util/field_mask_util.h"
#include "protobuf/src/google/protobuf/util/internal/datapiece.h"
#include "protobuf/src/google/protobuf/util/internal/default_value_objectwriter.h"
#include "protobuf/src/google/protobuf/util/internal/error_listener.h"
#include "protobuf/src/google/protobuf/util/internal/field_mask_utility.h"
#include "protobuf/src/google/protobuf/util/internal/json_escaping.h"
#include "protobuf/src/google/protobuf/util/internal/json_objectwriter.h"
#include "protobuf/src/google/protobuf/util/internal/json_stream_parser.h"
#include "protobuf/src/google/protobuf/util/internal/object_writer.h"
#include "protobuf/src/google/protobuf/util/internal/proto_writer.h"
#include "protobuf/src/google/protobuf/util/internal/protostream_objectsource.h"
#include "protobuf/src/google/protobuf/util/internal/protostream_objectwriter.h"
#include "protobuf/src/google/protobuf/util/internal/type_info.h"
#include "protobuf/src/google/protobuf/util/internal/type_info_test_helper.h"
#include "protobuf/src/google/protobuf/util/internal/utility.h"
#include "protobuf/src/google/protobuf/util/json_util.h"
#include "protobuf/src/google/protobuf/util/message_differencer.h"
#include "protobuf/src/google/protobuf/util/time_util.h"
#include "protobuf/src/google/protobuf/util/type_resolver_util.h"
#include "protobuf/src/google/protobuf/wire_format.h"
#include "protobuf/src/google/protobuf/wrappers.pb.h"























#endif