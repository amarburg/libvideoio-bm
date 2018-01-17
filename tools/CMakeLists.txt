
fips_begin_app( bm_recorder cmdline )
  fips_files(
    BmRecorder.cpp
  )

  fips_deps( videoio_bm videoio )

  fips_include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
  )

fips_end_app()

# add_executable( BmRecorder BmRecorder.cpp)
# target_link_libraries( BmRecorder
#   ${Boost_LIBRARIES}
#   ${OpenCV_LIBS}
#   ${CMAKE_THREAD_LIBS_INIT}
#   ${CONAN_LIBS}
#   videoio_decklink )
