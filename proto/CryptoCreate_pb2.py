# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: proto/CryptoCreate.proto
"""Generated protocol buffer code."""
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='proto/CryptoCreate.proto',
  package='',
  syntax='proto3',
  serialized_options=None,
  create_key=_descriptor._internal_create_key,
  serialized_pb=b'\n\x18proto/CryptoCreate.proto\";\n!HederaCryptoCreateTransactionBody\x12\x16\n\x0einitialBalance\x18\x02 \x01(\x04\x62\x06proto3'
)




_HEDERACRYPTOCREATETRANSACTIONBODY = _descriptor.Descriptor(
  name='HederaCryptoCreateTransactionBody',
  full_name='HederaCryptoCreateTransactionBody',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='initialBalance', full_name='HederaCryptoCreateTransactionBody.initialBalance', index=0,
      number=2, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
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
  serialized_start=28,
  serialized_end=87,
)

DESCRIPTOR.message_types_by_name['HederaCryptoCreateTransactionBody'] = _HEDERACRYPTOCREATETRANSACTIONBODY
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

HederaCryptoCreateTransactionBody = _reflection.GeneratedProtocolMessageType('HederaCryptoCreateTransactionBody', (_message.Message,), {
  'DESCRIPTOR' : _HEDERACRYPTOCREATETRANSACTIONBODY,
  '__module__' : 'proto.CryptoCreate_pb2'
  # @@protoc_insertion_point(class_scope:HederaCryptoCreateTransactionBody)
  })
_sym_db.RegisterMessage(HederaCryptoCreateTransactionBody)


# @@protoc_insertion_point(module_scope)
