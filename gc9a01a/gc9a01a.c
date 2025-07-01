#include "gc9a01a.h"
#include "hardware/gpio.h"
#include "utils/utils.h"
#include "pico/time.h"
#include "bits/bits.h"
#include "defs/defs.h"
#include <string.h>
//#include <stdio.h>

//extern void led_on(uint16_t led_pin);
//#define GC9A01A_GET_MEM_DEBUG

//! Продолжительность ресета.
#define GC9A01A_RESET_TIME_US  15
//#define GC9A01A_RESET_TIME_MS  150
#define GC9A01A_RESET_WAIT_TIME_MS  150
//#define GC9A01A_RESET_WAIT_TIME_MS  500
//#define GC9A01A_SLEEP_OUT_WAIT_TIME_MS  10


//! Максимальные значения параметров
#define GC9A01A__MAX     GC9A01A_

//! Команды и их параметры.
//! Размер команды.
#define GC9A01A_CMD_SIZE    0x1
//! Размер мусорных данных при приёме.
#define GC9A01A_TRASH_SIZE    0x1

//! Чтение идентификатора.
#define GC9A01A_CMD_READ_ID                 0x4
#define GC9A01A_RD_ID_DATA_SIZE             4
#define GC9A01A_RD_ID_ID1_BYTE              1
#define GC9A01A_RD_ID_ID2_BYTE              2
#define GC9A01A_RD_ID_ID3_BYTE              3

//! Чтение состояния.
#define GC9A01A_CMD_READ_DISPLAY_STATUS         0x9
#define GC9A01A_RD_DS_DATA_SIZE                 5
#define GC9A01A_RD_DS_BOOSTER_STATUS_OFFSET     31
#define GC9A01A_RD_DS_BOOSTER_STATUS_MASK       1
#define GC9A01A_RD_DS_ROW_ADDR_ORDER_OFFSET     30
#define GC9A01A_RD_DS_ROW_ADDR_ORDER_MASK       1
#define GC9A01A_RD_DS_COL_ADDR_ORDER_OFFSET     29
#define GC9A01A_RD_DS_COL_ADDR_ORDER_MASK       1
#define GC9A01A_RD_DS_ROW_COL_EXCH_OFFSET       28
#define GC9A01A_RD_DS_ROW_COL_EXCH_MASK         1
#define GC9A01A_RD_DS_VERTICAL_REFRESH_OFFSET   27
#define GC9A01A_RD_DS_VERTICAL_REFRESH_MASK     1
#define GC9A01A_RD_DS_COLOR_ORDER_OFFSET        26
#define GC9A01A_RD_DS_COLOR_ORDER_MASK          1
#define GC9A01A_RD_DS_HORIZONTAL_REFRESH_OFFSET 25
#define GC9A01A_RD_DS_HORIZONTAL_REFRESH_MASK   1
#define GC9A01A_RD_DS_PIXEL_FORMAT_OFFSET       20
#define GC9A01A_RD_DS_PIXEL_FORMAT_MASK         7
#define GC9A01A_RD_DS_IDLE_MODE_OFFSET          19
#define GC9A01A_RD_DS_IDLE_MODE_MASK            1
#define GC9A01A_RD_DS_PARTIAL_MODE_OFFSET       18
#define GC9A01A_RD_DS_PARTIAL_MODE_MASK         1
#define GC9A01A_RD_DS_SLEEP_MODE_OFFSET         17
#define GC9A01A_RD_DS_SLEEP_MODE_MASK           1
#define GC9A01A_RD_DS_NORMAL_MODE_OFFSET        16
#define GC9A01A_RD_DS_NORMAL_MODE_MASK          1
#define GC9A01A_RD_DS_VERTICAL_SCROLL_OFFSET    15
#define GC9A01A_RD_DS_VERTICAL_SCROLL_MASK      1
#define GC9A01A_RD_DS_DISPLAY_MODE_OFFSET       10
#define GC9A01A_RD_DS_DISPLAY_MODE_MASK         1
#define GC9A01A_RD_DS_TEARING_EFFECT_OFFSET     9
#define GC9A01A_RD_DS_TEARING_EFFECT_MASK       1
#define GC9A01A_RD_DS_TEARING_MODE_OFFSET       5
#define GC9A01A_RD_DS_TEARING_MODE_MASK         1

//! Вход в сон.
#define GC9A01A_CMD_SLEEP_IN                    0x10

//! Выход из сна.
#define GC9A01A_CMD_SLEEP_OUT                   0x11

//! Частичный режим вкл.
#define GC9A01A_CMD_PARTIAL_MODE_ON             0x12

//! Нормальный режим вкл.
#define GC9A01A_CMD_NORMAL_MODE_ON              0x13

//! Инверсия выкл.
#define GC9A01A_CMD_INVERSION_OFF               0x20

//! Инверсия вкл.
#define GC9A01A_CMD_INVERSION_ON                0x21

//! Дисплей выкл.
#define GC9A01A_CMD_DISPLAY_OFF                 0x28

//! Дисплей вкл.
#define GC9A01A_CMD_DISPLAY_ON                  0x29

//! Установка столбцов разрешённой области.
#define GC9A01A_CMD_WRITE_COL_ADDRESS           0x2a
#define GC9A01A_WR_CA_DATA_SIZE                 4

//! Установка строк разрешённой области.
#define GC9A01A_CMD_WRITE_PAGE_ADDRESS          0x2b
#define GC9A01A_WR_PGA_DATA_SIZE                 4

//! Запись памяти.
#define GC9A01A_CMD_WRITE_MEMORY                0x2c

//! Установка области частичного режима.
#define GC9A01A_CMD_WRITE_PARTIAL_AREA          0x30
#define GC9A01A_WR_PLA_DATA_SIZE                4

//! Установка области вертикального скролинга.
#define GC9A01A_CMD_WRITE_VERTICAL_SCROLL_DEF    0x33
#define GC9A01A_WR_VERT_SCROLL_DEF_DATA_SIZE     4

//! Выключение tearing effect line.
#define GC9A01A_CMD_TEARING_EFFECT_LINE_OFF     0x34

//! Включение tearing effect line.
#define GC9A01A_CMD_TEARING_EFFECT_LINE_ON      0x35
#define GC9A01A_WR_TE_LINE_ON_DATA_SIZE         1

//! Запись настроек доступа к памяти.
#define GC9A01A_CMD_WRITE_MADCTL                0x36
#define GC9A01A_WR_MC_DATA_SIZE                 1
#define GC9A01A_WR_MC_ROW_ADDR_ORDER_OFFSET     7
#define GC9A01A_WR_MC_ROW_ADDR_ORDER_MASK       1
#define GC9A01A_WR_MC_COL_ADDR_ORDER_OFFSET     6
#define GC9A01A_WR_MC_COL_ADDR_ORDER_MASK       1
#define GC9A01A_WR_MC_ROW_COL_EXCH_OFFSET       5
#define GC9A01A_WR_MC_ROW_COL_EXCH_MASK         1
#define GC9A01A_WR_MC_VERTICAL_REFRESH_OFFSET   4
#define GC9A01A_WR_MC_VERTICAL_REFRESH_MASK     1
#define GC9A01A_WR_MC_COLOR_ORDER_OFFSET        3
#define GC9A01A_WR_MC_COLOR_ORDER_MASK          1
#define GC9A01A_WR_MC_HORIZONTAL_REFRESH_OFFSET 2
#define GC9A01A_WR_MC_HORIZONTAL_REFRESH_MASK   1

//! Установка адреса скроллинга.
#define GC9A01A_CMD_WRITE_VERT_SCROLL_START_ADDR 0x37
#define GC9A01A_WR_VS_START_ADDR_DATA_SIZE       2

//! IDLE выкл.
#define GC9A01A_CMD_IDLE_OFF                    0x38

//! IDLE вкл.
#define GC9A01A_CMD_IDLE_ON                     0x39

//! Чтение формата пиксела экрана.
#define GC9A01A_CMD_WRITE_DISPLAY_PIXEL_FORMAT  0x3a
#define GC9A01A_WR_PF_DATA_SIZE                 1
#define GC9A01A_WR_PF_DPI_OFFSET                4
#define GC9A01A_WR_PF_DPI_MASK                  7
#define GC9A01A_WR_PF_DBI_OFFSET                0
#define GC9A01A_WR_PF_DBI_MASK                  7

//! Запись памяти.
#define GC9A01A_CMD_WRITE_MEMORY_CONTINUE       0x3c

//! Установка tear scan line.
#define GC9A01A_CMD_SET_TEAR_SCANLINE           0x44
#define GC9A01A_SET_TEAR_SL_DATA_SIZE           2

//! Получение scan line.
#define GC9A01A_CMD_GET_SCANLINE                0x45
#define GC9A01A_GET_SL_DATA_SIZE                3

//! Установка яркости.
#define GC9A01A_CMD_WRITE_BRIGHTNESS            0x51
#define GC9A01A_WR_BN_DATA_SIZE                 1

//! Запись настроек экрана.
#define GC9A01A_CMD_WRITE_DISPLAY_CONTROL       0x53
#define GC9A01A_WR_DC_DATA_SIZE                 1
#define GC9A01A_WR_DC_BRIGHTNESS_CONTROL_OFFSET 5
#define GC9A01A_WR_DC_BRIGHTNESS_CONTROL_MASK   1
#define GC9A01A_WR_DC_DISPLAY_DIMMING_OFFSET    3
#define GC9A01A_WR_DC_DISPLAY_DIMMING_MASK      1
#define GC9A01A_WR_DC_BACKLIGHT_OFFSET          2
#define GC9A01A_WR_DC_BACKLIGHT_MASK            1

//! Чтение ID1.
#define GC9A01A_CMD_READ_ID1                    0xda
#define GC9A01A_RD_ID1_DATA_SIZE                2

//! Чтение ID2.
#define GC9A01A_CMD_READ_ID2                    0xdb
#define GC9A01A_RD_ID2_DATA_SIZE                2

//! Чтение ID3.
#define GC9A01A_CMD_READ_ID3                    0xdc
#define GC9A01A_RD_ID3_DATA_SIZE                2


static const uint8_t _INIT_SEQUENCE[] = {
    0xfe, 0x00,  // Inter Register Enable1 (FEh)
    0xef, 0x00,  // Inter Register Enable2 (EFh)
    0xb6, 0x02, 0x00, 0x00,  // Display Function Control (B6h) [S1→S360 source, G1→G32 gate]
    0x36, 0x01, 0x48,  // Memory Access Control(36h) [Invert Row order, invert vertical scan order]
    0x3a, 0x01, 0x05,  // COLMOD: Pixel Format Set (3Ah) [16 bits / pixel]
    0xc3, 0x01, 0x13,  // Power Control 2 (C3h) [VREG1A = 5.06, VREG1B = 0.68]
    0xc4, 0x01, 0x13,  // Power Control 3 (C4h) [VREG2A = -3.7, VREG2B = 0.68]
    0xc9, 0x01, 0x22,  // Power Control 4 (C9h)
    0xf0, 0x06, 0x45, 0x09, 0x08, 0x08, 0x26, 0x2a,  // SET_GAMMA1 (F0h)
    0xf1, 0x06, 0x43, 0x70, 0x72, 0x36, 0x37, 0x6f,  // SET_GAMMA2 (F1h)
    0xf2, 0x06, 0x45, 0x09, 0x08, 0x08, 0x26, 0x2a,  // SET_GAMMA3 (F2h)
    0xf3, 0x06, 0x43, 0x70, 0x72, 0x36, 0x37, 0x6f,  // SET_GAMMA4 (F3h)
    0x66, 0x0a, 0x3c, 0x00, 0xcd, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00,
    0x67, 0x0a, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98,
    0x74, 0x07, 0x10, 0x85, 0x80, 0x00, 0x00, 0x4e, 0x00,
    0x98, 0x02, 0x3e, 0x07,
    0x35, 0x00,  // Tearing Effect Line ON (35h) [both V-blanking and H-blanking]
    0x21, 0x00,  // Display Inversion ON (21h)
    0x11, 0x80, 0x78,  // Sleep Out Mode (11h) and delay(120)
    0x29, 0x80, 0x14,  // Display ON (29h) and delay(20)
    0x2a, 0x04, 0x00, 0x00, 0x00, 0xef,  // Column Address Set (2Ah) [Start col = 0, end col = 239]
    0x2b, 0x04, 0x00, 0x00, 0x00, 0xef   // Row Address Set (2Bh) [Start row = 0, end row = 239]
};

static const size_t _INIT_SEQUENCE_LENGTH = sizeof(_INIT_SEQUENCE);




ALWAYS_INLINE static void gpio_set(uint port, uint pin)
{
    (void) port;

    gpio_put(pin, 1);
}

ALWAYS_INLINE static void gpio_reset(uint port, uint pin)
{
    (void) port;

    gpio_put(pin, 0);
}


/**
 * Выставляет значения управляющих пинов для начала обмена данными с TFT.
 * @param tft TFT.
 * @param is_data Флаг данные/команда.
 */
ALWAYS_INLINE static void gc9a01a_pins_begin(gc9a01a_t* tft, bool is_data)
{
    if(is_data){
        gpio_set(tft->dc_gpio, tft->dc_pin);
    }else{
        gpio_reset(tft->dc_gpio, tft->dc_pin);
    }
    gpio_reset(tft->ce_gpio, tft->ce_pin);
}

/**
 * Выставлят значения управляющих пинов для окончания обмена данными с TFT.
 * @param tft TFT.
 */
ALWAYS_INLINE static void gc9a01a_pins_end(gc9a01a_t* tft)
{
    gpio_set(tft->ce_gpio, tft->ce_pin);
    gpio_set(tft->dc_gpio, tft->dc_pin);
}

/**
 * Ждёт завершения текущей операции.
 * @return true, если шина i2c занята нами и мы дождались, иначе false.
 */
static bool gc9a01a_wait_current_op(gc9a01a_t* tft)
{
    // Если шина занята, да ещё и не нами - возврат ошибки занятости.
    if(spi_bus_busy(tft->spi) && spi_bus_transfer_id(tft->spi) != tft->transfer_id) return false;
    
    future_wait(&tft->future);
    spi_bus_wait(tft->spi);
    
    return true;
}

/**
 * Начинает обмен данными с TFT.
 * @param tft TFT.
 * @param is_data Флаг данные/команда.
 */
static void gc9a01a_begin(gc9a01a_t* tft, bool is_data)
{
    future_set_result(&tft->future, int_to_pvoid(E_NO_ERROR));
    
    future_start(&tft->future);
    
    gc9a01a_pins_begin(tft, is_data);
}

/**
 * Завершает обмен данными с TFT.
 * @param tft TFT.
 * @param err Код ошибки.
 */
static void gc9a01a_end(gc9a01a_t* tft, err_t err)
{
    future_finish(&tft->future, int_to_pvoid(err));
    
    gc9a01a_pins_end(tft);
}

/**
 * Каллбэк сообщения команды.
 * Устанавливает пин D/C на команду.
 * @param message Сообщение SPI.
 */
static void gc9a01a_cmd_message_start(spi_message_t* message)
{
    //printf("gc9a01a_cmd_message_start\r\n");
    gc9a01a_t* tft = (gc9a01a_t*)message->sender_data;
    if(tft != NULL){
        //gpio_set(tft->ce_gpio, tft->ce_pin);
        gpio_reset(tft->dc_gpio, tft->dc_pin);
        //gpio_reset(tft->ce_gpio, tft->ce_pin);
    }
}

/**
 * Каллбэк сообщения команды.
 * Устанавливает пин D/C на данные.
 * @param message Сообщение SPI.
 */
static void gc9a01a_cmd_message_end(spi_message_t* message)
{
    //printf("gc9a01a_cmd_message_end\r\n");
    gc9a01a_t* tft = (gc9a01a_t*)message->sender_data;
    if(tft != NULL){
        //gpio_set(tft->ce_gpio, tft->ce_pin);
        gpio_set(tft->dc_gpio, tft->dc_pin);
        //gpio_reset(tft->ce_gpio, tft->ce_pin);
    }
}

/**
 * Отправляет данные в TFT.
 * Асинхронно.
 * @param tft TFT.
 * @param is_data Флаг данные/команда.
 * @param messages_count Число сообщений.
 * @return Код ошибки.
 */
static err_t gc9a01a_transfer(gc9a01a_t* tft, bool is_data, size_t messages_count)
{
    if(!spi_bus_set_transfer_id(tft->spi, tft->transfer_id)) return E_BUSY;
    
    gc9a01a_begin(tft, is_data);
    
    err_t err = spi_bus_transfer(tft->spi, tft->messages, messages_count);
    
    if(err != E_NO_ERROR){
        gc9a01a_end(tft, err);
        return err;
    }
    
    return E_NO_ERROR;
}

bool gc9a01a_spi_callback(gc9a01a_t* tft)
{
    if(spi_bus_transfer_id(tft->spi) != tft->transfer_id) return false;
    
    gc9a01a_end(tft, spi_bus_status(tft->spi) != SPI_STATUS_TRANSFERED ? E_IO_ERROR : E_NO_ERROR);
    
    return true;
}

/**
 * Получает указатель на данные в буфере нужного размера.
 * @param tft TFT.
 * @param size Необходимый размер.
 * @param index Значение предыдущего индекса.
 * @return Указатель на данные в бефере.
 */
ALWAYS_INLINE static uint8_t* gc9a01a_get_buffer(gc9a01a_t* tft, size_t size, size_t* index)
{
#ifdef GC9A01A_GET_MEM_DEBUG
    if(index == NULL) return NULL;
#endif
    
    size_t i = *index;
    
#ifdef GC9A01A_GET_MEM_DEBUG
    if(i + size > GC9A01A_BUFFER_SIZE) return NULL;
#endif
    
    (*index) += size;
    
    return &tft->buffer[i];
}

/**
 * Получает указатель на сообщение.
 * @param tft TFT.
 * @param index Значение предыдущего индекса.
 * @return Указатель на данные в бефере.
 */
ALWAYS_INLINE static spi_message_t* gc9a01a_get_message(gc9a01a_t* tft, size_t* index)
{
#ifdef GC9A01A_GET_MEM_DEBUG
    if(index == NULL) return NULL;
#endif
    
    size_t i = *index;
    
#ifdef GC9A01A_GET_MEM_DEBUG
    if(i >= GC9A01A_MESSAGES_COUNT) return NULL;
#endif
    
    (*index) ++;
    
    return &tft->messages[i];
}

err_t gc9a01a_send(gc9a01a_t* tft, const uint8_t* cmd, const uint8_t* args, size_t args_size)
{
    //if(cmd == NULL) return E_NULL_POINTER;
    if(args == NULL && args_size != 0) return E_NULL_POINTER;
    if(args != NULL && args_size == 0) return E_INVALID_VALUE;

    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t message_index = 0;
    
    if(cmd != NULL){
        spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
        if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
        
        err = spi_message_init(cmd_msg, SPI_WRITE, cmd, NULL, GC9A01A_CMD_SIZE);
        if(err != E_NO_ERROR) return err;
        spi_message_set_sender_data(cmd_msg, tft);
        spi_message_set_callback(cmd_msg, gc9a01a_cmd_message_end);
    }
    
    if(args != NULL){
        spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
        if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
        
        err = spi_message_init(data_msg, SPI_WRITE, args, NULL, args_size);
        if(err != E_NO_ERROR) return err;
    }
    
    if(message_index == 0) return E_INVALID_OPERATION;

    err = gc9a01a_transfer(tft, cmd == NULL, message_index);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

static err_t gc9a01a_send_sequence(gc9a01a_t* tft, const uint8_t* seq, size_t seq_len)
{
    size_t pos = 0;

    const uint8_t* cmd;
    uint8_t info;
    uint8_t count;
    uint8_t delay;
    const uint8_t* args;

    err_t err;

    while(pos < seq_len){
        cmd = &seq[pos ++]; // первый байт - команда.
        info = seq[pos ++]; // второй байт - информация о команде.
        // Число параметров команды - 
        // нижние 7 бит информации о команде.
        count = info & 0x7f;
        // если верхний бит информации о команде
        // установлен - то следующий байт является
        // величиной задержки в мс.
        delay = (info & 0x80) ? seq[pos ++] : 0;
        // Если число параметров не 0 -
        // то параметры начинаются с текущего байта.
        args = (count == 0) ? NULL : &seq[pos];
        // Пропустим параметры команды.
        pos += count;

        err = gc9a01a_send(tft, cmd, args, count);
        if(err != E_NO_ERROR) return err;

        if(delay != 0) sleep_ms(delay);
    }

    return E_NO_ERROR;
}

err_t gc9a01a_init(gc9a01a_t* tft, gc9a01a_init_t* tft_init)
{
    // Clear.
    memset(tft, 0x0, sizeof(gc9a01a_t));
    
    // SPI.
    tft->spi = tft_init->spi;
    
    // CE.
    tft->ce_gpio = tft_init->ce_gpio;
    tft->ce_pin = tft_init->ce_pin;
    
    // D/C.
    tft->dc_gpio = tft_init->dc_gpio;
    tft->dc_pin = tft_init->dc_pin;
    
    // RESET.
    tft->rst_gpio = tft_init->reset_gpio;
    tft->rst_pin = tft_init->reset_pin;
    
    future_init(&tft->future);
    
    // Значения по-умолчанию для этих пинов.
    gc9a01a_pins_end(tft);
    
    // Высокий уровень для пина сброса.
    gpio_set(tft->rst_gpio, tft->rst_pin);
    // Сбросим дисплей.
    //gc9a01a_reset(tft);
    
    // Идентификатор передачи.
    tft->transfer_id = tft_init->transfer_id;
    
    return E_NO_ERROR;
}

bool gc9a01a_busy(gc9a01a_t* tft)
{
    return future_running(&tft->future);
}

err_t gc9a01a_error(gc9a01a_t* tft)
{
    return pvoid_to_int(err_t, future_result(&tft->future));
}

err_t gc9a01a_wait(gc9a01a_t* tft)
{
    future_wait(&tft->future);
    return pvoid_to_int(err_t, future_result(&tft->future));
}

void gc9a01a_reset(gc9a01a_t* tft)
{
    //wait
    future_wait(&tft->future);
    
    gpio_reset(tft->rst_gpio, tft->rst_pin);
    sleep_us(GC9A01A_RESET_TIME_US);
    //sleep_ms(GC9A01A_RESET_TIME_MS);
    gpio_set(tft->rst_gpio, tft->rst_pin);
    
    sleep_ms(GC9A01A_RESET_WAIT_TIME_MS);
}

err_t gc9a01a_send_init(gc9a01a_t* tft)
{
    return gc9a01a_send_sequence(tft, _INIT_SEQUENCE, _INIT_SEQUENCE_LENGTH);
}

err_t gc9a01a_read_id(gc9a01a_t* tft, gc9a01a_id_t* id)
{
    if(id == NULL) return E_NULL_POINTER;
    
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = gc9a01a_get_buffer(tft, GC9A01A_RD_ID_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_READ_ID;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, gc9a01a_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data_buf, GC9A01A_RD_ID_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    id->id1 = data_buf[1];
    id->id2 = data_buf[2];
    id->id3 = data_buf[3];
    
    return E_NO_ERROR;
}

err_t gc9a01a_read_status(gc9a01a_t* tft, gc9a01a_status_t* status)
{
    if(status == NULL) return E_NULL_POINTER;
    
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = gc9a01a_get_buffer(tft, GC9A01A_RD_DS_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_READ_DISPLAY_STATUS;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, gc9a01a_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data_buf, GC9A01A_RD_DS_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    uint32_t data = *((uint32_t*)&data_buf[1]);
    
    status->booster_status = BIT_VALUE_MASK(data, GC9A01A_RD_DS_BOOSTER_STATUS_OFFSET, GC9A01A_RD_DS_BOOSTER_STATUS_MASK);
    status->row_address_order = BIT_VALUE_MASK(data, GC9A01A_RD_DS_ROW_ADDR_ORDER_OFFSET, GC9A01A_RD_DS_ROW_ADDR_ORDER_MASK);
    status->col_address_order = BIT_VALUE_MASK(data, GC9A01A_RD_DS_COL_ADDR_ORDER_OFFSET, GC9A01A_RD_DS_COL_ADDR_ORDER_MASK);
    status->row_col_exchange = BIT_VALUE_MASK(data, GC9A01A_RD_DS_ROW_COL_EXCH_OFFSET, GC9A01A_RD_DS_ROW_COL_EXCH_MASK);
    status->vertical_refresh = BIT_VALUE_MASK(data, GC9A01A_RD_DS_VERTICAL_REFRESH_OFFSET, GC9A01A_RD_DS_VERTICAL_REFRESH_MASK);
    status->color_order = BIT_VALUE_MASK(data, GC9A01A_RD_DS_COLOR_ORDER_OFFSET, GC9A01A_RD_DS_COLOR_ORDER_MASK);
    status->horizontal_order = BIT_VALUE_MASK(data, GC9A01A_RD_DS_HORIZONTAL_REFRESH_OFFSET, GC9A01A_RD_DS_HORIZONTAL_REFRESH_MASK);
    status->pixel_format = BIT_VALUE_MASK(data, GC9A01A_RD_DS_PIXEL_FORMAT_OFFSET, GC9A01A_RD_DS_PIXEL_FORMAT_MASK);
    status->idle_mode = BIT_VALUE_MASK(data, GC9A01A_RD_DS_IDLE_MODE_OFFSET, GC9A01A_RD_DS_IDLE_MODE_MASK);
    status->partial_mode = BIT_VALUE_MASK(data, GC9A01A_RD_DS_PARTIAL_MODE_OFFSET, GC9A01A_RD_DS_PARTIAL_MODE_MASK);
    status->sleep_mode = BIT_VALUE_MASK(data, GC9A01A_RD_DS_SLEEP_MODE_OFFSET, GC9A01A_RD_DS_SLEEP_MODE_MASK);
    status->normal_mode = BIT_VALUE_MASK(data, GC9A01A_RD_DS_NORMAL_MODE_OFFSET, GC9A01A_RD_DS_NORMAL_MODE_MASK);
    status->vertical_scrolling = BIT_VALUE_MASK(data, GC9A01A_RD_DS_VERTICAL_SCROLL_OFFSET, GC9A01A_RD_DS_VERTICAL_SCROLL_MASK);
    status->display_mode = BIT_VALUE_MASK(data, GC9A01A_RD_DS_DISPLAY_MODE_OFFSET, GC9A01A_RD_DS_DISPLAY_MODE_MASK);
    status->tearing_effect_line = BIT_VALUE_MASK(data, GC9A01A_RD_DS_TEARING_EFFECT_OFFSET, GC9A01A_RD_DS_TEARING_EFFECT_MASK);
    status->tearing_effect_mode = BIT_VALUE_MASK(data, GC9A01A_RD_DS_TEARING_MODE_OFFSET, GC9A01A_RD_DS_TEARING_MODE_MASK);
    
    return E_NO_ERROR;
}

err_t gc9a01a_sleep_in(gc9a01a_t* tft)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_SLEEP_IN;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t gc9a01a_sleep_out(gc9a01a_t* tft)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_SLEEP_OUT;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    //sleep_ms(GC9A01A_SLEEP_OUT_WAIT_TIME_MS);
    
    return E_NO_ERROR;
}

err_t gc9a01a_partial_mode(gc9a01a_t* tft)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_PARTIAL_MODE_ON;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t gc9a01a_normal_mode(gc9a01a_t* tft)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_NORMAL_MODE_ON;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t gc9a01a_inversion_off(gc9a01a_t* tft)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_INVERSION_OFF;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t gc9a01a_inversion_on(gc9a01a_t* tft)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_INVERSION_ON;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t gc9a01a_display_off(gc9a01a_t* tft)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_DISPLAY_OFF;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t gc9a01a_display_on(gc9a01a_t* tft)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_DISPLAY_ON;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t gc9a01a_set_column_address(gc9a01a_t* tft, uint16_t start, uint16_t end)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = gc9a01a_get_buffer(tft, GC9A01A_WR_CA_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_WRITE_COL_ADDRESS;
    data_buf[0] = (start >> 8) & 0xff;
    data_buf[1] = start & 0xff;
    data_buf[2] = (end >> 8) & 0xff;
    data_buf[3] = end & 0xff;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, gc9a01a_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, GC9A01A_WR_CA_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

err_t gc9a01a_set_page_address(gc9a01a_t* tft, uint16_t start, uint16_t end)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = gc9a01a_get_buffer(tft, GC9A01A_WR_PGA_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_WRITE_PAGE_ADDRESS;
    data_buf[0] = (start >> 8) & 0xff;
    data_buf[1] = start & 0xff;
    data_buf[2] = (end >> 8) & 0xff;
    data_buf[3] = end & 0xff;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, gc9a01a_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, GC9A01A_WR_PGA_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

err_t gc9a01a_write(gc9a01a_t* tft, const void* data, size_t size)
{
    if(data == NULL) return E_NULL_POINTER;
    
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    //err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_WRITE_MEMORY;
    
    spi_message_setup(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, gc9a01a_cmd_message_end);
    
    spi_message_setup(data_msg, SPI_WRITE, data, NULL, size);

    return gc9a01a_transfer(tft, false, 2);
}

err_t gc9a01a_begin_write(gc9a01a_t* tft)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    //err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_WRITE_MEMORY;
    
    spi_message_setup(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);

    return gc9a01a_transfer(tft, false, 1);
}

err_t gc9a01a_set_partial_area(gc9a01a_t* tft, uint16_t start, uint16_t end)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = gc9a01a_get_buffer(tft, GC9A01A_WR_PLA_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_WRITE_PARTIAL_AREA;
    data_buf[0] = (start >> 8) & 0xff;
    data_buf[1] = start & 0xff;
    data_buf[2] = (end >> 8) & 0xff;
    data_buf[3] = end & 0xff;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, gc9a01a_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, GC9A01A_WR_PLA_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

err_t gc9a01a_set_vertical_scrolling(gc9a01a_t* tft, uint16_t tfa, uint16_t vsa)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = gc9a01a_get_buffer(tft, GC9A01A_WR_VERT_SCROLL_DEF_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_WRITE_VERTICAL_SCROLL_DEF;
    data_buf[0] = (tfa >> 8) & 0xff;
    data_buf[1] = tfa & 0xff;
    data_buf[2] = (vsa >> 8) & 0xff;
    data_buf[3] = vsa & 0xff;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, gc9a01a_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, GC9A01A_WR_VERT_SCROLL_DEF_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

err_t gc9a01a_tearing_effect_line_off(gc9a01a_t* tft)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_TEARING_EFFECT_LINE_OFF;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t gc9a01a_tearing_effect_line_on(gc9a01a_t* tft, gc9a01a_tearing_effect_mode_t mode)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = gc9a01a_get_buffer(tft, GC9A01A_WR_TE_LINE_ON_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_TEARING_EFFECT_LINE_ON;
    
    uint8_t data = 0;
    
    data |= mode;

    *data_buf = data;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, gc9a01a_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, GC9A01A_WR_TE_LINE_ON_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t gc9a01a_set_madctl(gc9a01a_t* tft, const gc9a01a_madctl_t* madctl)
{
    if(madctl == NULL) return E_NULL_POINTER;
    
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = gc9a01a_get_buffer(tft, GC9A01A_WR_MC_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_WRITE_MADCTL;
    
    uint8_t data = 0;
    
    // Да здравствует ARM!
    // orr r3, r3, r2, lsl #N
    data |= madctl->row_address_order << GC9A01A_WR_MC_ROW_ADDR_ORDER_OFFSET;
    data |= madctl->col_address_order << GC9A01A_WR_MC_COL_ADDR_ORDER_OFFSET;
    data |= madctl->row_col_exchange << GC9A01A_WR_MC_ROW_COL_EXCH_OFFSET;
    data |= madctl->vertical_refresh << GC9A01A_WR_MC_VERTICAL_REFRESH_OFFSET;
    data |= madctl->color_order << GC9A01A_WR_MC_COLOR_ORDER_OFFSET;
    data |= madctl->horizontal_refresh << GC9A01A_WR_MC_HORIZONTAL_REFRESH_OFFSET;
    
    *data_buf = data;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, gc9a01a_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, GC9A01A_WR_MC_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t gc9a01a_set_vertical_scrolling_start_address(gc9a01a_t* tft, uint16_t vsp)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = gc9a01a_get_buffer(tft, GC9A01A_WR_VS_START_ADDR_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_WRITE_VERT_SCROLL_START_ADDR;
    data_buf[0] = (vsp >> 8) & 0xff;
    data_buf[1] = vsp & 0xff;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, gc9a01a_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, GC9A01A_WR_VS_START_ADDR_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t gc9a01a_idle_off(gc9a01a_t* tft)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_IDLE_OFF;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t gc9a01a_idle_on(gc9a01a_t* tft)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_IDLE_ON;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t gc9a01a_set_pixel_format(gc9a01a_t* tft, gc9a01a_pixel_format_t rgb_iface_format, gc9a01a_pixel_format_t mcu_iface_format)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = gc9a01a_get_buffer(tft, GC9A01A_WR_PF_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_WRITE_DISPLAY_PIXEL_FORMAT;
    
    uint8_t data = 0;
    
    data |= rgb_iface_format << GC9A01A_WR_PF_DPI_OFFSET;
    data |= mcu_iface_format << GC9A01A_WR_PF_DBI_OFFSET;
    
    *data_buf = data;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, gc9a01a_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, GC9A01A_WR_PF_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

err_t gc9a01a_set_tear_scanline(gc9a01a_t* tft, uint16_t sts)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = gc9a01a_get_buffer(tft, GC9A01A_SET_TEAR_SL_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_SET_TEAR_SCANLINE;
    data_buf[0] = (sts >> 8) & 0xff;
    data_buf[1] = sts & 0xff;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, gc9a01a_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, GC9A01A_SET_TEAR_SL_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t gc9a01a_get_scanline(gc9a01a_t* tft, uint16_t* gts)
{
    if(gts == NULL) return E_NULL_POINTER;
    
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = gc9a01a_get_buffer(tft, GC9A01A_GET_SL_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_GET_SCANLINE;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, gc9a01a_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data_buf, GC9A01A_GET_SL_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    *gts = (((uint16_t)data_buf[1] << 8) | (data_buf[2])) & 0x1ff;
    
    return E_NO_ERROR;
}

err_t gc9a01a_write_continue(gc9a01a_t* tft, const void* data, size_t size)
{
    if(data == NULL) return E_NULL_POINTER;
    
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    //err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_WRITE_MEMORY_CONTINUE;
    
    spi_message_setup(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, gc9a01a_cmd_message_end);
    
    spi_message_setup(data_msg, SPI_WRITE, data, NULL, size);
    
    return gc9a01a_transfer(tft, false, 2);
}

err_t gc9a01a_begin_write_continue(gc9a01a_t* tft)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    //err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_WRITE_MEMORY_CONTINUE;
    
    spi_message_setup(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);

    return gc9a01a_transfer(tft, false, 1);
}

err_t gc9a01a_set_brightness(gc9a01a_t* tft, uint8_t brightness)
{
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = gc9a01a_get_buffer(tft, GC9A01A_WR_BN_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_WRITE_BRIGHTNESS;
    *data_buf = brightness;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, gc9a01a_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, GC9A01A_WR_BN_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

err_t gc9a01a_set_display_control(gc9a01a_t* tft, const gc9a01a_display_control_t* control)
{
    if(control == NULL) return E_NULL_POINTER;
    
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = gc9a01a_get_buffer(tft, GC9A01A_WR_DC_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_WRITE_DISPLAY_CONTROL;
    
    uint8_t data = 0;
    
    // Да здравствует ARM!
    // orr r3, r3, r2, lsl #N
    data |= control->brightness_control << GC9A01A_WR_DC_BRIGHTNESS_CONTROL_OFFSET;
    data |= control->display_dimming << GC9A01A_WR_DC_DISPLAY_DIMMING_OFFSET;
    data |= control->backlight_mode << GC9A01A_WR_DC_BACKLIGHT_OFFSET;
    
    *data_buf = data;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, gc9a01a_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, GC9A01A_WR_DC_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t gc9a01a_read_id1(gc9a01a_t* tft, uint8_t* id)
{
    if(id == NULL) return E_NULL_POINTER;
    
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = gc9a01a_get_buffer(tft, GC9A01A_RD_ID1_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_READ_ID1;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, gc9a01a_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data_buf, GC9A01A_RD_ID1_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    *id = data_buf[1];

    return E_NO_ERROR;
}

err_t gc9a01a_read_id2(gc9a01a_t* tft, uint8_t* id)
{
    if(id == NULL) return E_NULL_POINTER;
    
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = gc9a01a_get_buffer(tft, GC9A01A_RD_ID2_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_READ_ID2;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, gc9a01a_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data_buf, GC9A01A_RD_ID2_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    *id = data_buf[1];

    return E_NO_ERROR;
}

err_t gc9a01a_read_id3(gc9a01a_t* tft, uint8_t* id)
{
    if(id == NULL) return E_NULL_POINTER;
    
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = gc9a01a_get_buffer(tft, GC9A01A_RD_ID3_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = GC9A01A_CMD_READ_ID3;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, GC9A01A_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, gc9a01a_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data_buf, GC9A01A_RD_ID3_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = gc9a01a_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    *id = data_buf[1];

    return E_NO_ERROR;
}

err_t gc9a01a_data(gc9a01a_t* tft, const void* data, size_t size)
{
    if(data == NULL) return E_NULL_POINTER;
    
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    //err_t err = E_NO_ERROR;
    
    size_t message_index = 0;

    spi_message_t* data_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_setup(data_msg, SPI_WRITE, data, NULL, size);
    
    return gc9a01a_transfer(tft, true, 1);
}

err_t gc9a01a_set_pixel(gc9a01a_t* tft, uint16_t x, uint16_t y, const void* pixel, size_t size)
{
    if(pixel == NULL) return E_NULL_POINTER;
    if(size < GC9A01A_PIXEL_SIZE_MIN || size > GC9A01A_PIXEL_SIZE_MAX) return E_INVALID_VALUE;
    
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    //err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    //
    // Начальный и конечный столбец.
    //
    uint8_t* cmd_col_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_col_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_col_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_col_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_col_buf = gc9a01a_get_buffer(tft, GC9A01A_WR_CA_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_col_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_col_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_col_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    //
    // Начальная и конечная страница.
    //
    uint8_t* cmd_page_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_page_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_page_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_page_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_page_buf = gc9a01a_get_buffer(tft, GC9A01A_WR_PGA_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_page_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_page_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_page_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    //
    // Данные.
    //
    uint8_t* cmd_pixel_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_pixel_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_pixel_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_pixel_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_pixel_buf = gc9a01a_get_buffer(tft, size, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_pixel_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_pixel_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_pixel_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_col_buf = GC9A01A_CMD_WRITE_COL_ADDRESS;
    data_col_buf[0] = (x >> 8) & 0xff;
    data_col_buf[1] = x & 0xff;
    data_col_buf[2] = data_col_buf[0];
    data_col_buf[3] = data_col_buf[1];
    
    *cmd_page_buf = GC9A01A_CMD_WRITE_PAGE_ADDRESS;
    data_page_buf[0] = (y >> 8) & 0xff;
    data_page_buf[1] = y & 0xff;
    data_page_buf[2] = data_page_buf[0];
    data_page_buf[3] = data_page_buf[1];
    
    *cmd_pixel_buf = GC9A01A_CMD_WRITE_MEMORY;
    data_pixel_buf[0] = ((uint8_t*)pixel)[0];
    data_pixel_buf[1] = ((uint8_t*)pixel)[1];
    if(size == GC9A01A_PIXEL_SIZE_MAX){
        data_pixel_buf[2] = ((uint8_t*)pixel)[2];
    }
    
    spi_message_setup(cmd_col_msg, SPI_WRITE, cmd_col_buf, NULL, GC9A01A_CMD_SIZE);
    spi_message_set_sender_data(cmd_col_msg, tft);
    spi_message_set_callback(cmd_col_msg, gc9a01a_cmd_message_end);
    
    spi_message_setup(data_col_msg, SPI_WRITE, data_col_buf, NULL, GC9A01A_WR_CA_DATA_SIZE);
    spi_message_set_sender_data(data_col_msg, tft);
    spi_message_set_callback(data_col_msg, gc9a01a_cmd_message_start);
    
    spi_message_setup(cmd_page_msg, SPI_WRITE, cmd_page_buf, NULL, GC9A01A_CMD_SIZE);
    spi_message_set_sender_data(cmd_page_msg, tft);
    spi_message_set_callback(cmd_page_msg, gc9a01a_cmd_message_end);
    
    spi_message_setup(data_page_msg, SPI_WRITE, data_page_buf, NULL, GC9A01A_WR_PGA_DATA_SIZE);
    spi_message_set_sender_data(data_page_msg, tft);
    spi_message_set_callback(data_page_msg, gc9a01a_cmd_message_start);
    
    spi_message_setup(cmd_pixel_msg, SPI_WRITE, cmd_pixel_buf, NULL, GC9A01A_CMD_SIZE);
    spi_message_set_sender_data(cmd_pixel_msg, tft);
    spi_message_set_callback(cmd_pixel_msg, gc9a01a_cmd_message_end);
    
    spi_message_setup(data_pixel_msg, SPI_WRITE, data_pixel_buf, NULL, size);
    
    return gc9a01a_transfer(tft, false, 6);
}

err_t gc9a01a_write_region(gc9a01a_t* tft, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const void* data, size_t size)
{
    if(data == NULL) return E_NULL_POINTER;
    if(size < GC9A01A_PIXEL_SIZE_MIN) return E_INVALID_VALUE;
    
    if(!gc9a01a_wait_current_op(tft)) return E_BUSY;
    
    //err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    //
    // Начальный и конечный столбец.
    //
    uint8_t* cmd_col_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_col_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_col_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_col_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_col_buf = gc9a01a_get_buffer(tft, GC9A01A_WR_CA_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_col_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_col_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_col_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    //
    // Начальная и конечная страница.
    //
    uint8_t* cmd_page_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_page_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_page_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_page_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_page_buf = gc9a01a_get_buffer(tft, GC9A01A_WR_PGA_DATA_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_page_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_page_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_page_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    //
    // Данные.
    //
    uint8_t* cmd_pixel_buf = gc9a01a_get_buffer(tft, GC9A01A_CMD_SIZE, &buffer_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_pixel_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_pixel_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(cmd_pixel_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_pixel_msg = gc9a01a_get_message(tft, &message_index);
#ifdef GC9A01A_GET_MEM_DEBUG
    if(data_pixel_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_col_buf = GC9A01A_CMD_WRITE_COL_ADDRESS;
    data_col_buf[0] = (x0 >> 8) & 0xff;
    data_col_buf[1] = x0 & 0xff;
    data_col_buf[2] = (x1 >> 8) & 0xff;
    data_col_buf[3] = x1 & 0xff;
    
    *cmd_page_buf = GC9A01A_CMD_WRITE_PAGE_ADDRESS;
    data_page_buf[0] = (y0 >> 8) & 0xff;
    data_page_buf[1] = y0 & 0xff;
    data_page_buf[2] = (y1 >> 8) & 0xff;
    data_page_buf[3] = y1 & 0xff;
    
    *cmd_pixel_buf = GC9A01A_CMD_WRITE_MEMORY;
    
    spi_message_setup(cmd_col_msg, SPI_WRITE, cmd_col_buf, NULL, GC9A01A_CMD_SIZE);
    spi_message_set_sender_data(cmd_col_msg, tft);
    spi_message_set_callback(cmd_col_msg, gc9a01a_cmd_message_end);
    
    spi_message_setup(data_col_msg, SPI_WRITE, data_col_buf, NULL, GC9A01A_WR_CA_DATA_SIZE);
    spi_message_set_sender_data(data_col_msg, tft);
    spi_message_set_callback(data_col_msg, gc9a01a_cmd_message_start);
    
    spi_message_setup(cmd_page_msg, SPI_WRITE, cmd_page_buf, NULL, GC9A01A_CMD_SIZE);
    spi_message_set_sender_data(cmd_page_msg, tft);
    spi_message_set_callback(cmd_page_msg, gc9a01a_cmd_message_end);
    
    spi_message_setup(data_page_msg, SPI_WRITE, data_page_buf, NULL, GC9A01A_WR_PGA_DATA_SIZE);
    spi_message_set_sender_data(data_page_msg, tft);
    spi_message_set_callback(data_page_msg, gc9a01a_cmd_message_start);
    
    spi_message_setup(cmd_pixel_msg, SPI_WRITE, cmd_pixel_buf, NULL, GC9A01A_CMD_SIZE);
    spi_message_set_sender_data(cmd_pixel_msg, tft);
    spi_message_set_callback(cmd_pixel_msg, gc9a01a_cmd_message_end);
    
    spi_message_setup(data_pixel_msg, SPI_WRITE, data, NULL, size);
    
    return gc9a01a_transfer(tft, false, 6);
}
