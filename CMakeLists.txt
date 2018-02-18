cmake_minimum_required(VERSION 3.9)
project(c_project)

set(CMAKE_CXX_STANDARD 11)

#find_package( OpenCL )
#include_directories( /Users/khaleesi/Desktop/openCl/ )
#link_directories(/System/Library/Frameworks/OpenCL.framework/Libraries/)

#include_directories(/System/Library/Frameworks/OpenCL.framework/Headers/)

#link_directories(/System/Library/Frameworks/OpenCL.framework/Libraries/)
#
#add_executable(c_project main.cpp)
#
#target_link_libraries (c_project main.cpp)

#target_include_directories (c_project PUBLIC /Users/khaleesi/Desktop/openCl/)
#target_link_libraries (c_project /System/Library/Frameworks/OpenCL.framework/Libraries/)


find_package(OpenCL REQUIRED)

set(SOURCE_FILES
    test_kernel.cl
    #my_file_to_exclude.cpp
    )

include_directories(/Users/khaleesi/Desktop/openCl/)
link_directories(/System/Library/Frameworks/OpenCL.framework/Libraries/)
add_executable (c_project simulation_01.cpp)
#target_include_directories (c_project PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (c_project /System/Library/Frameworks/OpenCL.framework/OpenCL)