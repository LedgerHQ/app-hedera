cmake_minimum_required(VERSION 3.10)
project(ledger-app-hedera-unit-tests C)

# Try to find PkgConfig and CMocka
find_package(PkgConfig)
if(PkgConfig_FOUND)
    pkg_check_modules(CMOCKA cmocka)
endif()

# If CMocka not found via pkg-config, try find_library
if(NOT CMOCKA_FOUND)
    find_library(CMOCKA_LIBRARIES cmocka)
    find_path(CMOCKA_INCLUDE_DIRS cmocka.h)
    if(CMOCKA_LIBRARIES AND CMOCKA_INCLUDE_DIRS)
        set(CMOCKA_FOUND TRUE)
        set(CMOCKA_CFLAGS_OTHER "")
        set(CMOCKA_LIBRARY_DIRS "")
    endif()
endif()

# Error if CMocka still not found
if(NOT CMOCKA_FOUND)
    message(FATAL_ERROR "CMocka not found. Please install libcmocka-dev (Ubuntu/Debian) or cmocka (other systems)")
endif()

enable_testing()
include(CTest)

include_directories(../../)
include_directories(../../src)
include_directories(../../src/ui)
include_directories(../../proto)
include_directories(../../vendor/nanopb)
include_directories(./mock)
include_directories(.)
# Use host-mode mocks instead of BOLOS SDK when running unit tests
add_definitions(-DNO_BOLOS_SDK=1)

# Disabled due to BOLOS SDK dependencies
# add_library(hedera_format SHARED ../../src/hedera_format.c)
# add_executable(test_hedera_format test_hedera_format.c)
# target_link_libraries(test_hedera_format ${CMOCKA_LIBRARIES} hedera_format)
# target_include_directories(test_hedera_format PUBLIC ${CMOCKA_INCLUDE_DIRS})
# target_compile_options(test_hedera_format PUBLIC ${CMOCKA_CFLAGS_OTHER})
# target_link_directories(test_hedera_format PUBLIC ${CMOCKA_LIBRARY_DIRS})
# add_test(test_hedera_format ${CMAKE_CURRENT_BINARY_DIR}/test_hedera_format)

# Proto varlen parser tests (standalone, minimal dependencies)
add_library(proto_varlen_parser SHARED ../../src/proto_varlen_parser.c)
# Build parser with host-mode mocks
target_compile_definitions(proto_varlen_parser PRIVATE NO_BOLOS_SDK=1)

# EVM parser library for unit tests
add_library(evm_parser_lib SHARED ../../src/evm_parser.c)
# Build EVM parser with host-mode mocks
target_compile_definitions(evm_parser_lib PRIVATE NO_BOLOS_SDK=1)


# Parser tests for proto_varlen_parser (basic functionality, no security concerns)
add_executable(test_proto_varlen_parser proto_varlen/test_proto_varlen_parser.c)
target_link_libraries(test_proto_varlen_parser ${CMOCKA_LIBRARIES} proto_varlen_parser)
target_include_directories(test_proto_varlen_parser PUBLIC ${CMOCKA_INCLUDE_DIRS})
target_compile_options(test_proto_varlen_parser PUBLIC ${CMOCKA_CFLAGS_OTHER})
target_link_directories(test_proto_varlen_parser PUBLIC ${CMOCKA_LIBRARY_DIRS})
add_test(test_proto_varlen_parser ${CMAKE_CURRENT_BINARY_DIR}/test_proto_varlen_parser)

# EVM calldata parser tests
add_executable(test_evm_parser test_evm_parser.c)
target_link_libraries(test_evm_parser ${CMOCKA_LIBRARIES} evm_parser_lib)
target_include_directories(test_evm_parser PUBLIC ${CMOCKA_INCLUDE_DIRS})
target_compile_options(test_evm_parser PUBLIC ${CMOCKA_CFLAGS_OTHER})
target_link_directories(test_evm_parser PUBLIC ${CMOCKA_LIBRARY_DIRS})
add_test(test_evm_parser ${CMAKE_CURRENT_BINARY_DIR}/test_evm_parser)

# Contract call reformat tests (nanopb + sign_contract_call, still OS-free)
add_executable(test_sign_contract_call
    test_sign_contract_call.c
    mock/putchar.c
    mock/staking_mock.c
    mock/throw_mock.c
    ../../src/sign_contract_call.c
    ../../src/evm_parser.c
    ../../src/printf.c
    ../../src/time_format.c
    ../../src/hedera_format.c
    ../../proto/timestamp.pb.c
    ../../proto/wrappers.pb.c
    ../../proto/contract_call.pb.c
    ../../proto/basic_types.pb.c
    ../../vendor/nanopb/pb_common.c
    ../../vendor/nanopb/pb_decode.c
)
# Test runs with host-mode mocks
target_compile_definitions(test_sign_contract_call PRIVATE NO_BOLOS_SDK=1)
target_link_libraries(test_sign_contract_call ${CMOCKA_LIBRARIES})
target_include_directories(test_sign_contract_call PUBLIC ${CMOCKA_INCLUDE_DIRS})
target_compile_options(test_sign_contract_call PUBLIC ${CMOCKA_CFLAGS_OTHER})
target_link_directories(test_sign_contract_call PUBLIC ${CMOCKA_LIBRARY_DIRS})
add_test(test_sign_contract_call ${CMAKE_CURRENT_BINARY_DIR}/test_sign_contract_call)

# Security tests for proto_varlen_parser (buffer overflows, infinite loops)
add_executable(test_proto_varlen_security proto_varlen/test_proto_varlen_security.c)
target_link_libraries(test_proto_varlen_security ${CMOCKA_LIBRARIES} proto_varlen_parser)
target_include_directories(test_proto_varlen_security PUBLIC ${CMOCKA_INCLUDE_DIRS})
target_compile_options(test_proto_varlen_security PUBLIC ${CMOCKA_CFLAGS_OTHER})
target_link_directories(test_proto_varlen_security PUBLIC ${CMOCKA_LIBRARY_DIRS})
add_test(test_proto_varlen_security ${CMAKE_CURRENT_BINARY_DIR}/test_proto_varlen_security)

# Edge case tests for proto_varlen_parser (empty data, single bytes, pointer manipulation)
add_executable(test_proto_varlen_edge_cases proto_varlen/test_proto_varlen_edge_cases.c)
target_link_libraries(test_proto_varlen_edge_cases ${CMOCKA_LIBRARIES} proto_varlen_parser)
target_include_directories(test_proto_varlen_edge_cases PUBLIC ${CMOCKA_INCLUDE_DIRS})
target_compile_options(test_proto_varlen_edge_cases PUBLIC ${CMOCKA_CFLAGS_OTHER})
target_link_directories(test_proto_varlen_edge_cases PUBLIC ${CMOCKA_LIBRARY_DIRS})
add_test(test_proto_varlen_edge_cases ${CMAKE_CURRENT_BINARY_DIR}/test_proto_varlen_edge_cases)

# Contract call nanopb tests (using actual nanopb for protobuf decoding)
add_executable(test_contract_call_nanopb 
    proto_varlen/test_contract_call_nanopb.c
    ../../vendor/nanopb/pb_common.c
    ../../vendor/nanopb/pb_decode.c
    ../../vendor/nanopb/pb_encode.c
    ../../proto/contract_call.pb.c
    ../../proto/basic_types.pb.c
    ../../proto/timestamp.pb.c
    ../../proto/wrappers.pb.c
)
target_link_libraries(test_contract_call_nanopb ${CMOCKA_LIBRARIES})
target_include_directories(test_contract_call_nanopb PUBLIC ${CMOCKA_INCLUDE_DIRS} ../../proto ../../vendor/nanopb . ../../)
target_compile_options(test_contract_call_nanopb PUBLIC ${CMOCKA_CFLAGS_OTHER})
target_compile_definitions(test_contract_call_nanopb PRIVATE PB_SYSTEM_HEADER="nanopb_system.h")
target_link_directories(test_contract_call_nanopb PUBLIC ${CMOCKA_LIBRARY_DIRS})
add_test(test_contract_call_nanopb ${CMAKE_CURRENT_BINARY_DIR}/test_contract_call_nanopb)

# pb_decode end-to-end ERC20 transaction (encode->decode->format)
add_executable(test_pb_decode_erc20
    test_pb_decode_erc20.c
    ../../src/sign_contract_call.c
    ../../src/evm_parser.c
    ../../src/hedera_format.c
    ../../src/time_format.c
    ../../src/printf.c
    ../../proto/contract_call.pb.c
    ../../proto/basic_types.pb.c
    ../../proto/wrappers.pb.c
    ../../proto/timestamp.pb.c
    ../../vendor/nanopb/pb_common.c
    ../../vendor/nanopb/pb_decode.c
    ../../vendor/nanopb/pb_encode.c
    mock/throw_mock.c
    mock/putchar.c
    mock/staking_mock.c
)
# End-to-end decode test with nanopb and host-mode mocks
target_compile_definitions(test_pb_decode_erc20 PRIVATE PB_SYSTEM_HEADER="nanopb_system.h" NO_BOLOS_SDK=1)
target_link_libraries(test_pb_decode_erc20 ${CMOCKA_LIBRARIES})
target_include_directories(test_pb_decode_erc20 PUBLIC ${CMOCKA_INCLUDE_DIRS})
target_compile_options(test_pb_decode_erc20 PUBLIC ${CMOCKA_CFLAGS_OTHER})
target_link_directories(test_pb_decode_erc20 PUBLIC ${CMOCKA_LIBRARY_DIRS})
add_test(test_pb_decode_erc20 ${CMAKE_CURRENT_BINARY_DIR}/test_pb_decode_erc20)
