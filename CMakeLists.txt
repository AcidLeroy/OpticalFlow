cmake_minimum_required (VERSION 2.8)
project (OpticalFlow)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
find_package( OpenCV REQUIRED )
enable_testing()

add_subdirectory(external/gtest)
add_subdirectory(external/celero)

message("Gtest source dir is: ${gtest_SOURCE_DIR}")
message("Gmock source dir is: ${gmock_SOURCE_DIR}")
message("Celero source dir is: ${CeleroProject_SOURCE_DIR}")

include_directories(
  ${gtest_SOURCE_DIR}/include
  ${gtest_SOURCE_DIR}
  ${gmock_SOURCE_DIR}/include
  ${gmock_SOURCE_DIR}
  ${CeleroProject_SOURCE_DIR}/include
  ${OpenCV_INCLUDE_DIRS}
  src/datatypes
  src/optical_flow
  src/readers
)

add_subdirectory(src)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  SET(CPACK_GENERATOR "DEB")
  SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Cody W. Eilar") #required
endif()

INCLUDE(CPack)
