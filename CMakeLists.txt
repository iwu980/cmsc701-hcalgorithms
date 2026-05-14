cmake_minimum_required(VERSION 3.19)
project(MyProject VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

FetchContent_Declare(
  umappp 
  GIT_REPOSITORY https://github.com/libscran/umappp
  GIT_TAG v3.3.2 # replace with a pinned release
)

FetchContent_MakeAvailable(umappp)

# Add executable
add_executable(umapppExample src/umappp_example.cpp)

target_link_libraries(umapppExample libscran::umappp)


find_package(HDF5 REQUIRED)
add_executable(readdata src/readdata.cpp)
target_link_libraries(readdata HDF5::HDF5)