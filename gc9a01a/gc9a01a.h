/**
 * @file gc9a01a.h
 * Библиотека для работы с TFT на контроллере GC9A01A.
 */

#ifndef GC9A01A_H
#define	GC9A01A_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "spi/spi.h"
#include "errors/errors.h"
#include "future/future.h"
#include "defs/defs.h"


//! Вспомогательный макрос для создания 16 битного цвета.
#define GC9A01A_MAKE_RGB565_IMPL(r, g0, g1, b) ( (g1) | ((r) << 3) | ((b) << 8) | ((g0) << 13) )
//! Создаёт цвет RGB (16 бит) по заданным компонентам (0..255 на каждый канал).
#define GC9A01A_MAKE_RGB565(r, g, b) GC9A01A_MAKE_RGB565_IMPL((((r) >> 3) & 0x1f), (((g) >> 2) & 0x7), (((g) >> 5) & 0x7), (((b) >> 3) & 0x1f))


#define GC9A01A_BUFFER_SIZE 14 
#define GC9A01A_MESSAGES_COUNT 6

/**
 * Тип структуры дисплея.
 */
typedef struct _Gc9a01a {
    //! Шина SPI.
    spi_bus_t* spi;
    //! Идентификатор передачи.
    spi_transfer_id_t transfer_id;
    //! Порт выбора ведомого.
    uint ce_gpio;
    //! Пин выбора ведомого.
    uint ce_pin;
    //! Порт выбора данных/команды.
    uint dc_gpio;
    //! Пин выбора данных/команды.
    uint dc_pin;
    //! Порт сброса.
    uint rst_gpio;
    //! Пин сброса.
    uint rst_pin;
    //! Будущее.
    future_t future;
    //! Буфер.
    uint8_t buffer[GC9A01A_BUFFER_SIZE];
    //! Сообщения SPI.
    spi_message_t messages[GC9A01A_MESSAGES_COUNT];
} gc9a01a_t;

/**
 * Тип структуры инициализации дисплея.
 */
typedef struct _Gc9a01a_init {
    spi_bus_t*          spi; //!< Шина SPI.
    spi_transfer_id_t   transfer_id; //!< Идентификатор передачи.
    uint                ce_gpio; //!< Порт выбора ведомого.
    uint                ce_pin; //!< Пин выбора ведомого.
    uint                dc_gpio; //!< Порт выбора данных/команды.
    uint                dc_pin; //!< Пин выбора данных/команды.
    uint                reset_gpio; //!< Порт сброса.
    uint                reset_pin; //!< Пин сброса.
} gc9a01a_init_t;

//! Идентификатор передачи по-умолчанию.
#define GC9A01A_DEFAULT_TRANSFER_ID 93


//! Размер пиксела.
//! 16 Бит, 2 байта.
#define GC9A01A_PIXEL_SIZE_16BIT       2
//! 18 Бит, 3 байта.
#define GC9A01A_PIXEL_SIZE_18BIT       3

//! Минимум байт на пиксел.
#define GC9A01A_PIXEL_SIZE_MIN         GC9A01A_PIXEL_SIZE_16BIT

//! Максимум байт на пиксел.
#define GC9A01A_PIXEL_SIZE_MAX         GC9A01A_PIXEL_SIZE_18BIT

//! Число пикселов.
#define GC9A01A_PIXELS_COUNT           (320 * 240)

/**
 * Тип идентификатора экрана.
 */
typedef struct _Gc9a01a_Id {
    uint8_t id1; //!< Байт 1 идентификатора.
    uint8_t id2; //!< Байт 2 идентификатора.
    uint8_t id3; //!< Байт 3 идентификатора.
} gc9a01a_id_t;

//! Состояние бустера.
typedef enum _Gc9a01a_Booster_Status {
    GC9A01A_BOOSTER_OFF = 0, //!< On.
    GC9A01A_BOOSTER_ON = 1 //!< Off.
} gc9a01a_booster_status_t;

//! Порядок строк.
typedef enum _Gc9a01a_Row_Address_Order {
    GC9A01A_ROW_TOP_TO_BOTTOM = 0, //!< Сверху вниз.
    GC9A01A_ROW_BOTTOM_TO_TOP = 1 //!< Снизу вверх.
} gc9a01a_row_address_order_t;

//! Порядок столбцов.
typedef enum _Gc9a01a_Col_Address_Order {
    GC9A01A_COL_LEFT_TO_RIGHT = 0, //!< Слева направо.
    GC9A01A_COL_RIGHT_TO_LEFT = 1 //!< Справа налево.
} gc9a01a_col_address_order_t;

//! Подмена строк и столбцов.
typedef enum _Gc9a01a_Row_Col_Exchange {
    GC9A01A_ROW_COL_NORMAL_MODE = 0, //!< Нормальный режим.
    GC9A01A_ROW_COL_REVERSE_MODE = 1 //!< Реверсный режим.
} gc9a01a_row_col_exchange_t;

//! Вертикальное обновление.
typedef enum _Gc9a01a_Vertical_Refresh {
    GC9A01A_REFRESH_TOP_TO_BOTTOM = 0, //!< Сверху вниз.
    GC9A01A_REFRESH_BOTTOM_TO_TOP = 1 //!< Снизу вверх.
} gc9a01a_vertical_refresh_t;

//! Порядок цвета.
typedef enum _Gc9a01a_Color_Order {
    GC9A01A_COLOR_ORDER_RGB = 0, //! RGB
    GC9A01A_COLOR_ORDER_BGR = 1 //! BGR
} gc9a01a_color_order_t;

//! Горизонтальное обновление.
typedef enum _Gc9a01a_Horizontal_Refresh {
    GC9A01A_REFRESH_LEFT_TO_RIGHT = 0, //!< Сверху вниз.
    GC9A01A_REFRESH_RIGHT_TO_LEFT = 1 //!< Снизу вверх.
} gc9a01a_horizontal_refresh_t;

//! Формат пиксела интерфейса.
typedef enum _Gc9a01a_Pixel_Format {
    GC9A01A_PIXEL_12BIT = 3, //!< Пиксел 12 бит, только для MCU.
    GC9A01A_PIXEL_16BIT = 5, //!< Пиксел 16 бит.
    GC9A01A_PIXEL_18BIT = 6 //!< Пиксел 18 бит.
} gc9a01a_pixel_format_t;

//! Режим IDLE.
typedef enum _Gc9a01a_Idle_Mode {
    GC9A01A_IDLE_MODE_OFF = 0, //!< Off.
    GC9A01A_IDLE_MODE_ON = 1 //!< On.
} gc9a01a_idle_mode_t;

//! Частичный режим.
typedef enum _Gc9a01a_Partial_Mode {
    GC9A01A_PARTIAL_MODE_OFF = 0, //!< Off.
    GC9A01A_PARTIAL_MODE_ON = 1 //!< On.
} gc9a01a_partial_mode_t;

//! Режим сна.
typedef enum _Gc9a01a_Sleep_Mode {
    GC9A01A_SLEEP_MODE_IN = 0, //!< Off.
    GC9A01A_SLEEP_MODE_OUT = 1 //!< On.
} gc9a01a_sleep_mode_t;

//! Нормальный режим.
typedef enum _Gc9a01a_Normal_Mode {
    GC9A01A_NORMAL_MODE_OFF = 0, //!< Off.
    GC9A01A_NORMAL_MODE_ON = 1 //!< On.
} gc9a01a_normal_mode_t;

//! Состояние вертикального скроллинга.
typedef enum _Gc9a01a_Vertical_Scrolling_Status {
    GC9A01A_VERTICAL_SCROLLING_OFF = 0, //!< On.
    GC9A01A_VERTICAL_SCROLLING_ON = 1 //!< Off.
} gc9a01a_vertical_scrolling_status_t;

//! Состояние экрана.
typedef enum _Gc9a01a_Display_Mode {
    GC9A01A_DISPLAY_OFF = 0, //!< On.
    GC9A01A_DISPLAY_ON = 1 //!< Off.
} gc9a01a_display_mode_t;

//! Tearing effect line. Яхз ВТФ.
typedef enum _Gc9a01a_Tearing_Effect_Line {
    GC9A01A_TEARING_EFFECT_LINE_OFF, //!< Off.
    GC9A01A_TEARING_EFFECT_LINE_ON //!< On.
} gc9a01a_tearing_effect_line_t;

//! Tearing effect line mode.
typedef enum _Gc9a01a_Tearing_Effect_Mode {
    GC9A01A_TEARING_EFFECT_MODE1 = 0, //!< V-Blanking only.
    GC9A01A_TEARING_EFFECT_MODE2 = 1 //!< Both H and V blanking.
} gc9a01a_tearing_effect_mode_t;

//! Контроль яркости.
typedef enum _Gc9a01a_Brightness_Control {
    GC9A01A_BRIGHTNESS_CONTROL_OFF = 0, //!< Контроль яркости вкл.
    GC9A01A_BRIGHTNESS_CONTROL_ON = 1 //!< Контроль яркости выкл.
} gc9a01a_brightness_control_t;

//! Затемнение экрана.
typedef enum _Gc9a01a_Display_Dimming {
    GC9A01A_DISPLAY_DIMMING_OFF = 0, //!< Off.
    GC9A01A_DISPLAY_DIMMING_ON = 1 //!< On.
} gc9a01a_display_dimming_t;

//! Состояние подсветки.
typedef enum _Gc9a01a_Backlight_Mode {
    GC9A01A_BACKLIGHT_OFF = 0, //!< Off.
    GC9A01A_BACKLIGHT_ON = 1 //!< On.
} gc9a01a_backlight_mode_t;

/**
 * Структура состояния экрана.
 */
typedef struct _Gc9a01a_Status {
    gc9a01a_booster_status_t booster_status; //!< Состояние бустера.
    gc9a01a_row_address_order_t row_address_order; //!< Порядок строк.
    gc9a01a_col_address_order_t col_address_order; //!< Порядок столбцов.
    gc9a01a_row_col_exchange_t row_col_exchange; //!< Подмена строк и столбцов.
    gc9a01a_vertical_refresh_t vertical_refresh; //!< Вертикальное обновление.
    gc9a01a_color_order_t color_order; //!< Порядок цвета.
    gc9a01a_horizontal_refresh_t horizontal_order; //!< Горизонтальное обновление.
    gc9a01a_pixel_format_t pixel_format; //!< Формат пиксела.
    gc9a01a_idle_mode_t idle_mode; //!< Режим IDLE.
    gc9a01a_partial_mode_t partial_mode; //!< Частичный режим.
    gc9a01a_sleep_mode_t sleep_mode; //!< Режим сна.
    gc9a01a_normal_mode_t normal_mode; //!< Нормальный режим.
    gc9a01a_vertical_scrolling_status_t vertical_scrolling; //!< Состояние вертикального скроллинга.
    gc9a01a_display_mode_t display_mode; //!< Состояние экрана.
    gc9a01a_tearing_effect_line_t tearing_effect_line; //!< Tearing effect line.
    gc9a01a_tearing_effect_mode_t tearing_effect_mode; //!< Tearing effect line mode.
} gc9a01a_status_t;

/**
 * Структура настроек доступа к памяти.
 */
typedef struct _Gc9a01a_Mad_Ctl {
    gc9a01a_row_address_order_t row_address_order; //!< Порядок строк.
    gc9a01a_col_address_order_t col_address_order; //!< Порядок столбцов.
    gc9a01a_row_col_exchange_t row_col_exchange; //!< Реверсное отображение.
    gc9a01a_vertical_refresh_t vertical_refresh; //!< Вертикальное обновление.
    gc9a01a_color_order_t color_order; //!< Порядок цвета.
    gc9a01a_horizontal_refresh_t horizontal_refresh; //!< Горизонтальное обновление.
} gc9a01a_madctl_t;

/**
 * Структура контроля экрана.
 */
typedef struct _Gc9a01a_Display_Control {
    gc9a01a_brightness_control_t brightness_control; //!< Контроль яркости.
    gc9a01a_display_dimming_t display_dimming; //!< Затемнение экрана.
    gc9a01a_backlight_mode_t backlight_mode; //!< Состояние подсветки.
}gc9a01a_display_control_t;

/**
 * Каллбэк SPI.
 * @return true, если событие обработано, иначе false.
 */
EXTERN bool gc9a01a_spi_callback(gc9a01a_t* tft);

/**
 * Инициализирует TFT.
 * @param tft TFT.
 * @param tft_init Структура инициализации.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_init(gc9a01a_t* tft, gc9a01a_init_t* tft_init);

/**
 * Получает флаг занятости TFT.
 * @param tft TFT.
 * @return Флаг занятости TFT.
 */
EXTERN bool gc9a01a_busy(gc9a01a_t* tft);

/**
 * Получает код ошибки асинхронно выполненой операции.
 * @param tft TFT.
 * @return Код ошибки асинхронно выполненой операции.
 */
EXTERN err_t gc9a01a_error(gc9a01a_t* tft);

/**
 * Ожидает завершения операции с TFT.
 * @param tft TFT.
 * @return Код ошибки операции.
 */
EXTERN err_t gc9a01a_wait(gc9a01a_t* tft);

/**
 * Сбрасывает TFT.
 * @param tft TFT.
 */
EXTERN void gc9a01a_reset(gc9a01a_t* tft);

/**
 * Отправляет последовательность инициализации в экран.
 * @param tft TFT.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_send_init(gc9a01a_t* tft);

/**
 * Читает идентификатор TFT.
 * @param tft TFT.
 * @param id Адрес переменной для записи идентификатора.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_read_id(gc9a01a_t* tft, gc9a01a_id_t* id);

/**
 * Читает статус TFT.
 * @param tft TFT.
 * @param status Статус.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_read_status(gc9a01a_t* tft, gc9a01a_status_t* status);

/**
 * Засыпает.
 * @param tft TFT.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_sleep_in(gc9a01a_t* tft);

/**
 * Просыпается.
 * @param tft TFT.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_sleep_out(gc9a01a_t* tft);

/**
 * Частичный режим.
 * @param tft TFT.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_partial_mode(gc9a01a_t* tft);

/**
 * Нормальный режим.
 * @param tft TFT.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_normal_mode(gc9a01a_t* tft);

/**
 * Выключает инверсию.
 * @param tft TFT.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_inversion_off(gc9a01a_t* tft);

/**
 * Включает инверсию.
 * @param tft TFT.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_inversion_on(gc9a01a_t* tft);

/**
 * Выключает дисплей.
 * @param tft TFT.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_display_off(gc9a01a_t* tft);

/**
 * Включает дисплей.
 * @param tft TFT.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_display_on(gc9a01a_t* tft);

/**
 * Устанавливает столбцы области памяти, к которой есть доступ.
 * @param tft TFT.
 * @param start Начальный столбец.
 * @param end Конечный столбец.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_set_column_address(gc9a01a_t* tft, uint16_t start, uint16_t end);

/**
 * Устанавливает страницы области памяти, к которой есть доступ.
 * @param tft TFT.
 * @param start Начальная страница.
 * @param end Конечная страница.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_set_page_address(gc9a01a_t* tft, uint16_t start, uint16_t end);

/**
 * Записывает данные.
 * Отправка данных асинхронна.
 * @param tft TFT.
 * @param data Данные.
 * @param size Размер данных.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_write(gc9a01a_t* tft, const void* data, size_t size);

/**
 * Команда начала записи данных.
 * Отправка асинхронна.
 * @param tft TFT.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_begin_write(gc9a01a_t* tft);

/**
 * Устанавливает устанавливает область строк частичного режима.
 * @param tft TFT.
 * @param start Начальная строка.
 * @param end Конечная строка.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_set_partial_area(gc9a01a_t* tft, uint16_t start, uint16_t end);

/**
 * Устанавливает область скроллинга.
 * @param tft TFT.
 * @param tfa Верхняя фиксированная область.
 * @param vsa Размер области скроллинга.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_set_vertical_scrolling(gc9a01a_t* tft, uint16_t tfa, uint16_t vsa);

/**
 * Выключает генерацию сигнала на линии TE.
 * @param tft TFT.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_tearing_effect_line_off(gc9a01a_t* tft);

/**
 * Включает генерацию сигнала на линии TE.
 * @param tft TFT.
 * @param mode Режим.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_tearing_effect_line_on(gc9a01a_t* tft, gc9a01a_tearing_effect_mode_t mode);

/**
 * Устанавливает настройки доступа к памяти.
 * @param tft Экран.
 * @param madctl Настройки доступа к памяти.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_set_madctl(gc9a01a_t* tft, const gc9a01a_madctl_t* madctl);

/**
 * Устанавливает адрес памяти для скроллинга.
 * @param tft TFT.
 * @param vsp Адрес для скроллинга.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_set_vertical_scrolling_start_address(gc9a01a_t* tft, uint16_t vsp);

/**
 * Выключает режим ожидания.
 * @param tft TFT.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_idle_off(gc9a01a_t* tft);

/**
 * Включает режим ожидания.
 * @param tft TFT.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_idle_on(gc9a01a_t* tft);

/**
 * Устанавливает формат пиксела экрана.
 * @param tft Экран.
 * @param rgb_iface_format Формат пиксела RGB-интерфейса.
 * @param mcu_iface_format Формат пиксела MCU-интерфейса.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_set_pixel_format(gc9a01a_t* tft, gc9a01a_pixel_format_t rgb_iface_format, gc9a01a_pixel_format_t mcu_iface_format);

/**
 * Устанавливает tear scanline.
 * @param tft TFT.
 * @param sts tear scanline.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_set_tear_scanline(gc9a01a_t* tft, uint16_t sts);

/**
 * Получает scanline.
 * @param tft TFT.
 * @param gts scanline.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_get_scanline(gc9a01a_t* tft, uint16_t* gts);

/**
 * Записывает данные с адреса последней записи.
 * Отправка данных асинхронна.
 * @param tft TFT.
 * @param data Данные.
 * @param size Размер данных.
 */
EXTERN err_t gc9a01a_write_continue(gc9a01a_t* tft, const void* data, size_t size);

/**
 * Команда начала записи данных с адреса последней записи.
 * Отправка асинхронна.
 * @param tft TFT.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_begin_write_continue(gc9a01a_t* tft);

/**
 * Устанавливает яркость экрана.
 * @param tft Экран.
 * @param brightness Яркость.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_set_brightness(gc9a01a_t* tft, uint8_t brightness);

/**
 * Устанавливает настройки экрана.
 * @param tft TFT.
 * @param control Настройки экрана.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_set_display_control(gc9a01a_t* tft, const gc9a01a_display_control_t* control);

/**
 * Читает идентификатор ID1 TFT.
 * @param tft TFT.
 * @param id Адрес переменной для записи идентификатора.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_read_id1(gc9a01a_t* tft, uint8_t* id);

/**
 * Читает идентификатор ID2 TFT.
 * @param tft TFT.
 * @param id Адрес переменной для записи идентификатора.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_read_id2(gc9a01a_t* tft, uint8_t* id);

/**
 * Читает идентификатор ID3 TFT.
 * @param tft TFT.
 * @param id Адрес переменной для записи идентификатора.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_read_id3(gc9a01a_t* tft, uint8_t* id);

/**
 * Записывает данные в TFT без посыла команды.
 * Асинхронная операция.
 * @param tft TFT.
 * @param data Данные.
 * @param size Размер данных.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_data(gc9a01a_t* tft, const void* data, size_t size);

/**
 * Устанавливает пиксел на экране.
 * Асинхронная операция.
 * Память пиксела может быть освобождена или
 * использована сразу после возврата из функции.
 * @param tft TFT.
 * @param x Координата X.
 * @param y Координата Y.
 * @param pixel Данные пиксела.
 * @param size Размер пиксела в байтах.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_set_pixel(gc9a01a_t* tft, uint16_t x, uint16_t y, const void* pixel, size_t size);

/**
 * Записывает данные в регион на экране.
 * Асинхронная операция.
 * @param tft TFT.
 * @param x0 Координата X верхнего левого угла региона.
 * @param y0 Координата Y верхнего левого угла региона.
 * @param x1 Координата X правого нижнего угла региона.
 * @param y1 Координата Y правого нижнего угла региона.
 * @param data Данные.
 * @param size Размер данных в байтах.
 * @return Код ошибки.
 */
EXTERN err_t gc9a01a_write_region(gc9a01a_t* tft, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const void* data, size_t size);

//147
//209

#endif	/* GC9A01A_H */

