#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <stdbool.h>
#include <math.h>
#include "vec2.h"
#include "defs/defs.h"
#include "utils/utils.h"


//
// Сгенерировано https://chat.deepseek.com/
// Затем отрефакторено и дополнено.
//


// Минимальное отличное от нуля число.
#ifndef MATHUTILS_EPSYLON
#define MATHUTILS_EPSYLON 1e-4f
#endif

/* Линейная интерполяция между двумя float значениями */
ALWAYS_INLINE static float mathutils_lerp(float a, float b, float t) {
    return a + t * (b - a);
}

/* Дробная часть числа */
ALWAYS_INLINE static float mathutils_fract(float f) {
    return f - floorf(f);
}

/* Линейная интерполяция между двумя 8-битными целыми (фиксированная точка) */
ALWAYS_INLINE static int mathutils_lerpi8(int a, int b, int t) {
    return a + ((t * (b - a)) >> 8);
}

/* Ограничение значения в диапазон */
ALWAYS_INLINE static float mathutils_clampf(float val, float min_val, float max_val) {
    return fmaxf(fminf(val, max_val), min_val);
}

ALWAYS_INLINE static int mathutils_clampi(int val, int min_val, int max_val) {
    return MAX(MIN(val, max_val), min_val);
}

/* Проверка пересечения линий (без ограничения отрезками) */
EXTERN bool mathutils_lines_intersect_offsets(float* out_ta, float* out_tc,
                                              const vec2_t* A, const vec2_t* B, 
                                              const vec2_t* C, const vec2_t* D);

/* Проверка пересечения линий с вычислением точки пересечения */
EXTERN bool mathutils_lines_intersect(vec2_t* out_point,
                                      const vec2_t* A, const vec2_t* B,
                                      const vec2_t* C, const vec2_t* D);

/* Проверка пересечения отрезков */
EXTERN bool mathutils_segments_intersect(vec2_t* out_point,
                                         const vec2_t* A, const vec2_t* B,
                                         const vec2_t* C, const vec2_t* D);

/* Перпендикуляр от точки к линии */
EXTERN bool mathutils_line_perpendicular_from(vec2_t* out_point, const vec2_t* A,
                                              const vec2_t* B, const vec2_t* C);

/* Перпендикуляр от точки к отрезку */
EXTERN bool mathutils_segment_perpendicular_from(vec2_t* out_point, const vec2_t* A,
                                                 const vec2_t* B, const vec2_t* C);

#endif // MATH_UTILS_H
