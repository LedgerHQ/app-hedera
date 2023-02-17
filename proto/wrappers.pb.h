/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.5 */

#ifndef PB_HEDERA_PROTO_WRAPPERS_PB_H_INCLUDED
#define PB_HEDERA_PROTO_WRAPPERS_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
/* Wrapper message for `string`.

 The JSON representation for `StringValue` is JSON string. */
typedef struct _Hedera_StringValue { 
    /* The string value. */
    pb_callback_t value; 
} Hedera_StringValue;

/* Wrapper message for `bool`.

 The JSON representation for `BoolValue` is JSON `true` and `false`. */
typedef struct _Hedera_BoolValue { 
    /* The bool value. */
    bool value; 
} Hedera_BoolValue;

/* Wrapper message for `double`.

 The JSON representation for `DoubleValue` is JSON number. */
typedef struct _Hedera_DoubleValue { 
    /* The double value. */
    double value; 
} Hedera_DoubleValue;

/* Wrapper message for `float`.

 The JSON representation for `FloatValue` is JSON number. */
typedef struct _Hedera_FloatValue { 
    /* The float value. */
    float value; 
} Hedera_FloatValue;

/* Wrapper message for `int32`.

 The JSON representation for `Int32Value` is JSON number. */
typedef struct _Hedera_Int32Value { 
    /* The int32 value. */
    int32_t value; 
} Hedera_Int32Value;

/* Wrapper message for `int64`.

 The JSON representation for `Int64Value` is JSON string. */
typedef struct _Hedera_Int64Value { 
    /* The int64 value. */
    int64_t value; 
} Hedera_Int64Value;

/* Wrapper message for `uint32`.

 The JSON representation for `UInt32Value` is JSON number. */
typedef struct _Hedera_UInt32Value { 
    /* The uint32 value. */
    uint32_t value; 
} Hedera_UInt32Value;

/* Wrapper message for `uint64`.

 The JSON representation for `UInt64Value` is JSON string. */
typedef struct _Hedera_UInt64Value { 
    /* The uint64 value. */
    uint64_t value; 
} Hedera_UInt64Value;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define Hedera_DoubleValue_init_default          {0}
#define Hedera_FloatValue_init_default           {0}
#define Hedera_Int64Value_init_default           {0}
#define Hedera_UInt64Value_init_default          {0}
#define Hedera_Int32Value_init_default           {0}
#define Hedera_UInt32Value_init_default          {0}
#define Hedera_BoolValue_init_default            {0}
#define Hedera_StringValue_init_default          {{{NULL}, NULL}}
#define Hedera_DoubleValue_init_zero             {0}
#define Hedera_FloatValue_init_zero              {0}
#define Hedera_Int64Value_init_zero              {0}
#define Hedera_UInt64Value_init_zero             {0}
#define Hedera_Int32Value_init_zero              {0}
#define Hedera_UInt32Value_init_zero             {0}
#define Hedera_BoolValue_init_zero               {0}
#define Hedera_StringValue_init_zero             {{{NULL}, NULL}}

/* Field tags (for use in manual encoding/decoding) */
#define Hedera_StringValue_value_tag             1
#define Hedera_BoolValue_value_tag               1
#define Hedera_DoubleValue_value_tag             1
#define Hedera_FloatValue_value_tag              1
#define Hedera_Int32Value_value_tag              1
#define Hedera_Int64Value_value_tag              1
#define Hedera_UInt32Value_value_tag             1
#define Hedera_UInt64Value_value_tag             1

/* Struct field encoding specification for nanopb */
#define Hedera_DoubleValue_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, DOUBLE,   value,             1)
#define Hedera_DoubleValue_CALLBACK NULL
#define Hedera_DoubleValue_DEFAULT NULL

#define Hedera_FloatValue_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, FLOAT,    value,             1)
#define Hedera_FloatValue_CALLBACK NULL
#define Hedera_FloatValue_DEFAULT NULL

#define Hedera_Int64Value_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, INT64,    value,             1)
#define Hedera_Int64Value_CALLBACK NULL
#define Hedera_Int64Value_DEFAULT NULL

#define Hedera_UInt64Value_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT64,   value,             1)
#define Hedera_UInt64Value_CALLBACK NULL
#define Hedera_UInt64Value_DEFAULT NULL

#define Hedera_Int32Value_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, INT32,    value,             1)
#define Hedera_Int32Value_CALLBACK NULL
#define Hedera_Int32Value_DEFAULT NULL

#define Hedera_UInt32Value_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   value,             1)
#define Hedera_UInt32Value_CALLBACK NULL
#define Hedera_UInt32Value_DEFAULT NULL

#define Hedera_BoolValue_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, BOOL,     value,             1)
#define Hedera_BoolValue_CALLBACK NULL
#define Hedera_BoolValue_DEFAULT NULL

#define Hedera_StringValue_FIELDLIST(X, a) \
X(a, CALLBACK, SINGULAR, STRING,   value,             1)
#define Hedera_StringValue_CALLBACK pb_default_field_callback
#define Hedera_StringValue_DEFAULT NULL

extern const pb_msgdesc_t Hedera_DoubleValue_msg;
extern const pb_msgdesc_t Hedera_FloatValue_msg;
extern const pb_msgdesc_t Hedera_Int64Value_msg;
extern const pb_msgdesc_t Hedera_UInt64Value_msg;
extern const pb_msgdesc_t Hedera_Int32Value_msg;
extern const pb_msgdesc_t Hedera_UInt32Value_msg;
extern const pb_msgdesc_t Hedera_BoolValue_msg;
extern const pb_msgdesc_t Hedera_StringValue_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define Hedera_DoubleValue_fields &Hedera_DoubleValue_msg
#define Hedera_FloatValue_fields &Hedera_FloatValue_msg
#define Hedera_Int64Value_fields &Hedera_Int64Value_msg
#define Hedera_UInt64Value_fields &Hedera_UInt64Value_msg
#define Hedera_Int32Value_fields &Hedera_Int32Value_msg
#define Hedera_UInt32Value_fields &Hedera_UInt32Value_msg
#define Hedera_BoolValue_fields &Hedera_BoolValue_msg
#define Hedera_StringValue_fields &Hedera_StringValue_msg

/* Maximum encoded size of messages (where known) */
/* Hedera_StringValue_size depends on runtime parameters */
#define Hedera_BoolValue_size                    2
#define Hedera_DoubleValue_size                  9
#define Hedera_FloatValue_size                   5
#define Hedera_Int32Value_size                   11
#define Hedera_Int64Value_size                   11
#define Hedera_UInt32Value_size                  6
#define Hedera_UInt64Value_size                  11

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
