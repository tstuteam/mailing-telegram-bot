cmake_minimum_required(VERSION 3.2)
project(mailingbot)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra")
set(Boost_USE_MULTITHREADED ON)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(
  Boost
  COMPONENTS system
  REQUIRED)
find_package(CURL)
include_directories(/usr/local/include ${OPENSSL_INCLUDE_DIR}
                    ${Boost_INCLUDE_DIR})
if(CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIRS})
  add_definitions(-DHAVE_CURL)
endif()

add_executable(mailingbot src/main.cpp src/lib.h)

target_link_libraries(
  mailingbot /usr/local/lib64/libTgBot.a ${CMAKE_THREAD_LIBS_INIT}
  ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES})
