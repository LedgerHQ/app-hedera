#!/bin/bash

# Hedera App Fuzzing Runner
# Comprehensive fuzzing test automation for Hedera Ledger App

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
FUZZ_TIME=${FUZZ_TIME:-"60"}  # Default 60 seconds per fuzzer
CORPUS_DIR="./corpus"
ARTIFACTS_DIR="./artifacts"
BUILD_DIR="./build"

echo -e "${GREEN}Hedera App Fuzzing Suite${NC}"
echo "=================================="

# Function to print colored output
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check dependencies
check_dependencies() {
    print_status "Checking dependencies..."
    
    if ! command -v clang &> /dev/null; then
        print_error "clang not found. Please install clang."
        exit 1
    fi
    
    if ! command -v cmake &> /dev/null; then
        print_error "cmake not found. Please install cmake."
        exit 1
    fi
    
    print_status "Dependencies OK"
}

# Create directories
setup_directories() {
    print_status "Setting up directories..."
    
    mkdir -p "$CORPUS_DIR"
    mkdir -p "$ARTIFACTS_DIR"
    mkdir -p "$BUILD_DIR"
    
    # Create corpus directories for each fuzzer
    mkdir -p "$CORPUS_DIR/proto_varlen_parser"
    
    print_status "Directories created"
}

# Build fuzzers
build_fuzzers() {
    print_status "Building fuzzers..."
    
    cd "$BUILD_DIR"
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    make -j$(nproc)
    cd ..
    
    print_status "Fuzzers built successfully"
}

# Generate seed corpus
generate_corpus() {
    print_status "Generating seed corpus..."
    
    # Proto varlen parser corpus - create more comprehensive test cases
    printf "\\x0A\\x0E\\x72\\x0C\\x0A\\x0ATest Value" > "$CORPUS_DIR/proto_varlen_parser/valid_nested.bin"
    printf "\\x0A\\xFF\\x72\\x0C\\x0A\\x0ATest" > "$CORPUS_DIR/proto_varlen_parser/malformed_length.bin"
    printf "\\x08\\x01" > "$CORPUS_DIR/proto_varlen_parser/minimal.bin"
    printf "\\x0A\\x20\\x72\\x1E\\x0A\\x1C\\x72\\x1A\\x0A\\x18\\x72\\x16\\x0A\\x14Deeply Nested Value!" > "$CORPUS_DIR/proto_varlen_parser/deep_nested.bin"
    printf "\\xFF\\xFF\\xFF\\xFF\\x0F\\x0A\\x05Hello" > "$CORPUS_DIR/proto_varlen_parser/large_field.bin"
    printf "\\x00\\x01" > "$CORPUS_DIR/proto_varlen_parser/invalid_field.bin"
    printf "\\x0F\\x01" > "$CORPUS_DIR/proto_varlen_parser/unknown_wire.bin"
    
    print_status "Seed corpus generated"
}

# Run individual fuzzer
run_fuzzer() {
    local fuzzer_name="$1"
    local timeout="$2"
    
    print_status "Running fuzzer: $fuzzer_name (${timeout}s)"
    
    local corpus_path="$CORPUS_DIR/$fuzzer_name"
    local artifacts_path="$ARTIFACTS_DIR/$fuzzer_name"
    local fuzzer_binary="$BUILD_DIR/fuzz_$fuzzer_name"
    
    mkdir -p "$artifacts_path"
    
    if [ ! -f "$fuzzer_binary" ]; then
        print_error "Fuzzer binary not found: $fuzzer_binary"
        return 1
    fi
    
    # Run fuzzer with timeout
    timeout "$timeout" "$fuzzer_binary" \
        "$corpus_path" \
        -artifact_prefix="$artifacts_path/" \
        -max_total_time="$timeout" \
        -print_final_stats=1 \
        -max_len=8192 \
        || {
            local exit_code=$?
            if [ $exit_code -eq 124 ]; then
                print_status "Fuzzer $fuzzer_name completed (timeout reached)"
            elif [ $exit_code -eq 77 ]; then
                print_warning "Fuzzer $fuzzer_name found issues (check artifacts)"
            else
                print_error "Fuzzer $fuzzer_name failed with exit code $exit_code"
                return 1
            fi
        }
    
    # Check for crashes
    if [ -n "$(ls -A "$artifacts_path" 2>/dev/null)" ]; then
        print_warning "Artifacts found for $fuzzer_name:"
        ls -la "$artifacts_path"
    else
        print_status "No crashes found for $fuzzer_name"
    fi
}

# Run all fuzzers
run_all_fuzzers() {
    print_status "Starting fuzzing campaign..."
    
    local fuzzers=("proto_varlen_parser")
    
    for fuzzer in "${fuzzers[@]}"; do
        run_fuzzer "$fuzzer" "$FUZZ_TIME"
        echo ""
    done
}

# Generate report
generate_report() {
    print_status "Generating fuzzing report..."
    
    local report_file="fuzzing_report.txt"
    {
        echo "Hedera App Fuzzing Report"
        echo "========================="
        echo "Date: $(date)"
        echo "Fuzz time per target: ${FUZZ_TIME}s"
        echo ""
        
        echo "Artifacts found:"
        for dir in "$ARTIFACTS_DIR"/*; do
            if [ -d "$dir" ]; then
                local fuzzer_name=$(basename "$dir")
                local artifact_count=$(ls -1 "$dir" 2>/dev/null | wc -l)
                echo "  $fuzzer_name: $artifact_count artifacts"
                
                if [ "$artifact_count" -gt 0 ]; then
                    echo "    Files:"
                    ls -la "$dir" | sed 's/^/      /'
                fi
            fi
        done
        
        echo ""
        echo "Corpus statistics:"
        for dir in "$CORPUS_DIR"/*; do
            if [ -d "$dir" ]; then
                local fuzzer_name=$(basename "$dir")
                local corpus_count=$(ls -1 "$dir" 2>/dev/null | wc -l)
                echo "  $fuzzer_name: $corpus_count test cases"
            fi
        done
        
    } > "$report_file"
    
    cat "$report_file"
    print_status "Report saved to $report_file"
}

# Main execution
main() {
    check_dependencies
    setup_directories
    build_fuzzers
    generate_corpus
    run_all_fuzzers
    generate_report
    
    print_status "Fuzzing campaign completed!"
}

# Parse command line arguments
case "${1:-}" in
    "clean")
        print_status "Cleaning fuzzing artifacts..."
        rm -rf "$BUILD_DIR" "$ARTIFACTS_DIR" "$CORPUS_DIR"
        print_status "Cleanup complete"
        ;;
    "build")
        check_dependencies
        setup_directories
        build_fuzzers
        ;;
    "run")
        if [ -z "$2" ]; then
            print_error "Usage: $0 run <fuzzer_name>"
            exit 1
        fi
        run_fuzzer "$2" "$FUZZ_TIME"
        ;;
    "")
        main
        ;;
    *)
        echo "Usage: $0 [clean|build|run <fuzzer_name>]"
        echo "  clean: Remove all fuzzing artifacts"
        echo "  build: Build fuzzers only"
        echo "  run <name>: Run specific fuzzer"
        echo "  (no args): Run complete fuzzing campaign"
        exit 1
        ;;
esac 