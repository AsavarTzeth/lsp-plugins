/*
 * axis.cpp
 *
 *  Created on: 17 сент. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>

namespace native
{
    void axis_apply_log(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void axis_apply_log(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);
    }

    IF_ARCH_X86_64(
        namespace sse3
        {
            void x64_axis_apply_log(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);
        }
    )
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void axis_apply_log(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);
    }
)

typedef void (* axis_apply_log_t)(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);


UTEST_BEGIN("dsp.graphics", axis)
    void call(const char *label, size_t align, axis_apply_log_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        for (size_t mask=0; mask <= 0x07; ++mask)
        {
            UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0x1ff)
            {
                FloatBuffer v(count, align, mask & 0x01);
                FloatBuffer x1(count, align, mask & 0x02);
                FloatBuffer y1(count, align, mask & 0x04);
                FloatBuffer x2(x1);
                FloatBuffer y2(y1);

                for (size_t i=0; i<count; ++i)
                    v[i] += 0.001f;

                printf("Tesing %s count=%d mask=0x%x\n", label, int(count), int(mask));

                native::axis_apply_log(x1, y1, v, 0.1f, 0.5f, 1.0f, count);
                func(x2, y2, v, 0.1f, 0.5f, 1.0f, count);

                UTEST_ASSERT_MSG(v.valid(), "v corrupted");
                UTEST_ASSERT_MSG(x1.valid(), "x1 corrupted");
                UTEST_ASSERT_MSG(y1.valid(), "y1 corrupted");
                UTEST_ASSERT_MSG(x2.valid(), "x2 corrupted");
                UTEST_ASSERT_MSG(y2.valid(), "y2 corrupted");

                // Compare buffers
                if ((!x1.equals_absolute(x2, 1e-3)) || ((!y1.equals_absolute(y2, 1e-3))))
                {
                    v.dump("v ");
                    x1.dump("x1");
                    y1.dump("y1");
                    x2.dump("x2");
                    y2.dump("y2");
                    if (x1.last_diff() >= 0)
                        printf("x1=%.6f vs x2=%.6f\n", x1.get(x1.last_diff()), x2.get(x2.last_diff()));
                    if (y1.last_diff() >= 0)
                        printf("y1=%.6f vs y2=%.6f\n", y1.get(y1.last_diff()), y2.get(y2.last_diff()));
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("sse:axis_apply_log", 16, sse::axis_apply_log));
        IF_ARCH_X86_64(call("sse3:x64_axis_apply_log", 16, sse3::x64_axis_apply_log));
        IF_ARCH_ARM(call("neon_d32:axis_apply_log", 16, neon_d32::axis_apply_log));
    }

UTEST_END;
