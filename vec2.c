#include "vec2.h"



/* Отражение вектора v относительно вектора r */
void vec2_reflect(vec2_t* v_res, const vec2_t* v, const vec2_t* r)
{
    float rr = vec2_dot(r, r);

    // Если вектор отражения нулевой, возвращаем исходный вектор
    if (fabsf(rr) < VEC2_EPSYLON) {
        *v_res = *v;
        return;
    }

    // Формула отражения: v' = 2*(v·r)/(r·r)*r - v
    float vr = vec2_dot(v, r);
    vec2_t h;
    vec2_mul(&h, r, vr / rr);

    vec2_add(v_res, &h, &h);
    vec2_sub(v_res, v_res, v);
}

/* Длина (модуль) вектора */
float vec2_length(const vec2_t* v)
{
    return sqrtf(v->x * v->x + v->y * v->y);
}

/* Нормализация вектора (приведение к единичной длине) */
bool vec2_normalize(vec2_t* v_res, const vec2_t* v)
{
    float len = vec2_length(v);

    if(len < VEC2_EPSYLON){
        return false;
    }

    vec2_div(v_res, v, len);

    return true;
}

/* Вращает вектор на заданный угол */
void vec2_rotate(vec2_t* v_res, const vec2_t* v, float angle)
{
    float sin_val = sinf(angle);
    float cos_val = cosf(angle);

    float x = v->x;
    float y = v->y;

    v_res->x = x * cos_val - y * sin_val;
    v_res->y = x * sin_val + y * cos_val;
}

