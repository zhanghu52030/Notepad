cmake_minimum_required(VERSION 3.11)

PROJECT (Notepad)

# Require "qa" and "xml" for debug report.
if(MSVC)
    find_package(wxWidgets REQUIRED COMPONENTS core base qa xml)
else()
    # Require "net" for create server/client between two instances.
    find_package(wxWidgets REQUIRED COMPONENTS core base net qa xml)
endif()

include(${wxWidgets_USE_FILE})

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

include_directories(${PROJECT_SOURCE_DIR}/src)

add_subdirectory(src)
