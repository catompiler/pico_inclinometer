#include "angles.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "imu_main.h"


err_t view_angles_init(view_angles_t *view, view_angles_init_t* is)
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
    painter_set_brush_color(&view->painter, MAKE_RGB(0x0, 0x0, 0x0));

    return E_NO_ERROR;
}

void view_angles_paint(view_angles_t *view)
{
    painter_fill(&view->painter);

    const imu_process_state_t* imu_state = imu_process_get_state();

    const size_t str_buf_len = 128;
    char str_buf[str_buf_len];

    if(imu_state->status & IMU_PROCESS_STATUS_VALID){
        memset(str_buf, 0x0, str_buf_len);

        float roll  = imu_state->roll  / 3.14159265359f * 180.0f;
        float pitch = imu_state->pitch / 3.14159265359f * 180.0f;
        
        int n = snprintf(str_buf, str_buf_len-1, "roll: %.2f°\npitch: %.2f°", roll, pitch);
        if(n >= 0) str_buf[n] = '\0';

        painter_set_pen_color(&view->painter, MAKE_RGB(0xff, 0xff, 0xff));
        painter_set_source_image_mode(&view->painter, PAINTER_SOURCE_IMAGE_MODE_BITMAP);
        painter_draw_string(&view->painter, 10, 80, str_buf);
    }
    else if(imu_state->status & (IMU_PROCESS_STATUS_IMU_ERROR|IMU_PROCESS_STATUS_INIT_IMU_ERROR|IMU_PROCESS_STATUS_INIT_SENSOR_ERROR)){
        memset(str_buf, 0x0, str_buf_len);

        int n = snprintf(str_buf, str_buf_len-1,
                            "status: 0x%x\nimu err: %d\ninit err: %d",
                            (int)imu_state->status,
                            (int)imu_state->imu_error,
                            (int)imu_state->init_error);
        if(n >= 0) str_buf[n] = '\0';

        painter_set_pen_color(&view->painter, GC9A01A_MAKE_RGB565(0xff, 0xff, 0));
        painter_set_source_image_mode(&view->painter, PAINTER_SOURCE_IMAGE_MODE_BITMAP);
        painter_draw_string(&view->painter, 10, 80, str_buf);
    }

    painter_flush(&view->painter);
}
