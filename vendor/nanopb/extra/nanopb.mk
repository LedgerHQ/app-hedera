# This is an include file for Makefiles. It provides rules for building
# .pb.c and .pb.h files out of .proto, as well the path to nanopb core.

# Path to the nanopb root directory
NANOPB_DIR := $(patsubst %/,%,$(dir $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))))

# Files for the nanopb core
NANOPB_CORE = $(NANOPB_DIR)/pb_encode.c $(NANOPB_DIR)/pb_decode.c $(NANOPB_DIR)/pb_common.c

# Check if we are running on Windows
ifdef windir
WINDOWS = 1
endif
ifdef WINDIR
WINDOWS = 1
endif

# Check whether to use binary version of nanopb_generator or the
# system-supplied python interpreter.
ifneq "$(wildcard $(NANOPB_DIR)/generator-bin)" ""
	# Binary package
	PROTOC = $(NANOPB_DIR)/generator-bin/protoc
	PROTOC_OPTS =
else
	# Source only or git checkout
	PROTOC_OPTS =
	ifdef WINDOWS
	    PROTOC = python $(NANOPB_DIR)/generator/protoc
	else
	    PROTOC = $(NANOPB_DIR)/generator/protoc
	endif
endif

########################################
#      Protobuf files regeneration     #
########################################
.PHONY: proto
proto:
	@echo "Generating protobuf files..."
	@for proto_file in $(PB_FILES) ; do \
		echo "Processing $$proto_file..." ; \
		$(PROTOC) $(PROTOC_OPTS) --nanopb_out=. $$proto_file ; \
		$(PROTOC) $(PROTOC_OPTS) --python_out=. $$proto_file ; \
	done
	@echo "Protobuf generation complete."
