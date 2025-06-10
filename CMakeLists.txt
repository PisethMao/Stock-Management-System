cmake_minimum_required(VERSION 3.14)
project(StockManagementSystem)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)
set(TABULATE_NO_STD_BYTE ON CACHE INTERNAL "")
add_compile_definitions(TABULATE_NO_STD_BYTE)
FetchContent_Declare(
  xlnt
  GIT_REPOSITORY https://github.com/tfussell/xlnt.git
  GIT_TAG master
)
FetchContent_MakeAvailable(xlnt)
add_executable(StockManagementSystem 
  main.cpp
  src/User.cpp
  src/Auth.cpp
  src/AdminMenu.cpp
)
target_include_directories(StockManagementSystem PRIVATE include)
target_link_libraries(StockManagementSystem PRIVATE xlnt)
if(WIN32)
  add_custom_command(TARGET StockManagementSystem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/build/_deps/xlnt-build/source/Release/xlnt.dll"
    $<TARGET_FILE_DIR:StockManagementSystem>
  )
endif()
