cmake_minimum_required(VERSION 3.19)
project(MyProject VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executable
find_package(HDF5 REQUIRED)
find_package(Python COMPONENTS Development NumPy REQUIRED)
add_executable(greedyls src/greedyls.cpp)
target_compile_features(greedyls PRIVATE cxx_std_11)
target_include_directories(greedyls PRIVATE ${Python_INCLUDE_DIRS} ${Python_NumPy_INCLUDE_DIRS})
target_link_libraries(greedyls HDF5::HDF5)
target_link_libraries(greedyls Python::Python Python::NumPy)