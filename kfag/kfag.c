#include "kfag.h"
#include "port.h"
#include <string.h>


err_t kfag_init(kfag_t *kf, float X0[2], float P0[4], float Q[4], float R)
{
    memcpy(kf->X, X0, sizeof(float) * 2);
    memcpy(kf->P, P0, sizeof(float) * 4);
    memcpy(kf->Q, Q, sizeof(float) * 4);
    memcpy(&kf->R, &R, sizeof(float) * 1);

    return E_NO_ERROR;
}

RAM_FUNC
void kfag_predict(kfag_t* kf, float U, float dt)
{
    float x0 = kf->X[0];
    float x1 = kf->X[1];

    kf->X[0] = x1 * dt + x0;
    kf->X[1] = x1 + U * dt;

    float p0 = kf->P[0];
    float p1 = kf->P[1];
    float p2 = kf->P[2];
    float p3 = kf->P[3];

    float q0 = kf->Q[0];
    float q1 = kf->Q[1];
    float q2 = kf->Q[2];
    float q3 = kf->Q[3];

    float dtp3 = dt * p3;
    float dtp3p2 = dtp3 + p2;
    kf->P[0] = q0 + dt * (dtp3p2) + dt * p1 + p0;
    kf->P[1] = q1 + dtp3 + p1;
    kf->P[2] = q2 + dtp3p2;
    kf->P[3] = q3 + p3;
}

RAM_FUNC
float kfag_update(kfag_t* kf, float Z)
{
    float p0 = kf->P[0];
    float p1 = kf->P[1];
    float p2 = kf->P[2];
    float p3 = kf->P[3];

    float r = kf->R;

    float s = p0 + r;
    float invS = 1.0f / s;

    float k0 = p0 * invS;
    float k1 = p2 * invS;

    float x0 = kf->X[0];
    float x1 = kf->X[1];

    float y = Z - x0;

    kf->X[0] = x0 + k0 * y;
    kf->X[1] = x1 + k1 * y;

    float _1_k0 = 1.0f - k0;
    kf->P[0] = (_1_k0) * p0;
    kf->P[1] = (_1_k0) * p1;
    kf->P[2] = p2 - k1 * p0;
    kf->P[3] = p3 - k1 * p1;

    return kf->X[0];
}