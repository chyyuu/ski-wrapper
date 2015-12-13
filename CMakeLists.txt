cmake_minimum_required(VERSION 2.8)
project(wrapper)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall -Wextra")

set(SOURCE_FILES ski-race.cpp sympp.cpp converter.cpp pretty.cpp lines.cpp manual.cpp)

add_executable(env env.cpp )
add_executable(pretty ski-race.cpp sympp.cpp converter.cpp pretty.cpp)
add_executable(lines lines.cpp sympp.cpp)
add_executable(manual manual.cpp)

target_link_libraries(pretty boost_serialization)
target_link_libraries(lines boost_serialization)
target_link_libraries(manual boost_serialization)