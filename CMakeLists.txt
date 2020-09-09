cmake_minimum_required(VERSION 3.10)

project(SolutionWriter VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(inc)

add_library(TEC src/tec.cc)

add_executable(TEC_GRID_CONVERTER app/grid.cc)
target_link_libraries(TEC_GRID_CONVERTER PUBLIC TEC)

add_executable(TEC_DATA_CONVERTER app/data.cc)
target_link_libraries(TEC_DATA_CONVERTER PUBLIC TEC)

install(TARGETS TEC_GRID_CONVERTER
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin)

install(TARGETS TEC_DATA_CONVERTER
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin)
