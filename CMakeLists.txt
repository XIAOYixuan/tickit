cmake_minimum_required(VERSION 3.5)
project(calendar)
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_BUILD_TYPE Debug)
SET(XMLPATH "lib/pugixml-1.11")

# header
include_directories(inc)
include_directories(.)
include_directories(${XMLPATH}/src)

# third-party
find_library(XML NAMES pugixml PATHS "${XMLPATH}/build")

add_executable(calendar main.cc)
target_link_libraries(calendar ${XML})