#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "hardware/irq.h"
#include "dma/dma_irq_mux.h"
#include "spi/spi.h"
#include "gc9a01a/gc9a01a.h"
#include "gc9a01a/gc9a01a_cache.h"
#include "gc9a01a/gc9a01a_cache_vbuf.h"
#include "graphics/graphics.h"
#include "graphics/painter.h"
#include "graphics/font.h"
#include "fonts_decls.h"
#include "anime_image_240.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "pico/stdio.h"
#include "errors/errors.h"
#include "hardware/i2c.h"
#include "qmi8658c/qmi8658c.h"
#include "imu/imu.h"


// // It's a Trap!
// static err_t dbg_err_int(err_t err)
// {
//     asm("bkpt #0");

//     return err;
// }

// #define DBG_ERR(E) dbg_err_int(E)


#define IMU_SDA 6
#define IMU_SCL 7
#define IMU_INT1 23
#define IMU_INT2 24

#define i2c_imu i2c1
static qmi8658c_t imu_sensor;

// IMU.
imu_t imu;


#define TFT_MOSI 11
#define TFT_SCK 10
#define TFT_CS 9
#define TFT_RST 12
#define TFT_DC 8
#define TFT_BL 25


static spi_bus_t spi_tft;

/*
 * Экран.
 */
//! Размер пиксела - 2 байта (16 бит).
#define TFT_PIXEL_SIZE 2
//! Ширина экрана.
#define TFT_WIDTH 320
#define TFT_HEIGHT 240
//! Число буферов кэша TFT.
#define TFT_CACHE_BUFS_COUNT 2
//! Размер первого буфера.
#define TFT_CACHE_BUF0_PIXELS 240
#define TFT_CACHE_BUF0_SIZE (TFT_CACHE_BUF0_PIXELS * TFT_PIXEL_SIZE)
//! Размер второго буфера.
#define TFT_CACHE_BUF1_PIXELS 240
#define TFT_CACHE_BUF1_SIZE (TFT_CACHE_BUF1_PIXELS * TFT_PIXEL_SIZE)
// TFT.
static gc9a01a_t tft;
//! Первый буфер кэша TFT.
static uint8_t tft_cache_buf_data0[TFT_CACHE_BUF0_SIZE];
//! Второй буфер кэша TFT.
static uint8_t tft_cache_buf_data1[TFT_CACHE_BUF1_SIZE];

//! Буферы кэша TFT.
static gc9a01a_cache_buffer_t tft_cache_bufs[TFT_CACHE_BUFS_COUNT] = {
    make_gc9a01a_cache_buffer(tft_cache_buf_data0, TFT_CACHE_BUF0_SIZE),
    make_gc9a01a_cache_buffer(tft_cache_buf_data1, TFT_CACHE_BUF1_SIZE)
};
//! Кэш TFT.
static gc9a01a_cache_t tft_cache = make_gc9a01a_cache(&tft, TFT_PIXEL_SIZE, tft_cache_bufs, TFT_CACHE_BUFS_COUNT, GC9A01A_ROW_COL_REVERSE_MODE);
//! Виртуальный буфер изображения..
static graphics_vbuf_t graph_vbuf = make_gc9a01a_cache_vbuf();
//! Изображение на экране.
static graphics_t graphics = make_gc9a01a_cache_graphics(&tft_cache, &graph_vbuf, TFT_WIDTH, TFT_HEIGHT, GRAPHICS_FORMAT_RGB_565);
static painter_t painter = make_painter(&graphics);

/*
//! Битмапы шрифта 5x8.
static const font_bitmap_t font_5x8_utf8_bitmaps[] = {
    make_font_bitmap(32, 127, font_5x8_utf8_part0_data, FONT_5X8_UTF8_PART0_WIDTH, FONT_5X8_UTF8_PART0_HEIGHT, GRAPHICS_FORMAT_BW_1_V),
    make_font_bitmap(0xb0, 0xb0, font_5x8_utf8_part1_data, FONT_5X8_UTF8_PART1_WIDTH, FONT_5X8_UTF8_PART1_HEIGHT, GRAPHICS_FORMAT_BW_1_V),
    make_font_bitmap(0x400, 0x451, font_5x8_utf8_part2_data, FONT_5X8_UTF8_PART2_WIDTH, FONT_5X8_UTF8_PART2_HEIGHT, GRAPHICS_FORMAT_BW_1_V)
};
//! Шрифт 5x8.
static font_t font5x8 = make_font_defchar(font_5x8_utf8_bitmaps, 3, 5, 8, 1, 1, 63);
*/
/*
//! Битмапы шрифта 10x16.
const font_bitmap_t font_10x16_utf8_bitmaps[] = {
    make_font_bitmap(32, 127, font_10x16_utf8_part0_data, FONT_10X16_UTF8_PART0_WIDTH, FONT_10X16_UTF8_PART0_HEIGHT, GRAPHICS_FORMAT_BW_1_V),
    make_font_bitmap(0xb0, 0xb0, font_10x16_utf8_part1_data, FONT_10X16_UTF8_PART1_WIDTH, FONT_10X16_UTF8_PART1_HEIGHT, GRAPHICS_FORMAT_BW_1_V),
    make_font_bitmap(0x400, 0x451, font_10x16_utf8_part2_data, FONT_10X16_UTF8_PART2_WIDTH, FONT_10X16_UTF8_PART2_HEIGHT, GRAPHICS_FORMAT_BW_1_V)
};
//! Шрифт 10x16.
static font_t font10x16 = make_font_defchar(font_10x16_utf8_bitmaps, 3, 10, 16, 1, 0, 63);
*/
#define MAKE_RGB(r, g, b) GC9A01A_MAKE_RGB565(r, g, b)

static graphics_t img_graphics = make_graphics(anime_image_240, ANIME_IMAGE_240_WIDTH, ANIME_IMAGE_240_HEIGHT, GRAPHICS_FORMAT_RGB_565);


static void spi_irq_handler(void)
{
    spi_bus_irq_handler(&spi_tft);
}

/*static void spi_dma_rx_irq_handler(void)
{
    spi_bus_dma_rx_channel_irq_handler(&spi_tft);
}

static void spi_dma_tx_irq_handler(void)
{
    spi_bus_dma_tx_channel_irq_handler(&spi_tft);
}*/

static void init_dma_irq_mux(void)
{
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_mux_irq_0_handler);
    irq_set_priority(DMA_IRQ_0, 0);
    irq_set_enabled(DMA_IRQ_0, true);

    irq_set_exclusive_handler(DMA_IRQ_1, dma_irq_mux_irq_1_handler);
    irq_set_priority(DMA_IRQ_1, 0);
    irq_set_enabled(DMA_IRQ_1, true);
}

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


static void init_spi(void)
{
    spi_inst_t* spi = spi1;

    spi_init(spi, 62500000);
    spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    //hw_set_bits(&spi_get_hw(spi)->cr1, SPI_SSPCR1_LBM_BITS); // loopback.

    // mosi
    gpio_init(TFT_MOSI);
    gpio_set_slew_rate(TFT_MOSI, GPIO_SLEW_RATE_FAST);
    gpio_set_drive_strength(TFT_MOSI, GPIO_DRIVE_STRENGTH_4MA);
    gpio_put(TFT_MOSI, 0);
    gpio_set_dir(TFT_MOSI, true);
    gpio_set_function(TFT_MOSI, GPIO_FUNC_SPI);
    // sck
    gpio_init(TFT_SCK);
    gpio_set_slew_rate(TFT_SCK, GPIO_SLEW_RATE_FAST);
    gpio_set_drive_strength(TFT_SCK, GPIO_DRIVE_STRENGTH_4MA);
    gpio_put(TFT_SCK, 0);
    gpio_set_dir(TFT_SCK, true);
    gpio_set_function(TFT_SCK, GPIO_FUNC_SPI);

    irq_set_exclusive_handler(SPI0_IRQ, spi_irq_handler);
    irq_set_priority(SPI0_IRQ, 0);
    irq_set_enabled(SPI0_IRQ, true);

    spi_bus_init_t is;
    is.spi_device = spi_get_hw(spi);
    is.dma_rx_channel = 0;
    is.dma_rx_dreq = spi_get_dreq(spi, false);
    is.dma_rx_irq_index = 0;
    is.dma_tx_channel = 1;
    is.dma_tx_dreq = spi_get_dreq(spi, true);
    is.dma_tx_irq_index = 0;

    spi_bus_init(&spi_tft, &is);
    spi_bus_set_user_data(&spi_tft, &tft);
    spi_bus_set_callback(&spi_tft, (spi_callback_t)gc9a01a_spi_callback);
}

static void init_tft(void)
{
    // GPIO.
    // cs
    gpio_init(TFT_CS);
    gpio_put(TFT_CS, 1);
    gpio_set_dir(TFT_CS, GPIO_OUT);
    // dc
    gpio_init(TFT_DC);
    gpio_put(TFT_DC, 1);
    gpio_set_dir(TFT_DC, GPIO_OUT);
    // reset
    gpio_init(TFT_RST);
    gpio_put(TFT_RST, 1);
    gpio_set_dir(TFT_RST, GPIO_OUT);
    // bl
    gpio_init(TFT_BL);
    gpio_put(TFT_BL, 1);
    gpio_set_dir(TFT_BL, GPIO_OUT);

    gc9a01a_init_t is;

    is.spi = &spi_tft;
    is.transfer_id = GC9A01A_DEFAULT_TRANSFER_ID;
    is.ce_gpio = 0;
    is.ce_pin = TFT_CS;
    is.reset_gpio = 0;
    is.reset_pin = TFT_RST;
    is.dc_gpio = 0;
    is.dc_pin = TFT_DC;

    gc9a01a_init(&tft, &is);
}

static void setup_tft(void)
{
    gc9a01a_reset(&tft);

    gc9a01a_send_init(&tft);

    // gc9a01a_cache_fill(&tft_cache, GC9A01A_MAKE_RGB565(0xff, 0, 0xff));
    painter_set_brush(&painter, PAINTER_BRUSH_SOLID);
    painter_set_pen(&painter, PAINTER_PEN_SOLID);
    
    painter_set_font(&painter, &font_droid_sans_33x37);
    painter_set_pen_color(&painter, GC9A01A_MAKE_RGB565(0xff, 0, 0));
    painter_set_brush_color(&painter, GC9A01A_MAKE_RGB565(0x0, 0, 0x0));

    painter_fill(&painter);
    painter_flush(&painter);
}

static uint16_t pixel = GC9A01A_MAKE_RGB565(0x00, 0xff, 0xff);

int main(void)
{
    stdio_init_all();

    init_dma_irq_mux();

    err_t err;

    init_spi();
    init_tft();
    setup_tft();

    init_i2c();
    err = init_imu_sensor();

    if(err != E_NO_ERROR){
        painter_set_source_image_mode(&painter, PAINTER_SOURCE_IMAGE_MODE_BITMAP);
        painter_set_pen_color(&painter, GC9A01A_MAKE_RGB565(0xff, 0, 0));
        painter_draw_string(&painter, 100, 0, "IMU ERR!");
        for(;;){
            sleep_ms(1000);
        }
    }

    imu_init(&imu, &imu_sensor, IMU_INT1, IMU_INT2);

    const size_t str_buf_len = 128;
    char str_buf[str_buf_len];

    for(;;){
        painter_fill(&painter);

        err = imu_process(&imu);
        if(err == E_NO_ERROR){
            memset(str_buf, 0x0, str_buf_len);
            /*int n = printf(//str_buf, str_buf_len-1,
                             "%f,%f,%f,%f,%f,%f\n",
                             imu.data.accel_x,
                             imu.data.accel_y,
                             imu.data.accel_z,
                             imu.data.gyro_x,
                             imu.data.gyro_y,
                             imu.data.gyro_z);*/
            float roll  = imu.roll  / 3.14159265359f * 180.0f;
            float pitch = imu.pitch / 3.14159265359f * 180.0f;
            printf("roll: %.2f° pitch: %.2f°\n", roll, pitch);
            int n = snprintf(str_buf, str_buf_len-1, "roll: %.2f°\npitch: %.2f°", roll, pitch);
            if(n >= 0) str_buf[n] = '\0';
            painter_set_pen_color(&painter, GC9A01A_MAKE_RGB565(0xff, 0xff, 0xff));
            painter_set_source_image_mode(&painter, PAINTER_SOURCE_IMAGE_MODE_BITMAP);
            painter_draw_string(&painter, 10, 80, str_buf);
        }else if(err != E_AGAIN){
            memset(str_buf, 0x0, str_buf_len);
            int n = snprintf(str_buf, str_buf_len-1,
                             "imu\nrd\nerr:\n%d\n",
                             (int)err);
            if(n >= 0) str_buf[n] = '\0';
            painter_set_pen_color(&painter, GC9A01A_MAKE_RGB565(0xff, 0xff, 0));
                painter_set_source_image_mode(&painter, PAINTER_SOURCE_IMAGE_MODE_BITMAP);
                painter_draw_string(&painter, 100, 0, str_buf);
        }

        /*painter_set_pen_color(&painter, GC9A01A_MAKE_RGB565(0, 0, 0xff));
        painter_set_brush_color(&painter, GC9A01A_MAKE_RGB565(0x0, 0xff, 0x0));
        painter_set_source_image_mode(&painter, PAINTER_SOURCE_IMAGE_MODE_BITMAP);
        painter_draw_string(&painter, 100, 0, "Ня!");
        //gc9a01a_set_column_address(&tft, 0, 239);
        //gc9a01a_set_page_address(&tft, 0, 239);
        //gc9a01a_write(&tft, anime_image_240, ANIME_IMAGE_240_HEIGHT * ANIME_IMAGE_240_WIDTH * 2);
        sleep_ms(1000);
        painter_set_source_image_mode(&painter, PAINTER_SOURCE_IMAGE_MODE_NORMAL);
        painter_bitblt(&painter, 50, 50, &img_graphics, 50, 50, 150, 150);*/
    }

    return 0;
}
