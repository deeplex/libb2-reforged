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

   This is an optional libb2 compatibility header which provides aliases for
   most libb2 APIs.
*/
#ifndef BLAKE2_H
#define BLAKE2_H

#include <dplx/blake2.h>

#if defined(__cplusplus)
extern "C" {
#endif

  enum blake2s_constant
  {
    BLAKE2S_BLOCKBYTES = DPLX_BLAKE2S_BLOCKBYTES,
    BLAKE2S_OUTBYTES = DPLX_BLAKE2S_OUTBYTES,
    BLAKE2S_KEYBYTES = DPLX_BLAKE2S_KEYBYTES,
    BLAKE2S_SALTBYTES = DPLX_BLAKE2S_SALTBYTES,
    BLAKE2S_PERSONALBYTES = DPLX_BLAKE2S_PERSONALBYTES
  };

  enum blake2b_constant
  {
    BLAKE2B_BLOCKBYTES = DPLX_BLAKE2B_BLOCKBYTES,
    BLAKE2B_OUTBYTES = DPLX_BLAKE2B_OUTBYTES,
    BLAKE2B_KEYBYTES = DPLX_BLAKE2B_KEYBYTES,
    BLAKE2B_SALTBYTES = DPLX_BLAKE2B_SALTBYTES,
    BLAKE2B_PERSONALBYTES = DPLX_BLAKE2B_PERSONALBYTES
  };

  typedef struct dplx_blake2s_state blake2s_state;
  typedef struct dplx_blake2b_state blake2b_state;

  typedef struct dplx_blake2s_param blake2s_param;
  typedef struct dplx_blake2b_param blake2b_param;

  typedef struct dplx_blake2xs_state blake2xs_state;
  typedef struct dplx_blake2xb_state blake2xb_state;

  /* Streaming API */
  DPLX_BLAKE2_EXPORT int blake2s_init( blake2s_state *S, size_t outlen );
  DPLX_BLAKE2_EXPORT int blake2s_init_key( blake2s_state *S, size_t outlen, const void *key, size_t keylen );
  DPLX_BLAKE2_EXPORT int blake2s_init_param( blake2s_state *S, const blake2s_param *P );
  DPLX_BLAKE2_EXPORT int blake2s_update( blake2s_state *S, const void *in, size_t inlen );
  DPLX_BLAKE2_EXPORT int blake2s_final( blake2s_state *S, void *out, size_t outlen );

  DPLX_BLAKE2_EXPORT int blake2b_init( blake2b_state *S, size_t outlen );
  DPLX_BLAKE2_EXPORT int blake2b_init_key( blake2b_state *S, size_t outlen, const void *key, size_t keylen );
  DPLX_BLAKE2_EXPORT int blake2b_init_param( blake2b_state *S, const blake2b_param *P );
  DPLX_BLAKE2_EXPORT int blake2b_update( blake2b_state *S, const void *in, size_t inlen );
  DPLX_BLAKE2_EXPORT int blake2b_final( blake2b_state *S, void *out, size_t outlen );

  /* Variable output length API */
  DPLX_BLAKE2_EXPORT int blake2xs_init( blake2xs_state *S, const size_t outlen );
  DPLX_BLAKE2_EXPORT int blake2xs_init_key( blake2xs_state *S, const size_t outlen, const void *key, size_t keylen );
  DPLX_BLAKE2_EXPORT int blake2xs_update( blake2xs_state *S, const void *in, size_t inlen );
  DPLX_BLAKE2_EXPORT int blake2xs_final(blake2xs_state *S, void *out, size_t outlen);

  DPLX_BLAKE2_EXPORT int blake2xb_init( blake2xb_state *S, const size_t outlen );
  DPLX_BLAKE2_EXPORT int blake2xb_init_key( blake2xb_state *S, const size_t outlen, const void *key, size_t keylen );
  DPLX_BLAKE2_EXPORT int blake2xb_update( blake2xb_state *S, const void *in, size_t inlen );
  DPLX_BLAKE2_EXPORT int blake2xb_final(blake2xb_state *S, void *out, size_t outlen);

  /* Simple API */
  DPLX_BLAKE2_EXPORT int blake2s( void *out, size_t outlen, const void *in, size_t inlen, const void *key, size_t keylen );
  DPLX_BLAKE2_EXPORT int blake2b( void *out, size_t outlen, const void *in, size_t inlen, const void *key, size_t keylen );

  DPLX_BLAKE2_EXPORT int blake2xs( void *out, size_t outlen, const void *in, size_t inlen, const void *key, size_t keylen );
  DPLX_BLAKE2_EXPORT int blake2xb( void *out, size_t outlen, const void *in, size_t inlen, const void *key, size_t keylen );

#if defined(__cplusplus)
}
#endif

#endif
