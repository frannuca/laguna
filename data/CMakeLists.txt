cmake_minimum_required(VERSION 3.10)

project(laguna VERSION 1.0.0 DESCRIPTION "Data frame and series implementation")

set(CMAKE_CXX_STANDARD 17)

message(STATUS "------------------:configuring unix")
#set(BOOST_LIBRARYDIR " /usr/lib/x86_64-linux-gnu/")
#set(BOOST_ROOT "/user/include/")

set(BOOST_LIBRARYDIR "/opt/dev/boost_1_69/lib/")
set(BOOST_ROOT "/opt/dev/boost_1_69/include/")


set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.69 COMPONENTS system filesystem unit_test_framework date_time REQUIRED)

if(NOT Boost_FOUND)
    error("BOOST LIBS not found")
endif()

include_directories( ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/data)
link_directories(${Boost_LIBRARY_DIRS} )

#Add compile target

add_library(laguna SHARED
                    ${CMAKE_SOURCE_DIR}/data/vectoroperator.h
                    ${CMAKE_SOURCE_DIR}/data/converters.h
                    ${CMAKE_SOURCE_DIR}/data/locatortrait.h
                    ${CMAKE_SOURCE_DIR}/data/missingtraits.h
                    ${CMAKE_SOURCE_DIR}/data/series.h
                    ${CMAKE_SOURCE_DIR}/data/series.cpp)
target_link_libraries(laguna ${Boost_LIBRARIES})
