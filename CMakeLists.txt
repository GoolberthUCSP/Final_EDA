project("BallTree")
file(GLOB CSV_FILES ${CMAKE_BINARY_DIR}/*.csv)


if (NOT CSV_FILES)
    message(STATUS "********************* WARNING ********************")
    message(STATUS "The file of data .csv is necessary to run the code")
    message(STATUS "**************************************************")
endif()

cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_COMPILER g++)

find_package(Eigen3 REQUIRED)

add_executable(main main.cpp)

target_link_libraries(main PRIVATE Eigen3::Eigen)

target_compile_options(main PRIVATE -Wall -Wextra)

# Definir una regla de make personalizada para borrar el caché generado por CMake
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/main
    COMMENT "Removing CMake cache"
)