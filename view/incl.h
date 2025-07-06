#ifndef VIEW_INCL_H
#define VIEW_INCL_H

#include "errors/errors.h"
#include "defs/defs.h"
#include "graphics/graphics.h"
#include "graphics/painter.h"
#include "graphics/font.h"


// Макрос для значения цвета RGB.
#ifndef MAKE_RGB
#include "gc9a01a/gc9a01a.h"
#define MAKE_RGB(r, g, b) GC9A01A_MAKE_RGB565(r, g, b)
#endif


//! Тип структуры отображения.
typedef struct _View_Incl {
    graphics_t* graphics; //!< Графика.
    painter_t painter; //!< Рисовальщик.
    font_t* font_medium; //!< Средний шрифт.
} view_incl_t;


//! Структура инициализации отображения.
typedef struct _View_Incl_Init {
    graphics_t* graphics; //!< Графика.
    font_t* font_medium; //!< Средний шрифт.
} view_incl_init_t;


//! Инициализирует отображение.
EXTERN err_t view_incl_init(view_incl_t* view, view_incl_init_t* is);

//! Отрисовывает отображение.
EXTERN void view_incl_paint(view_incl_t* view);

#endif //VIEW_INCL_H