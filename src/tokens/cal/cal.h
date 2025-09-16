#ifndef CAL_H
#define CAL_H
#include "token_lookup.h"

extern const token_info_t token_info_table[];
extern const size_t token_info_table_size;

// Zero EVM address constant for unset EVM addresses
#define ERC20_ZERO_ADDRESS {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define HEDERA_ID_ZERO {0,0,0}

#endif // CAL_H 

