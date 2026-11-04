cmake_minimum_required(VERSION 3.10)
project(tcpserver LANGUAGES CXX)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the executable
add_executable(tcpserver
    main.cpp
    ../../../Lib/log.cpp
)

# Add include directory for headers
target_include_directories(tcpserver PRIVATE ../../../Lib)

# Explicitly link stdc++
target_link_libraries(tcpserver PRIVATE stdc++)
