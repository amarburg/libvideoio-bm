# Set source files
file(GLOB VIDEOIO_DECKLINK_SRCS *.cpp )

# This needs to be set before add_library .. only on OSX?
if(APPLE)
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
endif()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CONAN_INCLUDE_DIRS}
  ${VIDEOIO_DECKLINK_INCLUDE_DIRS}
)

# build shared library.
add_library( videoio_decklink SHARED
  ${BLACKMAGIC_SRCS}
  ${VIDEOIO_DECKLINK_SRCS}
)

target_link_libraries( videoio_decklink
  ${CONAN_LIBS}
  ${OpenCV_LIBS}
  ${Boost_LIBRARIES}
  ${BLACKMAGIC_LIBS}
)
