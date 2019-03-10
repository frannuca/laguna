project(tests)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "--------------------------->setting release mode")
    set(CMAKE_BUILD_TYPE Release)
endif()


message(STATUS "------------------:configuring unix ad build type " ${CMAKE_BUILD_TYPE})
#set(BOOST_LIBRARYDIR " /usr/lib/x86_64-linux-gnu/")
#set(BOOST_ROOT "/user/include/")

set(BOOST_LIBRARYDIR "/opt/dev/boost_1_69/lib/")
set(BOOST_ROOT "/opt/dev/boost_1_69/include/")


set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

#Setup CMake to run tests
enable_testing()

find_package(Boost 1.69 COMPONENTS system filesystem unit_test_framework date_time REQUIRED)

if(NOT Boost_FOUND)
    error("BOOST LIBS not found")
endif()

include_directories( ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/frame)
link_directories(${Boost_LIBRARY_DIRS}  ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

set(SOURCES ${CMAKE_SOURCE_DIR}/test/alltests.cpp)

foreach(testSrc ${SOURCES})
    #Extract the filename without an extension (NAME_WE)
    get_filename_component(testName ${testSrc} NAME_WE)

    #Add compile target
    set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    add_executable(${testName} ${testSrc})
    add_dependencies(${testName} laguna)

    target_link_libraries(${testName} ${Boost_LIBRARIES} laguna)
    #Finally add it to test execution -
    #Notice the WORKING_DIRECTORY and COMMAND
    add_test(NAME ${testName}
            WORKING_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${testName} )

    message(STATUS "------------------------------>TEST ADDED")

endforeach(testSrc)