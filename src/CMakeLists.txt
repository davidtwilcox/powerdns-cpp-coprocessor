cmake_minimum_required(VERSION 3.17)
project(cppbackend)

set(SOURCE_CODE
        format.cc ./fmt/core.h ./fmt/format.h ./fmt/format-inl.h
        ./base64/base64.cpp ./base64/base64.h
        main.cpp
        backend.cpp backend.h
        encoder.cpp encoder.h repository.cpp repository.h)

add_executable(cppbackend ${SOURCE_CODE})

find_library(CRYPTOPP cryptopp lib)
target_link_libraries(cppbackend LINK_PUBLIC ${CRYPTOPP})

find_package(SQLite3 REQUIRED)
include_directories(${SQLite3_INCLUDE_DIRS})
target_link_libraries(cppbackend LINK_PUBLIC ${SQLite3_LIBRARIES})