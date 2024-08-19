# CMakeList.txt : CMake project for CMakeTestWSL, include source and define
# project specific logic here.
#

# Add source to this project's executable.
project(WebCMD VERSION 1.0 DESCRIPTION "Run processes and keep track of them in a local page in localhost.")

file(GLOB_RECURSE source_list "src/*.cpp")

if (WIN32) 
    add_executable(${PROJECT_NAME} WIN32 ${source_list})
else()
    add_executable(${PROJECT_NAME} ${source_list})
endif()

# Release?
set(CMAKE_BUILD_TYPE Release)

target_include_directories(${PROJECT_NAME} PRIVATE 
    #httplib
    "deps"
    "include"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

