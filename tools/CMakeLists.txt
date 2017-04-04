
include_directories(
  ${LIBVIDEO_BM_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable( BmRecorder BmRecorder.cpp)
target_link_libraries( BmRecorder
  ${Boost_LIBRARIES}
  ${OpenCV_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
  ${CONAN_LIBS}
  videoio_decklink )
