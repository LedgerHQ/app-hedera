# Nano S
podman run -v $PWD:/app --platform linux/amd64 -e PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=python -e BOLOS_SDK=/opt/nanos-secure-sdk -it ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest make clean;
podman run -v $PWD:/app --platform linux/amd64 -e PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=python -e BOLOS_SDK=/opt/nanos-secure-sdk -it ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest make;
cp bin/app.elf tests/elfs/hedera_nanos.elf;

# Nano X
podman run -v $PWD:/app --platform linux/amd64 -e PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=python -e BOLOS_SDK=/opt/nanox-secure-sdk -it ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest make clean;
podman run -v $PWD:/app --platform linux/amd64 -e PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=python -e BOLOS_SDK=/opt/nanox-secure-sdk -it ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest make;
cp bin/app.elf tests/elfs/hedera_nanox.elf;

# Nano S Plus
podman run -v $PWD:/app --platform linux/amd64 -e PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=python -e BOLOS_SDK=/opt/nanosplus-secure-sdk -it ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest make clean;
podman run -v $PWD:/app --platform linux/amd64 -e PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=python -e BOLOS_SDK=/opt/nanosplus-secure-sdk -it ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest make;
cp bin/app.elf tests/elfs/hedera_nanosp.elf;