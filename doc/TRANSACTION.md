# BOLOK Transaction Serialization

## Overview

The transaction serialization detailed below pertains to the Hedera Hashgraph network. Hedera transactions are structured and cryptographically signed to ensure secure and verifiable transfers of value and messages on the network.

## Amount units

The base unit in Hedera is the HBAR, and the smallest unit used in raw transactions is the tinybar: 1 HBAR = 100,000,000 tinybars.

## Address format

Hedera account addresses are represented in the format shard.realm.account, where each component is a 64-bit integer. This hierarchical structure ensures uniqueness and efficient routing within the network.

## Links

- [Hedera Transaction](https://docs.hedera.com/hedera/sdks-and-apis/sdks/transactions)