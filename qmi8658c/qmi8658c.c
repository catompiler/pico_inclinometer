#include "qmi8658c.h"


//! Тайм-аут i2c, мкс.
#define I2C_TIMEOUT_US 10000


ALWAYS_INLINE static bool is_i2c_address_valid(qmi8658c_i2c_address_t address)
{
    if(address > 0x7f) return false;
    return true;
}


static err_t check_res(int res)
{
    switch(res){
    case PICO_ERROR_NONE:
        break;
    case PICO_ERROR_TIMEOUT:
        //asm("bkpt #0");
        return E_TIME_OUT;
    case PICO_ERROR_GENERIC:
    default:
        //asm("bkpt #0");
        return E_IO_ERROR;
    }

    return E_NO_ERROR;
}


err_t qmi8658c_init(qmi8658c_t *imu, i2c_inst_t *i2c, qmi8658c_i2c_address_t address)
{
    if(i2c == NULL) return E_NULL_POINTER;
    if(!is_i2c_address_valid(address)) return E_INVALID_VALUE;

    imu->i2c = i2c;
    imu->address = address;

    return E_NO_ERROR;
}

err_t qmi8658c_read_reg(qmi8658c_t *imu, qmi8658c_reg_t reg_address, qmi8658c_reg_value_t *value)
{
    if(value == NULL) return E_NULL_POINTER;

    uint8_t buf[] = {
        reg_address
    };

    int res;

    // Запишем адрес регистра.
    res = i2c_write_timeout_us(imu->i2c, imu->address, buf, sizeof(buf), true, I2C_TIMEOUT_US);
    if(res != sizeof(buf)) return check_res(res);

    // Прочитаем данные.
    res = i2c_read_timeout_us(imu->i2c, imu->address, value, 1, false, I2C_TIMEOUT_US);
    if(res != 1) return check_res(res);

    return E_NO_ERROR;
}

err_t qmi8658c_write_reg(qmi8658c_t *imu, qmi8658c_reg_t reg_address, qmi8658c_reg_value_t value)
{
    uint8_t buf[] = {
        reg_address,
        value
    };

    int res;

    // Запишем адрес и данные регистра.
    res = i2c_write_timeout_us(imu->i2c, imu->address, buf, sizeof(buf), false, I2C_TIMEOUT_US);
    if(res != sizeof(buf)) return check_res(res);

    return E_NO_ERROR;
}

err_t qmi8658c_read_regs(qmi8658c_t *imu, qmi8658c_reg_t reg_address, qmi8658c_reg_value_t *values, size_t count)
{
    if(values == NULL) return E_NULL_POINTER;
    if(count == 0) return E_INVALID_VALUE;

    uint8_t buf[] = {
        reg_address
    };

    int res;

    // Запишем адрес регистра.
    res = i2c_write_timeout_us(imu->i2c, imu->address, buf, sizeof(buf), true, I2C_TIMEOUT_US);
    if(res != sizeof(buf)) return check_res(res);

    // Прочитаем данные.
    res = i2c_read_timeout_us(imu->i2c, imu->address, values, count, false, I2C_TIMEOUT_US);
    if(res != count) return check_res(res);

    return E_NO_ERROR;
}

err_t qmi8658c_write_regs(qmi8658c_t *imu, qmi8658c_reg_t reg_address, const qmi8658c_reg_value_t *values, size_t count)
{
    if(values == NULL) return E_NULL_POINTER;
    if(count == 0) return E_INVALID_VALUE;

    uint8_t buf[] = {
        reg_address
    };

    int res;

    // Запишем адрес регистра.
    res = i2c_write_timeout_us(imu->i2c, imu->address, buf, sizeof(buf), true, I2C_TIMEOUT_US);
    if(res != sizeof(buf)) return check_res(res);

    // Запишем данные.
    res = i2c_write_timeout_us(imu->i2c, imu->address, values, count, false, I2C_TIMEOUT_US);
    if(res != count) return check_res(res);

    return E_NO_ERROR;
}

err_t qmi8658c_soft_reset(qmi8658c_t *imu)
{
    return qmi8658c_write_reg(imu, QMI8658C_REG_RESET, QMI8658C_RESET_KEY);
}
