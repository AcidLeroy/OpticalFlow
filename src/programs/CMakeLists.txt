set(prog_name playback_video)

# Google Tests
add_executable(${prog_name} ${prog_name}.cpp)
install(TARGETS ${prog_name} DESTINATION bin)

target_link_libraries(${prog_name}
  ${OpenCV_LIBS}
  readers)

set(prog_name extract_features)
add_executable(${prog_name} ${prog_name}.cpp)
target_link_libraries(${prog_name}
  ${OpenCV_LIBS}
  readers)
install(TARGETS ${prog_name} DESTINATION bin)

set(prog_name farneback_flow)
add_executable(${prog_name} ${prog_name}.cpp)
target_link_libraries(${prog_name}
  ${OpenCV_LIBS}
)
install(TARGETS ${prog_name} DESTINATION bin)
