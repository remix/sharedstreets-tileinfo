#include <string>
#include <filesystem>

#include "proto/sharedstreets.pb.h"
#include "sharedstreets-tile-parser.h"

std::map<std::string, SharedStreetsMetadata> ReadMetadataTile(const std::filesystem::path& filename) {
  std::map<std::string, SharedStreetsMetadata> metadata_by_geometry_id;
  SharedStreetsTileParser<SharedStreetsMetadata> parser(filename);
  for (const auto& m : parser.GetContents()) {
    metadata_by_geometry_id[m.geometryid()] = std::move(m);
  }
  return metadata_by_geometry_id;
}

std::map<std::string, SharedStreetsGeometry> ReadGeometryTile(const std::filesystem::path& filename) {
  std::map<std::string, SharedStreetsGeometry> geometry_by_id;
  SharedStreetsTileParser<SharedStreetsGeometry> parser(filename);
  for (const auto& g : parser.GetContents()) {
    geometry_by_id[g.id()] = std::move(g);
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
