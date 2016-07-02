set(lib_name utilities)

add_library(${lib_name} STATIC utilities.cpp)


add_executable(${lib_name}_tests tests/${lib_name}_tests.cpp)

target_link_libraries(${lib_name}_tests
  gtest gtest_main 
  ${OpenCV_LIBS})
  
add_test(NAME ${lib_name}_tests  COMMAND ${lib_name}_tests)