#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "hardware/irq.h"
#include "spi/spi.h"


spi_bus_t spi_test;


static void spi_irq_handler(void)
{
    spi_bus_irq_handler(&spi_test);
}

static void spi_dma_rx_irq_handler(void)
{
    spi_bus_dma_rx_channel_irq_handler(&spi_test);
}

static void spi_dma_tx_irq_handler(void)
{
    spi_bus_dma_tx_channel_irq_handler(&spi_test);
}




static void init_spi(void)
{
    spi_inst_t* spi = spi0;

    spi_init(spi, 10000000);
    spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    hw_set_bits(&spi_get_hw(spi)->cr1, SPI_SSPCR1_LBM_BITS); // loopback.

    irq_set_exclusive_handler(SPI0_IRQ, spi_irq_handler);
    irq_set_priority(SPI0_IRQ, 0);
    irq_set_enabled(SPI0_IRQ, true);
    irq_set_exclusive_handler(DMA_IRQ_0, spi_dma_rx_irq_handler);
    irq_set_priority(DMA_IRQ_0, 0);
    irq_set_enabled(DMA_IRQ_0, true);
    irq_set_exclusive_handler(DMA_IRQ_1, spi_dma_tx_irq_handler);
    irq_set_priority(DMA_IRQ_1, 0);
    irq_set_enabled(DMA_IRQ_1, true);

    spi_bus_init_t is;
    is.spi_device = spi_get_hw(spi);
    is.dma_rx_channel = 0;
    is.dma_rx_dreq = spi_get_dreq(spi, false);
    is.dma_rx_irq_index = 0;
    is.dma_tx_channel = 1;
    is.dma_tx_dreq = spi_get_dreq(spi, true);
    is.dma_tx_irq_index = 1;
    spi_bus_init(&spi_test, &is);
}


#define TEST_DATA_SIZE 8

static uint8_t tx_data[TEST_DATA_SIZE] = {
    0, 1, 2, 3, 4, 5, 6, 7
};

static uint8_t rx_data[TEST_DATA_SIZE] = {
    0
};

static spi_message_t msg;

static void test_spi(void)
{
    spi_message_setup(&msg, SPI_READ_WRITE, tx_data, rx_data, TEST_DATA_SIZE);

    spi_bus_transfer(&spi_test, &msg, 1);
}


int main(void)
{
    init_spi();
    test_spi();

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
