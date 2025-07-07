# Hedera App Fuzzing Infrastructure

This directory contains a comprehensive fuzzing framework for the Hedera Ledger application, designed to detect security vulnerabilities, memory safety issues, and edge cases in protobuf parsing and data formatting functions.

## Overview

The fuzzing infrastructure implements a **multi-layer approach** similar to production-grade security testing:

1. **LibFuzzer Integration**: Coverage-guided fuzzing with LLVM's libFuzzer
2. **Address Sanitizer**: Memory safety verification (buffer overflows, use-after-free)
3. **Undefined Behavior Sanitizer**: Detection of undefined behavior
4. **Comprehensive Unit Tests**: Traditional unit tests with edge cases and security scenarios

## Architecture

```
fuzzing/
├── CMakeLists.txt              # Build configuration
├── mock/                       # BOLOS SDK mocks for testing
│   ├── bolos_sdk_mock.c/h     # Mock SDK functions
│   └── os_task.c/h            # Mock OS functions
├── fuzzer_*.c                  # Individual fuzzer implementations
├── run_fuzzing.sh             # Automation script
└── README.md                  # This file

tests/unit/proto_varlen/
├── test_proto_varlen_parser.c     # Standard functionality tests
├── test_proto_varlen_security.c   # Security-focused tests
└── test_proto_varlen_edge_cases.c # Boundary condition tests
```

## Fuzzing Targets

### 1. `fuzzer_proto_varlen_parser.c`
**Purpose**: Tests the custom protobuf parser used for extracting nested string fields from Hedera transactions.

**Attack Vectors Tested**:
- Buffer overflow attacks
- Integer overflow in length calculations
- Malformed protobuf structures
- Deeply nested messages (depth bomb attacks)
- Field number boundary conditions

**Key Functions**:
- `extract_nested_string_field()`
- `parse_field_tag()`

### 2. `fuzzer_hedera_format.c`
**Purpose**: Tests Hedera-specific data formatting functions.

**Attack Vectors Tested**:
- Large numeric values
- Edge cases in account ID formatting
- Timestamp overflow conditions
- Token ID boundary values

**Key Functions**:
- `format_hbar()`
- `format_account_id()`
- `format_timestamp()`
- `format_token_id()`

### 3. `fuzzer_staking.c`
**Purpose**: Tests staking information formatting and validation.

**Attack Vectors Tested**:
- Invalid staking configurations
- Extreme account/node ID values
- Boolean flag edge cases

**Key Functions**:
- `format_staking_info()`
- `validate_staking_account_id()`
- `validate_staking_node_id()`

### 4. `fuzzer_time_format.c`
**Purpose**: Tests time and duration formatting functions.

**Attack Vectors Tested**:
- Timestamp overflow/underflow
- Invalid nanosecond values
- Duration calculation edge cases

**Key Functions**:
- `format_timestamp()`
- `format_duration()`
- `validate_timestamp()`

## Security Testing Approach

### Mock Infrastructure
The fuzzing framework includes comprehensive mocks for the BOLOS SDK to enable testing without actual hardware:

- **Cryptographic functions**: Mock implementations that return predictable values
- **UI functions**: No-op implementations for headless testing
- **Memory management**: Standard malloc/free with sanitizer integration
- **Exception handling**: Graceful handling instead of device resets

### Coverage Areas

1. **Memory Safety**
   - Buffer overflow detection
   - Use-after-free prevention
   - Null pointer dereference protection

2. **Input Validation**
   - Malformed protobuf handling
   - Invalid field number ranges
   - Corrupted length fields

3. **Integer Safety**
   - Overflow/underflow in calculations
   - Varint boundary conditions
   - Size calculation accuracy

4. **Protocol Compliance**
   - Protobuf wire format validation
   - Field type enforcement
   - Message structure verification

## Usage

### Quick Start
```bash
# Run complete fuzzing campaign (60 seconds per target)
./run_fuzzing.sh

# Run for longer duration
FUZZ_TIME=300 ./run_fuzzing.sh

# Build fuzzers only
./run_fuzzing.sh build

# Run specific fuzzer
./run_fuzzing.sh run proto_varlen_parser

# Clean all artifacts
./run_fuzzing.sh clean
```

### Manual Building
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Run individual fuzzer
./fuzz_proto_varlen_parser corpus/proto_varlen_parser -max_total_time=60
```

### Unit Tests
```bash
cd tests/unit
mkdir build && cd build
cmake .. 
make
ctest
```

## Expected Output

### Successful Run
```
[INFO] Hedera App Fuzzing Suite
==================================
[INFO] Checking dependencies...
[INFO] Dependencies OK
[INFO] Setting up directories...
[INFO] Building fuzzers...
[INFO] Running fuzzer: proto_varlen_parser (60s)
[INFO] No crashes found for proto_varlen_parser
[INFO] Fuzzing campaign completed!
```

### When Issues Are Found
```
[WARN] Artifacts found for proto_varlen_parser:
  crash-da39a3ee5e6b4b0d3255bfef95601890afd80709
  timeout-af3e133428b9e25c55bc59e231b7ace8d5a67e6b
```

## Interpreting Results

### Crash Files
- **crash-***: Input that caused a crash (segfault, assertion failure)
- **timeout-***: Input that caused excessive execution time
- **slow-unit-***: Input that was slower than expected

### Coverage Reports
Fuzzers automatically generate coverage information. High coverage indicates thorough testing of code paths.

### Performance Metrics
- **exec/s**: Executions per second (higher is better)
- **cov**: Coverage information (unique edges discovered)
- **corp**: Corpus size (number of interesting test cases)

## Integration with CI/CD

### GitHub Actions Integration
```yaml
- name: Run Fuzzing Tests
  run: |
    cd fuzzing
    FUZZ_TIME=30 ./run_fuzzing.sh
    if [ -n "$(ls -A artifacts/ 2>/dev/null)" ]; then
      echo "Fuzzing found issues!"
      exit 1
    fi
```

### Regression Testing
- Add crash-causing inputs to permanent test suite
- Include in regular unit test runs
- Verify fixes don't reintroduce vulnerabilities

## Configuration

### Environment Variables
- `FUZZ_TIME`: Duration for each fuzzer (default: 60 seconds)
- `MAX_LEN`: Maximum input size (default: 8192 bytes)

### CMake Options
- `CMAKE_BUILD_TYPE=Debug`: Include debug symbols
- `ENABLE_FUZZING=ON`: Enable fuzzer-specific flags

## Troubleshooting

### Common Issues

1. **Clang not found**: Install LLVM/Clang development tools
2. **Missing sanitizers**: Update to recent Clang version (10+)
3. **Build failures**: Check mock implementations match actual interfaces
4. **No coverage**: Verify libFuzzer instrumentation is enabled

### Performance Tuning

1. **Slow fuzzing**: Reduce `-max_len` parameter
2. **Memory issues**: Increase available RAM or reduce corpus size
3. **Coverage plateaus**: Add more diverse seed inputs

## Contributing

### Adding New Fuzzers
1. Create `fuzzer_<module>.c` with `LLVMFuzzerTestOneInput()`
2. Add target to `CMakeLists.txt`
3. Update `run_fuzzing.sh` to include new fuzzer
4. Add corresponding unit tests

### Enhancing Coverage
1. Review coverage reports for untested paths
2. Add targeted test inputs to corpus
3. Implement additional validation functions
4. Extend mock implementations as needed

## Security Considerations

This fuzzing infrastructure helps prevent:

- **Remote code execution** via malformed protobuf messages
- **Denial of service** through resource exhaustion
- **Information disclosure** via buffer over-reads
- **State corruption** through invalid input processing

Regular fuzzing should be part of the security development lifecycle for any code handling external input in the Ledger application. 