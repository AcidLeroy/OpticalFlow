set(lib_name image)

# Google Tests
add_executable(${lib_name}_tests tests/${lib_name}_tests.cpp)

target_link_libraries(${lib_name}_tests
  gtest gtest_main
  ${OpenCV_LIBS})

add_test(NAME ${lib_name}_tests  COMMAND ${lib_name}_tests)

# Celero benchmark
add_executable(${lib_name}_benchmark tests/${lib_name}_benchmark.cpp)

target_link_libraries(${lib_name}_benchmark
  celero
  ${OpenCV_LIBS})
