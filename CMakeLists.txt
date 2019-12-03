cmake_minimum_required(VERSION 2.8)
project(ypspur_mqtt)

string(ASCII 27 ESC)

set(CMAKE_CXX_FLAGS "-O2 -std=c++11 -Wall")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/yp-spur/include
    ${CMAKE_CURRENT_BINARY_DIR}/yp-spur/include
)

find_package(Boost REQUIRED COMPONENTS system thread)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

find_package(ypspur QUIET)

if(ypspur_FOUND)
    message("-- ${ESC}[32mypspur has been installed!!!${ESC}[0m")
else()
    message("-- ${ESC}[33mypspur has not been installed!!!${ESC}[0m")
    message("-- ${ESC}[33madd subdirectory${ESC}[0m")
    add_subdirectory(yp-spur)
endif()

add_executable(ypspur_mqtt src/main.cpp src/ypspur_mqtt.cpp)
target_link_libraries(ypspur_mqtt
    ypspur
    mosquitto
    ${Boost_LIBRARIES}
)

INSTALL(
    TARGETS ypspur_mqtt
    EXPORT ${PROJECT_NAME}-export
    RUNTIME DESTINATION bin/${PROJECT_NAME}
)

INSTALL(
    EXPORT ${PROJECT_NAME}-export
    FILE ${PROJECT_NAME}-config.cmake
    DESTINATION share/cmake/${PROJECT_NAME}
)
