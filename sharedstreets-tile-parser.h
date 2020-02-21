#ifndef SHAREDSTREETS_TILE_PARSER_H
#define SHAREDSTREETS_TILE_PARSER_H

#include <filesystem>

#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"

template <class T>
class SharedStreetsTileParser {
 public:
  SharedStreetsTileParser(const std::filesystem::path& tile_path);
  std::vector<T> GetContents();

 private:
  int fd_;
  google::protobuf::io::FileInputStream raw_input_;
  google::protobuf::io::CodedInputStream coded_input_;
};

#endif  // SHAREDSTREETS_TILE_PARSER_H