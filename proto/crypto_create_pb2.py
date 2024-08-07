# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: proto/crypto_create.proto
"""Generated protocol buffer code."""
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


import nanopb_pb2 as nanopb__pb2
from proto import basic_types_pb2 as proto_dot_basic__types__pb2
from proto import duration_pb2 as proto_dot_duration__pb2


DESCRIPTOR = _descriptor.FileDescriptor(
  name='proto/crypto_create.proto',
  package='Hedera',
  syntax='proto3',
  serialized_options=None,
  create_key=_descriptor._internal_create_key,
  serialized_pb=b'\n\x19proto/crypto_create.proto\x12\x06Hedera\x1a\x0cnanopb.proto\x1a\x17proto/basic_types.proto\x1a\x14proto/duration.proto\"\xa4\x04\n\x1b\x43ryptoCreateTransactionBody\x12\x18\n\x03key\x18\x01 \x01(\x0b\x32\x0b.Hedera.Key\x12\x16\n\x0einitialBalance\x18\x02 \x01(\x04\x12-\n\x0eproxyAccountID\x18\x03 \x01(\x0b\x32\x11.Hedera.AccountIDB\x02\x18\x01\x12\x1f\n\x13sendRecordThreshold\x18\x06 \x01(\x04\x42\x02\x18\x01\x12\"\n\x16receiveRecordThreshold\x18\x07 \x01(\x04\x42\x02\x18\x01\x12\x1b\n\x13receiverSigRequired\x18\x08 \x01(\x08\x12)\n\x0f\x61utoRenewPeriod\x18\t \x01(\x0b\x32\x10.Hedera.Duration\x12 \n\x07shardID\x18\n \x01(\x0b\x32\x0f.Hedera.ShardID\x12 \n\x07realmID\x18\x0b \x01(\x0b\x32\x0f.Hedera.RealmID\x12%\n\x10newRealmAdminKey\x18\x0c \x01(\x0b\x32\x0b.Hedera.Key\x12\x13\n\x04memo\x18\r \x01(\tB\x05\x92?\x02\x08\x64\x12(\n max_automatic_token_associations\x18\x0e \x01(\x05\x12.\n\x11staked_account_id\x18\x0f \x01(\x0b\x32\x11.Hedera.AccountIDH\x00\x12\x18\n\x0estaked_node_id\x18\x10 \x01(\x03H\x00\x12\x16\n\x0e\x64\x65\x63line_reward\x18\x11 \x01(\x08\x42\x0b\n\tstaked_idb\x06proto3'
  ,
  dependencies=[nanopb__pb2.DESCRIPTOR,proto_dot_basic__types__pb2.DESCRIPTOR,proto_dot_duration__pb2.DESCRIPTOR,])




_CRYPTOCREATETRANSACTIONBODY = _descriptor.Descriptor(
  name='CryptoCreateTransactionBody',
  full_name='Hedera.CryptoCreateTransactionBody',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='key', full_name='Hedera.CryptoCreateTransactionBody.key', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='initialBalance', full_name='Hedera.CryptoCreateTransactionBody.initialBalance', index=1,
      number=2, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='proxyAccountID', full_name='Hedera.CryptoCreateTransactionBody.proxyAccountID', index=2,
      number=3, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=b'\030\001', file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='sendRecordThreshold', full_name='Hedera.CryptoCreateTransactionBody.sendRecordThreshold', index=3,
      number=6, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=b'\030\001', file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='receiveRecordThreshold', full_name='Hedera.CryptoCreateTransactionBody.receiveRecordThreshold', index=4,
      number=7, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=b'\030\001', file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='receiverSigRequired', full_name='Hedera.CryptoCreateTransactionBody.receiverSigRequired', index=5,
      number=8, type=8, cpp_type=7, label=1,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='autoRenewPeriod', full_name='Hedera.CryptoCreateTransactionBody.autoRenewPeriod', index=6,
      number=9, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='shardID', full_name='Hedera.CryptoCreateTransactionBody.shardID', index=7,
      number=10, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='realmID', full_name='Hedera.CryptoCreateTransactionBody.realmID', index=8,
      number=11, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='newRealmAdminKey', full_name='Hedera.CryptoCreateTransactionBody.newRealmAdminKey', index=9,
      number=12, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='memo', full_name='Hedera.CryptoCreateTransactionBody.memo', index=10,
      number=13, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=b'\222?\002\010d', file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='max_automatic_token_associations', full_name='Hedera.CryptoCreateTransactionBody.max_automatic_token_associations', index=11,
      number=14, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='staked_account_id', full_name='Hedera.CryptoCreateTransactionBody.staked_account_id', index=12,
      number=15, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='staked_node_id', full_name='Hedera.CryptoCreateTransactionBody.staked_node_id', index=13,
      number=16, type=3, cpp_type=2, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='decline_reward', full_name='Hedera.CryptoCreateTransactionBody.decline_reward', index=14,
      number=17, type=8, cpp_type=7, label=1,
      has_default_value=False, default_value=False,
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
    _descriptor.OneofDescriptor(
      name='staked_id', full_name='Hedera.CryptoCreateTransactionBody.staked_id',
      index=0, containing_type=None,
      create_key=_descriptor._internal_create_key,
    fields=[]),
  ],
  serialized_start=99,
  serialized_end=647,
)

_CRYPTOCREATETRANSACTIONBODY.fields_by_name['key'].message_type = proto_dot_basic__types__pb2._KEY
_CRYPTOCREATETRANSACTIONBODY.fields_by_name['proxyAccountID'].message_type = proto_dot_basic__types__pb2._ACCOUNTID
_CRYPTOCREATETRANSACTIONBODY.fields_by_name['autoRenewPeriod'].message_type = proto_dot_duration__pb2._DURATION
_CRYPTOCREATETRANSACTIONBODY.fields_by_name['shardID'].message_type = proto_dot_basic__types__pb2._SHARDID
_CRYPTOCREATETRANSACTIONBODY.fields_by_name['realmID'].message_type = proto_dot_basic__types__pb2._REALMID
_CRYPTOCREATETRANSACTIONBODY.fields_by_name['newRealmAdminKey'].message_type = proto_dot_basic__types__pb2._KEY
_CRYPTOCREATETRANSACTIONBODY.fields_by_name['staked_account_id'].message_type = proto_dot_basic__types__pb2._ACCOUNTID
_CRYPTOCREATETRANSACTIONBODY.oneofs_by_name['staked_id'].fields.append(
  _CRYPTOCREATETRANSACTIONBODY.fields_by_name['staked_account_id'])
_CRYPTOCREATETRANSACTIONBODY.fields_by_name['staked_account_id'].containing_oneof = _CRYPTOCREATETRANSACTIONBODY.oneofs_by_name['staked_id']
_CRYPTOCREATETRANSACTIONBODY.oneofs_by_name['staked_id'].fields.append(
  _CRYPTOCREATETRANSACTIONBODY.fields_by_name['staked_node_id'])
_CRYPTOCREATETRANSACTIONBODY.fields_by_name['staked_node_id'].containing_oneof = _CRYPTOCREATETRANSACTIONBODY.oneofs_by_name['staked_id']
DESCRIPTOR.message_types_by_name['CryptoCreateTransactionBody'] = _CRYPTOCREATETRANSACTIONBODY
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

CryptoCreateTransactionBody = _reflection.GeneratedProtocolMessageType('CryptoCreateTransactionBody', (_message.Message,), {
  'DESCRIPTOR' : _CRYPTOCREATETRANSACTIONBODY,
  '__module__' : 'proto.crypto_create_pb2'
  # @@protoc_insertion_point(class_scope:Hedera.CryptoCreateTransactionBody)
  })
_sym_db.RegisterMessage(CryptoCreateTransactionBody)


_CRYPTOCREATETRANSACTIONBODY.fields_by_name['proxyAccountID']._options = None
_CRYPTOCREATETRANSACTIONBODY.fields_by_name['sendRecordThreshold']._options = None
_CRYPTOCREATETRANSACTIONBODY.fields_by_name['receiveRecordThreshold']._options = None
_CRYPTOCREATETRANSACTIONBODY.fields_by_name['memo']._options = None
# @@protoc_insertion_point(module_scope)
