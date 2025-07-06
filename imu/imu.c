#include "imu.h"
#include "port.h"
#include <string.h>
#include <math.h>



//! Получает состояние линии прерывания 2.
static bool imu_int2(imu_t* imu)
{
    return gpio_get(imu->pin_int2);
}

//! Ждёт высокого уровня сигнала на линии прерывания 2.
static void imu_wait_int2(imu_t* imu)
{
    while(!imu_int2(imu)){
        sleep_us(100);
    }
}

//! Ждёт низкого уровня сигнала на линии прерывания 2.
static void imu_wait_no_int2(imu_t* imu)
{
    while(imu_int2(imu)){
        sleep_us(100);
    }
}

//! Получает состояние линии прерывания 1.
static bool imu_int1(imu_t* imu)
{
    return gpio_get(imu->pin_int1);
}

//! Ждёт высокого уровня сигнала на линии прерывания 1.
static void imu_wait_int1(imu_t* imu)
{
    while(!imu_int1(imu)){
        sleep_us(100);
    }
}

//! Ждёт низкого уровня сигнала на линии прерывания 1.
static void imu_wait_no_int1(imu_t* imu)
{
    while(imu_int1(imu)){
        sleep_us(100);
    }
}



err_t imu_init(imu_t* imu, qmi8658c_t* sensor, uint pin_int1, uint pin_int2)
{
    if(sensor == NULL) return E_NULL_POINTER;

    err_t err;

    memset(imu, 0x0, sizeof(imu_t));

    imu->sensor = sensor;
    imu->pin_int1 = pin_int1;
    imu->pin_int2 = pin_int2;

    // Фильтры Калмана.
    float X0[2] = IMU_KF_X0;
    float P0[4] = IMU_KF_P0;
    float Q[4]  = IMU_KF_Q;
    float R     = IMU_KF_R;

    err = kfag_init(&imu->kf_roll, X0, P0, Q, R);
    if(err != E_NO_ERROR) return err;

    err = kfag_init(&imu->kf_pitch, X0, P0, Q, R);
    if(err != E_NO_ERROR) return err;

    // Калибровки.
    // Смещение.
    // Акселерометр.
    imu->offsets.accel_x = -0.001069362500f;
    imu->offsets.accel_y = 0.138892290f;
    imu->offsets.accel_z = 0.01857663500f;
    // Гироскоп.
    imu->offsets.gyro_x = -0.212103741f;
    imu->offsets.gyro_y = -3.272893438f;
    imu->offsets.gyro_z = 0.933690915f;
    // Усиление.
    // Акселерометр.
    imu->gains.accel_x = 0.984112665f;
    imu->gains.accel_y = 0.979883549f;
    imu->gains.accel_z = 0.990387262f;
    // Гироскоп.
    imu->gains.gyro_x = 1.0f;
    imu->gains.gyro_y = 1.0f;
    imu->gains.gyro_z = 1.0f;

    return E_NO_ERROR;
}

//! Отправляет команду.
static err_t imu_cmd(imu_t* imu, qmi8658c_reg_value_t cmd)
{
    err_t err;

    // Команда.
    err = qmi8658c_write_reg(imu->sensor, QMI8658C_REG_CTRL9, cmd);
    if(err != E_NO_ERROR) return err;

#if defined(IMU_INT1_CMD_DONE) && (IMU_INT1_CMD_DONE == 1)
    imu_wait_int1(imu);
#else
    uint8_t statusint;

    do{
        err = qmi8658c_read_reg(imu->sensor, QMI8658C_REG_STATUSINT, &statusint);
        if(err != E_NO_ERROR) return err;
    }while((statusint & QMI8658C_STATUSINT_CTRL9_CMD_DONE) == 0);
#endif

    // Подтверждение.
    err = qmi8658c_write_reg(imu->sensor, QMI8658C_REG_CTRL9, QMI8658C_CTRL9_CMD_ACK);
    if(err != E_NO_ERROR) return err;

#if defined(IMU_INT1_CMD_DONE) && (IMU_INT1_CMD_DONE == 1)
    imu_wait_no_int1(imu);
#else
    do{
        err = qmi8658c_read_reg(imu->sensor, QMI8658C_REG_STATUSINT, &statusint);
        if(err != E_NO_ERROR) return err;
    }while((statusint & QMI8658C_STATUSINT_CTRL9_CMD_DONE) == 1);
#endif

    return E_NO_ERROR;
}

//! Начинает чтение, если данные доступны.
//! При недоступности данных возвращает E_AGAIN.
static err_t imu_fifo_read_begin(imu_t* imu, size_t* avail)
{
    err_t err;

#if defined(IMU_INT2_DATA_AVAIL) && (IMU_INT2_DATA_AVAIL == 1)
    if(!imu_int2(imu)) return E_AGAIN;

    if(avail){
#endif

    uint8_t fifo_status;
    
    err = qmi8658c_read_reg(imu->sensor, QMI8658C_REG_FIFO_STATUS, &fifo_status);
    if(err != E_NO_ERROR) return err;

    if(fifo_status & QMI8658C_FIFO_STATUS_EMPTY) return E_AGAIN;
    
#if !defined(IMU_INT2_DATA_AVAIL) || (IMU_INT2_DATA_AVAIL == 0)
    if(avail){
#endif
        uint8_t fifo_smpl_cnt_lsb;
        
        err = qmi8658c_read_reg(imu->sensor, QMI8658C_REG_FIFO_SMPL_CNT, &fifo_smpl_cnt_lsb);
        if(err != E_NO_ERROR) return err;

        size_t size = (((size_t)fifo_status & QMI8658C_FIFO_STATUS_SMPL_CNT_MSB_Msk) << 8) | 
                      fifo_smpl_cnt_lsb;

        *avail = size;

        if(size == 0) return E_AGAIN;
    }

    err = imu_cmd(imu, QMI8658C_CTRL9_CMD_REQ_FIFO);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

//! Завершает чтение данных.
static err_t imu_fifo_read_end(imu_t* imu)
{
    err_t err;

    uint8_t fifo_ctrl;

    // Сброс флага FIFO_rd_mode.
    // Чтение.
    err = qmi8658c_read_reg(imu->sensor, QMI8658C_REG_FIFO_CTRL, &fifo_ctrl);
    if(err != E_NO_ERROR) return err;

    // Сброс.
    fifo_ctrl &= ~QMI8658C_FIFO_CTRL_RD_MODE_Msk;

    // Запись.
    err = qmi8658c_write_reg(imu->sensor, QMI8658C_REG_FIFO_CTRL, fifo_ctrl);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

//! Читает сырые данные.
static err_t imu_fifo_read_raw(imu_t* imu)
{
    err_t err;

    err = qmi8658c_read_regs(imu->sensor, QMI8658C_REG_FIFO_DATA,
                             (uint8_t*)&imu->raw_data, sizeof(imu_raw_data_t));
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}


RAM_FUNC
static void imu_calc_scaled(imu_t* imu)
{
    // Приведём к шкалам.
    // Акселерометр.
    imu->scaled_data.accel_x = (float)imu->raw_data.accel_x * (1.0f / IMU_ACCEL_1G);
    imu->scaled_data.accel_y = (float)imu->raw_data.accel_y * (1.0f / IMU_ACCEL_1G);
    imu->scaled_data.accel_z = (float)imu->raw_data.accel_z * (1.0f / IMU_ACCEL_1G);
    // Гироскоп.
    imu->scaled_data.gyro_x = (float)imu->raw_data.gyro_x * (1.0f / IMU_GYRO_1DPS);
    imu->scaled_data.gyro_y = (float)imu->raw_data.gyro_y * (1.0f / IMU_GYRO_1DPS);
    imu->scaled_data.gyro_z = (float)imu->raw_data.gyro_z * (1.0f / IMU_GYRO_1DPS);
}

RAM_FUNC
static void imu_calc_apply_offsets_gains(imu_t* imu)
{
    // Смещения.
    // Акселерометр.
    imu->data.accel_x = (imu->scaled_data.accel_x - imu->offsets.accel_x) * imu->gains.accel_x;
    imu->data.accel_y = (imu->scaled_data.accel_y - imu->offsets.accel_y) * imu->gains.accel_y;
    imu->data.accel_z = (imu->scaled_data.accel_z - imu->offsets.accel_z) * imu->gains.accel_z;
    // Гироскоп.
    imu->data.gyro_x = (imu->scaled_data.gyro_x - imu->offsets.gyro_x) * imu->gains.gyro_x;
    imu->data.gyro_y = (imu->scaled_data.gyro_y - imu->offsets.gyro_y) * imu->gains.gyro_y;
    imu->data.gyro_z = (imu->scaled_data.gyro_z - imu->offsets.gyro_z) * imu->gains.gyro_z;
}

RAM_FUNC
static void imu_calc_angles(imu_t* imu)
{
    float acc_x = imu->data.accel_x;
    float acc_y = imu->data.accel_y;
    float acc_z = imu->data.accel_z;

    imu->accel_roll = atan2f(-acc_x, acc_y) - 0.5f*3.14159265359f;
    imu->accel_pitch = atan2f(-acc_x, -acc_z) - 0.5f*3.14159265359f;

    float gyro_y = imu->data.gyro_y;
    float gyro_z = imu->data.gyro_z;

    imu->gyro_droll = -gyro_z;
    imu->gyro_dpitch = -gyro_y;
}

static void imu_filter_angles(imu_t* imu)
{
    imu->roll = kfag_calc(&imu->kf_roll, imu->gyro_droll, IMU_GYRO_DT, imu->accel_roll);
    imu->pitch = kfag_calc(&imu->kf_pitch, imu->gyro_dpitch, IMU_GYRO_DT, imu->accel_pitch);
}

static void imu_calc(imu_t* imu)
{
    imu_calc_scaled(imu);
    imu_calc_apply_offsets_gains(imu);
    imu_calc_angles(imu);
    imu_filter_angles(imu);
}


err_t imu_process(imu_t* imu)
{
    err_t err;
    size_t avail, i;

    err = imu_fifo_read_begin(imu, &avail);
    //if(err == E_AGAIN) return err;
    if(err != E_NO_ERROR) return err;

    for(i = 0; i < avail; i += (sizeof(imu->raw_data)/2)){
        err = imu_fifo_read_raw(imu);
        if(err != E_NO_ERROR) return err;

        // process readed raw data.
        imu_calc(imu);
    }

    err = imu_fifo_read_end(imu);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

