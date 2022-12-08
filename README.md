# sharedstreets-tileinfo
View geometries and detailed tag information from SharedStreets tiles.

This project includes a `SharedStreetsTileParser` class that can read a SharedStreets tile from file, and an example main function in `shst_tile_info.cc` that demonstrates how to use that parser.

## Usage
This project is built with [bazel](https://bazel.build).
```
$ bazel build :all
$ ./bazel-bin/shst_tile_info ~/path/to/tile.geometry.6.pbf
```
