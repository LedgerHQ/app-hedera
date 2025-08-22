# Hedera Ledger App Unit Tests

This directory contains comprehensive unit tests for the Hedera Ledger app, focusing on modules that can be tested without BOLOS SDK dependencies.

## Test Coverage

### Proto Varlen Parser Tests
- **`test_proto_varlen_parser`** (11 tests) - Basic functionality tests
- **`test_proto_varlen_security`** (10 tests) - Buffer overflow and infinite loop protection
- **`test_proto_varlen_edge_cases`** (15 tests) - Edge cases, empty data, single bytes, pointer manipulation

**Total: 36 tests** covering all critical paths and security scenarios.

## Prerequisites

### Ubuntu/Debian
```sh
sudo apt-get install cmake libcmocka-dev
```

### macOS
```sh
brew install cmake cmocka
```

### Other Systems
Install CMake and CMocka using your package manager.

## Building Tests

### From tests/unit directory (recommended)
```sh
# Install dependencies (Ubuntu/Debian)
make install-deps

# Build all unit tests
make build

# Run all unit tests
make test

# Clean unit test artifacts
make clean

# Show help
make help
```

### Manual build (if Makefile not preferred)
```sh
# Create build directory and configure
mkdir -p build
cd build
cmake ..

# Build all tests
make

# Run all tests
make test

# Run with verbose output
CTEST_OUTPUT_ON_FAILURE=1 make test
```

## Running Individual Test Suites

```sh
# Basic functionality tests
./build/test_proto_varlen_parser

# Security tests (buffer overflows, infinite loops)
./build/test_proto_varlen_security

# Edge case tests (empty data, single bytes)
./build/test_proto_varlen_edge_cases
```

## Running Tests with Verbose Output

```sh
# All tests with detailed output
cd build && ctest --verbose

# Specific test pattern
cd build && ctest -R proto_varlen --verbose

# Failed tests only
cd build && ctest --rerun-failed --output-on-failure
```
