#pragma once

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>

#include <string>

namespace foxglove {
    google::protobuf::FileDescriptorSet BuildFileDescriptorSet(const google::protobuf::Descriptor* toplevelDescriptor);
}
