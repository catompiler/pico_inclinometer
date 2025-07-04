#ifndef IMU_H
#define IMU_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "defs/defs.h"
#include "errors/errors.h"
#include "hardware/gpio.h"
#include "qmi8658c/qmi8658c.h"



//! Количество блоков данных в FIFO
//! для уведомления.
#define IMU_FIFO_WATERMARK 1

//! Использовать пин INT1 как флаг CMD_DONE.
#define IMU_INT1_CMD_DONE 1

//! Использовать пин INT2 как флаг DATA_AVAIL.
#define IMU_INT2_DATA_AVAIL 1


// Значение конфигурации чувствительности.
// Акселерометр.
//! Значение для регистра.
#define IMU_AFS QMI8658C_CTRL2_AFS_ACCEL_FULL_SCALE_8G
//! Кратность шкалы.
#define IMU_ACCEL_SCALE 8
//! Величина 1G акселерометра.
#define IMU_ACCEL_1G (32768L / (IMU_ACCEL_SCALE))
// Гироскоп.
//! Значение для регистра.
#define IMU_GFS QMI8658C_CTRL3_GFS_GYRO_FULL_SCALE_256DPS
//! Кратность шкалы.
#define IMU_GYRO_SCALE 256
//! Величина 1DPS гироскопа.
#define IMU_GYRO_1DPS (32768L / (IMU_GYRO_SCALE))


//! Сырые данные IMU.
typedef struct PACKED _Imu_Raw_Data {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
} imu_raw_data_t;

//! Данные IMU.
typedef struct _Imu_Data {
    float accel_x;
    float accel_y;
    float accel_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
} imu_data_t;

//! Структура IMU.
typedef struct _Imu {
    qmi8658c_t* sensor; //!< Датчик.
    uint pin_int1; //!< Пин int1.
    uint pin_int2; //!< Пин int2.
    //qmi8658c_reg_value_t status0; //!< STATUS0.
    //qmi8658c_reg_value_t status1; //!< STATUS1.
    //qmi8658c_reg_value_t statusint; //!< STATUSINT.
    imu_raw_data_t raw_data ALIGNED4; //!< Сырые данные.
    imu_data_t scaled_data; //!< Приведённые к шкалам данные.
    imu_data_t offsets; //!< Данные смещений.
    imu_data_t gains; //!< Данные коэффициентов.
    imu_data_t data; //!< Калиброванные данные.
} imu_t;


/**
 * @brief Инициализирует IMU.
 * @param imu IMU.
 * @param sensor Датчик.
 * @param pin_int1 Пин прерывания 1.
 * @param pin_int2 Пин прерывания 2.
 * @return Код ошибки.
 */
EXTERN err_t imu_init(imu_t* imu, qmi8658c_t* sensor, uint pin_int1, uint pin_int2);

/**
 * @brief Обрабатывает данные IMU.
 * @param imu IMU.
 * @return Код ошибки. Если нет данных для обработки - возвращает E_AGAIN.
 */
EXTERN err_t imu_process(imu_t* imu);

#endif //IMU_H
