cmake_minimum_required(VERSION 3.10)

add_subdirectory(Catch2)

add_executable(test_FlatLinkedList)
target_sources(test_FlatLinkedList PRIVATE test_FlatLinkedList.cpp)
target_link_libraries(test_FlatLinkedList PRIVATE Catch2::Catch2WithMain)
target_include_directories(
  test_FlatLinkedList
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/dummy_libs
          ${CMAKE_SOURCE_DIR}/src
          ${CMAKE_SOURCE_DIR}/src/libs
          ${CMAKE_SOURCE_DIR}/src/libs/date/includes
)
target_compile_features(test_FlatLinkedList PRIVATE cxx_std_14)
target_compile_options(
  test_FlatLinkedList PRIVATE -fsanitize=address -g3 -fno-omit-frame-pointer
)
target_link_options(
  test_FlatLinkedList
  PRIVATE
  -fsanitize=address
  -fno-omit-frame-pointer
  -fno-inline
)

add_executable(test_CalendarEvent)
target_sources(
  test_CalendarEvent
  PRIVATE test_CalendarEvent.cpp
          ${CMAKE_SOURCE_DIR}/src/components/ble/CalendarEventService.cpp
)
target_link_libraries(test_CalendarEvent PRIVATE Catch2::Catch2WithMain)
target_include_directories(
  test_CalendarEvent
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/dummy_libs
          ${CMAKE_SOURCE_DIR}/src
          ${CMAKE_SOURCE_DIR}/src/libs
          ${CMAKE_SOURCE_DIR}/src/libs/date/includes
)
target_compile_features(test_CalendarEvent PRIVATE cxx_std_14)
target_compile_options(
  test_CalendarEvent PRIVATE -fsanitize=address -g3 -fno-omit-frame-pointer
)
target_link_options(
  test_CalendarEvent
  PRIVATE
  -fsanitize=address
  -fno-omit-frame-pointer
  -fno-inline
)

include(CTest)
include(Catch)
catch_discover_tests(test_FlatLinkedList)
catch_discover_tests(test_CalendarEvent)
