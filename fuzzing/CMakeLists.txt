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
  # Prefer local fuzzing mocks first
  ${CMAKE_CURRENT_LIST_DIR}/mock
  mock/
  ..
  ../src/
  ../src/ui/
  ../proto/
  ../include/
  ../vendor/nanopb/
  ../tests/unit/
  ../tests/unit/mock/
)

# Mock implementations for BOLOS SDK dependencies
add_library(mock_bolos SHARED 
    mock/os_task.c
    mock/bolos_sdk_mock.c
)

# Libraries for core Hedera functionality - only what compiles successfully
add_library(proto_varlen_parser SHARED ../src/proto_varlen_parser.c)
# Build with host-mode mocks instead of BOLOS SDK
target_compile_definitions(proto_varlen_parser PRIVATE NO_BOLOS_SDK=1)
target_link_libraries(proto_varlen_parser mock_bolos)

# Fuzzer executables
add_executable(fuzz_proto_varlen_parser fuzzer_proto_varlen_parser.c)
target_link_libraries(fuzz_proto_varlen_parser proto_varlen_parser)

# Full nanopb-based proto decoding + handler validator
add_executable(fuzz_proto_full
    fuzzer_proto_full.c
    ../proto/contract_call.pb.c
    ../proto/basic_types.pb.c
    ../proto/wrappers.pb.c
    ../proto/timestamp.pb.c
    ../vendor/nanopb/pb_common.c
    ../vendor/nanopb/pb_decode.c
    ../src/sign_contract_call.c
    ../src/evm_parser.c
    ../src/hedera_format.c
    ../src/time_format.c
    ../src/printf.c
    ../tests/unit/mock/putchar.c
    ../tests/unit/mock/staking_mock.c
    ../tests/unit/mock/token_lookup_mock.c
)
target_compile_definitions(fuzz_proto_full PRIVATE NO_BOLOS_SDK=1)
target_compile_definitions(fuzz_proto_full PRIVATE PB_SYSTEM_HEADER="nanopb_system.h")
target_compile_options(fuzz_proto_full PRIVATE -include stdbool.h)
target_link_libraries(fuzz_proto_full mock_bolos)

# EVM calldata / formatting fuzzers (no nanopb, no Ledger OS)
add_executable(fuzz_evm_payload
    fuzzer_evm_payload.c
    ../src/evm_parser.c
)
target_compile_definitions(fuzz_evm_payload PRIVATE NO_BOLOS_SDK=1)
target_link_libraries(fuzz_evm_payload mock_bolos)