#include <blake2.h>
#include <dplx/blake2.h>

int blake2s_init( blake2s_state *S, size_t outlen )
{
    return dplx_blake2s_init(S, outlen);
}
int blake2s_init_key( blake2s_state *S, size_t outlen, const void *key, size_t keylen )
{
    return dplx_blake2s_init_key(S, outlen, key, keylen);
}
int blake2s_init_param( blake2s_state *S, const blake2s_param *P )
{
    return dplx_blake2s_init_param(S, P);
}
int blake2s_update( blake2s_state *S, const void *in, size_t inlen )
{
    return dplx_blake2s_update(S, in, inlen);
}
int blake2s_final( blake2s_state *S, void *out, size_t outlen )
{
    return dplx_blake2s_final(S, out, outlen);
}

int blake2b_init( blake2b_state *S, size_t outlen )
{
    return dplx_blake2b_init(S, outlen);
}
int blake2b_init_key( blake2b_state *S, size_t outlen, const void *key, size_t keylen )
{
    return dplx_blake2b_init_key(S, outlen, key, keylen);
}
int blake2b_init_param( blake2b_state *S, const blake2b_param *P )
{
    return dplx_blake2b_init_param(S, P);
}
int blake2b_update( blake2b_state *S, const void *in, size_t inlen )
{
    return dplx_blake2b_update(S, in, inlen);
}
int blake2b_final( blake2b_state *S, void *out, size_t outlen )
{
    return dplx_blake2b_final(S, out, outlen);
}

int blake2xs_init( blake2xs_state *S, const size_t outlen )
{
    return dplx_blake2xs_init(S, outlen);
}
int blake2xs_init_key( blake2xs_state *S, const size_t outlen, const void *key, size_t keylen )
{
    return dplx_blake2xs_init_key(S, outlen, key, keylen);
}
int blake2xs_update( blake2xs_state *S, const void *in, size_t inlen )
{
    return dplx_blake2xs_update(S, in, inlen);
}
int blake2xs_final(blake2xs_state *S, void *out, size_t outlen)
{
    return dplx_blake2xs_final(S, out, outlen);
}

int blake2xb_init( blake2xb_state *S, const size_t outlen )
{
    return dplx_blake2xb_init(S, outlen);
}
int blake2xb_init_key( blake2xb_state *S, const size_t outlen, const void *key, size_t keylen )
{
    return dplx_blake2xb_init_key(S, outlen, key, keylen);
}
int blake2xb_update( blake2xb_state *S, const void *in, size_t inlen )
{
    return dplx_blake2xb_update(S, in, inlen);
}
int blake2xb_final(blake2xb_state *S, void *out, size_t outlen)
{
    return dplx_blake2xb_final(S, out, outlen);
}

/* Simple API */
int blake2s( void *out, size_t outlen, const void *in, size_t inlen, const void *key, size_t keylen )
{
    return dplx_blake2s(out, outlen, in, inlen, key, keylen);
}
int blake2b( void *out, size_t outlen, const void *in, size_t inlen, const void *key, size_t keylen )
{
    return dplx_blake2b(out, outlen, in, inlen, key, keylen);
}

int blake2xs( void *out, size_t outlen, const void *in, size_t inlen, const void *key, size_t keylen )
{
    return dplx_blake2xs(out, outlen, in, inlen, key, keylen);
}
int blake2xb( void *out, size_t outlen, const void *in, size_t inlen, const void *key, size_t keylen )
{
    return dplx_blake2xb(out, outlen, in, inlen, key, keylen);
}
