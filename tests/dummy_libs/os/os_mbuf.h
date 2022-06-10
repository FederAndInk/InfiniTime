#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OS_MBUF_PKTLEN(__om) (0)

struct os_mbuf;

inline int os_mbuf_copydata(const struct os_mbuf* m, int off, int len, void* dst) {
  return -1;
}

inline struct os_mbuf* ble_hs_mbuf_from_flat(const void* buf, uint16_t len) {
  return NULL;
}

#ifdef __cplusplus
}
#endif
