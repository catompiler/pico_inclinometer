#ifndef VEC2_H
#define VEC2_H

#include <math.h>
#include <stdbool.h>
#include "defs/defs.h"


//
// Сгенерировано https://chat.deepseek.com/
// Затем отрефакторено и дополнено.
//


// 2D вектор с компонентами x и y
typedef struct ALIGNED4 _Vec2 {
    float x;
    float y;
} vec2_t;


// Минимальное отличное от нуля число.
#ifndef VEC2_EPSYLON
#define VEC2_EPSYLON 1e-4f
#endif


/* Инициализация вектора */
ALWAYS_INLINE static void vec2_init(vec2_t* v, float x, float y)
{
    v->x = x;
    v->y = y;
}

/* Копирование (установка) вектора */
ALWAYS_INLINE static void vec2_set(vec2_t* v_res, const vec2_t* v)
{
    v_res->x = v->x;
    v_res->y = v->y;
}

/* Смена знака вектора (унарный минус) */
ALWAYS_INLINE static void vec2_neg(vec2_t* v_res, const vec2_t* v)
{
    v_res->x = -v->x;
    v_res->y = -v->y;
}

/* Сложение двух векторов */
ALWAYS_INLINE static void vec2_add(vec2_t* v_res, const vec2_t* a, const vec2_t* b) {
    v_res->x = a->x + b->x;
    v_res->y = a->y + b->y;
}

/* Вычитание двух векторов */
ALWAYS_INLINE static void vec2_sub(vec2_t* v_res, const vec2_t* a, const vec2_t* b)
{
    v_res->x = a->x - b->x;
    v_res->y = a->y - b->y;
}

/* Умножение вектора на скаляр */
ALWAYS_INLINE static void vec2_mul(vec2_t* v_res, const vec2_t* v, float value)
{
    v_res->x = v->x * value;
    v_res->y = v->y * value;
}

/* Деление вектора на скаляр */
ALWAYS_INLINE static void vec2_div(vec2_t* v_res, const vec2_t* v, float value)
{
    v_res->x = v->x / value;
    v_res->y = v->y / value;
}

/* Поэлементное умножение векторов (адамарское произведение) */
ALWAYS_INLINE static void vec2_mulv(vec2_t* v_res, const vec2_t* a, const vec2_t* b)
{
    v_res->x = a->x * b->x;
    v_res->y = a->y * b->y;
}

/* Скалярное произведение векторов */
ALWAYS_INLINE static float vec2_dot(const vec2_t* a, const vec2_t* b)
{
    return a->x * b->x + a->y * b->y;
}

/* Векторное произведение (псевдоскаляр) */
ALWAYS_INLINE static float vec2_cross(const vec2_t* a, const vec2_t* b)
{
    return a->x * b->y - a->y * b->x;
}

/* Перпендикулярный вектор (поворот на 90 градусов против часовой стрелки) */
ALWAYS_INLINE static void vec2_perpendicular(vec2_t* v_res, const vec2_t* v)
{
    v_res->x = -v->y;
    v_res->y = v->x;
}

/* Отражение вектора v относительно вектора r */
EXTERN void vec2_reflect(vec2_t* v_res, const vec2_t* v, const vec2_t* r);

/* Длина (модуль) вектора */
EXTERN float vec2_length(const vec2_t* v);

/* Квадрат длины вектора (избегаем вычисления квадратного корня) */
ALWAYS_INLINE static float vec2_length_sq(const vec2_t* v)
{
    return v->x * v->x + v->y * v->y;
}

/* Нормализация вектора (приведение к единичной длине) */
EXTERN bool vec2_normalize(vec2_t* v_res, const vec2_t* v);

/* Проверка на точное равенство векторов */
ALWAYS_INLINE static bool vec2_eq(const vec2_t* a, const vec2_t* b)
{
    return a->x == b->x && a->y == b->y;
}

/* Вращает вектор на заданный угол */
EXTERN void vec2_rotate(vec2_t* v_res, const vec2_t* v, float angle);

#endif //VEC2_H
