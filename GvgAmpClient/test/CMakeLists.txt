cmake_minimum_required (VERSION 2.6)
#project (Test_GvgAmpClient)
file(GLOB_RECURSE TEST_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
#add_library(${MYLIB_NAME} STATIC ${MYLIB_SRC_FILES})
include_directories(
  ${PROJECT_SOURCE_DIR}/include
)
add_executable(Test_GvgAmpClient ${TEST_SRC_FILES})
target_link_libraries(Test_GvgAmpClient GvgAmpClient)
