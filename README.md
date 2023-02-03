# Hedera Ledger App

Hedera™ Hashgraph BOLOS application for Ledger Nano S, Nano S Plus, and Nano X.

## Development

- Clang features for formatting
- Ledger App Builder image for dependencies
- VS Code project settings included

### Prerequisites

- Podman
- Clang
- Tests: Python3, PDM

### Protos

```
podman run -v $PWD:/app --platform linux/amd64 -e PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=python -e BOLOS_SDK=/opt/nanos-secure-sdk -it ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest make c_pb
```

### Python Proto Type Definitions

You'll need to modify the ledger-app-builder image to install `protoc-gen-mypy` with pip

```
podman run -v $PWD:/app --platform linux/amd64 -e PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=python -e BOLOS_SDK=/opt/nanos-secure-sdk -it ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest make python_pb
```

or... you can temporarily install mypy-protobuf and use it to make the python typehints

```
podman run -v $PWD:/app --platform linux/amd64 -e PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=python -e BOLOS_SDK=/opt/nanos-secure-sdk -it ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest bash

bash-5.1# pip install mypy-protobuf
bash-5.1# make python_pb
```

### Build

```
podman run -v $PWD:/app --platform linux/amd64 -e PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=python -e BOLOS_SDK=/opt/nanos-secure-sdk -it ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest make
```

### Check

```
podman run -v $PWD:/app --platform linux/amd64 -it \
    ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest \
    PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=python scan-build --use-cc=clang -analyze-headers -enable-checker security \
    -enable-checker unix -enable-checker valist -o scan-build \
    --status-bugs make default
```

### Tests

```
# Install PDM and Python dependencies
sudo apt-get install gcc python3 python3-venv python3-dev
sudo apt-get update && sudo apt-get install qemu-user-static
curl -sSL https://raw.githubusercontent.com/pdm-project/pdm/main/install-pdm.py | python3 -

# Link and Copy dependencies
ln -s $(pwd)/proto $(pwd)/tests/proto
ln -s $(pwd)/vendor/nanopb/generator/proto/nanopb_pb2.py $(pwd)/tests/nanopb_pb2.py
mkdir -p tests/elfs
cp bin/app.elf tests/elfs/hedera_nanos.elf  # hedera_nanosplus.elf, hedera_nanox.elf

# Run tests
cd tests
pdm use 3.8
pdm install # Includes https://github.com/nipunn1313/mypy-protobuf here too
pdm run pytest -v --tb=short --nanos --display

```
