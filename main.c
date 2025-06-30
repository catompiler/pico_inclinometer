#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "hardware/irq.h"
#include "dma/dma_irq_mux.h"
#include "spi/spi.h"
#include "gc9a01a/gc9a01a.h"


#define TFT_MOSI 11
#define TFT_SCK 10
#define TFT_CS 9
#define TFT_RST 12
#define TFT_DC 8
#define TFT_BL 25


spi_bus_t spi_tft;
gc9a01a_t tft;


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
    spi_inst_t* spi = spi0;

    spi_init(spi, 100*1000*1000);
    spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    //hw_set_bits(&spi_get_hw(spi)->cr1, SPI_SSPCR1_LBM_BITS); // loopback.

    // mosi
    gpio_set_function(TFT_MOSI, GPIO_FUNC_SPI);
    // miso
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
    gpio_set_dir(TFT_CS, GPIO_OUT);
    gpio_put(TFT_CS, 1);
    // dc
    gpio_init(TFT_DC);
    gpio_set_dir(TFT_DC, GPIO_OUT);
    gpio_put(TFT_DC, 1);
    // reset
    gpio_init(TFT_RST);
    gpio_set_dir(TFT_RST, GPIO_OUT);
    gpio_put(TFT_RST, 1);
    // bl
    gpio_init(TFT_BL);
    gpio_set_dir(TFT_BL, GPIO_OUT);
    gpio_put(TFT_BL, 1);

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

    gc9a01a_madctl_t madctl;
    madctl.row_address_order = GC9A01A_ROW_TOP_TO_BOTTOM;//GC9A01A_ROW_BOTTOM_TO_TOP
    madctl.col_address_order = GC9A01A_COL_LEFT_TO_RIGHT;//GC9A01A_COL_RIGHT_TO_LEFT
    madctl.row_col_exchange = GC9A01A_ROW_COL_REVERSE_MODE;
    madctl.vertical_refresh = GC9A01A_REFRESH_TOP_TO_BOTTOM;
    madctl.color_order = GC9A01A_COLOR_ORDER_BGR;
    madctl.horizontal_refresh = GC9A01A_REFRESH_LEFT_TO_RIGHT;

    gc9a01a_set_madctl(&tft, &madctl);
    gc9a01a_set_pixel_format(&tft, GC9A01A_PIXEL_16BIT, GC9A01A_PIXEL_16BIT);
    gc9a01a_sleep_out(&tft);
    gc9a01a_display_on(&tft);

    //gc9a01a_cache_fill(&tft_cache, GC9A01A_MAKE_RGB565(0, 0, 0));
    // painter_set_brush(&painter, PAINTER_BRUSH_SOLID);
    // painter_set_pen(&painter, PAINTER_PEN_SOLID);
    // painter_fill(&painter);
}


int main(void)
{
    init_dma_irq_mux();

    init_spi();
    init_tft();
    setup_tft();

    static uint16_t pixel = GC9A01A_MAKE_RGB565(00, 0xff, 00);
    gc9a01a_set_pixel(&tft, 119, 119, &pixel, 2);

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, true);
    
    for(;;){
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        sleep_ms(500);
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        sleep_ms(500);
    }

    return 0;
}
