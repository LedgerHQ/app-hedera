
from ledger_app_clients.exchange.cal_helper import CurrencyConfiguration

from tests.application_client.hedera import HEDERA_PACKED_DERIVATION_PATH, HEDERA_CONF

# Define a configuration for each currency used in our tests: native coins and tokens

HEDERA_CURRENCY_CONFIGURATION = CurrencyConfiguration(ticker="HBAR", conf=HEDERA_CONF, packed_derivation_path=HEDERA_PACKED_DERIVATION_PATH)
