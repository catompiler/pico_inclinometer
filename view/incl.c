#include "incl.h"
#include <stddef.h>


err_t view_incl_init(view_incl_t *view, view_incl_init_t* is)
{
    if(is->graphics == NULL) return E_NULL_POINTER;
    if(is->font_medium == NULL) return E_NULL_POINTER;

    view->graphics = is->graphics;
    view->font_medium = is->font_medium;

    err_t err;

    err = painter_init(&view->painter, view->graphics);
    if(err != E_NO_ERROR) return err;

    painter_set_brush(&view->painter, PAINTER_BRUSH_SOLID);
    painter_set_pen(&view->painter, PAINTER_PEN_SOLID);
    
    painter_set_font(&view->painter, view->font_medium);
    painter_set_pen_color(&view->painter, MAKE_RGB(0xff, 0, 0));
    painter_set_brush_color(&view->painter, MAKE_RGB(0x0, 0, 0x0));

    return E_NO_ERROR;
}

void view_incl_paint(view_incl_t *view)
{
    painter_fill(&view->painter);
    painter_flush(&view->painter);
}
