# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

#set (CMAKE_CXX_STANDARD 17)

include_directories(SYSTEM "cmake-build-debug-esp_remote/esp-idf")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(blehr)
