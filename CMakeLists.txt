CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

project(jsonany)

include(ExternalProject)
include("${CMAKE_SOURCE_DIR}/vendor/rapidjson.cmake")

include_directories(
    ${RAPIDJSON_INCLUDE_DIR}
    "${CMAKE_SOURCE_DIR}/vendor/cereal/include"
)
set(SOURCE_FILES
    main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_dependencies(${PROJECT_NAME} rapidjson)