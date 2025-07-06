#ifndef VIEW_ALT_IND_H
#define VIEW_ALT_IND_H

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

// Цвета.
//! Небо.
#define ALT_IND_SKY_COLOR MAKE_RGB(0x01, 0x9f, 0xaf)
//! Земля.
#define ALT_IND_GND_COLOR MAKE_RGB(0xb0, 0x46, 0x02)


//! Тип структуры отображения.
typedef struct _View_Alt_Ind {
    graphics_t* graphics; //!< Графика.
    painter_t painter; //!< Рисовальщик.
    font_t* font_medium; //!< Средний шрифт.
} view_alt_ind_t;


//! Структура инициализации отображения.
typedef struct _View_Alt_Ind_Init {
    graphics_t* graphics; //!< Графика.
    font_t* font_medium; //!< Средний шрифт.
} view_alt_ind_init_t;


//! Инициализирует отображение.
EXTERN err_t view_alt_ind_init(view_alt_ind_t* view, view_alt_ind_init_t* is);

//! Отрисовывает отображение.
EXTERN void view_alt_ind_paint(view_alt_ind_t* view);

#endif //VIEW_ALT_IND_H
