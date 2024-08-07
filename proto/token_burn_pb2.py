# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: proto/token_burn.proto
"""Generated protocol buffer code."""
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


import nanopb_pb2 as nanopb__pb2
from proto import basic_types_pb2 as proto_dot_basic__types__pb2


DESCRIPTOR = _descriptor.FileDescriptor(
  name='proto/token_burn.proto',
  package='Hedera',
  syntax='proto3',
  serialized_options=None,
  create_key=_descriptor._internal_create_key,
  serialized_pb=b'\n\x16proto/token_burn.proto\x12\x06Hedera\x1a\x0cnanopb.proto\x1a\x17proto/basic_types.proto\"h\n\x18TokenBurnTransactionBody\x12\x1e\n\x05token\x18\x01 \x01(\x0b\x32\x0f.Hedera.TokenID\x12\x0e\n\x06\x61mount\x18\x02 \x01(\x04\x12\x1c\n\rserialNumbers\x18\x03 \x03(\x03\x42\x05\x92?\x02\x10\x01\x62\x06proto3'
  ,
  dependencies=[nanopb__pb2.DESCRIPTOR,proto_dot_basic__types__pb2.DESCRIPTOR,])




_TOKENBURNTRANSACTIONBODY = _descriptor.Descriptor(
  name='TokenBurnTransactionBody',
  full_name='Hedera.TokenBurnTransactionBody',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='token', full_name='Hedera.TokenBurnTransactionBody.token', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='amount', full_name='Hedera.TokenBurnTransactionBody.amount', index=1,
      number=2, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='serialNumbers', full_name='Hedera.TokenBurnTransactionBody.serialNumbers', index=2,
      number=3, type=3, cpp_type=2, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=b'\222?\002\020\001', file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=73,
  serialized_end=177,
)

_TOKENBURNTRANSACTIONBODY.fields_by_name['token'].message_type = proto_dot_basic__types__pb2._TOKENID
DESCRIPTOR.message_types_by_name['TokenBurnTransactionBody'] = _TOKENBURNTRANSACTIONBODY
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

TokenBurnTransactionBody = _reflection.GeneratedProtocolMessageType('TokenBurnTransactionBody', (_message.Message,), {
  'DESCRIPTOR' : _TOKENBURNTRANSACTIONBODY,
  '__module__' : 'proto.token_burn_pb2'
  # @@protoc_insertion_point(class_scope:Hedera.TokenBurnTransactionBody)
  })
_sym_db.RegisterMessage(TokenBurnTransactionBody)


_TOKENBURNTRANSACTIONBODY.fields_by_name['serialNumbers']._options = None
# @@protoc_insertion_point(module_scope)
