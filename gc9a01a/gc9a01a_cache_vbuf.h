/**
 * @file gc9a01a_cache_vbuf.h
 * Библиотека функций виртуального буфера для кэша экрана TFT на контроллере GC9A01A.
 */

#ifndef GC9A01A_CACHE_VBUF_H
#define	GC9A01A_CACHE_VBUF_H

#include "graphics/graphics.h"
#include "gc9a01a/gc9a01a_cache.h"

#ifdef USE_GRAPHICS_VIRTUAL_BUFFER

/**
 * Устанавливает цвет пиксела.
 * @param graphics Изображение.
 * @param x Координата X.
 * @param y Координата Y.
 * @param color Цвет пиксела.
 * @return true в случае успеха, иначе false.
 */
static bool gc9a01a_cache_vbuf_set_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    gc9a01a_cache_t* tft_cache = (gc9a01a_cache_t*)graphics_data(graphics);
    if(tft_cache == NULL) return false;
    
    return gc9a01a_cache_set_pixel(tft_cache, x, y, color) == E_NO_ERROR;
}

/**
 * Сбрасывает буфер в устройство.
 * @param graphics Изображение.
 * @return true в случае успеха, иначе false.
 */
static bool gc9a01a_cache_vbuf_flush(graphics_t* graphics)
{
    gc9a01a_cache_t* tft_cache = (gc9a01a_cache_t*)graphics_data(graphics);
    if(tft_cache == NULL) return false;
    
    return gc9a01a_cache_flush(tft_cache) == E_NO_ERROR;
}

/**
 * Быстро выводит залитый прямоугольник.
 * @param graphics Изображение.
 * @param left Лево.
 * @param top Верх.
 * @param right Право.
 * @param bottom Низ.
 * @param color Цвет.
 * @return true в случае успеха, иначе false.
 */
static bool gc9a01a_cache_vbuf_fast_fillrect(graphics_t* graphics, graphics_pos_t left, graphics_pos_t top, graphics_pos_t right, graphics_pos_t bottom, graphics_color_t color)
{
    gc9a01a_cache_t* tft_cache = (gc9a01a_cache_t*)graphics_data(graphics);
    if(tft_cache == NULL) return false;
    
    return gc9a01a_cache_fill_region(tft_cache, left, top, right, bottom, color) == E_NO_ERROR;
}

/**
 * Заполняет структуру виртуального буфера,
 * используюзего функции кэша TFT по месту объявления.
 */
#define make_gc9a01a_cache_vbuf()\
        make_graphics_vbuf(NULL, gc9a01a_cache_vbuf_set_pixel, NULL, NULL, NULL,\
                                                     gc9a01a_cache_vbuf_flush, gc9a01a_cache_vbuf_fast_fillrect);

/**
 * Заполняет структуру изображение,
 * используюзего виртуальный буфер и
 * функции кэша TFT по месту объявления.
 */
#define make_gc9a01a_cache_graphics(arg_tft_cache, arg_vbuf, arg_width, arg_height, arg_format)\
        make_graphics_virtual(arg_tft_cache, arg_width, arg_height, arg_format, arg_vbuf);


#else
#error gc9a01a_cache_vbuf need defined USE_GRAPHICS_VIRTUAL_BUFFER!
#endif

#endif	/* GC9A01A_CACHE_VBUF_H */

