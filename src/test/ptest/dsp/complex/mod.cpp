/*
 * mul.cpp
 *
 *  Created on: 22 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void pcomplex_mod(float *dst_mod, const float *src, size_t count);
    void complex_mod(float *dst_mod, const float *src_re, const float *src_im, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void pcomplex_mod(float *dst_mod, const float *src, size_t count);
        void complex_mod(float *dst_mod, const float *src_re, const float *src_im, size_t count);
    }

    namespace sse3
    {
        void pcomplex_mod(float *dst_mod, const float *src, size_t count);
        void x64_pcomplex_mod(float *dst_mod, const float *src, size_t count);
    }

    namespace avx
    {
        void x64_pcomplex_mod(float *dst_mod, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void pcomplex_mod(float *dst_mod, const float *src, size_t count);
    }
)

typedef void (* pcomplex_mod_t)(float *dst_mod, const float *src, size_t count);
typedef void (* complex_mod_t)(float *dst_mod, const float *src_re, const float *src_im, size_t count);

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.complex", mod, 5, 1000)

    void call(const char *label, float *dst, const float *src1, size_t count, pcomplex_mod_t mod)
    {
        if (!PTEST_SUPPORTED(mod))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            mod(dst, src1, count);
        );
    }

    void call(const char *label, float *dst, const float *src, size_t count, complex_mod_t mod)
    {
        if (!PTEST_SUPPORTED(mod))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            mod(dst, src, &src[count], count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *out      = alloc_aligned<float>(data, buf_size * 3, 64);
        float *in       = &out[buf_size];

        for (size_t i=0; i < (1 << (MAX_RANK + 1)); ++i)
            out[i]          = float(rand()) / RAND_MAX;

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            call("native:complex_mod", out, in, count, native::complex_mod);
            call("native:pcomplex_mod", out, in, count, native::pcomplex_mod);
            IF_ARCH_X86(call("sse:complex_mod", out, in, count, sse::complex_mod));
            IF_ARCH_X86(call("sse:pcomplex_mod", out, in, count, sse::pcomplex_mod));
            IF_ARCH_X86(call("sse3:pcomplex_mod", out, in, count, sse3::pcomplex_mod));
            IF_ARCH_X86_64(call("sse3:x64_pcomplex_mod", out, in, count, sse3::x64_pcomplex_mod));
            IF_ARCH_X86_64(call("avx:x64_pcomplex_mod", out, in, count, avx::x64_pcomplex_mod));

            IF_ARCH_ARM(call("neon_d32:pcomplex_mod", out, in, count, neon_d32::pcomplex_mod));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END


