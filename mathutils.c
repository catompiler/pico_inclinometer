#include "mathutils.h"



/* Проверка пересечения линий (без ограничения отрезками) */
bool mathutils_lines_intersect_offsets(float* out_ta, float* out_tc,
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

    if (fabsf(denom) < MATHUTILS_EPSYLON) {
        return false;
    }

    float denom_inv = 1.0f / denom;
    *out_ta = ta_numer * denom_inv;
    *out_tc = tc_numer * denom_inv;

    return true;
}

/* Проверка пересечения линий с вычислением точки пересечения */
bool mathutils_lines_intersect(vec2_t* out_point,
                               const vec2_t* A, const vec2_t* B,
                               const vec2_t* C, const vec2_t* D) 
{
    float ta = 0.0f;
    float tc = 0.0f;
    
    if (!mathutils_lines_intersect_offsets(&ta, &tc, A, B, C, D)) {
        return false;
    }

    out_point->x = A->x + (B->x - A->x) * ta;
    out_point->y = A->y + (B->y - A->y) * ta;

    return true;
}

/* Проверка пересечения отрезков */
bool mathutils_segments_intersect(vec2_t* out_point,
                                  const vec2_t* A, const vec2_t* B,
                                  const vec2_t* C, const vec2_t* D) 
{
    float ta = 0.0f;
    float tc = 0.0f;

    if (!mathutils_lines_intersect_offsets(&ta, &tc, A, B, C, D)) {
        return false;
    }

    if (ta < 0.0f || ta > 1.0f || tc < 0.0f || tc > 1.0f) {
        return false;
    }

    out_point->x = A->x + (B->x - A->x) * ta;
    out_point->y = A->y + (B->y - A->y) * ta;

    return true;
}

bool mathutils_line_perpendicular_from(vec2_t* out_point, const vec2_t* A,
                                       const vec2_t* B, const vec2_t* C)
{
    vec2_t AB = {B->x - A->x, B->y - A->y};
    vec2_t D = {0.0f, 0.0f};
    vec2_perpendicular(&D, &AB);
    vec2_add(&D, &D, C);

    return mathutils_lines_intersect(out_point, A, B, C, &D);
}

bool mathutils_segment_perpendicular_from(vec2_t* out_point, const vec2_t* A,
                                          const vec2_t* B, const vec2_t* C) 
{
    vec2_t AB = {B->x - A->x, B->y - A->y};
    vec2_t D = {0.0f, 0.0f};
    vec2_perpendicular(&D, &AB);
    vec2_add(&D, &D, C);

    float ta = 0.0f;
    float tc = 0.0f;

    if (!mathutils_lines_intersect_offsets(&ta, &tc, A, B, C, &D)) {
        return false;
    }
    
    if (ta < 0.0f || ta > 1.0f) {
        return false;
    }

    out_point->x = A->x + (B->x - A->x) * ta;
    out_point->y = A->y + (B->y - A->y) * ta;

    return true;
}

