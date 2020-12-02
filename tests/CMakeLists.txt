add_executable(Regex_test Regex_Unittest.cc)
target_include_directories(Regex_test PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(Regex_test Reg ${CONAN_LIBS})
add_test(NAME Regex_test COMMAND Regex_test)