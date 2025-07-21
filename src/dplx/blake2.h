/*
   BLAKE2 reference source code package - reference C implementations

   Copyright 2012, Samuel Neves <sneves@dei.uc.pt>.  You may use this under the
   terms of the CC0, the OpenSSL Licence, or the Apache Public License 2.0, at
   your option.  The terms of these licenses can be found at:

   - CC0 1.0 Universal : http://creativecommons.org/publicdomain/zero/1.0
   - OpenSSL license   : https://www.openssl.org/source/license.html
   - Apache 2.0        : http://www.apache.org/licenses/LICENSE-2.0

   More information about the BLAKE2 hash function can be found at
   https://blake2.net.
*/
#ifndef DPLX_BLAKE2_H
#define DPLX_BLAKE2_H

#include <stddef.h>
#include <stdint.h>

#if !defined(__cplusplus) && __STDC_VERSION__ < 202311l
#include <assert.h>
#include <stdbool.h>
#endif

#if defined(_MSC_VER)
#define DPLX_BLAKE2_PACKED(x) __pragma(pack(push, 1)) x __pragma(pack(pop))
#else
#define DPLX_BLAKE2_PACKED(x) x __attribute__((packed))
#endif

#if defined(__cplusplus)
extern "C" {
#endif

  enum dplx_blake2s_constant
  {
    DPLX_BLAKE2S_BLOCKBYTES = 64,
    DPLX_BLAKE2S_OUTBYTES   = 32,
    DPLX_BLAKE2S_KEYBYTES   = 32,
    DPLX_BLAKE2S_SALTBYTES  = 8,
    DPLX_BLAKE2S_PERSONALBYTES = 8
  };

  enum dplx_blake2b_constant
  {
    DPLX_BLAKE2B_BLOCKBYTES = 128,
    DPLX_BLAKE2B_OUTBYTES   = 64,
    DPLX_BLAKE2B_KEYBYTES   = 64,
    DPLX_BLAKE2B_SALTBYTES  = 16,
    DPLX_BLAKE2B_PERSONALBYTES = 16
  };

  typedef struct dplx_blake2s_state
  {
    uint32_t h[8];
    uint32_t t[2];
    uint32_t f[2];
    uint8_t  buf[DPLX_BLAKE2S_BLOCKBYTES];
    size_t   buflen;
    size_t   outlen;
    uint8_t  last_node;
  } dplx_blake2s_state;

  typedef struct dplx_blake2b_state
  {
    uint64_t h[8];
    uint64_t t[2];
    uint64_t f[2];
    uint8_t  buf[DPLX_BLAKE2B_BLOCKBYTES];
    size_t   buflen;
    size_t   outlen;
    uint8_t  last_node;
  } dplx_blake2b_state;

  DPLX_BLAKE2_PACKED(struct dplx_blake2s_param
  {
    uint8_t  digest_length; /* 1 */
    uint8_t  key_length;    /* 2 */
    uint8_t  fanout;        /* 3 */
    uint8_t  depth;         /* 4 */
    uint32_t leaf_length;   /* 8 */
    uint32_t node_offset;  /* 12 */
    uint16_t xof_length;    /* 14 */
    uint8_t  node_depth;    /* 15 */
    uint8_t  inner_length;  /* 16 */
    /* uint8_t  reserved[0]; */
    uint8_t  salt[DPLX_BLAKE2S_SALTBYTES]; /* 24 */
    uint8_t  personal[DPLX_BLAKE2S_PERSONALBYTES];  /* 32 */
  });

  typedef struct dplx_blake2s_param dplx_blake2s_param;
  static_assert(sizeof(dplx_blake2s_param) == DPLX_BLAKE2S_OUTBYTES, "dplx_blake2s_param must not be padded");

  DPLX_BLAKE2_PACKED(struct dplx_blake2b_param
  {
    uint8_t  digest_length; /* 1 */
    uint8_t  key_length;    /* 2 */
    uint8_t  fanout;        /* 3 */
    uint8_t  depth;         /* 4 */
    uint32_t leaf_length;   /* 8 */
    uint32_t node_offset;   /* 12 */
    uint32_t xof_length;    /* 16 */
    uint8_t  node_depth;    /* 17 */
    uint8_t  inner_length;  /* 18 */
    uint8_t  reserved[14];  /* 32 */
    uint8_t  salt[DPLX_BLAKE2B_SALTBYTES]; /* 48 */
    uint8_t  personal[DPLX_BLAKE2B_PERSONALBYTES];  /* 64 */
  });

  typedef struct dplx_blake2b_param dplx_blake2b_param;
  static_assert(sizeof(dplx_blake2b_param) == DPLX_BLAKE2B_OUTBYTES, "dplx_blake2b_param must not be padded");

  typedef struct dplx_blake2xs_state
  {
    dplx_blake2s_state S[1];
    dplx_blake2s_param P[1];
  } dplx_blake2xs_state;

  typedef struct dplx_blake2xb_state
  {
    dplx_blake2b_state S[1];
    dplx_blake2b_param P[1];
  } dplx_blake2xb_state;


  /* Streaming API */
  int dplx_blake2s_init( dplx_blake2s_state *S, size_t outlen );
  int dplx_blake2s_init_key( dplx_blake2s_state *S, size_t outlen, const void *key, size_t keylen );
  int dplx_blake2s_init_param( dplx_blake2s_state *S, const dplx_blake2s_param *P );
  int dplx_blake2s_update( dplx_blake2s_state *S, const void *in, size_t inlen );
  int dplx_blake2s_final( dplx_blake2s_state *S, void *out, size_t outlen );

  int dplx_blake2b_init( dplx_blake2b_state *S, size_t outlen );
  int dplx_blake2b_init_key( dplx_blake2b_state *S, size_t outlen, const void *key, size_t keylen );
  int dplx_blake2b_init_param( dplx_blake2b_state *S, const dplx_blake2b_param *P );
  int dplx_blake2b_update( dplx_blake2b_state *S, const void *in, size_t inlen );
  int dplx_blake2b_final( dplx_blake2b_state *S, void *out, size_t outlen );

  /* Variable output length API */
  int dplx_blake2xs_init( dplx_blake2xs_state *S, const size_t outlen );
  int dplx_blake2xs_init_key( dplx_blake2xs_state *S, const size_t outlen, const void *key, size_t keylen );
  int dplx_blake2xs_update( dplx_blake2xs_state *S, const void *in, size_t inlen );
  int dplx_blake2xs_final(dplx_blake2xs_state *S, void *out, size_t outlen);

  int dplx_blake2xb_init( dplx_blake2xb_state *S, const size_t outlen );
  int dplx_blake2xb_init_key( dplx_blake2xb_state *S, const size_t outlen, const void *key, size_t keylen );
  int dplx_blake2xb_update( dplx_blake2xb_state *S, const void *in, size_t inlen );
  int dplx_blake2xb_final(dplx_blake2xb_state *S, void *out, size_t outlen);

  /* Simple API */
  int dplx_blake2s( void *out, size_t outlen, const void *in, size_t inlen, const void *key, size_t keylen );
  int dplx_blake2b( void *out, size_t outlen, const void *in, size_t inlen, const void *key, size_t keylen );

  int dplx_blake2xs( void *out, size_t outlen, const void *in, size_t inlen, const void *key, size_t keylen );
  int dplx_blake2xb( void *out, size_t outlen, const void *in, size_t inlen, const void *key, size_t keylen );

  enum dplx_blake2_implementation_id
  {
    DPLX_BLAKE2_IMPL_FALLBACK,
    DPLX_BLAKE2_IMPL_GENERIC,
    DPLX_BLAKE2_IMPL_NEON,
    DPLX_BLAKE2_IMPL_SSE2,
    DPLX_BLAKE2_IMPL_SSE41,
    DPLX_BLAKE2_IMPL_AVX,
  };

  int dplx_blake2_choose_implementation( void );
  int dplx_blake2_use_implementation( enum dplx_blake2_implementation_id which );
  bool dplx_blake2_has_implementation( enum dplx_blake2_implementation_id which );

#if defined(__cplusplus)
}
#endif

#endif
