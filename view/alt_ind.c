#include "alt_ind.h"
#include <stdio.h>
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

    float widthf = (float)width;
    float heightf = (float)height;

    float widthf2 = (float)(width/2);
    float heightf2 = (float)(height/2);

    vec2_t v_zero = {0.0f, 0.0f};
    vec2_t v_right_down = {widthf2, -heightf2};
    vec2_t v_right_up = {widthf2, heightf2};
    vec2_t v_left_up = {-widthf2, heightf2};

    vec2_t v_left = {0.0f, 0.0f};
    vec2_t v_right = {widthf, 0.0f};
    vec2_rotate(&v_right, &v_right, -roll);

    if(!isnormal(v_right.x) || !isnormal(v_right.y)){
        painter_set_brush_color(&view->painter, MAKE_RGB(255, 0, 0));
        painter_draw_fillrect(&view->painter, 0, 0, 120, 120);
        return;
    }

    //printf("(%.4f, %.4f)\n", v_right.x, v_right.y);

    vec2_t v_cross = {0.0f, 0.0f};

    bool intersect_right = false;
    bool intersect_up = false;

    intersect_right = mathutils_segments_intersect(&v_cross,
                                &v_zero, &v_right,
                                &v_right_down, &v_right_up
                            );
    //
    if(!intersect_right){
        intersect_up = mathutils_segments_intersect(&v_cross,
                                        &v_zero, &v_right,
                                        &v_left_up, &v_right_up
                                    );
        //
    }

    if(!isnormal(v_cross.x) || !isnormal(v_cross.y)){
        if(intersect_right){
            painter_set_brush_color(&view->painter, MAKE_RGB(255, 0, 0));
        }else if(intersect_up){
            painter_set_brush_color(&view->painter, MAKE_RGB(0, 255, 0));
        }else{
            painter_set_brush_color(&view->painter, MAKE_RGB(0, 0, 255));
        }
        painter_draw_fillrect(&view->painter, 120, 120, 240, 240);
        return;
    }

    vec2_set(&v_right, &v_cross);
    vec2_neg(&v_cross, &v_cross);
    vec2_set(&v_left, &v_cross);

    graphics_pos_t x_left  = v_left.x + width/2;
    graphics_pos_t y_left  = height/2 - v_left.y;
    graphics_pos_t x_right = v_right.x + width/2;
    graphics_pos_t y_right = height/2 - v_right.y;

    painter_set_pen_color(&view->painter, ALT_IND_GND_COLOR);
    painter_draw_line(&view->painter, x_left, y_left, x_right, y_right);
    painter_draw_circle(&view->painter, x_left, y_left, 50);

    painter_set_brush_color(&view->painter, ALT_IND_GND_COLOR);
    if(intersect_right){
        painter_draw_triangle(&view->painter, x_left, y_left, x_right, y_left, x_right, y_right);
    }else if(intersect_up){
        //painter_draw_triangle(&view->painter, x_left, y_left, x_right, y_left, x_right, y_right);
    }

    // float k = pitch * (1.0f/PI_HALF);

    // graphics_pos_t x_left  = 0;
    // graphics_pos_t y_left  = (1.0 + k) * height/2;
    // graphics_pos_t x_right = width;
    // graphics_pos_t y_right = (1.0 + k) * height/2;

    // painter_set_brush_color(&view->painter, ALT_IND_GND_COLOR);
    // painter_draw_fillrect(&view->painter, x_left, y_left, x_right, height);
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
