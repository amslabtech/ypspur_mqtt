cmake_minimum_required(VERSION 3.5)
project(ypspur_mqtt)

string(ASCII 27 ESC)

set(CMAKE_CXX_FLAGS "-O2 -std=c++11 -Wall")

include_directories(
    include
)

find_package(Boost REQUIRED COMPONENTS system thread)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

find_package(ypspur REQUIRED)
include_directories(${ypspur_INCLUDE_DIRS})

find_package(mqttclass REQUIRED)

find_package(PkgConfig)
pkg_check_modules(libmosquitto REQUIRED libmosquitto)

include_directories(${libmosquitto_INCLUDE_DIRS})

set(HEADER_FILES
    include/${PROJECT_NAME}/velocity_data.h
    include/${PROJECT_NAME}/odometry_data.h
)

add_executable(ypspur_mqtt src/main.cpp src/ypspur_wrapper.cpp)
target_link_libraries(ypspur_mqtt
    ypspur
    mqttclass
    ${Boost_LIBRARIES}
    ${libmosquitto_LIBRARIES}
)
add_library(lib${PROJECT_NAME} INTERFACE)
target_include_directories(lib${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

install(
    FILES ${HEADER_FILES}
    DESTINATION include/${PROJECT_NAME}
)

install(
    TARGETS ${PROJECT_NAME} lib${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-export
    RUNTIME DESTINATION bin
)

install(
    EXPORT ${PROJECT_NAME}-export
    FILE ${PROJECT_NAME}-config.cmake
    DESTINATION share/cmake/${PROJECT_NAME}
)
