#include "sharedstreets-tile-parser.h"

#include <fcntl.h>
#include <stdint.h>

#include "proto/sharedstreets.pb.h"

using google::protobuf::io::CodedInputStream;
using google::protobuf::io::FileInputStream;

template <class T>
SharedStreetsTileParser<T>::SharedStreetsTileParser(const std::filesystem::path& tile_path) {
  fd_ = open(tile_path.c_str(), O_RDONLY);
  raw_input_ = std::make_unique<FileInputStream>(fd_);
  raw_input_->SetCloseOnDelete(true);  // closes the fd_ when raw_input_ is destroyed
  coded_input_ = std::make_unique<CodedInputStream>(raw_input_.get());
}

template <class T>
std::vector<T> SharedStreetsTileParser<T>::GetContents() {
  std::vector<T> messages;
  uint32_t next_message_length;
  while (coded_input_->ReadVarint32(&next_message_length)) {
    const auto limit = coded_input_->PushLimit(next_message_length);
    T message;
    message.ParseFromCodedStream(coded_input_.get());
    messages.emplace_back(std::move(message));
    coded_input_->PopLimit(limit);
  }
  return messages;
}

// Explicitly instantiate this template for the four SharedStreets message classes.
template class SharedStreetsTileParser<SharedStreetsGeometry>;
template class SharedStreetsTileParser<SharedStreetsIntersection>;
template class SharedStreetsTileParser<SharedStreetsMetadata>;
template class SharedStreetsTileParser<SharedStreetsReference>;