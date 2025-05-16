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

if [[ "$1" == "tests" ]]; then
  cd tests/ || exit 1
  rm ../debug.log

  # Check if a specific device is provided
  if [[ -n "$2" ]]; then
    if [[ " ${DEVICES[@]} " =~ " $2 " ]]; then
      echo "Running tests for $2..."
      pytest --tb=short -v --device "$2" -s 2>../debug.log
    else
      echo "Invalid device: $2. Valid devices are: ${DEVICES[*]}"
      exit 1
    fi
  else
    # Run tests for each device
    for device in "${DEVICES[@]}"; do
      echo "Running tests for $device..."
      #pytest --tb=short -v --device "$device" -k "token_3" --golden_run
      pytest --tb=short -v --device "$device" -s 2>../debug.log
    done
  fi
  exit 0
fi

#make clean

# Build for each device
for device in "${DEVICES[@]}"; do
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

cd tests/ || exit 1

# Run tests for each device
for device in "${DEVICES[@]}"; do
  echo "Running tests for $device..."
  pytest --tb=short -v --device "$device" --golden_run
done
