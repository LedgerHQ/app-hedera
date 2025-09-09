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

include_directories(../../src)

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
target_compile_definitions(proto_varlen_parser PRIVATE NO_BOLOS_SDK=1)

# Parser tests for proto_varlen_parser (basic functionality, no security concerns)
add_executable(test_proto_varlen_parser proto_varlen/test_proto_varlen_parser.c)
target_link_libraries(test_proto_varlen_parser ${CMOCKA_LIBRARIES} proto_varlen_parser)
target_include_directories(test_proto_varlen_parser PUBLIC ${CMOCKA_INCLUDE_DIRS})
target_compile_options(test_proto_varlen_parser PUBLIC ${CMOCKA_CFLAGS_OTHER})
target_link_directories(test_proto_varlen_parser PUBLIC ${CMOCKA_LIBRARY_DIRS})
add_test(test_proto_varlen_parser ${CMAKE_CURRENT_BINARY_DIR}/test_proto_varlen_parser)

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

# Contract call long varlen field tests (simple validation without nanopb dependencies)
add_executable(test_contract_call_simple proto_varlen/test_contract_call_simple.c)
target_link_libraries(test_contract_call_simple ${CMOCKA_LIBRARIES})
target_include_directories(test_contract_call_simple PUBLIC ${CMOCKA_INCLUDE_DIRS})
target_compile_options(test_contract_call_simple PUBLIC ${CMOCKA_CFLAGS_OTHER})
target_link_directories(test_contract_call_simple PUBLIC ${CMOCKA_LIBRARY_DIRS})
add_test(test_contract_call_simple ${CMAKE_CURRENT_BINARY_DIR}/test_contract_call_simple)
