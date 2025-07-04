#ifndef KFAG_H
#define KFAG_H

#include "errors/errors.h"
#include "defs/defs.h"


//! Структура фильтра Калмана
//! для акселерометра и гироскопа.
typedef struct _Kfag {
    float X[2] ALIGNED4; //!< Вектор состояний.
    float P[4] ALIGNED4; //!< Ковариационная матрица ошибки оценки.
    float Q[4] ALIGNED4; //!< Ковариационная матрица ошибки модели. 
    float R ALIGNED4; //!< Дисперсия шума измерений.
} kfag_t;


//! Инициализирует фильтр.
EXTERN err_t kfag_init(kfag_t* kf, float X0[2], float P0[4], float Q[4], float R);

//! Вычисляет предсказание фильтра.
EXTERN void kfag_predict(kfag_t* kf, float U, float dt);

//! Вычисляет коррекцию фильтра и обновляет значения.
EXTERN float kfag_update(kfag_t* kf, float Z);

//! Вычисляет фильтр.
ALWAYS_INLINE static float kfag_calc(kfag_t* kf, float U, float dt, float Z)
{
    kfag_predict(kf, U, dt);
    return kfag_update(kf, Z);
}

#endif //KFAG_H
