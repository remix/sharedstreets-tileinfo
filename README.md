# sharedstreets-tileinfo
View geometries and detailed tag information from SharedStreets tiles.

## Usage
This project is built with [bazel](https://bazel.build).
```
$ bazel build --cxxopt='-std=c++17' :all
$ ./bazel-bin/shst_tile_info ~/path/to/tile.geometry.6.pbf
```