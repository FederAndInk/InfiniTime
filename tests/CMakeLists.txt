cmake_minimum_required(VERSION 3.10)

add_subdirectory(Catch2)
add_executable(test_FlatLinkedList)
target_sources(test_FlatLinkedList PRIVATE test_FlatLinkedList.cpp)
target_link_libraries(test_FlatLinkedList PRIVATE Catch2::Catch2WithMain)
target_include_directories(
  test_FlatLinkedList PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_SOURCE_DIR}/src
)
target_compile_features(test_FlatLinkedList PRIVATE cxx_std_14)

include(CTest)
include(Catch)
catch_discover_tests(test_FlatLinkedList)
