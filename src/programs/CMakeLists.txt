set(prog_name playback_video)


# Google Tests
add_executable(${prog_name} ${prog_name}.cpp)

target_link_libraries(${prog_name}
  ${OpenCV_LIBS}
  readers)

