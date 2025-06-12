#!/bin/bash

# Map devices to environment variable names for SDKs
declare -A SDK_ENVS=(
  [nanosp]="NANOSP_SDK"
  [nanox]="NANOX_SDK"
  [flex]="FLEX_SDK"
  [stax]="STAX_SDK"
)

# List of Ledger devices
DEVICES=("nanosp" "nanox" "flex" "stax")

# Group devices by technology
NBGL_DEVICES=( "stax" "flex")
BAGL_DEVICES=( "nanox" "nanosp")

print_usage() {
  echo "Usage: $0 [COMMAND] [OPTIONS]"
  echo
  echo "COMMANDS:"
  echo "  tests [DEVICE|GROUP] [PYTEST_OPTIONS]   Run tests for specified device or group"
  echo "  build [DEVICE|GROUP]                    Build app for specified device or group"
  echo
  echo "DEVICE: nanosp, nanox, flex, stax"
  echo "GROUP: nbgl (all NBGL devices: nanox, nanosp, stax, flex), bagl (all BAGL devices)"
  echo
  echo "If no command is provided, this script will build and test for all supported devices."
  echo
  echo "PYTEST_OPTIONS: Any options to pass to pytest (e.g., -k, --golden_run, etc.)"
}

run_tests() {
  local devices=("$@")
  local pytest_args=${PYTEST_ARGS:-"--tb=short"}
  
  cd tests/ || exit 1
  rm -f ../debug.log
  
  for device in "${devices[@]}"; do
    echo "Running tests for $device..."
    # shellcheck disable=SC2086
    python3 -m pytest ${pytest_args} --device  "$device" --golden_run
  done
  
  cd .. || exit 1
}

build_devices() {
  local devices=("$@")
  
  for device in "${devices[@]}"; do
    sdk_env="${SDK_ENVS[$device]}"
    sdk_path="${!sdk_env}"

    if [[ -z "$sdk_path" ]]; then
      echo "Environment variable $sdk_env is not set. Skipping build for $device."
      continue
    fi

    echo "Building for $device using SDK: $sdk_path"
    BOLOS_SDK="$sdk_path" make DEBUG=1 -j8 || exit 1
    if [[ "$device" == "nanosp" ]]; then
      cp bin/app.elf ../app-exchange/test/python/lib_binaries/hedera_nanos2.elf
    else
      cp bin/app.elf ../app-exchange/test/python/lib_binaries/hedera_$device.elf
    fi
  done
}

get_devices_for_arg() {
  local arg="$1"
  
  case "$arg" in
    nbgl)
      echo "${NBGL_DEVICES[@]}"
      ;;
    bagl)
      echo "${BAGL_DEVICES[@]}"
      ;;
    nanosp|nanox|flex|stax)
      echo "$arg"
      ;;
    all)
      echo "${DEVICES[@]}"
      ;;
    *)
      echo "Invalid device or group: $arg. Valid options: ${DEVICES[*]}, nbgl, bagl, all" >&2
      exit 1
      ;;
  esac
}

# Parse command line arguments
COMMAND="$1"
shift || true

case "$COMMAND" in
  tests)
    # Get device/group argument if provided
    if [[ $# -gt 0 ]]; then
      DEVICE_ARG="$1"
      shift
      
      if [[ "$DEVICE_ARG" == "nbgl" ]]; then
        TARGET_DEVICES=("${NBGL_DEVICES[@]}")
      elif [[ "$DEVICE_ARG" == "bagl" ]]; then
        TARGET_DEVICES=("${BAGL_DEVICES[@]}")
      elif [[ "$DEVICE_ARG" == "all" ]]; then
        TARGET_DEVICES=("${DEVICES[@]}")
      elif [[ " ${DEVICES[*]} " =~ " $DEVICE_ARG " ]]; then
        TARGET_DEVICES=("$DEVICE_ARG")
      else
        echo "Invalid device or group: $DEVICE_ARG"
        print_usage
        exit 1
      fi
    else
      # Default to all devices
      TARGET_DEVICES=("${DEVICES[@]}")
    fi
    
    # Pass any remaining arguments to pytest
    PYTEST_ARGS="$*"
    
    run_tests "${TARGET_DEVICES[@]}"
    ;;
    
  build)
    # Get device/group argument if provided
    if [[ $# -gt 0 ]]; then
      DEVICE_ARG="$1"
      shift
      
      if [[ "$DEVICE_ARG" == "nbgl" ]]; then
        TARGET_DEVICES=("${NBGL_DEVICES[@]}")
      elif [[ "$DEVICE_ARG" == "bagl" ]]; then
        TARGET_DEVICES=("${BAGL_DEVICES[@]}")
      elif [[ "$DEVICE_ARG" == "all" ]]; then
        TARGET_DEVICES=("${DEVICES[@]}")
      elif [[ " ${DEVICES[*]} " =~ " $DEVICE_ARG " ]]; then
        TARGET_DEVICES=("$DEVICE_ARG")
      else
        echo "Invalid device or group: $DEVICE_ARG"
        print_usage
        exit 1
      fi
    else
      # Default to all devices
      TARGET_DEVICES=("${DEVICES[@]}")
    fi
    
    build_devices "${TARGET_DEVICES[@]}"
    ;;
    
  help|-h|--help)
    print_usage
    exit 0
    ;;
    
  "")
    # Default behavior when no command is provided - build and test all devices
    build_devices "${DEVICES[@]}"
    run_tests "${DEVICES[@]}" 
    ;;
    
  *)
    echo "Unknown command: $COMMAND"
    print_usage
    exit 1
    ;;
esac
