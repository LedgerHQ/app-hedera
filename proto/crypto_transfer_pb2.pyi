"""
@generated by mypy-protobuf.  Do not edit manually!
isort:skip_file
"""
import builtins
import collections.abc
import google.protobuf.descriptor
import google.protobuf.internal.containers
import google.protobuf.message
import proto.basic_types_pb2
import sys

if sys.version_info >= (3, 8):
    import typing as typing_extensions
else:
    import typing_extensions

DESCRIPTOR: google.protobuf.descriptor.FileDescriptor

@typing_extensions.final
class CryptoTransferTransactionBody(google.protobuf.message.Message):
    """*
    Transfers cryptocurrency among two or more accounts by making the desired
    adjustments to their balances. Each transfer list can specify up to 10
    adjustments. Each negative amount is withdrawn from the corresponding
    account (a sender), and each positive one is added to the corresponding
    account (a receiver). The amounts list must sum to zero. Each amount is a
    number of tinybars (there are 100,000,000 tinybars in one hbar).  If any
    sender account fails to have sufficient hbars, then the entire transaction
    fails, and none of those transfers occur, though the transaction fee is
    still charged. This transaction must be signed by the keys for all the
    sending accounts, and for any receiving accounts that have
    receiverSigRequired == true. The signatures are in the same order as the
    accounts, skipping those accounts that don't need a signature.
    """

    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    TRANSFERS_FIELD_NUMBER: builtins.int
    TOKENTRANSFERS_FIELD_NUMBER: builtins.int
    @property
    def transfers(self) -> proto.basic_types_pb2.TransferList:
        """*
        The desired hbar balance adjustments
        """
    @property
    def tokenTransfers(self) -> google.protobuf.internal.containers.RepeatedCompositeFieldContainer[proto.basic_types_pb2.TokenTransferList]:
        """*
        The desired token unit balance adjustments; if any custom fees are
        assessed, the ledger will try to deduct them from the payer of this
        CryptoTransfer, resolving the transaction to
        INSUFFICIENT_PAYER_BALANCE_FOR_CUSTOM_FEE if this is not possible
        Limited to 1 here
        """
    def __init__(
        self,
        *,
        transfers: proto.basic_types_pb2.TransferList | None = ...,
        tokenTransfers: collections.abc.Iterable[proto.basic_types_pb2.TokenTransferList] | None = ...,
    ) -> None: ...
    def HasField(self, field_name: typing_extensions.Literal["transfers", b"transfers"]) -> builtins.bool: ...
    def ClearField(self, field_name: typing_extensions.Literal["tokenTransfers", b"tokenTransfers", "transfers", b"transfers"]) -> None: ...

global___CryptoTransferTransactionBody = CryptoTransferTransactionBody
