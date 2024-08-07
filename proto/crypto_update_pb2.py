# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: proto/crypto_update.proto
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
from proto import timestamp_pb2 as proto_dot_timestamp__pb2
from proto import wrappers_pb2 as proto_dot_wrappers__pb2


DESCRIPTOR = _descriptor.FileDescriptor(
  name='proto/crypto_update.proto',
  package='Hedera',
  syntax='proto3',
  serialized_options=None,
  create_key=_descriptor._internal_create_key,
  serialized_pb=b'\n\x19proto/crypto_update.proto\x12\x06Hedera\x1a\x0cnanopb.proto\x1a\x17proto/basic_types.proto\x1a\x14proto/duration.proto\x1a\x15proto/timestamp.proto\x1a\x14proto/wrappers.proto\"\xe5\x06\n\x1b\x43ryptoUpdateTransactionBody\x12,\n\x11\x61\x63\x63ountIDToUpdate\x18\x02 \x01(\x0b\x32\x11.Hedera.AccountID\x12\x18\n\x03key\x18\x03 \x01(\x0b\x32\x0b.Hedera.Key\x12-\n\x0eproxyAccountID\x18\x04 \x01(\x0b\x32\x11.Hedera.AccountIDB\x02\x18\x01\x12\x19\n\rproxyFraction\x18\x05 \x01(\x05\x42\x02\x18\x01\x12!\n\x13sendRecordThreshold\x18\x06 \x01(\x04\x42\x02\x18\x01H\x00\x12=\n\x1asendRecordThresholdWrapper\x18\x0b \x01(\x0b\x32\x13.Hedera.UInt64ValueB\x02\x18\x01H\x00\x12$\n\x16receiveRecordThreshold\x18\x07 \x01(\x04\x42\x02\x18\x01H\x01\x12@\n\x1dreceiveRecordThresholdWrapper\x18\x0c \x01(\x0b\x32\x13.Hedera.UInt64ValueB\x02\x18\x01H\x01\x12)\n\x0f\x61utoRenewPeriod\x18\x08 \x01(\x0b\x32\x10.Hedera.Duration\x12)\n\x0e\x65xpirationTime\x18\t \x01(\x0b\x32\x11.Hedera.Timestamp\x12!\n\x13receiverSigRequired\x18\n \x01(\x08\x42\x02\x18\x01H\x02\x12\x37\n\x1areceiverSigRequiredWrapper\x18\r \x01(\x0b\x32\x11.Hedera.BoolValueH\x02\x12!\n\x04memo\x18\x0e \x01(\x0b\x32\x13.Hedera.StringValue\x12<\n max_automatic_token_associations\x18\x0f \x01(\x0b\x32\x12.Hedera.Int32Value\x12.\n\x11staked_account_id\x18\x10 \x01(\x0b\x32\x11.Hedera.AccountIDH\x03\x12\x18\n\x0estaked_node_id\x18\x11 \x01(\x03H\x03\x12)\n\x0e\x64\x65\x63line_reward\x18\x12 \x01(\x0b\x32\x11.Hedera.BoolValueB\x1a\n\x18sendRecordThresholdFieldB\x1d\n\x1breceiveRecordThresholdFieldB\x1a\n\x18receiverSigRequiredFieldB\x0b\n\tstaked_idb\x06proto3'
  ,
  dependencies=[nanopb__pb2.DESCRIPTOR,proto_dot_basic__types__pb2.DESCRIPTOR,proto_dot_duration__pb2.DESCRIPTOR,proto_dot_timestamp__pb2.DESCRIPTOR,proto_dot_wrappers__pb2.DESCRIPTOR,])




_CRYPTOUPDATETRANSACTIONBODY = _descriptor.Descriptor(
  name='CryptoUpdateTransactionBody',
  full_name='Hedera.CryptoUpdateTransactionBody',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='accountIDToUpdate', full_name='Hedera.CryptoUpdateTransactionBody.accountIDToUpdate', index=0,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='key', full_name='Hedera.CryptoUpdateTransactionBody.key', index=1,
      number=3, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='proxyAccountID', full_name='Hedera.CryptoUpdateTransactionBody.proxyAccountID', index=2,
      number=4, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=b'\030\001', file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='proxyFraction', full_name='Hedera.CryptoUpdateTransactionBody.proxyFraction', index=3,
      number=5, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=b'\030\001', file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='sendRecordThreshold', full_name='Hedera.CryptoUpdateTransactionBody.sendRecordThreshold', index=4,
      number=6, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=b'\030\001', file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='sendRecordThresholdWrapper', full_name='Hedera.CryptoUpdateTransactionBody.sendRecordThresholdWrapper', index=5,
      number=11, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=b'\030\001', file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='receiveRecordThreshold', full_name='Hedera.CryptoUpdateTransactionBody.receiveRecordThreshold', index=6,
      number=7, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=b'\030\001', file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='receiveRecordThresholdWrapper', full_name='Hedera.CryptoUpdateTransactionBody.receiveRecordThresholdWrapper', index=7,
      number=12, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=b'\030\001', file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='autoRenewPeriod', full_name='Hedera.CryptoUpdateTransactionBody.autoRenewPeriod', index=8,
      number=8, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='expirationTime', full_name='Hedera.CryptoUpdateTransactionBody.expirationTime', index=9,
      number=9, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='receiverSigRequired', full_name='Hedera.CryptoUpdateTransactionBody.receiverSigRequired', index=10,
      number=10, type=8, cpp_type=7, label=1,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=b'\030\001', file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='receiverSigRequiredWrapper', full_name='Hedera.CryptoUpdateTransactionBody.receiverSigRequiredWrapper', index=11,
      number=13, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='memo', full_name='Hedera.CryptoUpdateTransactionBody.memo', index=12,
      number=14, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='max_automatic_token_associations', full_name='Hedera.CryptoUpdateTransactionBody.max_automatic_token_associations', index=13,
      number=15, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='staked_account_id', full_name='Hedera.CryptoUpdateTransactionBody.staked_account_id', index=14,
      number=16, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='staked_node_id', full_name='Hedera.CryptoUpdateTransactionBody.staked_node_id', index=15,
      number=17, type=3, cpp_type=2, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='decline_reward', full_name='Hedera.CryptoUpdateTransactionBody.decline_reward', index=16,
      number=18, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
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
      name='sendRecordThresholdField', full_name='Hedera.CryptoUpdateTransactionBody.sendRecordThresholdField',
      index=0, containing_type=None,
      create_key=_descriptor._internal_create_key,
    fields=[]),
    _descriptor.OneofDescriptor(
      name='receiveRecordThresholdField', full_name='Hedera.CryptoUpdateTransactionBody.receiveRecordThresholdField',
      index=1, containing_type=None,
      create_key=_descriptor._internal_create_key,
    fields=[]),
    _descriptor.OneofDescriptor(
      name='receiverSigRequiredField', full_name='Hedera.CryptoUpdateTransactionBody.receiverSigRequiredField',
      index=2, containing_type=None,
      create_key=_descriptor._internal_create_key,
    fields=[]),
    _descriptor.OneofDescriptor(
      name='staked_id', full_name='Hedera.CryptoUpdateTransactionBody.staked_id',
      index=3, containing_type=None,
      create_key=_descriptor._internal_create_key,
    fields=[]),
  ],
  serialized_start=144,
  serialized_end=1013,
)

_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['accountIDToUpdate'].message_type = proto_dot_basic__types__pb2._ACCOUNTID
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['key'].message_type = proto_dot_basic__types__pb2._KEY
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['proxyAccountID'].message_type = proto_dot_basic__types__pb2._ACCOUNTID
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['sendRecordThresholdWrapper'].message_type = proto_dot_wrappers__pb2._UINT64VALUE
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['receiveRecordThresholdWrapper'].message_type = proto_dot_wrappers__pb2._UINT64VALUE
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['autoRenewPeriod'].message_type = proto_dot_duration__pb2._DURATION
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['expirationTime'].message_type = proto_dot_timestamp__pb2._TIMESTAMP
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['receiverSigRequiredWrapper'].message_type = proto_dot_wrappers__pb2._BOOLVALUE
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['memo'].message_type = proto_dot_wrappers__pb2._STRINGVALUE
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['max_automatic_token_associations'].message_type = proto_dot_wrappers__pb2._INT32VALUE
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['staked_account_id'].message_type = proto_dot_basic__types__pb2._ACCOUNTID
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['decline_reward'].message_type = proto_dot_wrappers__pb2._BOOLVALUE
_CRYPTOUPDATETRANSACTIONBODY.oneofs_by_name['sendRecordThresholdField'].fields.append(
  _CRYPTOUPDATETRANSACTIONBODY.fields_by_name['sendRecordThreshold'])
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['sendRecordThreshold'].containing_oneof = _CRYPTOUPDATETRANSACTIONBODY.oneofs_by_name['sendRecordThresholdField']
_CRYPTOUPDATETRANSACTIONBODY.oneofs_by_name['sendRecordThresholdField'].fields.append(
  _CRYPTOUPDATETRANSACTIONBODY.fields_by_name['sendRecordThresholdWrapper'])
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['sendRecordThresholdWrapper'].containing_oneof = _CRYPTOUPDATETRANSACTIONBODY.oneofs_by_name['sendRecordThresholdField']
_CRYPTOUPDATETRANSACTIONBODY.oneofs_by_name['receiveRecordThresholdField'].fields.append(
  _CRYPTOUPDATETRANSACTIONBODY.fields_by_name['receiveRecordThreshold'])
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['receiveRecordThreshold'].containing_oneof = _CRYPTOUPDATETRANSACTIONBODY.oneofs_by_name['receiveRecordThresholdField']
_CRYPTOUPDATETRANSACTIONBODY.oneofs_by_name['receiveRecordThresholdField'].fields.append(
  _CRYPTOUPDATETRANSACTIONBODY.fields_by_name['receiveRecordThresholdWrapper'])
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['receiveRecordThresholdWrapper'].containing_oneof = _CRYPTOUPDATETRANSACTIONBODY.oneofs_by_name['receiveRecordThresholdField']
_CRYPTOUPDATETRANSACTIONBODY.oneofs_by_name['receiverSigRequiredField'].fields.append(
  _CRYPTOUPDATETRANSACTIONBODY.fields_by_name['receiverSigRequired'])
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['receiverSigRequired'].containing_oneof = _CRYPTOUPDATETRANSACTIONBODY.oneofs_by_name['receiverSigRequiredField']
_CRYPTOUPDATETRANSACTIONBODY.oneofs_by_name['receiverSigRequiredField'].fields.append(
  _CRYPTOUPDATETRANSACTIONBODY.fields_by_name['receiverSigRequiredWrapper'])
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['receiverSigRequiredWrapper'].containing_oneof = _CRYPTOUPDATETRANSACTIONBODY.oneofs_by_name['receiverSigRequiredField']
_CRYPTOUPDATETRANSACTIONBODY.oneofs_by_name['staked_id'].fields.append(
  _CRYPTOUPDATETRANSACTIONBODY.fields_by_name['staked_account_id'])
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['staked_account_id'].containing_oneof = _CRYPTOUPDATETRANSACTIONBODY.oneofs_by_name['staked_id']
_CRYPTOUPDATETRANSACTIONBODY.oneofs_by_name['staked_id'].fields.append(
  _CRYPTOUPDATETRANSACTIONBODY.fields_by_name['staked_node_id'])
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['staked_node_id'].containing_oneof = _CRYPTOUPDATETRANSACTIONBODY.oneofs_by_name['staked_id']
DESCRIPTOR.message_types_by_name['CryptoUpdateTransactionBody'] = _CRYPTOUPDATETRANSACTIONBODY
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

CryptoUpdateTransactionBody = _reflection.GeneratedProtocolMessageType('CryptoUpdateTransactionBody', (_message.Message,), {
  'DESCRIPTOR' : _CRYPTOUPDATETRANSACTIONBODY,
  '__module__' : 'proto.crypto_update_pb2'
  # @@protoc_insertion_point(class_scope:Hedera.CryptoUpdateTransactionBody)
  })
_sym_db.RegisterMessage(CryptoUpdateTransactionBody)


_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['proxyAccountID']._options = None
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['proxyFraction']._options = None
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['sendRecordThreshold']._options = None
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['sendRecordThresholdWrapper']._options = None
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['receiveRecordThreshold']._options = None
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['receiveRecordThresholdWrapper']._options = None
_CRYPTOUPDATETRANSACTIONBODY.fields_by_name['receiverSigRequired']._options = None
# @@protoc_insertion_point(module_scope)
