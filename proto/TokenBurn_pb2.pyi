"""
@generated by mypy-protobuf.  Do not edit manually!
isort:skip_file
"""
import builtins
import google.protobuf.descriptor
import google.protobuf.message
import proto.BasicTypes_pb2
import proto.Wrappers_pb2
import sys

if sys.version_info >= (3, 8):
    import typing as typing_extensions
else:
    import typing_extensions

DESCRIPTOR: google.protobuf.descriptor.FileDescriptor

@typing_extensions.final
class HederaTokenBurnTransactionBody(google.protobuf.message.Message):
    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    TOKEN_FIELD_NUMBER: builtins.int
    AMOUNT_FIELD_NUMBER: builtins.int
    EXPECTED_DECIMALS_FIELD_NUMBER: builtins.int
    @property
    def token(self) -> proto.BasicTypes_pb2.HederaTokenID: ...
    amount: builtins.int
    @property
    def expected_decimals(self) -> proto.Wrappers_pb2.UInt32Value:
        """TODO(nft): repeated int64 serialNumbers = 3;"""
    def __init__(
        self,
        *,
        token: proto.BasicTypes_pb2.HederaTokenID | None = ...,
        amount: builtins.int = ...,
        expected_decimals: proto.Wrappers_pb2.UInt32Value | None = ...,
    ) -> None: ...
    def HasField(self, field_name: typing_extensions.Literal["expected_decimals", b"expected_decimals", "token", b"token"]) -> builtins.bool: ...
    def ClearField(self, field_name: typing_extensions.Literal["amount", b"amount", "expected_decimals", b"expected_decimals", "token", b"token"]) -> None: ...

global___HederaTokenBurnTransactionBody = HederaTokenBurnTransactionBody
