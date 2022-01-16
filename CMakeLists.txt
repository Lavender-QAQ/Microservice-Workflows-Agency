cmake_minimum_required(VERSION 3.0.0)
project(Microservice-Workflows-Sidecar VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(Microservice-Workflows-Sidecar main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(Microservice-Workflows-Sidecar ${Boost_LIBRARIES})
