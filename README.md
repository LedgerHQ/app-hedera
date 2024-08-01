# Hedera Ledger App

Hedera™ Hashgraph BOLOS application for Ledger devices.

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

Script `build_elfs.sh` just runs these commands in serial for each SDK and copies the ELFs to the tests/elfs directory with the names that the python tests expect

### Check

```
podman run -v $PWD:/app --platform linux/amd64 -it \
    ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest \
    PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=python scan-build --use-cc=clang -analyze-headers -enable-checker security \
    -enable-checker unix -enable-checker valist -o scan-build \
    --status-bugs make default
```

### Tests


### Ledger VS Code extension with macOS / Windows / Linux

To test your app on macOS or Windows, it is recommended to use [Ledger's VS Code extension](https://github.com/LedgerHQ/app-boilerplate#with-vscode) to quickly setup a working test environment.  This extension can also 
be used with Linux.

You can use the following sequence of tasks and commands (all accessible in the **extension sidebar menu**) :

* `Select build target`
* `Build app`

Then you can choose to execute the functional tests :

* Use `Run tests`.

Or simply run the app on the Speculos emulator :

* `Run with Speculos`.

### Alternatives

With the `ledger-app-dev-tools` image, whether you are developing on macOS, Windows or Linux, you can quickly test your app with the [Speculos](https://github.com/LedgerHQ/speculos) emulator or the [Ragger](https://github.com/LedgerHQ/ragger) test framework.
For examples of functional tests implemented with Ragger, you can have a look at the [app-boilerplate](https://github.com/LedgerHQ/app-boilerplate)

First, run the `ledger-app-dev-tools` docker image. Depending on your platform, the command will change slightly :

**Linux (Ubuntu)**

```bash
sudo docker run --rm -ti -v "$(realpath .):/app" --user $(id -u):$(id -g) -v "/tmp/.X11-unix:/tmp/.X11-unix" -e DISPLAY=$DISPLAY ghcr.io/ledgerhq/ledger-app-builder/ledger-app-dev-tools:latest
```

**Windows (with PowerShell)**

Assuming you already have a running X server like [VcXsrv](https://sourceforge.net/projects/vcxsrv/) configured to accept client connections.

```bash
docker run --rm -ti -v "$(Get-Location):/app" -e DISPLAY="host.docker.internal:0" ghcr.io/ledgerhq/ledger-app-builder/ledger-app-dev-tools:latest
```

**macOS**

Assuming you already have a running X server like [XQuartz](https://www.xquartz.org/) configured to accept client connections.

```bash
sudo docker run --rm -ti -v "$(pwd -P):/app" --user $(id -u):$(id -g) -v "/tmp/.X11-unix:/tmp/.X11-unix" -e DISPLAY="host.docker.internal:0" ghcr.io/ledgerhq/ledger-app-builder/ledger-app-dev-tools:latest
```

Then you can test your app either with the Speculos emulator :

```bash
# Run your app on Speculos
bash$ speculos build/nanos/bin/app.elf --model nanos
```

Or you can run your Ragger functional tests if you have implemented them :

```bash
# Creating a virtualenv so that the non-root user can install Python dependencies
bash$ python -m virtualenv venv --system-site-package
bash$ source ./venv/bin/activate
# Install tests dependencies
(venv) bash$ pip install -r tests/requirements.txt
# Run ragger functional tests
(venv) bash$ python -m pytest tests/ --tb=short -v --device nanos --display
```

