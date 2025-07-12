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
    graphics_pos_t width = (graphics_pos_t)graphics_width(view->graphics);
    graphics_pos_t height = (graphics_pos_t)graphics_height(view->graphics);

    graphics_pos_t width2 = width/2;
    graphics_pos_t height2 = height/2;

    float widthf = (float)width;
    float heightf = (float)height;

    float widthf2 = (float)(width2);
    float heightf2 = (float)(height2);

    vec2_t v_right_down = {widthf2, -heightf2};
    vec2_t v_right_up = {widthf2, heightf2};
    vec2_t v_left_up = {-widthf2, heightf2};

    // float k = pitch * (1.0f/PI_HALF);
    // float pitch_dhf = k * heightf2;
    // vec2_t v_zero = {0.0f, pitch_dhf};
    vec2_t v_zero = {0.0f, 0.0f};

    vec2_t v_left = {0.0f, 0.0f};
    vec2_t v_right = {widthf, 0.0f};
    vec2_rotate(&v_right, &v_right, -roll);

    v_left.x = -v_right.x;
    v_left.y = -v_right.y;
    //vec2_reflect(&v_left, &v_right, &v_zero);

    //printf("(%.4f, %.4f)\n", v_right.x, v_right.y);

    vec2_t v_cross = {0.0f, 0.0f};

    bool intersect_right = false;
    bool intersect_up = false;

    intersect_right = mathutils_segments_intersect(&v_cross,
                                &v_left, &v_right,
                                &v_right_down, &v_right_up
                            );
    //
    if(!intersect_right){
        intersect_up = mathutils_segments_intersect(&v_cross,
                                        &v_left, &v_right,
                                        &v_left_up, &v_right_up
                                    );
        //
    }

    //printf("r %u u %u\n", (unsigned)intersect_right, (unsigned)intersect_up);
    //printf("x(%.4f, %.4f)\n", v_cross.x, v_cross.y);

    if(!isnormal(v_cross.x) || !isnormal(v_cross.y)){
        // if(intersect_right){
        //     painter_set_brush_color(&view->painter, MAKE_RGB(255, 0, 0));
        // }else if(intersect_up){
        //     painter_set_brush_color(&view->painter, MAKE_RGB(0, 255, 0));
        // }else{
        //     painter_set_brush_color(&view->painter, MAKE_RGB(0, 0, 255));
        // }
        // painter_draw_fillrect(&view->painter, 120, 120, 240, 240);
        //printf("vcross isnormal fail!\n");
        return;
    }

    vec2_set(&v_right, &v_cross);
    vec2_neg(&v_cross, &v_cross);
    vec2_set(&v_left, &v_cross);

    if(v_right.x < v_left.x){
        vec2_set(&v_cross, &v_left);
        vec2_set(&v_left, &v_right);
        vec2_set(&v_right, &v_cross);
    }

    // float k = pitch * (1.0f/PI_HALF);
    // graphics_pos_t pitch_height2 = (1.0 + k) * height2;

    graphics_pos_t x_left  = v_left.x + width2;
    graphics_pos_t y_left  = height2 - v_left.y;
    graphics_pos_t x_right = v_right.x + width2;
    graphics_pos_t y_right = height2 - v_right.y;
    graphics_pos_t x_mid;
    graphics_pos_t y_mid;

    if(x_left  <  0)      x_left  = 0;
    if(x_right >= width)  x_right = width - 1;

    if(y_left  <  0)          y_left  = 0;
    else if(y_left >= height) y_left = height - 1;

    if(y_right  <  0)          y_right  = 0;
    else if(y_right >= height) y_right = height - 1;

    if(y_right >= y_left){
        x_mid = x_left;
        y_mid = y_right;
        //printf("m1(%d, %d)\n", x_mid, y_mid);
    }else{
        x_mid = x_right;
        y_mid = y_left;
        //printf("m2(%d, %d)\n", x_mid, y_mid);
    }

    //printf("m(%d, %d)\n", x_mid, y_mid);
    printf("ph2(%d) ", (int)heightf2);
    printf("l(%d, %d) ", x_left, y_left);
    printf("m(%d, %d) ", x_mid, y_mid);
    printf("r(%d, %d)\n", x_right, y_right);

    // painter_set_pen_color(&view->painter, ALT_IND_GND_COLOR);
    // painter_draw_line(&view->painter, x_left, y_left, x_right, y_right);
    //painter_set_brush_color(&view->painter, ALT_IND_GND_COLOR);
    //painter_draw_circle(&view->painter, x_left, y_left, 50);

    painter_set_brush_color(&view->painter, ALT_IND_GND_COLOR);
    painter_draw_triangle(&view->painter, x_left, y_left, x_mid, y_mid, x_right, y_right);
    if(intersect_right){
        if(y_mid < height2){
            painter_draw_fillrect(&view->painter, 0, 0, width - 1, y_mid);
        }else if(y_mid >= height2){
            painter_draw_fillrect(&view->painter, 0, y_mid, width - 1, height - 1);
        }
    }else if(intersect_up){
        if(x_mid < width2){
            painter_draw_fillrect(&view->painter, 0, 0, x_mid, height - 1);
        }else if(x_mid >= width2){
            painter_draw_fillrect(&view->painter, x_mid, 0, width - 1, height - 1);
        }
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
