//====================================================================================================
#ifndef USB_H
#define USB_H
//====================================================================================================
#include <stm32f4xx.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
//====================================================================================================
// Управление самим USB-устройством (базовые регистры устройства)
#define USB_DEVICE           ((USB_OTG_DeviceTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_DEVICE_BASE))

// Для OUT конечных точек (для приема данных от ПК)
#define USB_GET_OUT_EP(epnum)  ((USB_OTG_OUTEndpointTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_OUT_ENDPOINT_BASE + ((epnum) * 0x20)))
#define EP0_OUT  ((USB_OTG_OUTEndpointTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_OUT_ENDPOINT_BASE + (0 * 0x20)))
#define EP1_OUT  ((USB_OTG_OUTEndpointTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_OUT_ENDPOINT_BASE + (1 * 0x20)))
#define EP2_OUT  ((USB_OTG_OUTEndpointTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_OUT_ENDPOINT_BASE + (2 * 0x20)))
#define EP3_OUT  ((USB_OTG_OUTEndpointTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_OUT_ENDPOINT_BASE + (3 * 0x20)))

// Для IN конечных точек (для отправки данных на ПК)
#define USB_GET_IN_EP(epnum)   ((USB_OTG_INEndpointTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_IN_ENDPOINT_BASE + ((epnum) * 0x20)))
#define EP0_IN    ((USB_OTG_INEndpointTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_IN_ENDPOINT_BASE + (0 * 0x20)))
#define EP1_IN    ((USB_OTG_INEndpointTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_IN_ENDPOINT_BASE + (1 * 0x20)))
#define EP2_IN    ((USB_OTG_INEndpointTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_IN_ENDPOINT_BASE + (2 * 0x20)))
#define EP3_IN    ((USB_OTG_INEndpointTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_IN_ENDPOINT_BASE + (3 * 0x20)))

// Для доступа к FIFO конкретной конечной точки (0, 1, 2...)
#define USB_GET_FIFO(epnum)  ((volatile uint32_t *)((uint32_t)USB_OTG_FS + USB_OTG_FIFO_BASE + ((epnum) * 0x1000)))
#define USB_FIFO0            USB_GET_FIFO(0)
//====================================================================================================
#define LOBYTE(x)  ((uint8_t)((x) & 0x00FFU))
#define HIBYTE(x)  ((uint8_t)((((x)) & 0xFF00U) >> 8U))
//====================================================================================================
/* Настройки идентификаторов USB */
#define USB_VID               0x1209
#define USB_PID               0x0002
#define USB_LANGID_STRING     0x0409
//====================================================================================================
/* Параметры конечных точек (Endpoints) */
// Конечная точка 0 
#define EP0_MAX_PACKET_SIZE     64

// Конечная точка 1 
#define EP1_IN_ADDR             0x81
#define EP1_OUT_ADDR            0x01
#define EP1_MAX_PACKET_SIZE     64   

// Конечная точка 2 
#define EP2_IN_ADDR             0x82
#define EP2_OUT_ADDR            0x02
#define EP2_MAX_PACKET_SIZE     64    

// Конечная точка 3 
#define EP3_IN_ADDR             0x83 // Направление IN (в компьютер)
#define EP3_OUT_ADDR            0x03 // Направление OUT (из компьютера)
#define EP3_MAX_PACKET_SIZE     64   // Строго 64 для Full-Speed Bulk

#define CUSTOM_HID_FS_BINTERVAL 0x05 // Интервал опроса шины Full Speed
//====================================================================================================
typedef struct {
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} __attribute__((packed)) USB_SetupPacket_TypeDef;

// Cтруктуры MSC
typedef struct {
    uint32_t dCBWSignature;          // Сигнатура 'USBC' (0x43425355)
    uint32_t dCBWTag;                
    uint32_t dCBWDataTransferLength; 
    uint8_t  bmCBWFlags;             
    uint8_t  bCBWLUN;                
    uint8_t  bCBWCBLength;           
    uint8_t  CBWCB[16];              // SCSI команда
} __attribute__((packed)) MSC_CBW_TypeDef;

typedef struct {
    uint32_t dCSWSignature;          // Сигнатура 'USBS' (0x53425355)
    uint32_t dCSWTag;                
    uint32_t dCSWDataResidue;        
    uint8_t  bCSWStatus;             
} __attribute__((packed)) MSC_CSW_TypeDef;
//====================================================================================================
#define __ALIGN4 __attribute__((aligned(4)))
//====================================================================================================
// Маски модификаторов (USB HID Modifier Bits)
#define KEY_MOD_LCTRL      0x01
#define KEY_MOD_LSHIFT     0x02
#define KEY_MOD_LALT       0x04
#define KEY_MOD_LGUI       0x08  // Клавиша Windows / Command (левая)
#define KEY_MOD_RCTRL      0x10
#define KEY_MOD_RSHIFT     0x20
#define KEY_MOD_RALT       0x40  // AltGr
#define KEY_MOD_RGUI       0x80  // Клавиша Windows / Command (правая)

// Клавиши модификаторов
#define KEY_LCTRL   0xE0
#define KEY_LSHIFT  0xE1
#define KEY_LALT    0xE2
#define KEY_LGUI    0xE3
#define KEY_RCTRL   0xE4
#define KEY_RSHIFT  0xE5
#define KEY_RALT    0xE6
#define KEY_RGUI    0xE7
//====================================================================================================
// Специальные и системные клавиши
#define KEY_NONE           0x00  // Нет нажатой клавиши
#define KEY_ENTER          0x28  // Enter
#define KEY_ESCAPE         0x29  // Escape
#define KEY_BACKSPACE      0x2A  // Backspace
#define KEY_TAB            0x2B  // Tab
#define KEY_SPACE          0x2C  // Пробел
#define KEY_CAPSLOCK       0x39  // Caps Lock

// Буквенные клавиши (Алфавит A-Z)
#define KEY_A              0x04
#define KEY_B              0x05
#define KEY_C              0x06
#define KEY_D              0x07
#define KEY_E              0x08
#define KEY_F              0x09
#define KEY_G              0x0A
#define KEY_H              0x0B
#define KEY_I              0x0C
#define KEY_J              0x0D
#define KEY_K              0x0E
#define KEY_L              0x0F
#define KEY_M              0x10
#define KEY_N              0x11
#define KEY_O              0x12
#define KEY_P              0x13
#define KEY_Q              0x14
#define KEY_R              0x15
#define KEY_S              0x16
#define KEY_T              0x17
#define KEY_U              0x18
#define KEY_V              0x19
#define KEY_W              0x1A
#define KEY_X              0x1B
#define KEY_Y              0x1C
#define KEY_Z              0x1D

// Стрелки управления курсором
#define KEY_RIGHT          0x4F  // Стрелка Вправо
#define KEY_LEFT           0x50  // Стрелка Влево
#define KEY_DOWN           0x51  // Стрелка Вниз
#define KEY_UP             0x52  // Стрелка Вверх

#define KEY_PRINTSCREEN    0x46  // Клавиша Print Screen / SysRq
#define KEY_SCROLLLOCK     0x47  // Клавиша Scroll Lock
#define KEY_PAUSE          0x48  // Клавиша Pause / Break
#define KEY_INSERT         0x49  // Клавиша Insert
#define KEY_HOME           0x4A  // Клавиша Home
#define KEY_PAGEUP         0x4B  // Клавиша Page Up
#define KEY_DELETE         0x4C  // Клавиша Delete
#define KEY_END            0x4D  // Клавиша End
#define KEY_PAGEDOWN       0x4E  // Клавиша Page Down

#define KEY_1              0x1E  // 1 и !
#define KEY_2              0x1F  // 2 и @
#define KEY_3              0x20  // 3 и #
#define KEY_4              0x21  // 4 и $
#define KEY_5              0x22  // 5 и %
#define KEY_6              0x23  // 6 и ^
#define KEY_7              0x24  // 7 и &
#define KEY_8              0x25  // 8 и *
#define KEY_9              0x26  // 9 и (
#define KEY_0              0x27  // 0 и )

#define KEY_MINUS          0x2D  // Минус и Подчеркивание (- и _)
#define KEY_EQUAL          0x2E  // Равно и Плюс (= и +)
#define KEY_LEFTBRACE      0x2F  // Квадратная скобка [ (Х в рус. раскладке)
#define KEY_RIGHTBRACE     0x30  // Квадратная скобка ] (Ъ в рус. раскладке)
#define KEY_BACKSLASH      0x31  // Обратный слэш \ .
#define KEY_SEMICOLON      0x33  // Точка с запятой ; (Ж в рус. раскладке)
#define KEY_APOSTROPHE     0x34  // Апостроф ' (Э в рус. раскладке)
#define KEY_GRAVE          0x35  // Тильда / Ё `
#define KEY_COMMA          0x36  // Запятая , (Б в рус. раскладке)
#define KEY_DOT            0x37  // Точка . (Ю в рус. раскладке)
#define KEY_SLASH          0x38  // Слэш /

#define KEY_F1             0x3A
#define KEY_F2             0x3B
#define KEY_F3             0x3C
#define KEY_F4             0x3D
#define KEY_F5             0x3E
#define KEY_F6             0x3F
#define KEY_F7             0x40
#define KEY_F8             0x41
#define KEY_F9             0x42
#define KEY_F10            0x43
#define KEY_F11            0x44
#define KEY_F12            0x45

#define KEY_NUMLOCK        0x53  // Num Lock
#define KEY_KPSLASH        0x54  // Numpad /
#define KEY_KPASTERISK     0x55  // Numpad *
#define KEY_KPMINUS        0x56  // Numpad -
#define KEY_KPPLUS         0x57  // Numpad +
#define KEY_KPENTER        0x58  // Numpad Enter
#define KEY_KP1            0x59  // Numpad 1 (End)
#define KEY_KP2            0x5A  // Numpad 2 (Стрелка вниз)
#define KEY_KP3            0x5B  // Numpad 3 (Page Down)
#define KEY_KP4            0x5C  // Numpad 4 (Стрелка влево)
#define KEY_KP5            0x5D  // Numpad 5
#define KEY_KP6            0x5E  // Numpad 6 (Стрелка вправо)
#define KEY_KP7            0x5F  // Numpad 7 (Home)
#define KEY_KP8            0x60  // Numpad 8 (Стрелка вверх)
#define KEY_KP9            0x61  // Numpad 9 (Page Up)
#define KEY_KP0            0x62  // Numpad 0 (Insert)
#define KEY_KPDOT          0x63  // Numpad Точка (Delete)
#define KEY_KPEQUAL        0x67  // Numpad = (встречается на Mac)

#define KEY_CONTEXT_MENU   0x65  // Клавиша контекстного меню 
#define KEY_F13            0x68  // Расширенные F-клавиши 
#define KEY_F14            0x69  // 
#define KEY_F15            0x6A  // 
#define KEY_F24            0x73  // 

#define MEDIA_LAUNCH_CALC        0x0192  // Калькулятор (AL Calculator)
#define MEDIA_LAUNCH_EMAIL       0x018A  // Почтовый клиент (AL Email Reader)
#define MEDIA_LAUNCH_BROWSER     0x0194  // Браузер / Домашняя страница (AL Internet Browser)
#define MEDIA_LAUNCH_MY_COMP     0x0194  // "Мой компьютер" / Проводник (AL Local Machine Browser)
#define MEDIA_LAUNCH_MEDIA_PLAY  0x0183  // Медиа-плеер (AL Consumer Control Configuration)

#define MEDIA_AC_HOME            0x0223  // Домой (в браузере)
#define MEDIA_AC_BACK            0x0224  // Назад (History Back)
#define MEDIA_AC_FORWARD         0x0225  // Вперед (History Forward)
#define MEDIA_AC_REFRESH         0x0221  // Обновить страницу (F5)
#define MEDIA_AC_BOOKMARKS       0x022A  // Избранное / Закладки
#define MEDIA_AC_SEARCH          0x0221  // Системный поиск / Поиск на странице

#define MEDIA_SYSTEM_SLEEP       0x0082  // Перевести ПК в спящий режим
#define MEDIA_SYSTEM_POWER       0x0081  // Выключение ПК (Power Down)
#define MEDIA_SYSTEM_WAKE        0x0083  // Пробуждение ПК (Wake Up)

#define MEDIA_NEXT_TRACK     0x01  // Бит 0 (Usage 0xB5)
#define MEDIA_PREV_TRACK     0x02  // Бит 1 (Usage 0xB6)
#define MEDIA_STOP           0x04  // Бит 2 (Usage 0xB7)
#define MEDIA_PLAY_PAUSE     0x08  // Бит 3 (Usage 0xCD)
#define MEDIA_MUTE           0x10  // Бит 4 (Usage 0xE2)

// Байт данных 2: Битовая маска для громкости и браузера
#define MEDIA_VOL_UP         0x01  // Бит 0 (Usage 0xE9)
#define MEDIA_VOL_DOWN       0x02  // Бит 1 (Usage 0xEA)
#define MEDIA_BROWSER_HOME   0x04 
//====================================================================================================

#define STORAGE_SECTOR_NBR   8192   // 8192 сектора по 512 байт = 4 МБ виртуального диска
#define STORAGE_SECTOR_SIZE   512

void MSC_Send_CSW(uint8_t status);


extern const uint8_t USB_DeviceDescriptor[];
extern const uint8_t USB_DeviceQualifierDescriptor[];
extern const uint8_t USB_ConfigDescriptor[];
extern const uint8_t USB_StringLangID[];
extern const uint8_t USB_StringVendor[];
extern const uint8_t USB_StringProduct[];
extern const uint8_t USB_StringSerial[];
extern const uint8_t HID_MouseReportDescriptor[];
extern const uint8_t HID_KeyboardReportDescriptor[];

void USB_Core_Init(void);
void USB_EP_Tx(uint8_t epnum, const uint8_t *pdata, uint32_t len);
void USB_Control_Handle(USB_SetupPacket_TypeDef *setup);
uint8_t USB_EP_IsReady(uint8_t epnum);
uint32_t USB_VCP_Byte_Available(void);
uint8_t USB_VCP_Read(void);
void USB_VCP_Write_String(const char *str);
//====================================================================================================
#endif
//====================================================================================================