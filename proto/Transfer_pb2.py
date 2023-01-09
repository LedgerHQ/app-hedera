# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: proto/Transfer.proto
"""Generated protocol buffer code."""
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


import nanopb_pb2 as nanopb__pb2
from proto import Wrappers_pb2 as proto_dot_Wrappers__pb2
from proto import BasicTypes_pb2 as proto_dot_BasicTypes__pb2


DESCRIPTOR = _descriptor.FileDescriptor(
  name='proto/Transfer.proto',
  package='',
  syntax='proto3',
  serialized_options=None,
  create_key=_descriptor._internal_create_key,
  serialized_pb=b'\n\x14proto/Transfer.proto\x1a\x0cnanopb.proto\x1a\x14proto/Wrappers.proto\x1a\x16proto/BasicTypes.proto\"J\n\x13HederaAccountAmount\x12#\n\taccountID\x18\x01 \x01(\x0b\x32\x10.HederaAccountID\x12\x0e\n\x06\x61mount\x18\x02 \x01(\x12\"I\n\x12HederaTransferList\x12\x33\n\x0e\x61\x63\x63ountAmounts\x18\x01 \x03(\x0b\x32\x14.HederaAccountAmountB\x05\x92?\x02\x10\x02\"\x91\x01\n\x17HederaTokenTransferList\x12\x1d\n\x05token\x18\x01 \x01(\x0b\x32\x0e.HederaTokenID\x12.\n\ttransfers\x18\x02 \x03(\x0b\x32\x14.HederaAccountAmountB\x05\x92?\x02\x10\x02\x12\'\n\x11\x65xpected_decimals\x18\x04 \x01(\x0b\x32\x0c.UInt32Value\"\x86\x01\n#HederaCryptoTransferTransactionBody\x12&\n\ttransfers\x18\x01 \x01(\x0b\x32\x13.HederaTransferList\x12\x37\n\x0etokenTransfers\x18\x02 \x03(\x0b\x32\x18.HederaTokenTransferListB\x05\x92?\x02\x10\x01\x62\x06proto3'
  ,
  dependencies=[nanopb__pb2.DESCRIPTOR,proto_dot_Wrappers__pb2.DESCRIPTOR,proto_dot_BasicTypes__pb2.DESCRIPTOR,])




_HEDERAACCOUNTAMOUNT = _descriptor.Descriptor(
  name='HederaAccountAmount',
  full_name='HederaAccountAmount',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='accountID', full_name='HederaAccountAmount.accountID', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='amount', full_name='HederaAccountAmount.amount', index=1,
      number=2, type=18, cpp_type=2, label=1,
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
  serialized_start=84,
  serialized_end=158,
)


_HEDERATRANSFERLIST = _descriptor.Descriptor(
  name='HederaTransferList',
  full_name='HederaTransferList',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='accountAmounts', full_name='HederaTransferList.accountAmounts', index=0,
      number=1, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=b'\222?\002\020\002', file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
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
  serialized_start=160,
  serialized_end=233,
)


_HEDERATOKENTRANSFERLIST = _descriptor.Descriptor(
  name='HederaTokenTransferList',
  full_name='HederaTokenTransferList',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='token', full_name='HederaTokenTransferList.token', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='transfers', full_name='HederaTokenTransferList.transfers', index=1,
      number=2, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=b'\222?\002\020\002', file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='expected_decimals', full_name='HederaTokenTransferList.expected_decimals', index=2,
      number=4, type=11, cpp_type=10, label=1,
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
  ],
  serialized_start=236,
  serialized_end=381,
)


_HEDERACRYPTOTRANSFERTRANSACTIONBODY = _descriptor.Descriptor(
  name='HederaCryptoTransferTransactionBody',
  full_name='HederaCryptoTransferTransactionBody',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='transfers', full_name='HederaCryptoTransferTransactionBody.transfers', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='tokenTransfers', full_name='HederaCryptoTransferTransactionBody.tokenTransfers', index=1,
      number=2, type=11, cpp_type=10, label=3,
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
  serialized_start=384,
  serialized_end=518,
)

_HEDERAACCOUNTAMOUNT.fields_by_name['accountID'].message_type = proto_dot_BasicTypes__pb2._HEDERAACCOUNTID
_HEDERATRANSFERLIST.fields_by_name['accountAmounts'].message_type = _HEDERAACCOUNTAMOUNT
_HEDERATOKENTRANSFERLIST.fields_by_name['token'].message_type = proto_dot_BasicTypes__pb2._HEDERATOKENID
_HEDERATOKENTRANSFERLIST.fields_by_name['transfers'].message_type = _HEDERAACCOUNTAMOUNT
_HEDERATOKENTRANSFERLIST.fields_by_name['expected_decimals'].message_type = proto_dot_Wrappers__pb2._UINT32VALUE
_HEDERACRYPTOTRANSFERTRANSACTIONBODY.fields_by_name['transfers'].message_type = _HEDERATRANSFERLIST
_HEDERACRYPTOTRANSFERTRANSACTIONBODY.fields_by_name['tokenTransfers'].message_type = _HEDERATOKENTRANSFERLIST
DESCRIPTOR.message_types_by_name['HederaAccountAmount'] = _HEDERAACCOUNTAMOUNT
DESCRIPTOR.message_types_by_name['HederaTransferList'] = _HEDERATRANSFERLIST
DESCRIPTOR.message_types_by_name['HederaTokenTransferList'] = _HEDERATOKENTRANSFERLIST
DESCRIPTOR.message_types_by_name['HederaCryptoTransferTransactionBody'] = _HEDERACRYPTOTRANSFERTRANSACTIONBODY
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

HederaAccountAmount = _reflection.GeneratedProtocolMessageType('HederaAccountAmount', (_message.Message,), {
  'DESCRIPTOR' : _HEDERAACCOUNTAMOUNT,
  '__module__' : 'proto.Transfer_pb2'
  # @@protoc_insertion_point(class_scope:HederaAccountAmount)
  })
_sym_db.RegisterMessage(HederaAccountAmount)

HederaTransferList = _reflection.GeneratedProtocolMessageType('HederaTransferList', (_message.Message,), {
  'DESCRIPTOR' : _HEDERATRANSFERLIST,
  '__module__' : 'proto.Transfer_pb2'
  # @@protoc_insertion_point(class_scope:HederaTransferList)
  })
_sym_db.RegisterMessage(HederaTransferList)

HederaTokenTransferList = _reflection.GeneratedProtocolMessageType('HederaTokenTransferList', (_message.Message,), {
  'DESCRIPTOR' : _HEDERATOKENTRANSFERLIST,
  '__module__' : 'proto.Transfer_pb2'
  # @@protoc_insertion_point(class_scope:HederaTokenTransferList)
  })
_sym_db.RegisterMessage(HederaTokenTransferList)

HederaCryptoTransferTransactionBody = _reflection.GeneratedProtocolMessageType('HederaCryptoTransferTransactionBody', (_message.Message,), {
  'DESCRIPTOR' : _HEDERACRYPTOTRANSFERTRANSACTIONBODY,
  '__module__' : 'proto.Transfer_pb2'
  # @@protoc_insertion_point(class_scope:HederaCryptoTransferTransactionBody)
  })
_sym_db.RegisterMessage(HederaCryptoTransferTransactionBody)


_HEDERATRANSFERLIST.fields_by_name['accountAmounts']._options = None
_HEDERATOKENTRANSFERLIST.fields_by_name['transfers']._options = None
_HEDERACRYPTOTRANSFERTRANSACTIONBODY.fields_by_name['tokenTransfers']._options = None
# @@protoc_insertion_point(module_scope)
