#include "pico/stdlib.h"
#include "pico/multicore.h"
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
#include "imu_main.h"
#include "view/angles.h"
#include "view/incl.h"
#include "view/alt_ind.h"


// // It's a Trap!
// static err_t dbg_err_int(err_t err)
// {
//     asm("bkpt #0");

//     return err;
// }

// #define DBG_ERR(E) dbg_err_int(E)



#define TFT_MOSI 11
#define TFT_SCK 10
#define TFT_CS 9
#define TFT_RST 12
#define TFT_DC 8
#define TFT_BL 25



/*
 * Экран.
 */

//! Шина экрана.
static spi_bus_t spi_tft;

// TFT.
static gc9a01a_t tft;

//! Размер пиксела - 2 байта (16 бит).
#define TFT_PIXEL_SIZE 2
//! Ширина экрана.
#define TFT_WIDTH 240
#define TFT_HEIGHT 240

#if defined(DRAW_TO_FULL_BUFFER) && DRAW_TO_FULL_BUFFER == 1

    #define TFT_BUFFER_SIZE ((TFT_WIDTH)*(TFT_HEIGHT)*(TFT_PIXEL_SIZE))

    ALIGNED4 uint8_t tft_buffer[TFT_BUFFER_SIZE];

    graphics_t graphics = make_graphics(tft_buffer, TFT_WIDTH, TFT_HEIGHT, GRAPHICS_FORMAT_RGB_565);

#else // fallback - cache.

    //! Число буферов кэша TFT.
    #define TFT_CACHE_BUFS_COUNT 2
    //! Размер первого буфера.
    #define TFT_CACHE_BUF0_PIXELS 240
    #define TFT_CACHE_BUF0_SIZE (TFT_CACHE_BUF0_PIXELS * TFT_PIXEL_SIZE)
    //! Размер второго буфера.
    #define TFT_CACHE_BUF1_PIXELS 240
    #define TFT_CACHE_BUF1_SIZE (TFT_CACHE_BUF1_PIXELS * TFT_PIXEL_SIZE)
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

#endif

static painter_t painter = make_painter(&graphics);

/*
* Картинка для фона.
*/
static graphics_t img_graphics = make_graphics(anime_image_240, ANIME_IMAGE_240_WIDTH, ANIME_IMAGE_240_HEIGHT, GRAPHICS_FORMAT_RGB_565);



/*
* Отображения.
*/

//! Отображение двух углов в виде текста.
static view_angles_t view_angles;

//! Отображение инклинометра.
static view_incl_t view_incl;

//! Отображение авиагоризонта.
static view_alt_ind_t view_alt_ind;

//! Тип функции отрисовки.
typedef void (*view_paint_t)(void*);

//! Структура выбранного отображения.
typedef struct _View_Sel {
    void* view_ptr; //!< Указатель на View.
    view_paint_t view_paint; //!< Функция отрисовки.
} view_sel_t;

//! Текущее view.
view_sel_t selected_view;


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


static void init_spi(void)
{
    spi_inst_t* spi = spi1;

    spi_init(spi, 100*1000*1000);
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

/*     gc9a01a_madctl_t mc;
    mc.col_address_order = GC9A01A_COL_LEFT_TO_RIGHT;
    mc.row_address_order = GC9A01A_ROW_TOP_TO_BOTTOM;
    mc.color_order = GC9A01A_COLOR_ORDER_BGR;
    mc.row_col_exchange = GC9A01A_ROW_COL_NORMAL_MODE;
    mc.horizontal_refresh = GC9A01A_REFRESH_RIGHT_TO_LEFT;
    mc.vertical_refresh = GC9A01A_REFRESH_TOP_TO_BOTTOM;
    gc9a01a_set_madctl(&tft, &mc); */

#if defined(DRAW_TO_FULL_BUFFER) && DRAW_TO_FULL_BUFFER == 1
    memset(tft_buffer, 0x0, TFT_BUFFER_SIZE);
#else
    gc9a01a_cache_fill(&tft_cache, GC9A01A_MAKE_RGB565(0x0, 0, 0x0));
    gc9a01a_cache_flush(&tft_cache);
#endif
}

static err_t init_view_angles(void)
{
    err_t err;

    view_angles_init_t is;
    is.graphics = &graphics;
    is.font_medium = &font_droid_sans_33x37;

    err = view_angles_init(&view_angles, &is);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

static err_t init_view_incl(void)
{
    err_t err;

    view_incl_init_t is;
    is.graphics = &graphics;
    is.font_medium = &font_droid_sans_33x37;

    err = view_incl_init(&view_incl, &is);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

static err_t init_view_alt_ind(void)
{
    err_t err;

    view_alt_ind_init_t is;
    is.graphics = &graphics;
    is.font_medium = &font_droid_sans_33x37;

    err = view_alt_ind_init(&view_alt_ind, &is);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

static err_t init_views(void)
{
    err_t err;

    err = init_view_angles();
    if(err != E_NO_ERROR) return err;
    selected_view.view_ptr = (void*)&view_angles;
    selected_view.view_paint = (view_paint_t)view_angles_paint;

    err = init_view_incl();
    if(err != E_NO_ERROR) return err;
    selected_view.view_ptr = (void*)&view_incl;
    selected_view.view_paint = (view_paint_t)view_incl_paint;

    err = init_view_alt_ind();
    if(err != E_NO_ERROR) return err;
    selected_view.view_ptr = (void*)&view_alt_ind;
    selected_view.view_paint = (view_paint_t)view_alt_ind_paint;

    return E_NO_ERROR;
}

int main(void)
{
    stdio_init_all();

    init_dma_irq_mux();

    err_t err;

#if defined(IMU_ON_CORE) && IMU_ON_CORE == 1
    multicore_reset_core1();
    multicore_launch_core1(imu_main);
#else
    err = imu_process_init();
    if(err != E_NO_ERROR){
        //asm("bkpt #0");
        for(;;){
            printf("imu process init error!\n");
            sleep_ms(100);
        }
    }
#endif

    init_spi();
    init_tft();
    setup_tft();

    err = init_views();
    if(err != E_NO_ERROR){
        //asm("bkpt #0");
        for(;;){
            printf("views init error!\n");
            sleep_ms(100);
        }
    }

    // selected_view.view_ptr = (void*)&view_angles;
    // selected_view.view_paint = (view_paint_t)view_angles_paint;

    for(;;){
#if !defined(IMU_ON_CORE) || IMU_ON_CORE == 0
        imu_process_iter();
#endif

        // const imu_process_state_t* imu_state = imu_process_get_state();

        // float roll  = imu_state->roll  / 3.14159265359f * 180.0f;
        // float pitch = imu_state->pitch / 3.14159265359f * 180.0f;

        // printf("roll: %.2f° pitch: %.2f°\n", roll, pitch);

        if(selected_view.view_paint && selected_view.view_ptr){
            selected_view.view_paint(selected_view.view_ptr);
        }

#if defined(DRAW_TO_FULL_BUFFER) && DRAW_TO_FULL_BUFFER == 1
        gc9a01a_write_region(&tft, 0, 0, TFT_WIDTH-1, TFT_HEIGHT-1, tft_buffer, TFT_BUFFER_SIZE);
        gc9a01a_wait(&tft);
#endif

        /*
        painter_set_font(&painter, &font_droid_sans_33x37);painter_set_pen_color(&painter, GC9A01A_MAKE_RGB565(0, 0, 0xff));
        painter_set_brush_color(&painter, GC9A01A_MAKE_RGB565(0x0, 0xff, 0x0));
        painter_set_source_image_mode(&painter, PAINTER_SOURCE_IMAGE_MODE_BITMAP);
        painter_draw_string(&painter, 100, 0, "Ня!");
        //gc9a01a_set_column_address(&tft, 0, 239);
        //gc9a01a_set_page_address(&tft, 0, 239);
        //gc9a01a_write(&tft, anime_image_240, ANIME_IMAGE_240_HEIGHT * ANIME_IMAGE_240_WIDTH * 2);
        //sleep_ms(1000);
        painter_set_source_image_mode(&painter, PAINTER_SOURCE_IMAGE_MODE_NORMAL);
        painter_bitblt(&painter, 50, 50, &img_graphics, 50, 50, 150, 150);*/
    }

    return 0;
}
