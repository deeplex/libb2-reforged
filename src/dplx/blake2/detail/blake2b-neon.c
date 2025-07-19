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

#include "blake2b-common.c.inc"

#include "blake2b-neon-round.h"

static void blake2b_compress( blake2b_state *S, const uint8_t block[BLAKE2B_BLOCKBYTES] )
{
  const uint64x2_t m0 = vreinterpretq_u64_u8(vld1q_u8(&block[  0]));
  const uint64x2_t m1 = vreinterpretq_u64_u8(vld1q_u8(&block[ 16]));
  const uint64x2_t m2 = vreinterpretq_u64_u8(vld1q_u8(&block[ 32]));
  const uint64x2_t m3 = vreinterpretq_u64_u8(vld1q_u8(&block[ 48]));
  const uint64x2_t m4 = vreinterpretq_u64_u8(vld1q_u8(&block[ 64]));
  const uint64x2_t m5 = vreinterpretq_u64_u8(vld1q_u8(&block[ 80]));
  const uint64x2_t m6 = vreinterpretq_u64_u8(vld1q_u8(&block[ 96]));
  const uint64x2_t m7 = vreinterpretq_u64_u8(vld1q_u8(&block[112]));

  uint64x2_t row1l, row1h, row2l, row2h;
  uint64x2_t row3l, row3h, row4l, row4h;
  uint64x2_t t0, t1, b0, b1;

  const uint64x2_t h0 = row1l = vld1q_u64(&S->h[0]);
  const uint64x2_t h1 = row1h = vld1q_u64(&S->h[2]);
  const uint64x2_t h2 = row2l = vld1q_u64(&S->h[4]);
  const uint64x2_t h3 = row2h = vld1q_u64(&S->h[6]);

  row3l = vld1q_u64(&blake2b_IV[0]);
  row3h = vld1q_u64(&blake2b_IV[2]);
  row4l = veorq_u64(vld1q_u64(&blake2b_IV[4]), vld1q_u64(&S->t[0]));
  row4h = veorq_u64(vld1q_u64(&blake2b_IV[6]), vld1q_u64(&S->f[0]));

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
  ROUND( 10 );
  ROUND( 11 );

  vst1q_u64(&S->h[0], veorq_u64(h0, veorq_u64(row1l, row3l)));
  vst1q_u64(&S->h[2], veorq_u64(h1, veorq_u64(row1h, row3h)));
  vst1q_u64(&S->h[4], veorq_u64(h2, veorq_u64(row2l, row4l)));
  vst1q_u64(&S->h[6], veorq_u64(h3, veorq_u64(row2h, row4h)));
}
