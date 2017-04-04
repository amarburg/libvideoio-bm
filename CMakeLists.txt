cmake_minimum_required(VERSION 2.8.7)
project(libvideoio_bm)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Eigen3 REQUIRED )
find_package(Boost COMPONENTS filesystem system program_options REQUIRED )  #n.b. program_options is only required for my version of Stereolabs "Recorder"

## Default OpenCV version
set( OPENCV_VERSION 2.4)
list(APPEND OPENCV_COMPONENTS core highgui)

## C++ flags applied to all builds
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

## Configure OpenCV ##
find_package(OpenCV ${OPENCV_VERSION} COMPONENTS ${OPENCV_COMPONENTS} REQUIRED)

## Find Blackmagic SDK
set( BLACKMAGIC_FORCE_LINUX TRUE )
find_package( BlackmagicSDK REQUIRED )

## Set global include paths
set( VIDEOIO_DECKLINK_INCLUDE_DIRS
  ${CMAKE_SOURCE_DIR}/include
  ${BLACKMAGIC_INCLUDE_DIR}
  ${OpenCV_INCLUDE_DIRS}
	${CONAN_INCLUDE_DIRS} )

include_directories( ${VIDEOIO_DECKLINK_INCLUDE_DIRS} )

add_subdirectory("lib/")
add_subdirectory("tools/")

option( BUILD_UNIT_TESTS "Build unit tests" OFF )
if( BUILD_UNIT_TESTS )
  #add_subdirectory( test/unit/ )
endif()
