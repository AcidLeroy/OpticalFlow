set(lib_name readers)

add_library(${lib_name} STATIC video_file_reader.cpp)
install(TARGETS ${lib_name} DESTINATION lib)
