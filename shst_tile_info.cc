#include <string>
#include <fcntl.h>
#include <fstream>
#include <memory>
#include <stdint.h>

#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"

#include "proto/sharedstreets.pb.h"

using google::protobuf::io::CodedInputStream;
using google::protobuf::io::FileInputStream;
using google::protobuf::io::ZeroCopyInputStream;

std::map<std::string, SharedStreetsMetadata> ReadMetadataTile(const std::string& filename) {
  int fd = open(filename.c_str(), O_RDONLY);
  FileInputStream raw_input(fd);
  CodedInputStream coded_input(&raw_input);

  std::map<std::string, SharedStreetsMetadata> metadata_by_geometry_id;
  uint32_t next_message_length;
  while (coded_input.ReadVarint32(&next_message_length)) {
    const auto limit = coded_input.PushLimit(next_message_length);
    SharedStreetsMetadata m;
    m.ParseFromCodedStream(&coded_input);
    metadata_by_geometry_id[m.geometryid()] = m;
    coded_input.PopLimit(limit);
  }

  return metadata_by_geometry_id;
}

std::map<std::string, SharedStreetsGeometry> ReadGeometryTile(const std::string& filename) {
  int fd = open(filename.c_str(), O_RDONLY);
  FileInputStream raw_input(fd);
  CodedInputStream coded_input(&raw_input);

  std::map<std::string, SharedStreetsGeometry> geometry_by_id;
  uint32_t next_message_length;
  while (coded_input.ReadVarint32(&next_message_length)) {
    const auto limit = coded_input.PushLimit(next_message_length);
    SharedStreetsGeometry g;
    g.ParseFromCodedStream(&coded_input);
    geometry_by_id[g.id()] = g;
    coded_input.PopLimit(limit);
  }

  return geometry_by_id;
}

void JoinAndPrintGeometries(
  std::map<std::string, SharedStreetsGeometry> geometries,
  std::map<std::string, SharedStreetsMetadata> metadatas) {
    for (const auto& id_geometry : geometries) {
      auto id = id_geometry.first;
      const auto& geometry = id_geometry.second;
      geometry.PrintDebugString();
      metadatas[id].PrintDebugString();
    }
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " <geometry.pbf> <metadata.pbf>" << std::endl;
    return 1;
  }

  std::map<std::string, SharedStreetsMetadata> metadata_by_geometry_id = ReadMetadataTile(argv[2]);
  std::map<std::string, SharedStreetsGeometry> geometry_by_id = ReadGeometryTile(argv[1]);
  JoinAndPrintGeometries(geometry_by_id, metadata_by_geometry_id);
  return 0;
}
