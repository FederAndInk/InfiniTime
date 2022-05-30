#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct os_mbuf;

/** Type of UUID */
enum {
    /** 16-bit UUID (BT SIG assigned) */
    BLE_UUID_TYPE_16 = 16,

    /** 32-bit UUID (BT SIG assigned) */
    BLE_UUID_TYPE_32 = 32,

    /** 128-bit UUID */
    BLE_UUID_TYPE_128 = 128,
};

/** Generic UUID type, to be used only as a pointer */
typedef struct {
    /** Type of the UUID */
    uint8_t type;
} ble_uuid_t;

/** 16-bit UUID */
typedef struct {
    ble_uuid_t u;
    uint16_t value;
} ble_uuid16_t;

/** 32-bit UUID */
typedef struct {
    ble_uuid_t u;
    uint32_t value;
} ble_uuid32_t;

/** 128-bit UUID */
typedef struct {
    ble_uuid_t u;
    uint8_t value[16];
} ble_uuid128_t;

/** Universal UUID type, to be used for any-UUID static allocation */
typedef union {
    ble_uuid_t u;
    ble_uuid16_t u16;
    ble_uuid32_t u32;
    ble_uuid128_t u128;
} ble_uuid_any_t;

#ifdef __cplusplus
}
#endif
