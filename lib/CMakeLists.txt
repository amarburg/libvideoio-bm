# Set source files

fips_begin_module( videoio_bm )
  file(GLOB VIDEOIO_DECKLINK_SRCS *.cpp )

  ## The Blackmagic API is distributed as source ...
  fips_files(
    ${VIDEOIO_DECKLINK_SRCS}
    ${BLACKMAGIC_SRCS}
  )

  fips_deps( videoio )

  fips_libs( ${OpenCV_LIBS} dl )

  fips_include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
  )
fips_end_module()

# # This needs to be set before add_library .. only on OSX?
# if(APPLE)
#   set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
# endif()
#
# include_directories(
#   ${CMAKE_CURRENT_SOURCE_DIR}
#   ${CONAN_INCLUDE_DIRS}
#   ${VIDEOIO_DECKLINK_INCLUDE_DIRS}
# )
#
# # build shared library.
# add_library( videoio_decklink SHARED
#   ${BLACKMAGIC_SRCS}
#   ${VIDEOIO_DECKLINK_SRCS}
# )
#
# target_link_libraries( videoio_decklink
#   ${CONAN_LIBS}
#   ${OpenCV_LIBS}
#   ${Boost_LIBRARIES}
#   ${BLACKMAGIC_LIBS}
# )
