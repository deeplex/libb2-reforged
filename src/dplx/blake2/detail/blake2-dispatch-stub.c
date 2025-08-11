/*
   Deeplex libb2 implementation dispatch

   Copyright 2025, Henrik S. Gaßmann <henrik@gassmann.onl>

   You may use this under the terms of the CC0, the OpenSSL Licence, or the
   Apache Public License 2.0, at your option. The terms of these licenses can be
   found at:

   - CC0 1.0 Universal : http://creativecommons.org/publicdomain/zero/1.0
   - OpenSSL license   : https://www.openssl.org/source/license.html
   - Apache 2.0        : http://www.apache.org/licenses/LICENSE-2.0
*/
#include "dplx/blake2.h"

int dplx_blake2_choose_implementation( void )
{
    return 0;
}
#define X(l, u) return DPLX_BLAKE2_IMPL_##u == which || DPLX_BLAKE2_IMPL_FALLBACK == which ? 0 : -1;
int dplx_blake2_use_implementation( enum dplx_blake2_implementation_id which )
{
#include "blake2-impl-type.def"
}
#undef X
#define X(l, u) return DPLX_BLAKE2_IMPL_##u == which || DPLX_BLAKE2_IMPL_FALLBACK == which;
bool dplx_blake2_has_implementation( enum dplx_blake2_implementation_id which )
{
#include "blake2-impl-type.def"
}
#undef X
