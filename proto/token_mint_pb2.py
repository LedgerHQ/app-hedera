# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: proto/token_mint.proto
"""Generated protocol buffer code."""
from google.protobuf.internal import builder as _builder
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


import nanopb_pb2 as nanopb__pb2
from proto import basic_types_pb2 as proto_dot_basic__types__pb2


DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\x16proto/token_mint.proto\x12\x06Hedera\x1a\x0cnanopb.proto\x1a\x17proto/basic_types.proto\"h\n\x18TokenMintTransactionBody\x12\x1e\n\x05token\x18\x01 \x01(\x0b\x32\x0f.Hedera.TokenID\x12\x0e\n\x06\x61mount\x18\x02 \x01(\x04\x12\x1c\n\x08metadata\x18\x03 \x03(\x0c\x42\n\x92?\x02\x08\x64\x92?\x02\x10\x01\x62\x06proto3')

_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, globals())
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'proto.token_mint_pb2', globals())
if _descriptor._USE_C_DESCRIPTORS == False:

  DESCRIPTOR._options = None
  _TOKENMINTTRANSACTIONBODY.fields_by_name['metadata']._options = None
  _TOKENMINTTRANSACTIONBODY.fields_by_name['metadata']._serialized_options = b'\222?\002\010d\222?\002\020\001'
  _TOKENMINTTRANSACTIONBODY._serialized_start=73
  _TOKENMINTTRANSACTIONBODY._serialized_end=177
# @@protoc_insertion_point(module_scope)
