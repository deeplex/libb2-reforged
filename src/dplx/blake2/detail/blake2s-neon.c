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

#include <stdint.h>
#include <arm_neon.h>

#include "blake2.h"
#include "blake2-impl.h"

#include "blake2s-common.c.inc"

#include "blake2s-neon-round.h"

static void blake2s_compress( blake2s_state *S, const uint8_t in[BLAKE2S_BLOCKBYTES] )
{
  uint32x4_t row1, row2, row3, row4;
  uint32x4_t buf1, buf2, buf3, buf4;
  uint32x2x2_t t1, t2;
  const uint32x4_t h1234 = row1 = vld1q_u32(&S->h[0]);
  const uint32x4_t h5678 = row2 = vld1q_u32(&S->h[4]);

  const uint32x2_t m0 = vreinterpret_u32_u8(vld1_u8(&in[ 0]));
  const uint32x2_t m1 = vreinterpret_u32_u8(vld1_u8(&in[ 8]));
  const uint32x2_t m2 = vreinterpret_u32_u8(vld1_u8(&in[16]));
  const uint32x2_t m3 = vreinterpret_u32_u8(vld1_u8(&in[24]));
  const uint32x2_t m4 = vreinterpret_u32_u8(vld1_u8(&in[32]));
  const uint32x2_t m5 = vreinterpret_u32_u8(vld1_u8(&in[40]));
  const uint32x2_t m6 = vreinterpret_u32_u8(vld1_u8(&in[48]));
  const uint32x2_t m7 = vreinterpret_u32_u8(vld1_u8(&in[56]));

  row3 = vld1q_u32(&blake2s_IV[0]);

  row4 = veorq_u32(vcombine_u32(vld1_u32(&S->t[0]), vld1_u32(&S->f[0])),
		    vld1q_u32(&blake2s_IV[4]));

  ROUND( 0 );
  ROUND( 1 );
  ROUND( 2 );
  ROUND( 3 );
  ROUND( 4 );
  ROUND( 5 );
  ROUND( 6 );
  ROUND( 7 );
  ROUND( 8 );
  ROUND( 9 );

  vst1q_u32(&S->h[0], veorq_u32(h1234, veorq_u32(row1, row3)));
  vst1q_u32(&S->h[4], veorq_u32(h5678, veorq_u32(row2, row4)));
}
