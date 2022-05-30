#pragma once

#include "ble_uuid.h"
#include "ble_att.h"
#include "os/os_mbuf.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_GATT_REGISTER_OP_SVC 1
#define BLE_GATT_REGISTER_OP_CHR 2
#define BLE_GATT_REGISTER_OP_DSC 3

#define BLE_GATT_SVC_UUID16         0x1801
#define BLE_GATT_DSC_CLT_CFG_UUID16 0x2902

#define BLE_GATT_CHR_PROP_BROADCAST       0x01
#define BLE_GATT_CHR_PROP_READ            0x02
#define BLE_GATT_CHR_PROP_WRITE_NO_RSP    0x04
#define BLE_GATT_CHR_PROP_WRITE           0x08
#define BLE_GATT_CHR_PROP_NOTIFY          0x10
#define BLE_GATT_CHR_PROP_INDICATE        0x20
#define BLE_GATT_CHR_PROP_AUTH_SIGN_WRITE 0x40
#define BLE_GATT_CHR_PROP_EXTENDED        0x80

#define BLE_GATT_ACCESS_OP_READ_CHR  0
#define BLE_GATT_ACCESS_OP_WRITE_CHR 1
#define BLE_GATT_ACCESS_OP_READ_DSC  2
#define BLE_GATT_ACCESS_OP_WRITE_DSC 3

#define BLE_GATT_CHR_F_BROADCAST       0x0001
#define BLE_GATT_CHR_F_READ            0x0002
#define BLE_GATT_CHR_F_WRITE_NO_RSP    0x0004
#define BLE_GATT_CHR_F_WRITE           0x0008
#define BLE_GATT_CHR_F_NOTIFY          0x0010
#define BLE_GATT_CHR_F_INDICATE        0x0020
#define BLE_GATT_CHR_F_AUTH_SIGN_WRITE 0x0040
#define BLE_GATT_CHR_F_RELIABLE_WRITE  0x0080
#define BLE_GATT_CHR_F_AUX_WRITE       0x0100
#define BLE_GATT_CHR_F_READ_ENC        0x0200
#define BLE_GATT_CHR_F_READ_AUTHEN     0x0400
#define BLE_GATT_CHR_F_READ_AUTHOR     0x0800
#define BLE_GATT_CHR_F_WRITE_ENC       0x1000
#define BLE_GATT_CHR_F_WRITE_AUTHEN    0x2000
#define BLE_GATT_CHR_F_WRITE_AUTHOR    0x4000

#define BLE_GATT_SVC_TYPE_END       0
#define BLE_GATT_SVC_TYPE_PRIMARY   1
#define BLE_GATT_SVC_TYPE_SECONDARY 2

inline int ble_gatts_count_cfg(const struct ble_gatt_svc_def* defs) {
  return 0;
}

inline int ble_gatts_add_svcs(const struct ble_gatt_svc_def* svcs) {
  return 0;
}

/**
 * Context for an access to a GATT characteristic or descriptor.  When a client
 * reads or writes a locally registered characteristic or descriptor, an
 * instance of this struct gets passed to the application callback.
 */
struct ble_gatt_access_ctxt {
  /**
   * Indicates the gatt operation being performed.  This is equal to one of
   * the following values:
   *     o  BLE_GATT_ACCESS_OP_READ_CHR
   *     o  BLE_GATT_ACCESS_OP_WRITE_CHR
   *     o  BLE_GATT_ACCESS_OP_READ_DSC
   *     o  BLE_GATT_ACCESS_OP_WRITE_DSC
   */
  uint8_t op;

  /**
   * A container for the GATT access data.
   *     o For reads: The application populates this with the value of the
   *       characteristic or descriptor being read.
   *     o For writes: This is already populated with the value being written
   *       by the peer.  If the application wishes to retain this mbuf for
   *       later use, the access callback must set this pointer to NULL to
   *       prevent the stack from freeing it.
   */
  struct os_mbuf* om;

  /**
   * The GATT operation being performed dictates which field in this union is
   * valid.  If a characteristic is being accessed, the chr field is valid.
   * Otherwise a descriptor is being accessed, in which case the dsc field
   * is valid.
   */
  union {
    /**
     * The characteristic definition corresponding to the characteristic
     * being accessed.  This is what the app registered at startup.
     */
    const struct ble_gatt_chr_def* chr;

    /**
     * The descriptor definition corresponding to the descriptor being
     * accessed.  This is what the app registered at startup.
     */
    const struct ble_gatt_dsc_def* dsc;
  };
};

struct ble_gatt_access_ctxt;
typedef int ble_gatt_access_fn(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt* ctxt, void* arg);

typedef uint16_t ble_gatt_chr_flags;

struct ble_gatt_chr_def {
  /**
   * Pointer to characteristic UUID; use BLE_UUIDxx_DECLARE macros to declare
   * proper UUID; NULL if there are no more characteristics in the service.
   */
  const ble_uuid_t* uuid;

  /**
   * Callback that gets executed when this characteristic is read or
   * written.
   */
  ble_gatt_access_fn* access_cb;

  /** Optional argument for callback. */
  void* arg;

  /**
   * Array of this characteristic's descriptors.  NULL if no descriptors.
   * Do not include CCCD; it gets added automatically if this
   * characteristic's notify or indicate flag is set.
   */
  struct ble_gatt_dsc_def* descriptors;

  /** Specifies the set of permitted operations for this characteristic. */
  ble_gatt_chr_flags flags;

  /** Specifies minimum required key size to access this characteristic. */
  uint8_t min_key_size;

  /**
   * At registration time, this is filled in with the characteristic's value
   * attribute handle.
   */
  uint16_t* val_handle;
};

struct ble_gatt_svc_def {
  /**
   * One of the following:
   *     o BLE_GATT_SVC_TYPE_PRIMARY - primary service
   *     o BLE_GATT_SVC_TYPE_SECONDARY - secondary service
   *     o 0 - No more services in this array.
   */
  uint8_t type;

  /**
   * Pointer to service UUID; use BLE_UUIDxx_DECLARE macros to declare
   * proper UUID; NULL if there are no more characteristics in the service.
   */
  const ble_uuid_t* uuid;

  /**
   * Array of pointers to other service definitions.  These services are
   * reported as "included services" during service discovery.  Terminate the
   * array with NULL.
   */
  const struct ble_gatt_svc_def** includes;

  /**
   * Array of characteristic definitions corresponding to characteristics
   * belonging to this service.
   */
  const struct ble_gatt_chr_def* characteristics;
};

#ifdef __cplusplus
}
#endif
