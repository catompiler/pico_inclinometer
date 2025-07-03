#ifndef QMI8658C_H
#define QMI8658C_H

#include "hardware/i2c.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "defs/defs.h"
#include "bits/bits.h"
#include "errors/errors.h"


//qmi8658c
//Qmi8658c
//QMI8658C

// Адреса регистров
typedef enum _Qmi8658c_Reg {
    QMI8658C_REG_WHO_AM_I = 0x00,          // Device Identifier
    QMI8658C_REG_REVISION_ID = 0x01,       // Device Revision ID
    QMI8658C_REG_CTRL1 = 0x02,             // SPI Interface and Sensor Enable
    QMI8658C_REG_CTRL2 = 0x03,             // Accelerometer: Output Data Rate, Full Scale, Self Test
    QMI8658C_REG_CTRL3 = 0x04,             // Gyroscope: Output Data Rate, Full Scale, Self Test
    QMI8658C_REG_CTRL4 = 0x05,             // Reserved - Special Settings
    QMI8658C_REG_CTRL5 = 0x06,             // Low pass filter setting
    QMI8658C_REG_CTRL6 = 0x07,             // AttitudeEngine Settings: Output Data Rate, Motion on Demand
    QMI8658C_REG_CTRL7 = 0x08,             // Enable Sensors
    QMI8658C_REG_CTRL8 = 0x09,             // Motion Detection Control
    QMI8658C_REG_CTRL9 = 0x0A,             // Host Commands
    QMI8658C_REG_CAL1_L = 0x0B,            // Calibration Register (lower 8 bits)
    QMI8658C_REG_CAL1_H = 0x0C,            // Calibration Register (upper 8 bits)
    QMI8658C_REG_CAL2_L = 0x0D,            // Calibration Register (lower 8 bits)
    QMI8658C_REG_CAL2_H = 0x0E,            // Calibration Register (upper 8 bits)
    QMI8658C_REG_CAL3_L = 0x0F,            // Calibration Register (lower 8 bits)
    QMI8658C_REG_CAL3_H = 0x10,            // Calibration Register (upper 8 bits)
    QMI8658C_REG_CAL4_L = 0x11,            // Calibration Register (lower 8 bits)
    QMI8658C_REG_CAL4_H = 0x12,            // Calibration Register (upper 8 bits)
    QMI8658C_REG_FIFO_WTM_TH = 0x13,       // FIFO watermark level, in ODRs
    QMI8658C_REG_FIFO_CTRL = 0x14,         // FIFO Setup
    QMI8658C_REG_FIFO_SMPL_CNT = 0x15,     // FIFO sample count LSBs
    QMI8658C_REG_FIFO_STATUS = 0x16,       // FIFO Status
    QMI8658C_REG_FIFO_DATA = 0x17,         // FIFO Data
    QMI8658C_REG_STATUSINT = 0x2D,         // Sensor Data Availability with the Locking mechanism, CmdDone
    QMI8658C_REG_STATUS0 = 0x2E,           // Output Data Over Run and Data Availability
    QMI8658C_REG_STATUS1 = 0x2F,           // Miscellaneous Status: Wake on Motion
    QMI8658C_REG_TIMESTAMP_LOW = 0x30,     // Sample Time Stamp (lower 8 bits)
    QMI8658C_REG_TIMESTAMP_MID = 0x31,     // Sample Time Stamp (middle 8 bits)
    QMI8658C_REG_TIMESTAMP_HIGH = 0x32,    // Sample Time Stamp (upper 8 bits)
    QMI8658C_REG_TEMP_L = 0x33,            // Temperature Output Data (lower 8 bits)
    QMI8658C_REG_TEMP_H = 0x34,            // Temperature Output Data (upper 8 bits)
    QMI8658C_REG_AX_L = 0x35,              // X-axis Acceleration (lower 8 bits)
    QMI8658C_REG_AX_H = 0x36,              // X-axis Acceleration (upper 8 bits)
    QMI8658C_REG_AY_L = 0x37,              // Y-axis Acceleration (lower 8 bits)
    QMI8658C_REG_AY_H = 0x38,              // Y-axis Acceleration (upper 8 bits)
    QMI8658C_REG_AZ_L = 0x39,              // Z-axis Acceleration (lower 8 bits)
    QMI8658C_REG_AZ_H = 0x3A,              // Z-axis Acceleration (upper 8 bits)
    QMI8658C_REG_GX_L = 0x3B,              // X-axis Angular Rate (lower 8 bits)
    QMI8658C_REG_GX_H = 0x3C,              // X-axis Angular Rate (upper 8 bits)
    QMI8658C_REG_GY_L = 0x3D,              // Y-axis Angular Rate (lower 8 bits)
    QMI8658C_REG_GY_H = 0x3E,              // Y-axis Angular Rate (upper 8 bits)
    QMI8658C_REG_GZ_L = 0x3F,              // Z-axis Angular Rate (lower 8 bits)
    QMI8658C_REG_GZ_H = 0x40,              // Z-axis Angular Rate (upper 8 bits)
    QMI8658C_REG_dQW_L = 0x49,             // Quaternion Increment dQW (lower 8 bits)
    QMI8658C_REG_dQW_H = 0x4A,             // Quaternion Increment dQW (upper 8 bits)
    QMI8658C_REG_dQX_L = 0x4B,             // Quaternion Increment dQX (lower 8 bits)
    QMI8658C_REG_dQX_H = 0x4C,             // Quaternion Increment dQX (upper 8 bits)
    QMI8658C_REG_dQY_L = 0x4D,             // Quaternion Increment dQY (lower 8 bits)
    QMI8658C_REG_dQY_H = 0x4E,             // Quaternion Increment dQY (upper 8 bits)
    QMI8658C_REG_dQZ_L = 0x4F,             // Quaternion Increment dQZ (lower 8 bits)
    QMI8658C_REG_dQZ_H = 0x50,             // Quaternion Increment dQZ (upper 8 bits)
    QMI8658C_REG_dVX_L = 0x51,             // Velocity Increment along X-axis (lower 8 bits)
    QMI8658C_REG_dVX_H = 0x52,             // Velocity Increment along X-axis (upper 8 bits)
    QMI8658C_REG_dVY_L = 0x53,             // Velocity Increment along Y-axis (lower 8 bits)
    QMI8658C_REG_dVY_H = 0x54,             // Velocity Increment along Y-axis (upper 8 bits)
    QMI8658C_REG_dVZ_L = 0x55,             // Velocity Increment along Z-axis (lower 8 bits)
    QMI8658C_REG_dVZ_H = 0x56,             // Velocity Increment along Z-axis (upper 8 bits)
    QMI8658C_REG_AE_REG1 = 0x57,           // AttitudeEngine Register 1
    QMI8658C_REG_AE_REG2 = 0x58,           // AttitudeEngine Register 2
    QMI8658C_REG_RESET = 0x60              // Soft Reset Register
} qmi8658c_reg_t;

// Константы регистров.
// Сгенерировано DeepSeek.
/**
 * Control Register 1 (CTRL1)
 * Address: 0x02
 * Type: Read/Write
 */
enum _Qmi8658c_Ctrl1 {
    QMI8658C_CTRL1_NONE = 0,
    
    // Sensor Disable (bit 0)
    // 0: Enable internal 2MHz oscillator, 1: Disable oscillator
    QMI8658C_CTRL1_SENSOR_DISABLE_Pos = 0,
    QMI8658C_CTRL1_SENSOR_DISABLE_Msk = (0x1 << 0),
    
    // Reserved bits (bits 4:1)
    QMI8658C_CTRL1_RESERVED_Pos      = 1,
    QMI8658C_CTRL1_RESERVED_Msk      = (0xF << 1),  // 4 бита
    
    // Data Endianness (bit 5)
    // 0: Little endian, 1: Big endian
    QMI8658C_CTRL1_BE_Pos            = 5,
    QMI8658C_CTRL1_BE_Msk            = (0x1 << 5),
    
    // Address Auto Increment (bit 6)
    // 0: Disabled, 1: Enabled
    QMI8658C_CTRL1_ADDR_AI_Pos       = 6,
    QMI8658C_CTRL1_ADDR_AI_Msk       = (0x1 << 6),
    
    // SPI Interface Mode (bit 7)
    // 0: 4-wire SPI, 1: 3-wire SPI
    QMI8658C_CTRL1_SPI_MODE_Pos      = 7,
    QMI8658C_CTRL1_SPI_MODE_Msk      = (0x1 << 7)
};

/**
 * Control Register 2 (CTRL2)
 * Address: 0x03
 * Type: Read/Write
 */
enum _Qmi8658c_Ctrl2 {
    QMI8658C_CTRL2_NONE = 0,
    
    // Accelerometer Output Data Rate (bits 3:0)
    // See Table 10 for ODR values
    QMI8658C_CTRL2_AODR_Pos          = 0,
    QMI8658C_CTRL2_AODR_Msk          = (0xF << 0),  // 4 бита
    
    // Accelerometer Full Scale (bits 6:4)
    // 000: ±2g, 001: ±4g, 010: ±8g, 011: ±16g
    QMI8658C_CTRL2_AFS_Pos           = 4,
    QMI8658C_CTRL2_AFS_Msk           = (0x7 << 4),  // 3 бита
    
    // Accelerometer Self Test (bit 7)
    // 0: Disabled, 1: Enabled
    QMI8658C_CTRL2_AST_Pos           = 7,
    QMI8658C_CTRL2_AST_Msk           = (0x1 << 7)
};

/**
 * Control Register 3 (CTRL3)
 * Address: 0x04
 * Type: Read/Write
 */
enum _Qmi8658c_Ctrl3 {
    QMI8658C_CTRL3_NONE = 0,

    // Gyroscope Output Data Rate (bits 3:0)
    // See Table 13 for ODR values
    QMI8658C_CTRL3_GODR_Pos          = 0,
    QMI8658C_CTRL3_GODR_Msk          = (0xF << 0),  // 4 бита

    // Gyroscope Full Scale (bits 6:4)
    // 000: ±16dps, 001: ±32dps, 010: ±64dps, 011: ±128dps
    // 100: ±256dps, 101: ±512dps, 110: ±1024dps, 111: ±2048dps
    QMI8658C_CTRL3_GFS_Pos           = 4,
    QMI8658C_CTRL3_GFS_Msk           = (0x7 << 4),  // 3 бита

    // Gyroscope Self Test (bit 7)
    // 0: Disabled, 1: Enabled
    QMI8658C_CTRL3_GST_Pos           = 7,
    QMI8658C_CTRL3_GST_Msk           = (0x1 << 7)
};

/**
 * Control Register 5 (CTRL5)
 * Address: 0x06
 * Type: Read/Write
 */
enum _Qmi8658c_Ctrl5 {
    QMI8658C_CTRL5_NONE = 0,

    // Accelerometer Low-Pass Filter Enable (bit 0)
    // 0: Disabled, 1: Enabled
    QMI8658C_CTRL5_ALPF_EN_Pos       = 0,
    QMI8658C_CTRL5_ALPF_EN_Msk       = (0x1 << 0),

    // Accelerometer Low-Pass Filter Mode (bits 2:1)
    // See Table 11 for bandwidth settings
    QMI8658C_CTRL5_ALPF_MODE_Pos     = 1,
    QMI8658C_CTRL5_ALPF_MODE_Msk     = (0x3 << 1),  // 2 бита

    // Reserved (bit 3)
    QMI8658C_CTRL5_RESERVED1_Pos     = 3,
    QMI8658C_CTRL5_RESERVED1_Msk     = (0x1 << 3),

    // Gyroscope Low-Pass Filter Enable (bit 4)
    // 0: Disabled, 1: Enabled
    QMI8658C_CTRL5_GLPF_EN_Pos       = 4,
    QMI8658C_CTRL5_GLPF_EN_Msk       = (0x1 << 4),

    // Gyroscope Low-Pass Filter Mode (bits 6:5)
    // See Table 12 for bandwidth settings
    QMI8658C_CTRL5_GLPF_MODE_Pos     = 5,
    QMI8658C_CTRL5_GLPF_MODE_Msk     = (0x3 << 5),  // 2 бита

    // Reserved (bit 7)
    QMI8658C_CTRL5_RESERVED2_Pos     = 7,
    QMI8658C_CTRL5_RESERVED2_Msk     = (0x1 << 7)
};

/**
 * Control Register 6 (CTRL6) - AttitudeEngine Settings
 * Address: 0x07
 * Type: Read/Write
 */
enum _Qmi8658c_Ctrl6 {
    QMI8658C_CTRL6_NONE = 0,

    // AttitudeEngine Output Data Rate (bits 2:0)
    // 000: 1Hz, 001: 2Hz, 010: 4Hz, 011: 8Hz
    // 100: 16Hz, 101: 32Hz, 110: 64Hz
    QMI8658C_CTRL6_AE_ODR_Pos        = 0,
    QMI8658C_CTRL6_AE_ODR_Msk        = (0x7 << 0),  // 3 бита

    // Reserved bits (bits 6:3) - Must be kept 0
    QMI8658C_CTRL6_RESERVED_Msk      = (0xF << 3),  // 4 бита

    // Motion on Demand Mode (bit 7)
    // 0: Disabled (ODR mode), 1: Enabled (polling mode)
    QMI8658C_CTRL6_MOTION_ON_DEMAND_Pos = 7,
    QMI8658C_CTRL6_MOTION_ON_DEMAND_Msk = (0x1 << 7)
};

/**
 * Control Register 7 (CTRL7)
 * Address: 0x08
 * Type: Read/Write
 */
enum _Qmi8658c_Ctrl7 {
    QMI8658C_CTRL7_NONE = 0,

    // Accelerometer Enable (bit 0)
    // 0: Standby/Power-down, 1: Enabled
    QMI8658C_CTRL7_AEN_Pos           = 0,
    QMI8658C_CTRL7_AEN_Msk           = (0x1 << 0),

    // Gyroscope Enable (bit 1)
    // 0: Standby/Power-down, 1: Enabled
    QMI8658C_CTRL7_GEN_Pos           = 1,
    QMI8658C_CTRL7_GEN_Msk           = (0x1 << 1),

    // Reserved (bit 2)
    QMI8658C_CTRL7_RESERVED1_Pos     = 2,
    QMI8658C_CTRL7_RESERVED1_Msk     = (0x1 << 2),

    // AttitudeEngine Enable (bit 3)
    // 0: Disabled, 1: Enabled
    QMI8658C_CTRL7_SEN_Pos           = 3,
    QMI8658C_CTRL7_SEN_Msk           = (0x1 << 3),

    // Gyroscope Snooze Mode (bit 4)
    // 0: Full Mode, 1: Snooze Mode (Drive only)
    QMI8658C_CTRL7_GSN_Pos           = 4,
    QMI8658C_CTRL7_GSN_Msk           = (0x1 << 4),

    // Reserved (bit 5)
    QMI8658C_CTRL7_RESERVED2_Pos     = 5,
    QMI8658C_CTRL7_RESERVED2_Msk     = (0x1 << 5),

    // High Speed Internal Clock (bit 6)
    // 0: Clock based on ODR, 1: High Speed
    QMI8658C_CTRL7_SYS_HS_Pos        = 6,
    QMI8658C_CTRL7_SYS_HS_Msk        = (0x1 << 6),

    // Sync Sample Mode (bit 7)
    // 0: Edge trigger mode, 1: Level mode
    QMI8658C_CTRL7_SYNC_SMPL_Pos     = 7,
    QMI8658C_CTRL7_SYNC_SMPL_Msk     = (0x1 << 7)
};

/**
 * Control Register 8 (CTRL8)
 * Address: 0x09
 * Type: Read/Write
 */
enum _Qmi8658c_Ctrl8 {
    QMI8658C_CTRL8_NONE = 0,

    // Tap Enable (bit 0)
    // 0: Disabled, 1: Enabled
    QMI8658C_CTRL8_TAP_EN_Pos        = 0,
    QMI8658C_CTRL8_TAP_EN_Msk        = (0x1 << 0),

    // Any Motion Enable (bit 1)
    // 0: Disabled, 1: Enabled
    QMI8658C_CTRL8_ANY_MOTION_EN_Pos = 1,
    QMI8658C_CTRL8_ANY_MOTION_EN_Msk = (0x1 << 1),

    // No Motion Enable (bit 2)
    // 0: Disabled, 1: Enabled
    QMI8658C_CTRL8_NO_MOTION_EN_Pos  = 2,
    QMI8658C_CTRL8_NO_MOTION_EN_Msk  = (0x1 << 2),

    // Significant Motion Enable (bit 3)
    // 0: Disabled, 1: Enabled
    QMI8658C_CTRL8_SIG_MOTION_EN_Pos = 3,
    QMI8658C_CTRL8_SIG_MOTION_EN_Msk = (0x1 << 3),

    // Pedometer Enable (bit 4)
    // 0: Disabled, 1: Enabled
    QMI8658C_CTRL8_PEDOMETER_EN_Pos  = 4,
    QMI8658C_CTRL8_PEDOMETER_EN_Msk  = (0x1 << 4),

    // Reserved (bits 5:6)
    QMI8658C_CTRL8_RESERVED_Pos      = 5,
    QMI8658C_CTRL8_RESERVED_Msk      = (0x3 << 5),  // 2 бита

    // CTRL9 Handshaking Type (bit 7)
    // 0: Use INT1, 1: Use STATUSINT.bit7
    QMI8658C_CTRL8_CTRL9_HS_TYPE_Pos = 7,
    QMI8658C_CTRL8_CTRL9_HS_TYPE_Msk = (0x1 << 7)
};

/**
 * Status Interrupt Register (STATUSINT)
 * Address: 0x2D
 * Type: Read Only
 */
enum _Qmi8658c_StatusInt {
    QMI8658C_STATUSINT_NONE = 0,

    // Data Available (bit 0)
    // 0: No new data, 1: New data available
    QMI8658C_STATUSINT_AVAIL_Pos     = 0,
    QMI8658C_STATUSINT_AVAIL_Msk     = (0x1 << 0),

    // Data Locked (bit 1)
    // 0: Not locked, 1: Data locked
    QMI8658C_STATUSINT_LOCKED_Pos    = 1,
    QMI8658C_STATUSINT_LOCKED_Msk    = (0x1 << 1),

    // Reserved (bits 4:2)
    QMI8658C_STATUSINT_RESERVED1_Pos = 2,
    QMI8658C_STATUSINT_RESERVED1_Msk = (0x7 << 2),  // 3 бита

    // CTRL9 Command Done (bit 7)
    // 0: Not completed, 1: Command done
    QMI8658C_STATUSINT_CMD_DONE_Pos  = 7,
    QMI8658C_STATUSINT_CMD_DONE_Msk  = (0x1 << 7)
};

/**
 * FIFO Control Register (FIFO_CTRL)
 * Address: 0x14
 * Type: Read/Write
 */
enum _Qmi8658c_FifoCtrl {
    QMI8658C_FIFO_CTRL_NONE = 0,

    // FIFO Mode (bits 1:0)
    // 00: Bypass, 01: FIFO, 10: Streaming, 11: Stream to FIFO
    QMI8658C_FIFO_CTRL_MODE_Pos      = 0,
    QMI8658C_FIFO_CTRL_MODE_Msk      = (0x3 << 0),  // 2 бита

    // FIFO Size (bits 3:2)
    // 00: 16 samples, 01: 32 samples, 10: 64 samples, 11: 128 samples
    QMI8658C_FIFO_CTRL_SIZE_Pos      = 2,
    QMI8658C_FIFO_CTRL_SIZE_Msk      = (0x3 << 2),  // 2 бита

    // Reserved (bits 6:4)
    QMI8658C_FIFO_CTRL_RESERVED_Pos  = 4,
    QMI8658C_FIFO_CTRL_RESERVED_Msk  = (0x7 << 4),  // 3 бита

    // FIFO Read Mode (bit 7)
    // 0: Disable FIFO read, 1: Enable FIFO read
    QMI8658C_FIFO_CTRL_RD_MODE_Pos   = 7,
    QMI8658C_FIFO_CTRL_RD_MODE_Msk   = (0x1 << 7)
};

/**
 * FIFO Status Register (FIFO_STATUS)
 * Address: 0x16
 * Type: Read Only
 */
enum _Qmi8658c_FifoStatus {
    QMI8658C_FIFO_STATUS_NONE = 0,

    // FIFO Sample Count MSB (bits 1:0)
    QMI8658C_FIFO_STATUS_SMPL_CNT_MSB_Pos = 0,
    QMI8658C_FIFO_STATUS_SMPL_CNT_MSB_Msk = (0x3 << 0),  // 2 бита

    // Reserved (bits 3:2)
    QMI8658C_FIFO_STATUS_RESERVED_Pos = 2,
    QMI8658C_FIFO_STATUS_RESERVED_Msk = (0x3 << 2),  // 2 бита

    // FIFO Not Empty (bit 4)
    // 0: Empty, 1: Not empty
    QMI8658C_FIFO_STATUS_NOT_EMPTY_Pos = 4,
    QMI8658C_FIFO_STATUS_NOT_EMPTY_Msk = (0x1 << 4),

    // FIFO Overflow (bit 5)
    // 0: No overflow, 1: Overflow occurred
    QMI8658C_FIFO_STATUS_OVFLOW_Pos  = 5,
    QMI8658C_FIFO_STATUS_OVFLOW_Msk  = (0x1 << 5),

    // Watermark Reached (bit 6)
    // 0: Not reached, 1: Reached
    QMI8658C_FIFO_STATUS_WTM_Pos     = 6,
    QMI8658C_FIFO_STATUS_WTM_Msk     = (0x1 << 6),

    // FIFO Full (bit 7)
    // 0: Not full, 1: Full
    QMI8658C_FIFO_STATUS_FULL_Pos    = 7,
    QMI8658C_FIFO_STATUS_FULL_Msk    = (0x1 << 7)
};

/**
 * Reset Register (RESET)
 * Address: 0x60
 * Type: Write Only
 */
enum _Qmi8658c_Reset {
    // Soft Reset (write 0xB0 to trigger reset)
    QMI8658C_RESET_KEY               = 0xB0
};


// Константы для значений полей регистров.

/* ==================================================== */
/* Control Register 1 (CTRL1) - Address 0x02 */
/* ==================================================== */

/* SPI Interface Mode (bit 7) */
#define QMI8658C_CTRL1_SPI_MODE_4WIRE        (0x0 << 7) /* 4-wire SPI интерфейс */
#define QMI8658C_CTRL1_SPI_MODE_3WIRE        (0x1 << 7) /* 3-wire SPI интерфейс */

/* Address Auto Increment (bit 6) */
#define QMI8658C_CTRL1_ADDR_AI_DISABLED      (0x0 << 6) /* Автоинкремент адреса выключен */
#define QMI8658C_CTRL1_ADDR_AI_ENABLED       (0x1 << 6) /* Автоинкремент адреса включен */

/* Data Endianness (bit 5) */
#define QMI8658C_CTRL1_BE_LITTLE_ENDIAN      (0x0 << 5) /* Little-endian порядок байт */
#define QMI8658C_CTRL1_BE_BIG_ENDIAN         (0x1 << 5) /* Big-endian порядок байт */

/* Sensor Disable (bit 0) */
#define QMI8658C_CTRL1_SENSOR_DISABLE_ENABLE (0x0 << 0) /* Включить внутренний 2МГц генератор */
#define QMI8658C_CTRL1_SENSOR_DISABLE_PWRDWN (0x1 << 0) /* Выключить генератор (режим пониженного потребления) */

/* ==================================================== */
/* Control Register 2 (CTRL2) - Address 0x03 */
/* ==================================================== */

/* Accelerometer Self Test (bit 7) */
#define QMI8658C_CTRL2_ACCEL_SELF_TEST  (0x1 << 7) /* Самотестирование акселерометра */

/* Accelerometer Full Scale (bits 6:4) */
#define QMI8658C_CTRL2_AFS_ACCEL_FULL_SCALE_2G  (0x0 << 4) /* ±2g диапазон */
#define QMI8658C_CTRL2_AFS_ACCEL_FULL_SCALE_4G  (0x1 << 4) /* ±4g диапазон */
#define QMI8658C_CTRL2_AFS_ACCEL_FULL_SCALE_8G  (0x2 << 4) /* ±8g диапазон */
#define QMI8658C_CTRL2_AFS_ACCEL_FULL_SCALE_16G (0x3 << 4) /* ±16g диапазон */

/* Accelerometer Output Data Rate (bits 3:0) */
#define QMI8658C_CTRL2_AODR_1000HZ          (0x3 << 0)  /* 1000 Hz (акселерометр только) */
#define QMI8658C_CTRL2_AODR_500HZ           (0x4 << 0)  /* 500 Hz */
#define QMI8658C_CTRL2_AODR_250HZ           (0x5 << 0)  /* 250 Hz */
#define QMI8658C_CTRL2_AODR_125HZ           (0x6 << 0)  /* 125 Hz */
#define QMI8658C_CTRL2_AODR_62_5HZ          (0x7 << 0)  /* 62.5 Hz */
#define QMI8658C_CTRL2_AODR_31_25HZ         (0x8 << 0)  /* 31.25 Hz */
#define QMI8658C_CTRL2_AODR_128HZ_LP        (0xC << 0)  /* 128 Hz (Low Power режим) */
#define QMI8658C_CTRL2_AODR_21HZ_LP         (0xD << 0)  /* 21 Hz (Low Power) */
#define QMI8658C_CTRL2_AODR_11HZ_LP         (0xE << 0)  /* 11 Hz (Low Power) */
#define QMI8658C_CTRL2_AODR_3HZ_LP          (0xF << 0)  /* 3 Hz (Low Power) */

/* ==================================================== */
/* Control Register 3 (CTRL3) - Address 0x04 */
/* ==================================================== */

/* Gyroscope Self Test (bit 7) */
#define QMI8658C_CTRL3_GYRO_SELF_TEST  (0x1 << 7) /* Самотестирование гироскопа */

/* Gyroscope Full Scale (bits 6:4) */
#define QMI8658C_CTRL3_GFS_GYRO_FULL_SCALE_16DPS   (0x0 << 4)  /* ±16 dps */
#define QMI8658C_CTRL3_GFS_GYRO_FULL_SCALE_32DPS   (0x1 << 4)  /* ±32 dps */
#define QMI8658C_CTRL3_GFS_GYRO_FULL_SCALE_64DPS   (0x2 << 4)  /* ±64 dps */
#define QMI8658C_CTRL3_GFS_GYRO_FULL_SCALE_128DPS  (0x3 << 4)  /* ±128 dps */
#define QMI8658C_CTRL3_GFS_GYRO_FULL_SCALE_256DPS  (0x4 << 4)  /* ±256 dps */
#define QMI8658C_CTRL3_GFS_GYRO_FULL_SCALE_512DPS  (0x5 << 4)  /* ±512 dps */
#define QMI8658C_CTRL3_GFS_GYRO_FULL_SCALE_1024DPS (0x6 << 4)  /* ±1024 dps */
#define QMI8658C_CTRL3_GFS_GYRO_FULL_SCALE_2048DPS (0x7 << 4)  /* ±2048 dps */

/* Gyroscope Output Data Rate (bits 3:0) */
#define QMI8658C_CTRL3_GODR_7520HZ         (0x0 << 0)  /* 7520 Hz */
#define QMI8658C_CTRL3_GODR_3760HZ         (0x1 << 0)  /* 3760 Hz */
#define QMI8658C_CTRL3_GODR_1880HZ         (0x2 << 0)  /* 1880 Hz */
#define QMI8658C_CTRL3_GODR_940HZ          (0x3 << 0)  /* 940 Hz */
#define QMI8658C_CTRL3_GODR_470HZ          (0x4 << 0)  /* 470 Hz */
#define QMI8658C_CTRL3_GODR_235HZ          (0x5 << 0)  /* 235 Hz */
#define QMI8658C_CTRL3_GODR_117_5HZ        (0x6 << 0)  /* 117.5 Hz */
#define QMI8658C_CTRL3_GODR_58_75HZ        (0x7 << 0)  /* 58.75 Hz */
#define QMI8658C_CTRL3_GODR_29_375HZ       (0x8 << 0)  /* 29.375 Hz */

/* ==================================================== */
/* Control Register 5 (CTRL5) - Address 0x06 */
/* ==================================================== */

/* Accelerometer Low-Pass Filter Mode (bits 2:1) */
#define QMI8658C_CTRL5_ALPF_MODE_2_66_PERCENT_ODR  (0x0 << 1)  /* BW = 2.66% of ODR */
#define QMI8658C_CTRL5_ALPF_MODE_3_63_PERCENT_ODR  (0x1 << 1)  /* BW = 3.63% of ODR */
#define QMI8658C_CTRL5_ALPF_MODE_5_39_PERCENT_ODR  (0x2 << 1)  /* BW = 5.39% of ODR */
#define QMI8658C_CTRL5_ALPF_MODE_13_37_PERCENT_ODR (0x3 << 1)  /* BW = 13.37% of ODR */

/* Gyroscope Low-Pass Filter Mode (bits 6:5) */
#define QMI8658C_CTRL5_GLPF_MODE_2_66_PERCENT_ODR  (0x0 << 5)  /* BW = 2.66% of ODR */
#define QMI8658C_CTRL5_GLPF_MODE_3_63_PERCENT_ODR  (0x1 << 5)  /* BW = 3.63% of ODR */
#define QMI8658C_CTRL5_GLPF_MODE_5_39_PERCENT_ODR  (0x2 << 5)  /* BW = 5.39% of ODR */
#define QMI8658C_CTRL5_GLPF_MODE_13_37_PERCENT_ODR (0x3 << 5)  /* BW = 13.37% of ODR */

/* Enable bits (bit 0 for Accel, bit 4 for Gyro) */
#define QMI8658C_CTRL5_ALPF_DISABLED        (0x0 << 0)  /* Акселерометр: фильтр выключен */
#define QMI8658C_CTRL5_ALPF_ENABLED         (0x1 << 0)  /* Акселерометр: фильтр включен */
#define QMI8658C_CTRL5_GLPF_DISABLED        (0x0 << 4)  /* Гироскоп: фильтр выключен */
#define QMI8658C_CTRL5_GLPF_ENABLED         (0x1 << 4)  /* Гироскоп: фильтр включен */

/* ==================================================== */
/* Control Register 6 (CTRL6) - Address 0x07 */
/* ==================================================== */

/* AttitudeEngine Output Data Rate (bits 2:0) */
#define QMI8658C_CTRL6_SODR_1HZ             (0x0 << 0)  /* 1 Hz Output Data Rate */
#define QMI8658C_CTRL6_SODR_2HZ             (0x1 << 0)  /* 2 Hz Output Data Rate */ 
#define QMI8658C_CTRL6_SODR_4HZ             (0x2 << 0)  /* 4 Hz Output Data Rate */
#define QMI8658C_CTRL6_SODR_8HZ             (0x3 << 0)  /* 8 Hz Output Data Rate */
#define QMI8658C_CTRL6_SODR_16HZ            (0x4 << 0)  /* 16 Hz Output Data Rate */
#define QMI8658C_CTRL6_SODR_32HZ            (0x5 << 0)  /* 32 Hz Output Data Rate */
#define QMI8658C_CTRL6_SODR_64HZ            (0x6 << 0)  /* 64 Hz Output Data Rate */

/* Motion on Demand (bit 7) */
#define QMI8658C_CTRL6_SMOD_DISABLED        (0x0 << 7)  /* Motion on Demand выключен */
#define QMI8658C_CTRL6_SMOD_ENABLED         (0x1 << 7)  /* Motion on Demand включен */

/* Reserved bits (bits 6:3) */
#define QMI8658C_CTRL6_RESERVED_MASK        (0x7 << 3)  /* Зарезервированные биты */

/* ==================================================== */
/* Control Register 7 (CTRL7) - Address 0x08           */
/* ==================================================== */

/* syncSmpl (bit 7) - Synchronous Sample Mode */
#define QMI8658C_CTRL7_SYNC_SMPL_DISABLED      (0x0 << 7)  /* Отключение синхронного режима выборки */
#define QMI8658C_CTRL7_SYNC_SMPL_ENABLED       (0x1 << 7)  /* Включение синхронного режима выборки (данные блокируются до чтения) */

/* sys_hs (bit 6) - High Speed Internal Clock */
#define QMI8658C_CTRL7_SYS_HS_NORMAL           (0x0 << 6)  /* Нормальный режим работы часов (на основе ODR) */
#define QMI8658C_CTRL7_SYS_HS_HIGH_SPEED       (0x1 << 6)  /* Высокоскоростные внутренние часы */

/* Reserved (bit 5) */
/* Этот бит зарезервирован и не должен изменяться */

/* gSN (bit 4) - Gyroscope Snooze Mode */
#define QMI8658C_CTRL7_GSN_FULL_MODE           (0x0 << 4)  /* Гироскоп в полном режиме (включены и привод, и сенсор) */
#define QMI8658C_CTRL7_GSN_SNOOZE_MODE         (0x1 << 4)  /* Гироскоп в режиме Snooze (только привод включен) */

/* sEN (bit 3) - AttitudeEngine Enable */
#define QMI8658C_CTRL7_SEN_DISABLED            (0x0 << 3)  /* AttitudeEngine выключен */
#define QMI8658C_CTRL7_SEN_ENABLED             (0x1 << 3)  /* AttitudeEngine включен */

/* Reserved (bit 2) */
/* Этот бит зарезервирован и не должен изменяться */

/* gEN (bit 1) - Gyroscope Enable */
#define QMI8658C_CTRL7_GEN_DISABLED            (0x0 << 1)  /* Гироскоп в режиме Standby или Power-down */
#define QMI8658C_CTRL7_GEN_ENABLED             (0x1 << 1)  /* Гироскоп включен */

/* aEN (bit 0) - Accelerometer Enable */
#define QMI8658C_CTRL7_AEN_DISABLED            (0x0 << 0)  /* Акселерометр в режиме Standby или Power-down */
#define QMI8658C_CTRL7_AEN_ENABLED             (0x1 << 0)  /* Акселерометр включен */

/* ==================================================== */
/* Control Register 8 (CTRL8) - Address 0x09           */
/* ==================================================== */

/* CTRL9 Handshaking Type (bit 7) */
#define QMI8658C_CTRL8_CTRL9_HS_INT1          (0x0 << 7)  /* Использовать INT1 для подтверждения выполнения команд CTRL9 */
#define QMI8658C_CTRL8_CTRL9_HS_STATUSINT     (0x1 << 7)  /* Использовать STATUSINT.bit7 для подтверждения выполнения команд CTRL9 */

/* Interrupt Pin for Motion Detection (bit 6) */
#define QMI8658C_CTRL8_MOTION_INT2            (0x0 << 6)  /* Использовать INT2 для прерываний motion detection */
#define QMI8658C_CTRL8_MOTION_INT1            (0x1 << 6)  /* Использовать INT1 для прерываний motion detection */

/* Reserved (bit 5) */
/* Этот бит зарезервирован и не должен изменяться */

/* Pedometer Enable (bit 4) */
#define QMI8658C_CTRL8_PEDOMETER_DISABLED     (0x0 << 4)  /* Шагомер выключен */
#define QMI8658C_CTRL8_PEDOMETER_ENABLED      (0x1 << 4)  /* Шагомер включен */

/* Significant Motion Enable (bit 3) */
#define QMI8658C_CTRL8_SIG_MOTION_DISABLED    (0x0 << 3)  /* Детектор значительного движения выключен */
#define QMI8658C_CTRL8_SIG_MOTION_ENABLED     (0x1 << 3)  /* Детектор значительного движения включен */

/* No Motion Enable (bit 2) */
#define QMI8658C_CTRL8_NO_MOTION_DISABLED     (0x0 << 2)  /* Детектор отсутствия движения выключен */
#define QMI8658C_CTRL8_NO_MOTION_ENABLED      (0x1 << 2)  /* Детектор отсутствия движения включен */

/* Any Motion Enable (bit 1) */
#define QMI8658C_CTRL8_ANY_MOTION_DISABLED    (0x0 << 1)  /* Детектор любого движения выключен */
#define QMI8658C_CTRL8_ANY_MOTION_ENABLED     (0x1 << 1)  /* Детектор любого движения включен */

/* Tap Enable (bit 0) */
#define QMI8658C_CTRL8_TAP_DISABLED           (0x0 << 0)  /* Детектор касаний (Tap) выключен */
#define QMI8658C_CTRL8_TAP_ENABLED            (0x1 << 0)  /* Детектор касаний (Tap) включен */

/* ==================================================== */
/* FIFO Control Register (FIFO_CTRL) - Address 0x14 */
/* ==================================================== */

/* FIFO Mode (bits 1:0) */
#define QMI8658C_FIFO_CTRL_MODE_BYPASS     (0x0 << 0)  /* FIFO отключен */
#define QMI8658C_FIFO_CTRL_MODE_FIFO       (0x1 << 0)  /* Обычный FIFO режим */
#define QMI8658C_FIFO_CTRL_MODE_STREAM     (0x2 << 0)  /* Режим потоковой передачи */
#define QMI8658C_FIFO_CTRL_MODE_STREAM_TO_FIFO (0x3 << 0) /* Потоковая передача с переключением в FIFO */

/* FIFO Size (bits 3:2) */
#define QMI8658C_FIFO_CTRL_SIZE_16SAMPLES  (0x0 << 2)  /* 16 сэмплов на сенсор */
#define QMI8658C_FIFO_CTRL_SIZE_32SAMPLES  (0x1 << 2)  /* 32 сэмпла */
#define QMI8658C_FIFO_CTRL_SIZE_64SAMPLES  (0x2 << 2)  /* 64 сэмпла */
#define QMI8658C_FIFO_CTRL_SIZE_128SAMPLES (0x3 << 2)  /* 128 сэмплов (только для 2 сенсоров) */

/* FIFO Read Mode (bit 7) */
#define QMI8658C_FIFO_CTRL_RD_MODE_DISABLED   (0x0 << 7)  /* Чтение FIFO через регистр FIFO_DATA отключено */
#define QMI8658C_FIFO_CTRL_RD_MODE_ENABLED    (0x1 << 7)  /* Чтение FIFO через регистр FIFO_DATA включено */

/* ==================================================== */
/* FIFO Status Register (FIFO_STATUS) - Address 0x16   */
/* ==================================================== */

/* FIFO Full Flag (bit 7) */
#define QMI8658C_FIFO_STATUS_FULL             (0x1 << 7)  /* FIFO полностью заполнен */
#define QMI8658C_FIFO_STATUS_NOT_FULL         (0x0 << 7)  /* FIFO не заполнен */

/* FIFO Watermark Reached (bit 6) */
#define QMI8658C_FIFO_STATUS_WTM_REACHED      (0x1 << 6)  /* Уровень Watermark достигнут */
#define QMI8658C_FIFO_STATUS_WTM_NOT_REACHED  (0x0 << 6)  /* Уровень Watermark не достигнут */

/* FIFO Overflow Flag (bit 5) */
#define QMI8658C_FIFO_STATUS_OVERFLOW         (0x1 << 5)  /* Произошло переполнение FIFO */
#define QMI8658C_FIFO_STATUS_NO_OVERFLOW      (0x0 << 5)  /* Переполнения нет */

/* FIFO Not Empty Flag (bit 4) */
#define QMI8658C_FIFO_STATUS_NOT_EMPTY        (0x1 << 4)  /* FIFO не пуст */
#define QMI8658C_FIFO_STATUS_EMPTY            (0x0 << 4)  /* FIFO пуст */

/* Reserved Bits (bits 3-2) - Не используются */

/* FIFO Sample Count MSB (bits 1-0) */
#define QMI8658C_FIFO_STATUS_SMPL_CNT_MSB_MASK (0x3)      /* Маска для старших битов счётчика */
#define QMI8658C_FIFO_STATUS_SMPL_CNT_MSB_VAL(val) ((val) & 0x3) /* Извлечение MSB счётчика */

/* ==================================================== */
/* Status Interrupt Register (STATUSINT) - Address 0x2D */
/* ==================================================== */

/* Data Available (bit 0) */
#define QMI8658C_STATUSINT_AVAIL_NO_DATA      (0x0 << 0) /* Новых данных нет */
#define QMI8658C_STATUSINT_AVAIL_NEW_DATA     (0x1 << 0) /* Есть новые данные */

/* Data Locked (bit 1) */
#define QMI8658C_STATUSINT_LOCKED_UNLOCKED    (0x0 << 1) /* Данные не заблокированы */
#define QMI8658C_STATUSINT_LOCKED_LOCKED      (0x1 << 1) /* Данные заблокированы */

/* CTRL9 Command Done (bit 7) */
#define QMI8658C_STATUSINT_CTRL9_CMD_NOT_COMPLETED (0x0 << 7) /* Команда CTRL9 не выполнена */
#define QMI8658C_STATUSINT_CTRL9_CMD_DONE          (0x1 << 7) /* Команда CTRL9 выполнена */

/* ==================================================== */
/* Status Register 0 (STATUS0) - Address 0x2E */
/* ==================================================== */

/* Accelerometer Data Available (bit 0) */
#define QMI8658C_STATUS0_ADA_NO_DATA         (0x0 << 0) /* Нет новых данных акселерометра */
#define QMI8658C_STATUS0_ADA_NEW_DATA        (0x1 << 0) /* Новые данные акселерометра доступны */

/* Gyroscope Data Available (bit 1) */
#define QMI8658C_STATUS0_GDA_NO_DATA         (0x0 << 1) /* Нет новых данных гироскопа */
#define QMI8658C_STATUS0_GDA_NEW_DATA        (0x1 << 1) /* Новые данные гироскопа доступны */

/* AttitudeEngine Data Available (bit 3) */
#define QMI8658C_STATUS0_SDA_NO_DATA         (0x0 << 3) /* Нет новых данных AttitudeEngine */
#define QMI8658C_STATUS0_SDA_NEW_DATA        (0x1 << 3) /* Новые данные AttitudeEngine доступны */

/* ==================================================== */
/* Status Register 1 (STATUS1) - Address 0x2F          */
/* ==================================================== */

/* Significant Motion Detected (bit 7) */
#define QMI8658C_STATUS1_SIGNIFICANT_MOTION    (0x1 << 7)  /* Зафиксировано значительное движение */
#define QMI8658C_STATUS1_NO_SIGNIFICANT_MOTION (0x0 << 7)  /* Значительное движение не обнаружено */

/* No Motion Detected (bit 6) */
#define QMI8658C_STATUS1_NO_MOTION_DETECTED    (0x1 << 6)  /* Зафиксировано отсутствие движения */
#define QMI8658C_STATUS1_MOTION_DETECTED       (0x0 << 6)  /* Отсутствие движения не обнаружено */

/* Any Motion Detected (bit 5) */
#define QMI8658C_STATUS1_ANY_MOTION_DETECTED   (0x1 << 5)  /* Зафиксировано любое движение */
#define QMI8658C_STATUS1_NO_ANY_MOTION         (0x0 << 5)  /* Движение не обнаружено */

/* Pedometer Step Detected (bit 4) */
#define QMI8658C_STATUS1_PEDOMETER_STEP        (0x1 << 4)  /* Зафиксирован шаг (для шагомера) */
#define QMI8658C_STATUS1_NO_PEDOMETER_STEP     (0x0 << 4)  /* Шаг не зафиксирован */

/* Reserved Bits (bits 3-2) - Не используются */

/* Tap Detected (bit 1) */
#define QMI8658C_STATUS1_TAP_DETECTED          (0x1 << 1)  /* Зафиксировано касание (Tap) */
#define QMI8658C_STATUS1_NO_TAP_DETECTED       (0x0 << 1)  /* Касание не зафиксировано */

/* Reserved Bit (bit 0) - Не используется */

/* Комбинированные флаги для удобства */
#define QMI8658C_STATUS1_MOTION_EVENTS_MASK    (0xF0)      /* Маска всех событий движения (биты 7-4) */
#define QMI8658C_STATUS1_TAP_EVENT_MASK        (0x02)      /* Маска события касания */

/* ==================================================== */
/* AE Status Registers (AE_REG1, AE_REG2) */
/* ==================================================== */

/* Sensor Clipping Status (AE_REG1 bits 0:5) */
#define QMI8658C_AE_REG1_AX_CLIP            (0x1 << 0) /* Акселерометр X: выход за пределы */
#define QMI8658C_AE_REG1_AY_CLIP            (0x1 << 1) /* Акселерометр Y: выход за пределы */
#define QMI8658C_AE_REG1_AZ_CLIP            (0x1 << 2) /* Акселерометр Z: выход за пределы */
#define QMI8658C_AE_REG1_WX_CLIP            (0x1 << 3) /* Гироскоп X: выход за пределы */
#define QMI8658C_AE_REG1_WY_CLIP            (0x1 << 4) /* Гироскоп Y: выход за пределы */
#define QMI8658C_AE_REG1_WZ_CLIP            (0x1 << 5) /* Гироскоп Z: выход за пределы */

/* Gyroscope Bias Update (AE_REG1 bit 6) */
#define QMI8658C_AE_REG1_GYRO_BIAS_UPDATED  (0x1 << 6) /* Смещение гироскопа обновлено */

/* Velocity Increment Overflow (AE_REG2 bits 0:2) */
#define QMI8658C_AE_REG2_DVX_OF             (0x1 << 0) /* Переполнение по оси X */
#define QMI8658C_AE_REG2_DVY_OF             (0x1 << 1) /* Переполнение по оси Y */
#define QMI8658C_AE_REG2_DVZ_OF             (0x1 << 2) /* Переполнение по оси Z */


// Команды для CTRL9.

/**
 * Control Register 9 (CTRL9) Commands
 * Address: 0x0A
 * Usage: Write command value to execute predefined functions
 * Protocol types:
 *   - WCtrl9: Write data to CAL regs before command
 *   - Ctrl9R: Read data after command
 *   - Ctrl9: No data transfer
 */
typedef enum {
    /* No operation */
    QMI8658C_CTRL9_CMD_NOP = 0x00,           // Ctrl9: Пустая команда

    /* Gyroscope Bias Update */
    QMI8658C_CTRL9_CMD_GYRO_BIAS = 0x01,      // WCtrl9: Копирует bias_gx/y/z из CAL1-3 в FIFO
    
    /* Motion-on-Demand Data Request */
    QMI8658C_CTRL9_CMD_REQ_SDI = 0x03,        // Ctrl9R: Запрос данных в Motion-on-Demand режиме
    
    /* FIFO Reset */
    QMI8658C_CTRL9_CMD_RST_FIFO = 0x04,       // Ctrl9: Сброс FIFO
    
    /* FIFO Data Request */
    QMI8658C_CTRL9_CMD_REQ_FIFO = 0x05,       // Ctrl9R: Чтение данных FIFO
    
    /* Wake-on-Motion Configuration */
    QMI8658C_CTRL9_CMD_WRITE_WOM = 0x08,      // WCtrl9: Настройка WoM (порог, интервал)
    
    /* Accelerometer Offset Calibration */
    QMI8658C_CTRL9_CMD_ACCEL_OFFSET = 0x09,   // WCtrl9: Ручная коррекция смещения акселерометра
    
    /* Gyroscope Offset Calibration */
    QMI8658C_CTRL9_CMD_GYRO_OFFSET = 0x0A,    // WCtrl9: Ручная коррекция смещения гироскопа
    
    /* Device ID and Firmware Read */
    QMI8658C_CTRL9_CMD_COPY_USID = 0x10,      // Ctrl9R: Чтение USID и версии прошивки
    
    /* IO Pull-up Configuration */
    QMI8658C_CTRL9_CMD_SET_RPU = 0x11,        // WCtrl9: Настройка подтягивающих резисторов
    
    /* AHB Clock Gating Control */
    QMI8658C_CTRL9_CMD_AHB_CLOCK = 0x12,      // WCtrl9: Управление тактированием AHB
    
    /* On-Demand Gyro Calibration */
    QMI8658C_CTRL9_CMD_ONDEMAND_CAL = 0xA2    // WCtrl9: Фоновая калибровка гироскопа
} QMI8658C_Ctrl9_Cmnd;


//! Значение подтверждения команды.
#define QMI8658C_CTRL9_CMD_ACK 0x0


//! Тип значения регистра.
typedef uint8_t qmi8658c_reg_value_t;

//! Адреса на шине в зависимости от состояния ножки SA0.
enum _Qmi8658c_I2c_Address {
    QMI8658C_I2C_ADDRESS_FIRST = 0x6a,
    QMI8658C_I2C_ADDRESS_SECOND = 0x6b,
};

//! Адрес на шине по-умолчанию.
#define QMI8658C_I2C_ADDRESS_DEFAULT QMI8658C_I2C_ADDRESS_FIRST

//! Тип адреса.
typedef uint8_t qmi8658c_i2c_address_t;

//! Значение WHO_AM_I.
#define QMI8658C_WHO_AM_I 0x5


//! Продолжительность сброса.
#define QMI8658C_RESET_TIME_MIN_MS 4


//! Тип датчика.
typedef struct _Qmi8658c {
    i2c_inst_t* i2c; //!< Шина i2c.
    qmi8658c_i2c_address_t address; //!< Адрес.
} qmi8658c_t;


/**
 * @brief Инициализирует датчик.
 * @param imu Датчик.
 * @param i2c Шина i2c.
 * @param address Адрес.
 * @return Код ошибки.
 */
EXTERN err_t qmi8658c_init(qmi8658c_t* imu, i2c_inst_t* i2c, qmi8658c_i2c_address_t address);

/**
 * @brief Читает значение регистра.
 * @param imu Датчик.
 * @param reg_address Адрес регистра.
 * @param value Значение регистра.
 * @return Код ошибки.
 */
EXTERN err_t qmi8658c_read_reg(qmi8658c_t* imu, qmi8658c_reg_t reg_address, qmi8658c_reg_value_t* value);

/**
 * @brief Записывает значение регистра.
 * @param imu Датчик.
 * @param reg_address Адрес регистра.
 * @param value Значение регистра.
 * @return Код ошибки.
 */
EXTERN err_t qmi8658c_write_reg(qmi8658c_t* imu, qmi8658c_reg_t reg_address, qmi8658c_reg_value_t value);

/**
 * @brief Читает значения регистров.
 * @param imu Датчик.
 * @param reg_address Адрес начального регистра.
 * @param value Значения регистров.
 * @param count Количество регистров.
 * @return Код ошибки.
 */
EXTERN err_t qmi8658c_read_regs(qmi8658c_t* imu, qmi8658c_reg_t reg_address, qmi8658c_reg_value_t* values, size_t count);

/**
 * @brief Записывает значения регистров.
 * @param imu Датчик.
 * @param reg_address Адрес начального регистра.
 * @param value Значения регистров.
 * @param count Количество регистров.
 * @return Код ошибки.
 */
EXTERN err_t qmi8658c_write_regs(qmi8658c_t* imu, qmi8658c_reg_t reg_address, const qmi8658c_reg_value_t* values, size_t count);

/**
 * @brief Программно сбрасывает датчик.
 * @param imu Датчик.
 * @return Код ошибки.
 */
EXTERN err_t qmi8658c_soft_reset(qmi8658c_t* imu);


#endif //QMI8658C_H