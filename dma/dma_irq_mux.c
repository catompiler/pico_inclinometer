#include "dma_irq_mux.h"



typedef struct _Dma_Handler_Data {
    dma_irq_mux_callback_t callback;
    void* user_data;
} dma_handler_data_t;


static dma_handler_data_t handlers0[DMA_CHANNELS_COUNT] = {
        {NULL, NULL}, // 1
        {NULL, NULL}, // 2
        {NULL, NULL}, // 3
        {NULL, NULL}, // 4
        {NULL, NULL}, // 5
        {NULL, NULL}, // 6
        {NULL, NULL}, // 7
        {NULL, NULL}, // 8
        {NULL, NULL}, // 9
        {NULL, NULL}, // 10
        {NULL, NULL}, // 11
        {NULL, NULL}  // 12
};

static dma_handler_data_t handlers1[DMA_CHANNELS_COUNT] = {
        {NULL, NULL}, // 1
        {NULL, NULL}, // 2
        {NULL, NULL}, // 3
        {NULL, NULL}, // 4
        {NULL, NULL}, // 5
        {NULL, NULL}, // 6
        {NULL, NULL}, // 7
        {NULL, NULL}, // 8
        {NULL, NULL}, // 9
        {NULL, NULL}, // 10
        {NULL, NULL}, // 11
        {NULL, NULL}  // 12
};

ALWAYS_INLINE static dma_handler_data_t* dma_irq_mux_get_handlers_array(uint irq_index)
{
    return (irq_index == 0) ? handlers0 : handlers1;
}

ALWAYS_INLINE static dma_handler_data_t* dma_irq_mux_get_handler(uint irq_index, uint channel)
{
    dma_handler_data_t* handlers = dma_irq_mux_get_handlers_array(irq_index);

    return &handlers[channel];
}

void dma_irq_mux_set_callback(uint irq_index, uint channel, dma_irq_mux_callback_t callback, void* user_data)
{
    if(irq_index >= DMA_IRQS_COUNT) return;
    if(channel >= DMA_CHANNELS_COUNT) return;

    dma_handler_data_t* handler = dma_irq_mux_get_handler(irq_index, channel);
    handler->callback = callback;
    handler->user_data = user_data;
}



void dma_irq_mux_irq_0_handler(void)
{
    uint32_t irq_stat = dma_hw->ints0;
    dma_handler_data_t* handlers = handlers0;

    uint32_t ch_mask = 1;
    size_t ch_n = 0;
    for(; ch_n < DMA_CHANNELS_COUNT; ch_n ++, ch_mask <<= 1){
        if(irq_stat & ch_mask){
            // Handler data.
            dma_handler_data_t* handler = &handlers[ch_n];
            // Callback.
            dma_irq_mux_callback_t callback = handler->callback;
            // If callback is not NULL - call handler with user data.
            if(callback){
                callback(handler->user_data);
            } // Else - clear all interrupt flags.
            //else{
                dma_hw->ints0 = ch_mask;
            //}
        }
    }
}


void dma_irq_mux_irq_1_handler(void)
{
    uint32_t irq_stat = dma_hw->ints1;
    dma_handler_data_t* handlers = handlers1;

    uint32_t ch_mask = 1;
    size_t ch_n = 0;
    for(; ch_n < DMA_CHANNELS_COUNT; ch_n ++, ch_mask <<= 1){
        if(irq_stat & ch_mask){
            // Handler data.
            dma_handler_data_t* handler = &handlers[ch_n];
            // Callback.
            dma_irq_mux_callback_t callback = handler->callback;
            // If callback is not NULL - call handler with user data.
            if(callback){
                callback(handler->user_data);
            } // Else - clear all interrupt flags.
            //else{
                dma_hw->ints1 = ch_mask;
            //}
        }
    }
}


