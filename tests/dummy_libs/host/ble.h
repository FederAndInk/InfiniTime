#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* BLE Error Codes (Core v4.2 Vol 2 part D) */
enum ble_error_codes
{
    /* An "error" code of 0x0 means success */
    BLE_ERR_SUCCESS             = 0x00,
    BLE_ERR_UNKNOWN_HCI_CMD     = 0x01,
    BLE_ERR_UNK_CONN_ID         = 0x02,
    BLE_ERR_HW_FAIL             = 0x03,
    BLE_ERR_PAGE_TMO            = 0x04,
    BLE_ERR_AUTH_FAIL           = 0x05,
    BLE_ERR_PINKEY_MISSING      = 0x06,
    BLE_ERR_MEM_CAPACITY        = 0x07,
    BLE_ERR_CONN_SPVN_TMO       = 0x08,
    BLE_ERR_CONN_LIMIT          = 0x09,
    BLE_ERR_SYNCH_CONN_LIMIT    = 0x0a,
    BLE_ERR_ACL_CONN_EXISTS     = 0x0b,
    BLE_ERR_CMD_DISALLOWED      = 0x0c,
    BLE_ERR_CONN_REJ_RESOURCES  = 0x0d,
    BLE_ERR_CONN_REJ_SECURITY   = 0x0e,
    BLE_ERR_CONN_REJ_BD_ADDR    = 0x0f,
    BLE_ERR_CONN_ACCEPT_TMO     = 0x10,
    BLE_ERR_UNSUPPORTED         = 0x11,
    BLE_ERR_INV_HCI_CMD_PARMS   = 0x12,
    BLE_ERR_REM_USER_CONN_TERM  = 0x13,
    BLE_ERR_RD_CONN_TERM_RESRCS = 0x14,
    BLE_ERR_RD_CONN_TERM_PWROFF = 0x15,
    BLE_ERR_CONN_TERM_LOCAL     = 0x16,
    BLE_ERR_REPEATED_ATTEMPTS   = 0x17,
    BLE_ERR_NO_PAIRING          = 0x18,
    BLE_ERR_UNK_LMP             = 0x19,
    BLE_ERR_UNSUPP_REM_FEATURE  = 0x1a,
    BLE_ERR_SCO_OFFSET          = 0x1b,
    BLE_ERR_SCO_ITVL            = 0x1c,
    BLE_ERR_SCO_AIR_MODE        = 0x1d,
    BLE_ERR_INV_LMP_LL_PARM     = 0x1e,
    BLE_ERR_UNSPECIFIED         = 0x1f,
    BLE_ERR_UNSUPP_LMP_LL_PARM  = 0x20,
    BLE_ERR_NO_ROLE_CHANGE      = 0x21,
    BLE_ERR_LMP_LL_RSP_TMO      = 0x22,
    BLE_ERR_LMP_COLLISION       = 0x23,
    BLE_ERR_LMP_PDU             = 0x24,
    BLE_ERR_ENCRYPTION_MODE     = 0x25,
    BLE_ERR_LINK_KEY_CHANGE     = 0x26,
    BLE_ERR_UNSUPP_QOS          = 0x27,
    BLE_ERR_INSTANT_PASSED      = 0x28,
    BLE_ERR_UNIT_KEY_PAIRING    = 0x29,
    BLE_ERR_DIFF_TRANS_COLL     = 0x2a,
    /* BLE_ERR_RESERVED         = 0x2b */
    BLE_ERR_QOS_PARM            = 0x2c,
    BLE_ERR_QOS_REJECTED        = 0x2d,
    BLE_ERR_CHAN_CLASS          = 0x2e,
    BLE_ERR_INSUFFICIENT_SEC    = 0x2f,
    BLE_ERR_PARM_OUT_OF_RANGE   = 0x30,
    /* BLE_ERR_RESERVED         = 0x31 */
    BLE_ERR_PENDING_ROLE_SW     = 0x32,
    /* BLE_ERR_RESERVED         = 0x33 */
    BLE_ERR_RESERVED_SLOT       = 0x34,
    BLE_ERR_ROLE_SW_FAIL        = 0x35,
    BLE_ERR_INQ_RSP_TOO_BIG     = 0x36,
    BLE_ERR_SEC_SIMPLE_PAIR     = 0x37,
    BLE_ERR_HOST_BUSY_PAIR      = 0x38,
    BLE_ERR_CONN_REJ_CHANNEL    = 0x39,
    BLE_ERR_CTLR_BUSY           = 0x3a,
    BLE_ERR_CONN_PARMS          = 0x3b,
    BLE_ERR_DIR_ADV_TMO         = 0x3c,
    BLE_ERR_CONN_TERM_MIC       = 0x3d,
    BLE_ERR_CONN_ESTABLISHMENT  = 0x3e,
    BLE_ERR_MAC_CONN_FAIL       = 0x3f,
    BLE_ERR_COARSE_CLK_ADJ      = 0x40,
    BLE_ERR_TYPE0_SUBMAP_NDEF   = 0x41,
    BLE_ERR_UNK_ADV_INDENT      = 0x42,
    BLE_ERR_LIMIT_REACHED       = 0x43,
    BLE_ERR_OPERATION_CANCELLED = 0x44,
    BLE_ERR_PACKET_TOO_LONG     = 0x45,
    BLE_ERR_MAX                 = 0xff
};

#ifdef __cplusplus
}
#endif
