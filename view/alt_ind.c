#include "alt_ind.h"
#include <stddef.h>
#include <math.h>
#include "vec2.h"
#include "mathutils.h"
#include "imu_main.h"
#include "port.h"


#define PI 3.14159265359f
#define PI_HALF (0.5f*(PI))


err_t view_alt_ind_init(view_alt_ind_t *view, view_alt_ind_init_t* is)
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

RAM_FUNC
static void fill_sky(view_alt_ind_t* view)
{
    size_t width = graphics_width(view->graphics);
    size_t height = graphics_width(view->graphics);
    size_t buf_size = (size_t)graphics_data_size(view->graphics);
    uint32_t* buf = (uint32_t*)graphics_data(view->graphics);
    uint32_t* buf_end = (uint32_t*)((uint8_t*)buf + buf_size);

    //painter_set_brush_color(&view->painter, ALT_IND_SKY_COLOR);
    uint32_t cc = ((uint32_t)ALT_IND_SKY_COLOR << 16) | ALT_IND_SKY_COLOR;
    //painter_fill(&view->painter);
    // fast buffer fill.
#pragma GCC unroll 16
    while(buf < buf_end){
        *buf ++ = cc;
    }
}

RAM_FUNC
static void draw_gnd(view_alt_ind_t* view, float roll, float pitch)
{
    graphics_size_t width = graphics_width(view->graphics);
    graphics_size_t height = graphics_height(view->graphics);

    float x = 1.0f;
    float y = 0.0f;

    //iq15_t roll_iq = (iq15_t)((roll - (PI*0.5f)) * ((float)IQ15_PI_PU / PI));
    //painter_rotate(&x_right, &y_right, roll_iq);
    //painter_rotate(&x_left, &y_left, -roll_iq);

    float k = pitch * (1.0f/PI_HALF);
    // graphics_pos_t pitch_offset = (graphics_pos_t)(k * (height));

    graphics_pos_t x_left  = 0;
    graphics_pos_t y_left  = (1.0 + k) * height/2;
    graphics_pos_t x_right = width;
    graphics_pos_t y_right = (1.0 + k) * height/2;

    //painter_set_pen_color(&view->painter, ALT_IND_GND_COLOR);
    //painter_draw_line(&view->painter, x_left, y_left, x_right, y_right);
    painter_set_brush_color(&view->painter, ALT_IND_GND_COLOR);
    painter_draw_fillrect(&view->painter, x_left, y_left, x_right, height);
}

void view_alt_ind_paint(view_alt_ind_t *view)
{
    //painter_fill(&view->painter);

    size_t width = graphics_width(view->graphics);
    size_t height = graphics_width(view->graphics);

    const imu_process_state_t* imu_state = imu_process_get_state();

    float roll_radf  = imu_state->roll;
    float pitch_radf = imu_state->pitch;

    // float roll_degf  = imu_state->roll  / PI * 180.0f;
    // float pitch_degf = imu_state->pitch / PI * 180.0f;

    // float roll_iq  = imu_state->roll  * IQ15_PI_PU / PI;
    // float pitch_iq = imu_state->pitch * IQ15_PI_PU / PI;

    fill_sky(view);
    draw_gnd(view, roll_radf, pitch_radf);

    painter_flush(&view->painter);
}
