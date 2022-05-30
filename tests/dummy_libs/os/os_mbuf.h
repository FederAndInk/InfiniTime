#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define OS_MBUF_PKTLEN(__om) (0)

struct os_mbuf;

inline int
os_mbuf_copydata(const struct os_mbuf *m, int off, int len, void *dst) {
  return -1;
}

#ifdef __cplusplus
}
#endif
