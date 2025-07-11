#ifndef IMU_MAIN
#define IMU_MAIN

#include <stdint.h>
#include "defs/defs.h"
#include "errors/errors.h"


/**
 * Функция main процесса работы с IMU.
 */
void imu_main(void);


//! Флаги статуса.
enum _E_Imu_Process_Status {
    IMU_PROCESS_STATUS_NONE = 0,
    IMU_PROCESS_STATUS_INIT_SENSOR_ERROR = 0x1,
    IMU_PROCESS_STATUS_INIT_IMU_ERROR = 0x2,
    IMU_PROCESS_STATUS_IMU_ERROR = 0x4,
    IMU_PROCESS_STATUS_RUN = 0x8,
    IMU_PROCESS_STATUS_VALID = 0x10,
};

//! Статус.
typedef uint32_t imu_process_status_t;


//! Состояние процесса IMU.
typedef struct _Imu_Process_State {
    imu_process_status_t status; //!< Статус.
    err_t init_error; //!< Ошибка инициализации.
    err_t imu_error; //!< Ошибка IMU.
    float roll; //!< Крен.
    float pitch; //!< Тангаж.
} imu_process_state_t;


//! Инициализирует процесс IMU.
EXTERN err_t imu_process_init(void);

//! Итерация процесса IMU.
EXTERN void imu_process_iter(void);

//! Получает состояние процесса.
const imu_process_state_t* imu_process_get_state(void);


#endif //IMU_MAIN
