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
#include "blake2.h"
#include "dplx/blake2.h"

#include <assert.h>
#include <stdint.h>

#define X_MODE_NOT 1
#define X_MODE_ALL 2
#define DPLX_BLAKE2_CAT2(a, b) a ## b

#if !defined(__STDC_NO_ATOMICS__)

#include <stdatomic.h>
#define DPLX_BLAKE2_ATOMIC_PTR_T(type) _Atomic(type)
#define DPLX_BLAKE2_ATOMIC_PTR_LOAD_RELAXED(type, p) (atomic_load_explicit((p), memory_order_relaxed))
#define DPLX_BLAKE2_ATOMIC_PTR_STORE_RELAXED(type, p, v) do { atomic_store_explicit((p), (v), memory_order_relaxed); } while(0)

#elif defined(_MSC_VER)

#include <intrin.h>
#define DPLX_BLAKE2_ATOMIC_PTR_T(type) type
static_assert(sizeof(uintptr_t) == sizeof(void*), "MSVC atomic impl requires pointer size knowledge");
// implementation strategy analogous to MS STL's std::atomic
#if UINTPTR_MAX == UINT32_MAX
#define DPLX_BLAKE2_ATOMIC_PTR_LOAD_RELAXED(type, p) ((type)__iso_volatile_load32((__int32*)(p)))
#define DPLX_BLAKE2_ATOMIC_PTR_STORE_RELAXED(type, p, v) do { __iso_volatile_store32((__int32*)(p), (__int32)(v)); } while(0)
#elif UINTPTR_MAX == UINT64_MAX
#define DPLX_BLAKE2_ATOMIC_PTR_LOAD_RELAXED(type, p) ((type)__iso_volatile_load64((__int64*)(p)))
#define DPLX_BLAKE2_ATOMIC_PTR_STORE_RELAXED(type, p, v) do { __iso_volatile_store64((__int64*)(p), (__int64)(v)); } while(0)
#else
#error "unknown (and therefore unsupported) pointer size on MSVC"
#endif

#else

#error "don't know how to perform atomic pointer operations on your platform"

#endif

#if DPLX_BLAKE2_DISPATCH_sse2 || DPLX_BLAKE2_DISPATCH_sse41 || DPLX_BLAKE2_DISPATCH_avx
#include <immintrin.h>
#if defined(_MSC_VER)

#include <intrin.h>
#if !defined(bit_SSE2)
#define bit_SSE2 0x04000000
#endif
#if !defined(bit_SSE4_1)
#define bit_SSE4_1 0x00080000
#endif
#if !defined(bit_AVX)
#define bit_AVX 0x10000000
#endif
#if !defined(bit_OSXSAVE)
#define bit_OSXSAVE 0x08000000
#endif
static inline int dplx_get_cpuid(unsigned const level, unsigned *const eax,
                                 unsigned *const ebx, unsigned *const ecx,
                                 unsigned *const edx)
{
    // see https://learn.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex
    int regs[4] = {0};
    unsigned ext = level & 0x80000000U;
    __cpuid(regs, (int)ext);
    if (level > regs[0])
    {
        return 0;
    }
    __cpuid(regs, level);
    *eax = (unsigned)regs[0];
    *ebx = (unsigned)regs[1];
    *ecx = (unsigned)regs[2];
    *edx = (unsigned)regs[3];
    return 1;
}

#elif defined(__GNUC__)

#include <cpuid.h>
static inline int dplx_get_cpuid(unsigned const level, unsigned *const eax,
                                 unsigned *const ebx, unsigned *const ecx,
                                 unsigned *const edx)
{
    return __get_cpuid(level, eax, ebx, ecx, edx);
}

#else

#error "don't know how to invoke cpuid on your compiler"

#endif
#endif

#define X_FOR_BLAKE2B_API(X, suffix) \
    X(int, dplx_blake2b_init ## suffix, ( blake2b_state *S, size_t outlen ), ( S, outlen )) \
    X(int, dplx_blake2b_init_key ## suffix, ( blake2b_state *S, size_t outlen, const void *key, size_t keylen ), ( S, outlen, key, keylen )) \
    X(int, dplx_blake2b_init_param ## suffix, ( blake2b_state *S, const blake2b_param *P ), ( S, P )) \
    X(int, dplx_blake2b_update ## suffix, ( blake2b_state *S, const void *in, size_t inlen ), ( S, in, inlen )) \
    X(int, dplx_blake2b_final ## suffix, ( blake2b_state *S, void *out, size_t outlen ), ( S, out, outlen )) \
    X(int, dplx_blake2b ## suffix, ( void *out, size_t outlen, const void *in, size_t inlen, const void *key, size_t keylen ), ( out, outlen, in, inlen, key, keylen ))

#define X_FOR_BLAKE2S_API(X, suffix) \
    X(int, dplx_blake2s_init ## suffix, ( blake2s_state *S, size_t outlen ), ( S, outlen )) \
    X(int, dplx_blake2s_init_key ## suffix, ( blake2s_state *S, size_t outlen, const void *key, size_t keylen ), ( S, outlen, key, keylen )) \
    X(int, dplx_blake2s_init_param ## suffix, ( blake2s_state *S, const blake2s_param *P ), ( S, P )) \
    X(int, dplx_blake2s_update ## suffix, ( blake2s_state *S, const void *in, size_t inlen ), ( S, in, inlen )) \
    X(int, dplx_blake2s_final ## suffix, ( blake2s_state *S, void *out, size_t outlen ), ( S, out, outlen )) \
    X(int, dplx_blake2s ## suffix, ( void *out, size_t outlen, const void *in, size_t inlen, const void *key, size_t keylen ), ( out, outlen, in, inlen, key, keylen ))

#define X_FOR_BLAKE2_API(X, suffix) X_FOR_BLAKE2B_API(X, suffix) X_FOR_BLAKE2S_API(X, suffix)

enum dplx_blake2_implementation_slot
{
    // default implementation, currently sorta random... maybe fix this
    DPLX_BLAKE2_IMPL_SLOT_FALLBACK = 0,
    DPLX_BLAKE2_IMPL_SLOT_INVALID = -1,
    // disabled implementations don't get a slot
#define X_MODE X_MODE_NOT
#define X(l, u) DPLX_BLAKE2_IMPL_SLOT_ ## u = DPLX_BLAKE2_IMPL_SLOT_INVALID,
#include "blake2-impl-type.def"
#undef X
#undef X_MODE
    // now auto assign implementation ids
#define X(l, u) DPLX_BLAKE2_IMPL_SLOT_ ## u,
#include "blake2-impl-type.def"
#undef X

    DPLX_BLAKE2_IMPL_SLOT_COUNT,
};
static inline enum dplx_blake2_implementation_slot dplx_blake2_impl_to_slot(enum dplx_blake2_implementation_id id)
{
    switch (id)
    {
        case DPLX_BLAKE2_IMPL_FALLBACK: return DPLX_BLAKE2_IMPL_SLOT_FALLBACK;
#define X_MODE X_MODE_ALL
#define X(l, u) case DPLX_BLAKE2_IMPL_ ## u: return DPLX_BLAKE2_IMPL_SLOT_ ## u;
#include "blake2-impl-type.def"
#undef X
#undef X_MODE
        default: return DPLX_BLAKE2_IMPL_SLOT_INVALID;
    }
}

#define X_DISPATCH_TRAMPOLINE(ret, name, params, args) \
    static ret name##_dispatch params ; \
    typedef ret (*name##_fn_t) params ; \
    static DPLX_BLAKE2_ATOMIC_PTR_T(name##_fn_t) name##_fn = & name##_dispatch ; \
    ret name params \
    { \
        return DPLX_BLAKE2_ATOMIC_PTR_LOAD_RELAXED(name##_fn_t, &name##_fn) args ; \
    }
X_FOR_BLAKE2_API(X_DISPATCH_TRAMPOLINE,)
#undef X_DISPATCH_TRAMPOLINE

#define X_IMPL_DECL(ret, name, params, args) ret name params;
#define X(l, u) X_FOR_BLAKE2_API(X_IMPL_DECL, _##l)
#include "blake2-impl-type.def"
#undef X
#undef X_IMPL_DECL

typedef struct dplx_blake2_impl_vtable {
#define X_VTABLE_MEMBER(ret, name, params, args) name##_fn_t name;
X_FOR_BLAKE2_API(X_VTABLE_MEMBER,)
#undef X_VTABLE_MEMBER
} dplx_blake2_impl_vtable_t;

static dplx_blake2_impl_vtable_t const dplx_blake2_impl_vtables[DPLX_BLAKE2_IMPL_SLOT_COUNT] = {
#define X_VTABLE_VALUE(ret, name, params, args) & name ,
#define X(l, u) {X_FOR_BLAKE2_API(X_VTABLE_VALUE, _##l)},
#include "blake2-impl-type.def"
#undef X
#undef X_VTABLE_VALUE
};

static inline int dplx_blake2_choose_impl()
{
    enum dplx_blake2_implementation_slot which = DPLX_BLAKE2_IMPL_SLOT_FALLBACK;

#if DPLX_BLAKE2_DISPATCH_sse2 || DPLX_BLAKE2_DISPATCH_sse41 || DPLX_BLAKE2_DISPATCH_avx
    unsigned eax = 0, ebx = 0, ecx = 0, edx = 0; \
    (void)dplx_get_cpuid(1, &eax, &ebx, &ecx, &edx);

#if DPLX_BLAKE2_DISPATCH_sse2
    if ((edx & bit_SSE2) == bit_SSE2) { which = DPLX_BLAKE2_IMPL_SLOT_SSE2; }
#endif
#if DPLX_BLAKE2_DISPATCH_sse41
    if ((ecx & bit_SSE4_1) == bit_SSE4_1) { which = DPLX_BLAKE2_IMPL_SLOT_SSE41; }
#endif
#if DPLX_BLAKE2_DISPATCH_avx
    // constants have been taken from "Intel 64 and IA-32 Architectures Software Developer's Manual"
    unsigned long long const bit_XSAVE_SSE_SUPPORT = 0x2;
    unsigned long long const bit_XSAVE_AVX_SUPPORT = 0x4;
    unsigned long long const REQUIRED_XSAVE_SUPPORT = bit_XSAVE_SSE_SUPPORT | bit_XSAVE_AVX_SUPPORT;
    if ((ecx & (bit_AVX | bit_OSXSAVE)) == (bit_AVX | bit_OSXSAVE)
        && (_xgetbv(0) & REQUIRED_XSAVE_SUPPORT) == REQUIRED_XSAVE_SUPPORT)
    {
        which = DPLX_BLAKE2_IMPL_SLOT_AVX;
    }
#endif
#endif

#if DPLX_BLAKE2_DISPATCH_neon
    which = DPLX_BLAKE2_IMPL_SLOT_NEON;
#endif

    return (int)which;
}

#define X_DEF_DISPATCH(ret, name, params, args) ret name ## _dispatch params { \
        enum dplx_blake2_implementation_slot const slot = dplx_blake2_choose_impl(); \
        if (slot == DPLX_BLAKE2_IMPL_SLOT_INVALID) { return -1; } \
        name##_fn_t const fn = dplx_blake2_impl_vtables[slot]. name ; \
        DPLX_BLAKE2_ATOMIC_PTR_STORE_RELAXED(name##_fn_t, &name##_fn, fn); \
        return fn args; \
    }
X_FOR_BLAKE2_API(X_DEF_DISPATCH,)
#undef X_DEF_DISPATCH

int dplx_blake2_choose_implementation( void )
{
    return dplx_blake2_use_implementation(dplx_blake2_choose_impl());
}
int dplx_blake2_use_implementation( enum dplx_blake2_implementation_id which )
{
    enum dplx_blake2_implementation_slot slot = dplx_blake2_impl_to_slot(which);
    if (slot == DPLX_BLAKE2_IMPL_SLOT_INVALID)
    {
        return -1;
    }

#define X_SET_IMPL(ret, name, params, args) DPLX_BLAKE2_ATOMIC_PTR_STORE_RELAXED(name##_fn_t, &name##_fn, dplx_blake2_impl_vtables[slot]. name );
X_FOR_BLAKE2_API(X_SET_IMPL,)
#undef X_SET_IMPL

    return 0;
}
bool dplx_blake2_has_implementation( enum dplx_blake2_implementation_id which )
{
    return dplx_blake2_impl_to_slot(which) != DPLX_BLAKE2_IMPL_SLOT_INVALID;
}
