cmake_minimum_required(VERSION 3.17)
project(testcppbackend)

set(SOURCE_CODE
        catch.hpp
        ../src/format.cc ../src/fmt/core.h ../src/fmt/format.h ../src/fmt/format-inl.h
        ../src/base64/base64.cpp ../src/base64/base64.h
        main.cpp
        ../src/backend.cpp ../src/backend.h
        ../src/encoder.cpp ../src/encoder.h
        ../src/repository.cpp ../src/repository.h
        testbackend.cpp testencoder.cpp testrepository.cpp common.h)

add_executable(testcppbackend ${SOURCE_CODE})

find_library(CRYPTOPP cryptopp lib)
target_link_libraries(testcppbackend LINK_PUBLIC ${CRYPTOPP})

find_package(SQLite3 REQUIRED)
include_directories(${SQLite3_INCLUDE_DIRS})
target_link_libraries(testcppbackend LINK_PUBLIC ${SQLite3_LIBRARIES})