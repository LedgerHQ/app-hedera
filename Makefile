#*******************************************************************************
#   Ledger App Hedera
#   (c) 2024 Hedera Hashgraph
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#*******************************************************************************

ifeq ($(BOLOS_SDK),)
$(error Environment variable BOLOS_SDK is not set)
endif

include $(BOLOS_SDK)/Makefile.defines

########################################
#        Mandatory configuration       #
########################################
# Application name
APPNAME = Hedera

# Application version
APPVERSION_M = 1
APPVERSION_N = 5
APPVERSION_P = 0
APPVERSION = "$(APPVERSION_M).$(APPVERSION_N).$(APPVERSION_P)"

# Application source files
APP_SOURCE_PATH  += src proto
SDK_SOURCE_PATH  += lib_u2f

# Application icons
ICON_NANOS = icons/nanos_app_hedera.gif
ICON_NANOX = icons/nanox_app_hedera.gif
ICON_NANOSP = icons/nanox_app_hedera.gif
ICON_STAX = icons/stax_app_hedera.gif
ICON_FLEX = icons/flex_app_hedera.gif

# Application allowed derivation curves.
CURVE_APP_LOAD_PARAMS = ed25519

# Application allowed derivation paths.
PATH_APP_LOAD_PARAMS = "44'/3030'"   # purpose=coin(44) / coin_type=Hedera HBAR(3030)

VARIANT_PARAM = COIN
VARIANT_VALUES = hedera

# Enabling DEBUG flag will enable PRINTF and disable optimizations
#DEBUG = 1

########################################
#     Application custom permissions   #
########################################
HAVE_APPLICATION_FLAG_BOLOS_SETTINGS = 1

########################################
# Application communication interfaces #
########################################
ENABLE_BLUETOOTH = 1

########################################
#         NBGL custom features         #
########################################
ENABLE_NBGL_QRCODE = 1

########################################
#          Features disablers          #
########################################
# These advanced settings allow to disable some feature that are by
# default enabled in the SDK `Makefile.standard_app`.
DISABLE_STANDARD_APP_FILES = 1

########################################
#          App specific configuration  #
########################################
ifeq ($(TARGET_NAME),TARGET_NANOS)
DISABLE_STANDARD_BAGL_UX_FLOW = 1
endif

# vendor/printf
DEFINES   += PRINTF_DISABLE_SUPPORT_FLOAT PRINTF_DISABLE_SUPPORT_EXPONENTIAL PRINTF_DISABLE_SUPPORT_PTRDIFF_T
DEFINES   += PRINTF_FTOA_BUFFER_SIZE=0

# U2F
DEFINES   += HAVE_U2F HAVE_IO_U2F
DEFINES   += U2F_PROXY_MAGIC=\"BOIL\"

# Allow usage of function from lib_standard_app/crypto_helpers.c
APP_SOURCE_FILES += ${BOLOS_SDK}/lib_standard_app/crypto_helpers.c

# Additional include paths
INCLUDES_PATH += ${BOLOS_SDK}/lib_standard_app $(NANOPB_DIR) .

include vendor/nanopb/extra/nanopb.mk

DEFINES   += PB_NO_ERRMSG=1
SOURCE_FILES += $(NANOPB_CORE)

PB_FILES = $(wildcard proto/*.proto)
C_PB_FILES = $(patsubst %.proto,%.pb.c,$(PB_FILES))
PYTHON_PB_FILES = $(patsubst %.proto,%_pb2.py,$(PB_FILES))

# Build rule for C proto files
SOURCE_FILES += $(C_PB_FILES)
$(C_PB_FILES): %.pb.c: $(PB_FILES)
	$(PROTOC) $(PROTOC_OPTS) --nanopb_out=. $*.proto

# Build rule for Python proto files
$(PYTHON_PB_FILES): %_pb2.py: $(PB_FILES)
	$(PROTOC) $(PROTOC_OPTS) --python_out=. $*.proto

.PHONY: c_pb python_pb clean_python_pb
c_pb: $(C_PB_FILES)
python_pb: $(PYTHON_PB_FILES)
clean_python_pb:
	rm -f $(PYTHON_PB_FILES)

# target to also clean generated proto c files
.SILENT : cleanall
cleanall : clean
	-@rm -rf proto/*.pb.c proto/*.pb.h

check:
	@ clang-tidy \
		$(foreach path, $(APP_SOURCE_PATH), $(shell find $(path) -name "*.c" -and -not -name "pb*" -and -not -name "glyphs*")) -- \
		$(CFLAGS) \
		$(addprefix -D, $(DEFINES)) \
		$(addprefix -I, $(INCLUDES_PATH))

include $(BOLOS_SDK)/Makefile.standard_app