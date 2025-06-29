#ifndef DMA_IRQ_MUX_H
#define DMA_IRQ_MUX_H

#include "hardware/dma.h"
#include "defs/defs.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>



//! Число векторов прерываний DMA.
#define DMA_IRQS_COUNT (2)

//! Число каналов DMA.
#define DMA_CHANNELS_COUNT (12)


//! Тип коллбэка DMA.
typedef void (*dma_irq_mux_callback_t)(void* user_data);


/*
 *
 * Handlers data & callbacks.
 *
 */

EXTERN void dma_irq_mux_set_callback(uint irq_index, uint channel, dma_irq_mux_callback_t callback, void* user_data);

/**
 * Обработчик вектора прерываний 0 DMA.
 * Должен быть установлен как эксклюзивный обработчик.
 */
EXTERN void dma_irq_mux_irq_0_handler(void);

/**
 * Обработчик вектора прерываний 1 DMA.
 * Должен быть установлен как эксклюзивный обработчик.
 */
EXTERN void dma_irq_mux_irq_1_handler(void);


#endif /* DMA_IRQ_MUX_H */
