# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/danygagnon/esp/esp-idf/components/bootloader/subproject"
  "/Users/danygagnon/esp/traffic/cmake-build-debug/bootloader"
  "/Users/danygagnon/esp/traffic/cmake-build-debug/bootloader-prefix"
  "/Users/danygagnon/esp/traffic/cmake-build-debug/bootloader-prefix/tmp"
  "/Users/danygagnon/esp/traffic/cmake-build-debug/bootloader-prefix/src/bootloader-stamp"
  "/Users/danygagnon/esp/traffic/cmake-build-debug/bootloader-prefix/src"
  "/Users/danygagnon/esp/traffic/cmake-build-debug/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/danygagnon/esp/traffic/cmake-build-debug/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
