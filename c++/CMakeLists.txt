
cmake_minimum_required(VERSION 2.6)
project(TEST)

include_directories(../libqrencode/lib/)

## Target
set(TEST_SRCS main.cpp AsyncSerial.cpp BufferedAsyncSerial.cpp)
add_executable(async ${TEST_SRCS})

## Link libraries
set(BOOST_LIBS thread date_time system)
find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
target_link_libraries(async ${Boost_LIBRARIES})
find_package(Threads REQUIRED)
target_link_libraries(async ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(async libqrencode.so)
