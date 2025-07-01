/**
 * @file gc9a01a_cache.h
 * Библиотека для работы с кэшем экрана TFT на контроллере ILI9341.
 */

#ifndef GC9A01A_CACHE_H
#define GC9A01A_CACHE_H

#include <stdint.h>
#include <stddef.h>
#include "errors/errors.h"
#include "defs/defs.h"
#include "gc9a01a/gc9a01a.h"
#include "graphics/graphics.h"

//! Тип позиции буфера.
typedef enum _Gc9a01a_Cache_buf_pos {
    GC9A01A_CACHE_BUF_POS_UNALIGNED = 0, //!< Не выбрана позиция (один пиксел).
    GC9A01A_CACHE_BUF_POS_HORISONTAL = 1, //!< Горизонтальная позиция.
    GC9A01A_CACHE_BUF_POS_VERTICAL = 2, //!< Вертикальная позиция.
} gc9a01a_cache_buf_pos_t;

/**
 * Структура буфера кэша.
 */
typedef struct _Gc9a01a_Cache_Buffer {
    uint8_t* data; //!< Данные буфера.
    size_t size; //!< Размер данных.
    size_t byte_index; //!< Текущий индекс байта.
    size_t pixels_count; //!< Число пикселов в буфере.
    graphics_pos_t x; //!< Координата X буфера.
    graphics_pos_t y; //!< Координата Y буфера.
    gc9a01a_cache_buf_pos_t pos; //!< Положение буфера.
} gc9a01a_cache_buffer_t;

/**
 * Структура кэша экрана.
 */
typedef struct _Gc9a01a_Cache {
    gc9a01a_t* tft; //!< TFT.
    size_t pixel_size; //!< Размер пиксела.
    gc9a01a_cache_buffer_t* buffers; //!< Буферы кэша.
    size_t buffers_count; //!< Число буферов.
    size_t current_buffer; //!< Текущий буфер.
    gc9a01a_row_col_exchange_t row_col_exchange; //!< Ориентация экрана.
} gc9a01a_cache_t;

#define make_gc9a01a_cache_buffer(arg_data, arg_size)\
    { .data = (uint8_t*)arg_data, .size = arg_size, .byte_index = 0, .pixels_count = 0,\
      .x = 0, .y = 0, .pos = GC9A01A_CACHE_BUF_POS_UNALIGNED }

#define make_gc9a01a_cache(arg_tft, arg_pixel_size, arg_buffers, arg_buffers_count, arg_row_col_exchange)\
    { .tft = arg_tft, .pixel_size = arg_pixel_size, .buffers = arg_buffers,\
      .buffers_count = arg_buffers_count, .current_buffer = 0, .row_col_exchange = arg_row_col_exchange }

/**
 * Инициализирует буфер кэша.
 * @param buffer Буфер.
 * @param data Данные.
 * @param size Размер данных.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_cache_buffer_init(gc9a01a_cache_buffer_t* buffer, void* data, size_t size);

/**
 * Инициализирует кэш.
 * @param cache Кэш.
 * @param tft TFT.
 * @param pixel_size Размер пиксела в байтах.
 * @param buffers Буферы кэша.
 * @param buffers_count Число буферов.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_cache_init(gc9a01a_cache_t* cache, gc9a01a_t* tft, size_t pixel_size,
                                gc9a01a_cache_buffer_t* buffers, size_t buffers_count,
                                gc9a01a_row_col_exchange_t row_col_exchange);

/**
 * Получает ориентацию TFT.
 * @param cache Кэш TFT.
 * @return Ориентация TFT.
 */
ALWAYS_INLINE static gc9a01a_row_col_exchange_t gc9a01a_cache_tft_row_col_exchange(gc9a01a_cache_t* cache)
{
    return cache->row_col_exchange;
}

/**
 * Устанавливает ориентацию TFT.
 * @param cache Кэш TFT.
 * @param row_col_exchange Ориентация TFT.
 */
ALWAYS_INLINE static void gc9a01a_cache_set_tft_row_col_exchange(gc9a01a_cache_t* cache, gc9a01a_row_col_exchange_t row_col_exchange)
{
    cache->row_col_exchange = row_col_exchange;
}


/**
 * Устанавливает пиксел.
 * @param cache Кэш.
 * @param x Координата X.
 * @param y Координата Y.
 * @param color Цвет.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_cache_set_pixel(gc9a01a_cache_t* cache, graphics_pos_t x, graphics_pos_t y, graphics_color_t color);

/**
 * Сбрасывает кэш в экран.
 * @param cache Кэш.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_cache_flush(gc9a01a_cache_t* cache);

/**
 * Заливает TFT заданным цветом.
 * @param cache Кэш.
 * @param color Цвет.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_cache_fill(gc9a01a_cache_t* cache, graphics_color_t color);

/**
 * Заливает регион TFT заданным цветом.
 * @param cache Кэш.
 * @param x0 Координата X верхнего левого угла региона.
 * @param y0 Координата Y верхнего левого угла региона.
 * @param x1 Координата X правого нижнего угла региона.
 * @param y1 Координата Y правого нижнего угла региона.
 * @param color Цвет.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_cache_fill_region(gc9a01a_cache_t* cache, graphics_pos_t x0, graphics_pos_t y0, graphics_pos_t x1, graphics_pos_t y1, graphics_color_t color);

#endif	//GC9A01A_CACHE_H
