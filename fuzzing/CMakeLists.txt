cmake_minimum_required(VERSION 3.10)

project(HederaFuzzer
        VERSION 1.0
        DESCRIPTION "Hedera App Fuzzer"
        LANGUAGES C)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CPP_COMPILER clang)

set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_C_STANDARD 11)
# Check if address sanitizer runtime is available
execute_process(
    COMMAND ${CMAKE_C_COMPILER} -fsanitize=address -xc /dev/null -o /dev/null
    RESULT_VARIABLE ASAN_CHECK_RESULT
    ERROR_QUIET
)

if(ASAN_CHECK_RESULT EQUAL 0)
    message(STATUS "AddressSanitizer available - building with ASAN")
    set(CMAKE_C_FLAGS
        "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -DFUZZ -g -O0 -fsanitize=fuzzer,address,undefined"
    )
else()
    message(WARNING "AddressSanitizer runtime not available, building without ASAN")
    set(CMAKE_C_FLAGS
        "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -DFUZZ -g -O0 -fsanitize=fuzzer,undefined"
    )
endif()

include_directories(
  ../src/
  ../include/
  ../vendor/nanopb/
  mock/
)

# Mock implementations for BOLOS SDK dependencies
add_library(mock_bolos SHARED 
    mock/os_task.c
    mock/bolos_sdk_mock.c
)

# Libraries for core Hedera functionality - only what compiles successfully
add_library(proto_varlen_parser SHARED ../src/proto_varlen_parser.c)
target_compile_definitions(proto_varlen_parser PRIVATE NO_BOLOS_SDK=1)
target_link_libraries(proto_varlen_parser mock_bolos)

# Fuzzer executables - start with proto_varlen_parser
add_executable(fuzz_proto_varlen_parser fuzzer_proto_varlen_parser.c)

# Link fuzzer targets
target_link_libraries(fuzz_proto_varlen_parser proto_varlen_parser) 