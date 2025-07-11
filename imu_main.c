#include <string.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "errors/errors.h"
#include "hardware/i2c.h"
#include "qmi8658c/qmi8658c.h"
#include "imu/imu.h"
#include "imu_main.h"



#define IMU_SDA 6
#define IMU_SCL 7
#define IMU_INT1 23
#define IMU_INT2 24

#define i2c_imu i2c1
static qmi8658c_t imu_sensor;

//! IMU.
imu_t imu;

//! Состояние процесса.
imu_process_state_t process_state;


static void init_i2c(void)
{
    i2c_inst_t* i2c = i2c_imu;

    i2c_init(i2c, 100000);
    gpio_set_function(IMU_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(IMU_SDA);
    gpio_set_function(IMU_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(IMU_SCL);
}

static err_t init_imu_sensor(void)
{
    qmi8658c_reg_value_t value;
    err_t err;

    qmi8658c_t* imu = &imu_sensor;

    // gpio.
    // int1.
    gpio_init(IMU_INT1);
    gpio_set_pulls(IMU_INT1, false, false);
    // int2.
    gpio_init(IMU_INT2);
    gpio_set_pulls(IMU_INT2, false, false);

    // imu.
    // init.
    err = qmi8658c_init(imu, i2c_imu, QMI8658C_I2C_ADDRESS_SECOND);
    if(err != E_NO_ERROR) return err;

    err = qmi8658c_soft_reset(imu);
    if(err != E_NO_ERROR) return err;

    sleep_ms(QMI8658C_RESET_TIME_MIN_MS + 1);

    err = qmi8658c_read_reg(imu, QMI8658C_REG_WHO_AM_I, &value);
    if(err != E_NO_ERROR) return err;

    if(value != QMI8658C_WHO_AM_I) return E_INVALID_OPERATION;

    // Автоинкремент адреса.
    err = qmi8658c_write_reg(imu, QMI8658C_REG_CTRL1, QMI8658C_CTRL1_ADDR_AI_DISABLED);
    if(err != E_NO_ERROR) return err;

    // Остановить всё.
    err = qmi8658c_write_reg(imu, QMI8658C_CTRL7_NONE, 0);
    if(err != E_NO_ERROR) return err;

    sleep_ms(1);

    // Настройка акселерометра.
    err = qmi8658c_write_reg(imu, QMI8658C_REG_CTRL2,
                                IMU_AFS |
                                QMI8658C_CTRL2_AODR_125HZ
                            );
    if(err != E_NO_ERROR) return err;

    // Настройка гироскопа.
    err = qmi8658c_write_reg(imu, QMI8658C_REG_CTRL3,
                                IMU_GFS |
                                IMU_GODR
                            );
    if(err != E_NO_ERROR) return err;

    // Настройка ФНЧ.
    err = qmi8658c_write_reg(imu, QMI8658C_REG_CTRL5,
                                QMI8658C_CTRL5_GLPF_ENABLED |
                                QMI8658C_CTRL5_GLPF_MODE_2_66_PERCENT_ODR |
                                QMI8658C_CTRL5_ALPF_ENABLED |
                                QMI8658C_CTRL5_ALPF_MODE_2_66_PERCENT_ODR
                            );
    if(err != E_NO_ERROR) return err;

    // Отключение детектора движений.
    err = qmi8658c_write_reg(imu, QMI8658C_REG_CTRL6, QMI8658C_CTRL6_SMOD_DISABLED);
    if(err != E_NO_ERROR) return err;

    // Включение.
    err = qmi8658c_write_reg(imu, QMI8658C_REG_CTRL7,
                                QMI8658C_CTRL7_SYNC_SMPL_DISABLED |
                                QMI8658C_CTRL7_SYS_HS_NORMAL |
                                QMI8658C_CTRL7_GSN_FULL_MODE |
                                QMI8658C_CTRL7_SEN_DISABLED |
                                QMI8658C_CTRL7_GEN_ENABLED |
                                QMI8658C_CTRL7_AEN_ENABLED
                            );
    if(err != E_NO_ERROR) return err;

    // CTRL8.
    err = qmi8658c_write_reg(imu, QMI8658C_REG_CTRL8, QMI8658C_CTRL8_CTRL9_HS_INT1);
    if(err != E_NO_ERROR) return err;

    // Настройка FIFO.
    err = qmi8658c_write_reg(imu, QMI8658C_REG_FIFO_CTRL,
                                QMI8658C_FIFO_CTRL_MODE_STREAM |
                                QMI8658C_FIFO_CTRL_SIZE_64SAMPLES
                            );
    if(err != E_NO_ERROR) return err;

    // Уровень заполнения FIFO для индикации.
    err = qmi8658c_write_reg(imu, QMI8658C_REG_FIFO_WTM_TH, IMU_FIFO_WATERMARK);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}


err_t imu_process_init(void)
{
    memset(&process_state, 0x0, sizeof(process_state));

    err_t err;

    init_i2c();

    err = init_imu_sensor();
    process_state.init_error = err;

    if(err != E_NO_ERROR){
        //for(;;){
            process_state.status = IMU_PROCESS_STATUS_INIT_SENSOR_ERROR;
            //asm("bkpt #0");
            //sleep_ms(100);
        //}
        return err;
    }

    err = imu_init(&imu, &imu_sensor, IMU_INT1, IMU_INT2);
    process_state.init_error = err;

    if(err != E_NO_ERROR){
        //for(;;){
            process_state.status = IMU_PROCESS_STATUS_INIT_IMU_ERROR;
            //asm("bkpt #0");
            //sleep_ms(100);
        //}
        return err;
    }

    return E_NO_ERROR;
}

void imu_process_iter(void)
{
    err_t err;

    err = imu_process(&imu);
    process_state.imu_error = err;

    if(err == E_NO_ERROR){
        process_state.roll = imu.roll;
        process_state.pitch = imu.pitch;

        process_state.status |= IMU_PROCESS_STATUS_VALID;
        process_state.status &= ~IMU_PROCESS_STATUS_IMU_ERROR;
    }else if(err != E_AGAIN){
        process_state.status &= ~IMU_PROCESS_STATUS_VALID;
        process_state.status |= IMU_PROCESS_STATUS_IMU_ERROR;
        //asm("bkpt #0");
        sleep_ms(1);
    }
}

void imu_main(void)
{
    err_t err;

    err = imu_process_init();
    if(err != E_NO_ERROR){
        for(;;){
            //asm("bkpt #0");
            sleep_ms(100);
        }
    }

    process_state.status = IMU_PROCESS_STATUS_RUN;

    for(;;){
        imu_process_iter();
    }
}

const imu_process_state_t *imu_process_get_state(void)
{
    return &process_state;
}