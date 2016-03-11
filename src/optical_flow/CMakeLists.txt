#
# lk_flow
#
set(lib_name lk_flow)
# Google Tests
add_executable(${lib_name}_tests tests/${lib_name}_tests.cpp ${lib_name}.cpp )

target_link_libraries(${lib_name}_tests
  gtest gtest_main 
  ${OpenCV_LIBS})

add_test(NAME ${lib_name}_tests  COMMAND ${lib_name}_tests)

  # Celero benchmark
add_executable(${lib_name}_benchmark tests/${lib_name}_benchmark.cpp ${lib_name}.cpp)
target_link_libraries(${lib_name}_benchmark
  celero
  ${OpenCV_LIBS})
  
add_library(lk_flow STATIC lk_flow.cpp)


#
# Vector statistics
#
set(lib_name vector_statistics)
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
  

#
# Motion Estimation
#
set(lib_name motion_estimation)
# Google Tests
add_executable(${lib_name}_tests tests/${lib_name}_tests.cpp )

target_link_libraries(${lib_name}_tests
  gtest gtest_main gmock
  ${OpenCV_LIBS})

#
# Optical Flow Tests
#
set(lib_name optical_flow)
# Google Tests
add_executable(${lib_name}_tests tests/${lib_name}_tests.cpp )
target_link_libraries(${lib_name}_tests
  gtest gtest_main gmock
  ${OpenCV_LIBS})


