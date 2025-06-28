#include "spi.h"
#include <stddef.h>
#include "utils/utils.h"
#include "defs/defs.h"


//#define SPI_BUS_DEBUG
#ifdef SPI_BUS_DEBUG
#include <stdio.h>
#endif

// Значение по-умолчанию для мусорных данных.
#define SPI_DUMMY_DATA_DEF_VAL 0xffff

static uint16_t spi_rx_default_data = 0;


#define SPI_STATE_IDLE             0
#define SPI_STATE_READING          1
#define SPI_STATE_WRITING          2
#define SPI_STATE_READING_WRITING  3



ALWAYS_INLINE static bool spi_bus_is_duplex(spi_bus_t* spi)
{
    return true;
}

ALWAYS_INLINE static bool spi_bus_can_rx(spi_bus_t* spi)
{
    return true;
}

ALWAYS_INLINE static bool spi_bus_can_tx(spi_bus_t* spi)
{
    return true;
}

ALWAYS_INLINE static bool spi_bus_is_crc_enabled(spi_bus_t* spi)
{
    (void) spi;

    return false;
}


ALWAYS_INLINE static bool spi_bus_is_frame_16bit(spi_bus_t* spi)
{
    return ((spi->spi_device->cr0 & SPI_SSPCR0_DSS_BITS) >> SPI_SSPCR0_DSS_LSB) == 15;
}

ALWAYS_INLINE static bool spi_bus_is_frame_lsbfirst(spi_bus_t* spi)
{
    return false;
}

ALWAYS_INLINE static void spi_bus_clear_tx_events(spi_bus_t* spi)
{
}

ALWAYS_INLINE static void spi_bus_clear_rx_events(spi_bus_t* spi)
{
    spi->spi_device->icr = SPI_SSPICR_RTIC_BITS | SPI_SSPICR_RTIC_BITS;
}

ALWAYS_INLINE static void spi_bus_clear_rxtx_events(spi_bus_t* spi)
{
    spi_bus_clear_tx_events(spi);
    spi_bus_clear_rx_events(spi);
}


ALWAYS_INLINE static void spi_bus_wait_can_tx(spi_bus_t* spi)
{
    WAIT_WHILE_TRUE((spi->spi_device->sr & SPI_SSPSR_TNF_BITS) != 0);
}

ALWAYS_INLINE static void spi_bus_wait_can_rx(spi_bus_t* spi)
{
    WAIT_WHILE_TRUE((spi->spi_device->sr & SPI_SSPSR_RNE_BITS) == 0);
}


MAYBE_UNUSED
ALWAYS_INLINE static bool spi_bus_tx_it_enabled(spi_bus_t* spi)
{
    return (spi->spi_device->imsc & SPI_SSPIMSC_TXIM_BITS) != 0;
}

ALWAYS_INLINE static void spi_bus_tx_it_enable(spi_bus_t* spi)
{
    spi->spi_device->imsc |= SPI_SSPIMSC_TXIM_BITS;
}

ALWAYS_INLINE static void spi_bus_tx_it_disable(spi_bus_t* spi)
{
    spi->spi_device->imsc &= ~SPI_SSPIMSC_TXIM_BITS;
}

MAYBE_UNUSED
ALWAYS_INLINE static void spi_bus_tx_it_set_enabled(spi_bus_t* spi, bool enabled)
{
    if(enabled) spi_bus_tx_it_enable(spi);
    else spi_bus_tx_it_disable(spi);
}


MAYBE_UNUSED
ALWAYS_INLINE static bool spi_bus_rx_it_enabled(spi_bus_t* spi)
{
    return (spi->spi_device->imsc & SPI_SSPIMSC_RXIM_BITS) != 0;
}

ALWAYS_INLINE static void spi_bus_rx_it_enable(spi_bus_t* spi)
{
    spi->spi_device->imsc |= SPI_SSPIMSC_RXIM_BITS;
}

ALWAYS_INLINE static void spi_bus_rx_it_disable(spi_bus_t* spi)
{
    spi->spi_device->imsc &= ~SPI_SSPIMSC_RXIM_BITS;
}

MAYBE_UNUSED
ALWAYS_INLINE static void spi_bus_rx_it_set_enabled(spi_bus_t* spi, bool enabled)
{
    if(enabled) spi_bus_rx_it_enable(spi);
    else spi_bus_rx_it_disable(spi);
}


MAYBE_UNUSED
ALWAYS_INLINE static bool spi_bus_er_it_enabled(spi_bus_t* spi)
{
    return ((spi->spi_device->imsc & SPI_SSPIMSC_RTIM_BITS) != 0) || ((spi->spi_device->imsc & SPI_SSPIMSC_RORIM_BITS) != 0);
}

ALWAYS_INLINE static void spi_bus_er_it_enable(spi_bus_t* spi)
{
    spi->spi_device->imsc |= SPI_SSPIMSC_RTIM_BITS;
    spi->spi_device->imsc |= SPI_SSPIMSC_RORIM_BITS;
}

ALWAYS_INLINE static void spi_bus_er_it_disable(spi_bus_t* spi)
{
    spi->spi_device->imsc &= ~SPI_SSPIMSC_RTIM_BITS;
    spi->spi_device->imsc &= ~SPI_SSPIMSC_RORIM_BITS;
}

MAYBE_UNUSED
ALWAYS_INLINE static void spi_bus_er_it_set_enabled(spi_bus_t* spi, bool enabled)
{
    if(enabled) spi_bus_er_it_enable(spi);
    else spi_bus_er_it_disable(spi);
}


ALWAYS_INLINE static const void* spi_bus_rx_reg_ptr(spi_bus_t* spi)
{
    return (const void*)&spi->spi_device->dr;
}

ALWAYS_INLINE static void* spi_bus_tx_reg_ptr(spi_bus_t* spi)
{
    return (void*)&spi->spi_device->dr;
}

static void spi_bus_write_transmit_data(spi_bus_t* spi, uint16_t data)
{
    spi->spi_device->dr = data;
}

static uint16_t spi_bus_read_received_data(spi_bus_t* spi)
{
    return spi->spi_device->dr;
}


ALWAYS_INLINE static void spi_bus_trigger_tx_req(spi_bus_t* spi)
{
}

MAYBE_UNUSED
ALWAYS_INLINE static void spi_bus_trigger_rx_req(spi_bus_t* spi)
{
}


ALWAYS_INLINE static void spi_bus_frame_begin(spi_bus_t* spi)
{
}

ALWAYS_INLINE static void spi_bus_frame_end(spi_bus_t* spi)
{
}


MAYBE_UNUSED
ALWAYS_INLINE static bool spi_bus_tx_dma_enabled(spi_bus_t* spi)
{
    return (spi->spi_device->dmacr & SPI_SSPDMACR_TXDMAE_BITS) != 0;
}

ALWAYS_INLINE static void spi_bus_tx_dma_enable(spi_bus_t* spi)
{
    spi->spi_device->dmacr |= SPI_SSPDMACR_TXDMAE_BITS;
}

ALWAYS_INLINE static void spi_bus_tx_dma_disable(spi_bus_t* spi)
{
    spi->spi_device->dmacr &= ~SPI_SSPDMACR_TXDMAE_BITS;
}

MAYBE_UNUSED
ALWAYS_INLINE static void spi_bus_tx_dma_set_enabled(spi_bus_t* spi, bool enabled)
{
    if(enabled) spi_bus_tx_dma_enable(spi);
    else spi_bus_tx_dma_disable(spi);
}


MAYBE_UNUSED
ALWAYS_INLINE static bool spi_bus_rx_dma_enabled(spi_bus_t* spi)
{
    return (spi->spi_device->dmacr & SPI_SSPDMACR_RXDMAE_BITS) != 0;
}

ALWAYS_INLINE static void spi_bus_rx_dma_enable(spi_bus_t* spi)
{
    spi->spi_device->dmacr |= SPI_SSPDMACR_RXDMAE_BITS;
}

ALWAYS_INLINE static void spi_bus_rx_dma_disable(spi_bus_t* spi)
{
    spi->spi_device->dmacr &= ~SPI_SSPDMACR_RXDMAE_BITS;
}

MAYBE_UNUSED
ALWAYS_INLINE static void spi_bus_rx_dma_set_enabled(spi_bus_t* spi, bool enabled)
{
    if(enabled) spi_bus_rx_dma_enable(spi);
    else spi_bus_rx_dma_disable(spi);
}

ALWAYS_INLINE static bool spi_bus_dma_channel_has_error(uint channel)
{
    return (dma_channel_hw_addr(channel)->ctrl_trig & DMA_CH0_CTRL_TRIG_AHB_ERROR_BITS) != 0;
}


err_t spi_bus_init(spi_bus_t* spi, spi_bus_init_t* init)
{
    if(init == NULL) return E_NULL_POINTER;
    
    // DMA.
    spi->dma_rx_channel = init->dma_rx_channel;
    spi->dma_rx_dreq = init->dma_rx_dreq;
    spi->dma_rx_irq_index = init->dma_rx_irq_index;
    spi->dma_tx_channel = init->dma_tx_channel;
    spi->dma_tx_dreq = init->dma_tx_dreq;
    spi->dma_tx_irq_index = init->dma_tx_irq_index;
    // SPI.
    spi->spi_device = init->spi_device;
    spi->messages = NULL;
    spi->messages_count = 0;
    spi->message_index = 0;
    spi->status = SPI_STATUS_IDLE;
    spi->errors = SPI_NO_ERROR;
    spi->transfer_id = SPI_BUS_DEFAULT_TRANSFER_ID;
    spi->callback = NULL;
    spi->user_data = NULL;
    
    spi->state = SPI_STATE_IDLE;
    spi->dma_rx_locked = false;
    spi->dma_tx_locked = false;
    
    spi->tx_default = SPI_DUMMY_DATA_DEF_VAL;

    spi_bus_er_it_enable(spi);
    
    return E_NO_ERROR;
}

static void spi_bus_dma_rxtx_config(spi_bus_t* spi, void* rx_address, const void* tx_address, size_t size)
{
    enum dma_channel_transfer_size tf_w = spi_bus_is_frame_16bit(spi) ? DMA_SIZE_16 : DMA_SIZE_8;
    bool rx_addr_inc = (rx_address != NULL) ? true : false;
    bool tx_addr_inc = (tx_address != NULL) ? true : false;

    dma_channel_config c;

    // RX.
    if(spi->dma_rx_locked){
        
        if(rx_address == NULL) rx_address = &spi_rx_default_data;

        c = dma_channel_get_default_config(spi->dma_rx_channel);
        channel_config_set_dreq(&c, spi->dma_rx_dreq);
        channel_config_set_transfer_data_size(&c, tf_w);
        channel_config_set_write_increment(&c, rx_addr_inc);
        channel_config_set_read_increment(&c, false);
        channel_config_set_enable(&c, true);

        dma_channel_configure(spi->dma_rx_channel, &c, rx_address, spi_bus_rx_reg_ptr(spi), size, false);
        dma_irqn_set_channel_enabled(spi->dma_rx_irq_index, spi->dma_rx_channel, true);
    }
    
    // TX.
    if(spi->dma_tx_locked){
        
        if(tx_address == NULL) tx_address = &spi->tx_default;

        c = dma_channel_get_default_config(spi->dma_tx_channel);
        channel_config_set_dreq(&c, spi->dma_tx_dreq);
        channel_config_set_transfer_data_size(&c, tf_w);
        channel_config_set_write_increment(&c, false);
        channel_config_set_read_increment(&c, tx_addr_inc);
        channel_config_set_enable(&c, true);

        dma_channel_configure(spi->dma_tx_channel, &c, spi_bus_tx_reg_ptr(spi), tx_address, size, false);
        dma_irqn_set_channel_enabled(spi->dma_tx_irq_index, spi->dma_tx_channel, true);
    }
}

ALWAYS_INLINE static void spi_bus_dma_start(spi_bus_t* spi)
{
    spi_bus_frame_begin(spi);

    if(spi->dma_rx_locked){
        spi_bus_clear_rx_events(spi);
        spi_bus_rx_dma_enable(spi);
        //spi_bus_rx_it_enable(spi);
        dma_channel_start(spi->dma_rx_channel);
    }
    if(spi->dma_tx_locked){
        spi_bus_clear_tx_events(spi);
        spi_bus_tx_dma_enable(spi);
        //spi_bus_tx_it_enable(spi);
        dma_channel_start(spi->dma_tx_channel);
    }
}


ALWAYS_INLINE static void spi_bus_dma_stop_rx(spi_bus_t* spi)
{
    if(spi->dma_rx_locked){
        spi_bus_rx_it_disable(spi);
        spi_bus_rx_dma_disable(spi);

        dma_irqn_set_channel_enabled(spi->dma_rx_irq_index, spi->dma_rx_channel, false);

        if(dma_channel_hw_addr(spi->dma_rx_channel)->ctrl_trig & DMA_CH0_CTRL_TRIG_BUSY_BITS){
            dma_channel_abort(spi->dma_rx_channel);
            dma_irqn_acknowledge_channel(spi->dma_rx_irq_index, spi->dma_rx_channel);
        }

        //dma_channel_cleanup(spi->dma_rx_channel);
    }
}

ALWAYS_INLINE static void spi_bus_dma_stop_tx(spi_bus_t* spi)
{
    if(spi->dma_tx_locked){
        spi_bus_tx_it_disable(spi);
        spi_bus_tx_dma_disable(spi);

        dma_irqn_set_channel_enabled(spi->dma_tx_irq_index, spi->dma_tx_channel, false);
        
        if(dma_channel_hw_addr(spi->dma_tx_channel)->ctrl_trig & DMA_CH0_CTRL_TRIG_BUSY_BITS){
            dma_channel_abort(spi->dma_tx_channel);
            dma_irqn_acknowledge_channel(spi->dma_tx_irq_index, spi->dma_tx_channel);
        }

        //dma_channel_cleanup(spi->dma_tx_channel);
    }
}

ALWAYS_INLINE static void spi_bus_dma_stop(spi_bus_t* spi)
{
    spi_bus_dma_stop_rx(spi);
    spi_bus_dma_stop_tx(spi);
}

static bool spi_bus_dma_channel_trylock(uint channel)
{
    if(dma_channel_is_claimed(channel)) return false;
    dma_channel_claim(channel);
    return true;
}

static void spi_bus_dma_channel_unlock(uint channel)
{
    dma_channel_unclaim(channel);
}

static bool spi_bus_dma_lock_channels(spi_bus_t* spi, bool lock_rx, bool lock_tx)
{
    if(lock_rx){
        spi->dma_rx_locked = spi_bus_dma_channel_trylock(spi->dma_rx_channel);
        if(!spi->dma_rx_locked) return false;
    }
    if(lock_tx){
        spi->dma_tx_locked = spi_bus_dma_channel_trylock(spi->dma_tx_channel);
        if(!spi->dma_tx_locked){
            if(spi->dma_rx_locked){
                spi_bus_dma_channel_unlock(spi->dma_rx_channel);
                spi->dma_rx_locked = false;
            }
            return false;
        }
    }

    return true;
}

static void spi_bus_dma_unlock_channels(spi_bus_t* spi)
{
    if(spi->dma_rx_locked){
        dma_channel_cleanup(spi->dma_rx_channel);
        spi_bus_dma_channel_unlock(spi->dma_rx_channel);
        spi->dma_rx_locked = false;
    }
    if(spi->dma_tx_locked){
        dma_channel_cleanup(spi->dma_tx_channel);
        spi_bus_dma_channel_unlock(spi->dma_tx_channel);
        spi->dma_tx_locked = false;
    }
}

static void spi_bus_setup_message(spi_bus_t* spi)
{
    spi_message_t* msg = &spi->messages[spi->message_index];
    switch(msg->direction){
        case SPI_WRITE:
            spi->state = SPI_STATE_WRITING;
            spi_bus_dma_rxtx_config(spi, NULL, msg->tx_data, msg->data_size);
            break;
        case SPI_READ:
            spi->state = SPI_STATE_READING;
            spi_bus_dma_rxtx_config(spi, msg->rx_data, NULL, msg->data_size);
            break;
        case SPI_READ_WRITE:
            spi->state = SPI_STATE_READING_WRITING;
            spi_bus_dma_rxtx_config(spi, msg->rx_data, msg->tx_data, msg->data_size);
            break;
    }
}

static ALWAYS_INLINE void spi_bus_on_message_sent(spi_bus_t* spi)
{
    if(spi->messages[spi->message_index].callback != NULL){
        spi->messages[spi->message_index].callback(&spi->messages[spi->message_index]);
    }
}

static bool spi_bus_setup_next_message(spi_bus_t* spi)
{
    spi_bus_on_message_sent(spi);
    if(++ spi->message_index >= spi->messages_count) return false;
    spi_bus_setup_message(spi);
    return true;
}

static ALWAYS_INLINE bool spi_bus_done(spi_bus_t* spi)
{
    spi_bus_frame_end(spi);

    if(spi->callback) spi->callback(spi->user_data);
    return spi->state == SPI_STATE_IDLE;
}

static void spi_bus_transfer_done(spi_bus_t* spi)
{
    spi_bus_dma_stop(spi);

    if(spi_bus_setup_next_message(spi)){
        spi_bus_dma_start(spi);
    }else{
        spi_bus_dma_unlock_channels(spi);

        spi->state = SPI_STATE_IDLE;
        spi->status = SPI_STATUS_TRANSFERED;

        spi_bus_done(spi);
    }
}

static void spi_bus_transfer_error(spi_bus_t* spi)
{
    spi_bus_dma_stop(spi);
    spi_bus_dma_unlock_channels(spi);

    spi->state = SPI_STATE_IDLE;
    spi->status = SPI_STATUS_ERROR;
    
    spi_bus_done(spi);
}

void spi_bus_irq_handler(spi_bus_t* spi)
{
#ifdef SPI_BUS_DEBUG
    printf("[SPI] ERR\r\n");
#endif
    
    uint16_t SR = spi->spi_device->mis;
    
    if(SR & SPI_SSPMIS_RORMIS_BITS){
        // Clear flag.
        spi->spi_device->icr = SPI_SSPICR_RORIC_BITS;
        
        spi->errors |= SPI_ERROR_OVERRUN;

    }
    if(SR & SPI_SSPMIS_RTMIS_BITS){
        spi->spi_device->icr = SPI_SSPICR_RTIC_BITS;
        
        spi->errors |= SPI_ERROR_TIMEOUT;
    }

    if(SR & SPI_SSPMIS_RXMIS_BITS){
        // Clear flag.
        spi_bus_clear_rx_events(spi);
        // Disable interrupt.
        spi_bus_rx_it_disable(spi);
        
    }else if(SR & SPI_SSPMIS_TXMIS_BITS){
        // Clear flag.
        spi_bus_clear_tx_events(spi);
        // Disable interrupt.
        spi_bus_tx_it_disable(spi);
    }
    
    if(spi->errors == SPI_NO_ERROR){
        spi_bus_transfer_done(spi);
    }else{
        spi_bus_transfer_error(spi);
    }
}

bool spi_bus_dma_rx_channel_irq_handler(spi_bus_t* spi)
{
    //bool duplex = spi_bus_is_duplex(spi);
    bool can_rx = spi_bus_can_rx(spi);
    //bool can_tx = spi_bus_can_tx(spi);
    
#ifdef SPI_BUS_DEBUG
    printf("[SPI] DMA RX\r\n");
#endif
    
    // Если мы не можем принимать - возврат.
    if(!can_rx || !spi->dma_rx_locked) return false;

    if(!spi_bus_dma_channel_has_error(spi->dma_rx_channel)){

        dma_irqn_acknowledge_channel(spi->dma_rx_irq_index, spi->dma_rx_channel);
        
//        if(!spi_bus_is_crc_enabled(spi)){
            spi_bus_transfer_done(spi);
//        }else{
//            spi_bus_rx_it_enable(spi);
//        }

    }else /*if(spi_bus_dma_channel_has_error(spi->dma_rx_channel))*/{

        dma_irqn_acknowledge_channel(spi->dma_rx_irq_index, spi->dma_rx_channel);
        
        spi->errors |= SPI_ERROR_DMA;
        spi_bus_transfer_error(spi);
    }

    return true;
}

bool spi_bus_dma_tx_channel_irq_handler(spi_bus_t* spi)
{
    //bool duplex = spi_bus_is_duplex(spi);
    bool can_rx = spi_bus_can_rx(spi);
    bool can_tx = spi_bus_can_tx(spi);
    
#ifdef SPI_BUS_DEBUG
    printf("[SPI] DMA TX\r\n");
#endif
    
    // Если мы не можем передавать - возврат.
    if(!can_tx || !spi->dma_tx_locked) return false;

    if(!spi_bus_dma_channel_has_error(spi->dma_rx_channel)){

        dma_irqn_acknowledge_channel(spi->dma_tx_irq_index, spi->dma_tx_channel);

        //WAIT_WHILE_TRUE((spi->spi_device->TCSR & USIC_CH_TCSR_TDV_Msk) != 0);

        if(!can_rx){
//            if(!spi_bus_is_crc_enabled(spi)){
                spi_bus_transfer_done(spi);
//            }else{
//                spi_bus_tx_it_enable(spi);
//            }
        }

    }else /*if(spi_bus_dma_channel_has_error(spi->dma_rx_channel))*/{

        dma_irqn_acknowledge_channel(spi->dma_tx_irq_index, spi->dma_tx_channel);
        
        spi->errors |= SPI_ERROR_DMA;
        spi_bus_transfer_error(spi);
    }
    return true;
}

#if defined(SPI_BUS_HW_SEL) && SPI_BUS_HW_SEL == 1
bool spi_bus_set_hw_sel(spi_bus_t* spi, uint32_t sel)
{
    // RP2040 does not support this.
    // spi->spi_device->...;

    return false;
}
#endif

bool spi_bus_busy(spi_bus_t* spi)
{
    return (spi->spi_device->sr & SPI_SSPSR_BSY_BITS) != 0;// && spi->state != SPI_STATE_IDLE;
}

void spi_bus_wait(spi_bus_t* spi)
{
    WAIT_WHILE_TRUE(spi_bus_busy(spi));
}

bool spi_bus_enabled(spi_bus_t* spi)
{
    return (spi->spi_device->cr1 & SPI_SSPCR1_SSE_BITS) != 0;
}

bool spi_bus_set_enabled(spi_bus_t* spi, bool enabled)
{
    if(spi_bus_busy(spi)) return false;
    
    if(enabled){
        spi->spi_device->cr1 = (spi->spi_device->cr1 & ~SPI_SSPCR1_SSE_BITS) | (0b01 << SPI_SSPCR1_SSE_LSB);
    }else{
        spi->spi_device->cr1 = (spi->spi_device->cr1 & ~SPI_SSPCR1_SSE_BITS);
    }
    
    return true;
}

spi_callback_t spi_bus_callback(spi_bus_t* spi)
{
    return spi->callback;
}

void spi_bus_set_callback(spi_bus_t* spi, spi_callback_t callback)
{
    spi->callback = callback;
}

void* spi_bus_user_data(spi_bus_t* spi)
{
    return spi->user_data;
}

void spi_bus_set_user_data(spi_bus_t* spi, void* user_data)
{
    spi->user_data = user_data;
}

spi_transfer_id_t spi_bus_transfer_id(spi_bus_t* spi)
{
    return spi->transfer_id;
}

bool spi_bus_set_transfer_id(spi_bus_t* spi, spi_transfer_id_t id)
{
    if(spi_bus_busy(spi)) return false;
    
    spi->transfer_id = id;
    
    return true;
}

void spi_bus_set_tx_default_value(spi_bus_t* spi, uint16_t value)
{
    spi->tx_default = value;
}

bool spi_bus_crc_enabled(spi_bus_t* spi)
{
    return spi_bus_is_crc_enabled(spi);
}

bool spi_bus_set_crc_enabled(spi_bus_t* spi, bool enabled)
{
    (void) spi;
    (void) enabled;

    return false;
}

uint16_t spi_bus_crc_polynomial(spi_bus_t* spi)
{
    (void) spi;

    return 0;
}

bool spi_bus_set_crc_polynomial(spi_bus_t* spi, uint16_t polynomial)
{
    (void) spi;
    (void) polynomial;

    return false;
}

uint16_t spi_bus_tx_crc(spi_bus_t* spi)
{
    (void) spi;

    return 0;
}

uint16_t spi_bus_rx_crc(spi_bus_t* spi)
{
    (void) spi;

    return 0;
}

bool spi_bus_reset_crc(spi_bus_t* spi)
{
    (void) spi;
    
    return false;
}

spi_data_frame_format_t spi_bus_data_frame_format(spi_bus_t* spi)
{
    if(spi_bus_is_frame_16bit(spi)){
        return SPI_DATA_FRAME_FORMAT_16BIT;
    }
    return SPI_DATA_FRAME_FORMAT_8BIT;
}

bool spi_bus_set_data_frame_format(spi_bus_t* spi, spi_data_frame_format_t format)
{
    if(spi_bus_busy(spi)) return false;

    if(format == SPI_DATA_FRAME_FORMAT_8BIT){
        spi->spi_device->cr0 = (spi->spi_device->cr0 & ~SPI_SSPCR0_DSS_BITS) | (7 << SPI_SSPCR0_DSS_LSB);
    }else{
        spi->spi_device->cr0 = (spi->spi_device->cr0 & ~SPI_SSPCR0_DSS_BITS) | (15 << SPI_SSPCR0_DSS_LSB);
    }

    return true;
}

spi_frame_format_t spi_bus_frame_format(spi_bus_t* spi)
{
    if(spi_bus_is_frame_lsbfirst(spi)){
        return SPI_FRAME_FORMAT_LSBFIRST;
    }
    return SPI_FRAME_FORMAT_MSBFIRST;
}

bool spi_bus_set_frame_format(spi_bus_t* spi, spi_frame_format_t format)
{
    if(spi_bus_busy(spi)) return false;

    if(format == SPI_FRAME_FORMAT_MSBFIRST){
        //spi->spi_device->...;
    }else{
        // RP2040 does not support this.
        //spi->spi_device->...;
        return false;
    }

    return true;
}

spi_status_t spi_bus_status(spi_bus_t* spi)
{
    return spi->status;
}

spi_errors_t spi_bus_errors(spi_bus_t* spi)
{
    return spi->errors;
}

err_t spi_message_init(spi_message_t* message, spi_direction_t direction, const void* tx_data, void* rx_data, size_t data_size)
{
    if(data_size == 0) return E_INVALID_VALUE;
    
    switch(direction){
        case SPI_READ:
            if(rx_data == NULL) return E_NULL_POINTER;
            if(tx_data != NULL) return E_INVALID_VALUE;
            break;
        case SPI_WRITE:
            if(tx_data == NULL) return E_NULL_POINTER;
            if(rx_data != NULL) return E_INVALID_VALUE;
            break;
        case SPI_READ_WRITE:
            if(tx_data == NULL || rx_data == NULL) return E_NULL_POINTER;
            break;
    }
    
    message->direction = direction;
    message->tx_data = tx_data;
    message->rx_data = rx_data;
    message->data_size = data_size;
    message->callback = NULL;
    message->sender_data = NULL;
    
    return E_NO_ERROR;
}

err_t spi_bus_transfer(spi_bus_t* spi, spi_message_t* messages, size_t messages_count)
{
    if(spi_bus_busy(spi)) return E_BUSY;
    if(messages == NULL) return E_NULL_POINTER;
    if(messages_count == 0) return E_INVALID_VALUE;
    
    bool need_rx_channel = false;
    bool need_tx_channel = false;
    
    size_t i = 0;
    
    bool duplex = spi_bus_is_duplex(spi);
    
    spi_message_t* msg = NULL;
    // Если режим передачи по обоим линиям.
    if(duplex){
        need_rx_channel = true;
        need_tx_channel = true;
    // Иначе.
    }else{
        // Возможность передачи и приёма.
        bool can_rx = spi_bus_can_rx(spi);
        bool can_tx = spi_bus_can_tx(spi);
        // Проверим все сообщения.
        for(; i < messages_count; i ++){
            msg = &messages[i];
            switch(msg->direction){
                case SPI_READ:
                    // Принимать данные можно только при BIDIOE == 0.
                    if(!can_rx) return E_SPI_INVALID_MESSAGE;
                    need_rx_channel = true;
                    break;
                case SPI_WRITE:
                    // Передавать данные можно только при BIDIOE == 1.
                    if(!can_tx) return E_SPI_INVALID_MESSAGE;
                    need_tx_channel = true;
                    break;
                case SPI_READ_WRITE:
                    // Передавать и принимать данные одновременно нельзя
                    // в режиме однопроводной шины.
                    return E_SPI_INVALID_MESSAGE;
            }
            if(need_rx_channel && need_tx_channel) break;
        }
    }
    
    if(!spi_bus_dma_lock_channels(spi, need_rx_channel, need_tx_channel)) return E_BUSY;
    
    spi->messages = messages;
    spi->messages_count = messages_count;
    spi->message_index = 0;
    spi->errors = E_NO_ERROR;
    spi->status = SPI_STATUS_TRANSFERING;
    
    spi_bus_setup_message(spi);
    
    spi_bus_dma_start(spi);
    
    return E_NO_ERROR;
}

err_t spi_bus_transmit(spi_bus_t* spi, uint16_t tx_data, uint16_t* rx_data)
{
    if(spi_bus_busy(spi)) return E_BUSY;

    spi_bus_clear_rxtx_events(spi);

    spi_bus_write_transmit_data(spi, tx_data);

    // Подождать отправки.
    spi_bus_wait_can_tx(spi);
    // Подождать получения.
    spi_bus_wait_can_rx(spi);

    // Очистим RXNE flag.
    uint16_t data = spi_bus_read_received_data(spi);
    if(rx_data) *rx_data = data;

    return E_NO_ERROR;
}
