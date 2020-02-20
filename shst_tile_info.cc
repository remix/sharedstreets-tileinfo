#include <string>
#include <fcntl.h>
#include <filesystem>
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

std::filesystem::path ReplaceFilenameSubstring(
    std::filesystem::path source,
    const std::string& find, const std::string& replace) {
  std::string replacement_filename = source.filename().string();
  auto pos = replacement_filename.find(find);
  if (pos != std::string::npos) {
    replacement_filename.replace(pos, find.length(), replace);
  }
  return source.replace_filename(replacement_filename);
}

void PrintUsage(const std::string& argv_0) {
  std::cout << "Usage: " << argv_0 << " <geometry.pbf>" << std::endl;
  std::cout << "A corresponding metadata tile should exist in the same directory." << std::endl;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    PrintUsage(argv[0]);
    return 1;
  }

  std::filesystem::path geometry_path(argv[1]);

  // Check that the path has "geometry" as a substring, because we'll need it to generate the
  // filenames of the other layers of that tile.
  if (geometry_path.filename().string().find("geometry") == std::string::npos) {
    PrintUsage(argv[0]);
    return 1;
  }
  auto metadata_path = ReplaceFilenameSubstring(geometry_path, "geometry", "metadata");

  auto metadata_by_geometry_id = ReadMetadataTile(metadata_path);
  auto geometry_by_id = ReadGeometryTile(geometry_path);
  JoinAndPrintGeometries(geometry_by_id, metadata_by_geometry_id);
  return 0;
}
