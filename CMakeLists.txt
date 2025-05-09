cmake_minimum_required(VERSION 3.2)

project(SortTest)

set(CMAKE_CXX_STANDARD 26)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
set(CMAKE_CXX_FLAGS "-ggdb")
set(CMAKE_CXX_COMPILER "clang++")

# add_executable(SortTest SortList.cpp SortTest.cpp Algorithms/BubbleSort.cpp)
add_executable(SortTest
  test.cpp
  SortList.cpp
  UI/impl_Raylib.cpp
  UI/GUI.cpp
  Util.cpp
  Algorithms/BubbleSort.cpp
  Algorithms/StalinSort.cpp
  Algorithms/QuickSort.cpp
  Algorithms/ReculinSort.cpp
  Algorithms/GulagSort.cpp
  Algorithms/SelectionSort.cpp
  Algorithms/TooManyCocktails.cpp
)

target_link_libraries(SortTest -lraylib)
