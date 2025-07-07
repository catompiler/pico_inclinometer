#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <stdbool.h>
#include "vec2.h"
#include "defs/defs.h"
#include "utils/utils.h"


//
// Сгенерировано https://chat.deepseek.com/
// Затем отрефакторено и дополнено.
//


// Минимальное отличное от нуля число.
#ifndef MATHUTILS_EPSYLON
#define MATHUTILS_EPSYLON 1e-6f
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
    return MAX(MIN(val, max_val), min_val);
}

ALWAYS_INLINE static int mathutils_clampi(int val, int min_val, int max_val) {
    return MAX(MIN(val, max_val), min_val);
}

/* Проверка пересечения линий (без ограничения отрезками) */
ALWAYS_INLINE static bool mathutils_lines_intersect_offsets(
    float* out_ta, float* out_tc,
    const vec2_t* A, const vec2_t* B, 
    const vec2_t* C, const vec2_t* D) 
{
    float AxB = vec2_cross(A, B);
    float AxC = vec2_cross(A, C);
    float AxD = vec2_cross(A, D);
    float BxC = vec2_cross(B, C);
    float BxD = vec2_cross(B, D);
    float CxD = vec2_cross(C, D);

    float ta_numer = -CxD + AxD - AxC;
    float tc_numer = BxC - AxC + AxB;
    float denom = -BxD + BxC + AxD - AxC;

    if (ABS(denom) < MATHUTILS_EPSYLON) {
        return false;
    }

    float denom_inv = 1.0f / denom;
    *out_ta = ta_numer * denom_inv;
    *out_tc = tc_numer * denom_inv;

    return true;
}

/* Проверка пересечения линий с вычислением точки пересечения */
ALWAYS_INLINE static bool mathutils_lines_intersect(
    vec2_t* out_point,
    const vec2_t* A, const vec2_t* B,
    const vec2_t* C, const vec2_t* D) 
{
    float ta, tc;
    if (!mathutils_lines_intersect_offsets(&ta, &tc, A, B, C, D)) {
        return false;
    }

    out_point->x = A->x + (B->x - A->x) * ta;
    out_point->y = A->y + (B->y - A->y) * ta;

    return true;
}

/* Проверка пересечения отрезков */
ALWAYS_INLINE static bool mathutils_segments_intersect(
    vec2_t* out_point,
    const vec2_t* A, const vec2_t* B,
    const vec2_t* C, const vec2_t* D) 
{
    float ta, tc;
    if (!mathutils_lines_intersect_offsets(&ta, &tc, A, B, C, D) ||
        ta < 0.0f || ta > 1.0f || tc < 0.0f || tc > 1.0f) {
        return false;
    }

    out_point->x = A->x + (B->x - A->x) * ta;
    out_point->y = A->y + (B->y - A->y) * ta;

    return true;
}

/* Перпендикуляр от точки к линии */
ALWAYS_INLINE static bool mathutils_line_perpendicular_from(
    vec2_t* out_point,
    const vec2_t* A, const vec2_t* B,
    const vec2_t* C) 
{
    vec2_t AB = {B->x - A->x, B->y - A->y};
    vec2_t D;
    vec2_perpendicular(&D, &AB);
    vec2_add(&D, &D, C);

    return mathutils_lines_intersect(out_point, A, B, C, &D);
}

/* Перпендикуляр от точки к отрезку */
ALWAYS_INLINE static bool mathutils_segment_perpendicular_from(
    vec2_t* out_point,
    const vec2_t* A, const vec2_t* B,
    const vec2_t* C) 
{
    vec2_t AB = {B->x - A->x, B->y - A->y};
    vec2_t D;
    vec2_perpendicular(&D, &AB);
    vec2_add(&D, &D, C);

    float ta;
    float tc;
    if (!mathutils_lines_intersect_offsets(&ta, &tc, A, B, C, &D) ||
        ta < 0.0f || ta > 1.0f) {
        return false;
    }

    out_point->x = A->x + (B->x - A->x) * ta;
    out_point->y = A->y + (B->y - A->y) * ta;

    return true;
}

#endif // MATH_UTILS_H
