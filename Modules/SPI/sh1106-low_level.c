#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/spi/spi.h>

#include "sh1106.h"

static uint8_t SH1106_Line   = 0;
static uint8_t SH1106_Cursor = 0;

// Array for letters
static const unsigned char sh1106_ascii[][SH1106_DEF_FONT_SIZE] =
{
        { 0x00, 0x00, 0x00, 0x00, 0x00 }, // 20   (space)
        { 0x00, 0x00, 0x5f, 0x00, 0x00 }, // 21 !
        { 0x00, 0x07, 0x00, 0x07, 0x00 }, // 22 "
        { 0x14, 0x7f, 0x14, 0x7f, 0x14 }, // 23 #
        { 0x24, 0x2a, 0x7f, 0x2a, 0x12 }, // 24 $
        { 0x23, 0x13, 0x08, 0x64, 0x62 }, // 25 %
        { 0x36, 0x49, 0x55, 0x22, 0x50 }, // 26 &
        { 0x00, 0x05, 0x03, 0x00, 0x00 }, // 27 '
        { 0x00, 0x1c, 0x22, 0x41, 0x00 }, // 28 (
        { 0x00, 0x41, 0x22, 0x1c, 0x00 }, // 29 )
        { 0x14, 0x08, 0x3e, 0x08, 0x14 }, // 2a *
        { 0x08, 0x08, 0x3e, 0x08, 0x08 }, // 2b +
        { 0x00, 0x50, 0x30, 0x00, 0x00 }, // 2c ,
        { 0x08, 0x08, 0x08, 0x08, 0x08 }, // 2d -
        { 0x00, 0x60, 0x60, 0x00, 0x00 }, // 2e .
        { 0x20, 0x10, 0x08, 0x04, 0x02 }, // 2f /
        { 0x3e, 0x51, 0x49, 0x45, 0x3e }, // 30 0
        { 0x00, 0x42, 0x7f, 0x40, 0x00 }, // 31 1
        { 0x42, 0x61, 0x51, 0x49, 0x46 }, // 32 2
        { 0x21, 0x41, 0x45, 0x4b, 0x31 }, // 33 3
        { 0x18, 0x14, 0x12, 0x7f, 0x10 }, // 34 4
        { 0x27, 0x45, 0x45, 0x45, 0x39 }, // 35 5
        { 0x3c, 0x4a, 0x49, 0x49, 0x30 }, // 36 6
        { 0x01, 0x71, 0x09, 0x05, 0x03 }, // 37 7
        { 0x36, 0x49, 0x49, 0x49, 0x36 }, // 38 8
        { 0x06, 0x49, 0x49, 0x29, 0x1e }, // 39 9
        { 0x00, 0x36, 0x36, 0x00, 0x00 }, // 3a :
        { 0x00, 0x56, 0x36, 0x00, 0x00 }, // 3b ;
        { 0x08, 0x14, 0x22, 0x41, 0x00 }, // 3c <
        { 0x14, 0x14, 0x14, 0x14, 0x14 }, // 3d =
        { 0x00, 0x41, 0x22, 0x14, 0x08 }, // 3e >
        { 0x02, 0x01, 0x51, 0x09, 0x06 }, // 3f ?
        { 0x32, 0x49, 0x79, 0x41, 0x3e }, // 40 @
        { 0x7e, 0x11, 0x11, 0x11, 0x7e }, // 41 A
        { 0x7f, 0x49, 0x49, 0x49, 0x36 }, // 42 B
        { 0x3e, 0x41, 0x41, 0x41, 0x22 }, // 43 C
        { 0x7f, 0x41, 0x41, 0x22, 0x1c }, // 44 D
        { 0x7f, 0x49, 0x49, 0x49, 0x41 }, // 45 E
        { 0x7f, 0x09, 0x09, 0x09, 0x01 }, // 46 F
        { 0x3e, 0x41, 0x49, 0x49, 0x7a }, // 47 G
        { 0x7f, 0x08, 0x08, 0x08, 0x7f }, // 48 H
        { 0x00, 0x41, 0x7f, 0x41, 0x00 }, // 49 I
        { 0x20, 0x40, 0x41, 0x3f, 0x01 }, // 4a J
        { 0x7f, 0x08, 0x14, 0x22, 0x41 }, // 4b K
        { 0x7f, 0x40, 0x40, 0x40, 0x40 }, // 4c L
        { 0x7f, 0x02, 0x0c, 0x02, 0x7f }, // 4d M
        { 0x7f, 0x04, 0x08, 0x10, 0x7f }, // 4e N
        { 0x3e, 0x41, 0x41, 0x41, 0x3e }, // 4f O
        { 0x7f, 0x09, 0x09, 0x09, 0x06 }, // 50 P
        { 0x3e, 0x41, 0x51, 0x21, 0x5e }, // 51 Q
        { 0x7f, 0x09, 0x19, 0x29, 0x46 }, // 52 R
        { 0x46, 0x49, 0x49, 0x49, 0x31 }, // 53 S
        { 0x01, 0x01, 0x7f, 0x01, 0x01 }, // 54 T
        { 0x3f, 0x40, 0x40, 0x40, 0x3f }, // 55 U
        { 0x1f, 0x20, 0x40, 0x20, 0x1f }, // 56 V
        { 0x3f, 0x40, 0x38, 0x40, 0x3f }, // 57 W
        { 0x63, 0x14, 0x08, 0x14, 0x63 }, // 58 X
        { 0x07, 0x08, 0x70, 0x08, 0x07 }, // 59 Y
        { 0x61, 0x51, 0x49, 0x45, 0x43 }, // 5a Z
        { 0x00, 0x7f, 0x41, 0x41, 0x00 }, // 5b [
        { 0x02, 0x04, 0x08, 0x10, 0x20 }, // 5c backslash
        { 0x00, 0x41, 0x41, 0x7f, 0x00 }, // 5d ]
        { 0x04, 0x02, 0x01, 0x02, 0x04 }, // 5e ^
        { 0x40, 0x40, 0x40, 0x40, 0x40 }, // 5f _
        { 0x00, 0x01, 0x02, 0x04, 0x00 }, // 60 `
        { 0x20, 0x54, 0x54, 0x54, 0x78 }, // 61 a
        { 0x7f, 0x48, 0x44, 0x44, 0x38 }, // 62 b
        { 0x38, 0x44, 0x44, 0x44, 0x20 }, // 63 c
        { 0x38, 0x44, 0x44, 0x48, 0x7f }, // 64 d
        { 0x38, 0x54, 0x54, 0x54, 0x18 }, // 65 e
        { 0x08, 0x7e, 0x09, 0x01, 0x02 }, // 66 f
        { 0x0c, 0x52, 0x52, 0x52, 0x3e }, // 67 g
        { 0x7f, 0x08, 0x04, 0x04, 0x78 }, // 68 h
        { 0x00, 0x44, 0x7d, 0x40, 0x00 }, // 69 i
        { 0x20, 0x40, 0x44, 0x3d, 0x00 }, // 6a j 
        { 0x7f, 0x10, 0x28, 0x44, 0x00 }, // 6b k
        { 0x00, 0x41, 0x7f, 0x40, 0x00 }, // 6c l
        { 0x7c, 0x04, 0x18, 0x04, 0x78 }, // 6d m
        { 0x7c, 0x08, 0x04, 0x04, 0x78 }, // 6e n
        { 0x38, 0x44, 0x44, 0x44, 0x38 }, // 6f o
        { 0x7c, 0x14, 0x14, 0x14, 0x08 }, // 70 p
        { 0x08, 0x14, 0x14, 0x18, 0x7c }, // 71 q
        { 0x7c, 0x08, 0x04, 0x04, 0x08 }, // 72 r
        { 0x48, 0x54, 0x54, 0x54, 0x20 }, // 73 s
        { 0x04, 0x3f, 0x44, 0x40, 0x20 }, // 74 t
        { 0x3c, 0x40, 0x40, 0x20, 0x7c }, // 75 u
        { 0x1c, 0x20, 0x40, 0x20, 0x1c }, // 76 v
        { 0x3c, 0x40, 0x30, 0x40, 0x3c }, // 77 w
        { 0x44, 0x28, 0x10, 0x28, 0x44 }, // 78 x
        { 0x0c, 0x50, 0x50, 0x50, 0x3c }, // 79 y
        { 0x44, 0x64, 0x54, 0x4c, 0x44 }, // 7a z
        { 0x00, 0x08, 0x36, 0x41, 0x00 }, // 7b {
        { 0x00, 0x00, 0x7f, 0x00, 0x00 }, // 7c |
        { 0x00, 0x41, 0x36, 0x08, 0x00 }, // 7d }
        { 0x10, 0x08, 0x08, 0x10, 0x08 }, // 7e ~
        { 0x78, 0x46, 0x41, 0x46, 0x78 }  // 7f DEL
};

static int SH1106_ResetDCInit(void)
{
        if (gpio_is_valid(SH1106_RES) == false)
        {
                printk(KERN_ERR "Reset GPIO %d is not valid\n", SH1106_RES);
                return -1;
        }

        if (gpio_request(SH1106_RES, "SH1106_RES") < 0)
        {
                printk(KERN_ERR "ERROR: Reset GPIO %d Request\n", SH1106_RES);
                return -1;
        }

        // Configure Reset
        gpio_direction_output(SH1106_RES, 1);

        if (gpio_is_valid(SH1106_DC) == false)
        {
                printk(KERN_ERR "DC GPIO %d is not valid\n", SH1106_DC);
                gpio_free(SH1106_RES);
                return -1;
        }
        
        if (gpio_request(SH1106_DC, "SH1106_DC") < 0)
        {
                printk(KERN_ERR "ERROR: DC GPIO %d Request\n", SH1106_DC);
                gpio_free(SH1106_DC);
                return -1;
        }

        // Configure DC
        gpio_direction_output(SH1106_DC, 1);

        return 0;
}

static void SH1106_ResetDCDeinit(void)
{
        gpio_free(SH1106_RES);
        gpio_free(SH1106_DC);
}

int SH1106_Write(bool cmd, uint8_t data)
{
        if (cmd)
        {
                gpio_set_value(SH1106_DC, 0);
        }
        else
        {
                gpio_set_value(SH1106_DC, 1);
        }

        return SH1106_SPI_Write(data);
}

void SH1106_SetCursor(uint8_t line, uint8_t cursor)
{
        if ((line <= SH1106_MAX_LINE) && (cursor < SH1106_MAX_SEG))
        {
                SH1106_Line   = line;           // Save specified line number
                SH1106_Cursor = cursor;         // Save specified cursor position

                // Send Command for Column Start and End Address
                SH1106_Write(true, 0x21);

                // Set Column Start Address
                SH1106_Write(true, cursor);

                // Set Column End Address
                SH1106_Write(true, SH1106_MAX_SEG - 1);

                // Send Command for Page Start and End Address
                SH1106_Write(true, 0x22);

                // Set Page Start Address
                SH1106_Write(true, line);

                // Set Page End Address
                SH1106_Write(true, SH1106_MAX_LINE); 
        }
}

void SH1106_NextLine(void)
{
        SH1106_Line++;
        SH1106_Line &= SH1106_MAX_LINE;

        SH1106_SetCursor(SH1106_Line, 0);
}

void SH1106_PrintChar(unsigned char c)
{
        if (((SH1106_Cursor + SH1106_DEF_FONT_SIZE) >= SH1106_MAX_SEG) || (c == '\n'))
        {
                SH1106_NextLine();
        }
        else
        {
                for (uint8_t i = 0; i < SH1106_DEF_FONT_SIZE; i++)
                {
                        uint8_t data = sh1106_ascii[c - 0x20][i];
                        SH1106_Write(false, data);
                        SH1106_Cursor++;
                }

                // Display
                SH1106_Write(false, 0x00);
                SH1106_Cursor++;
        }
}

void SH1106_String(char* str)
{
        while (*str)
        {
                SH1106_PrintChar(*str++);
        }
}

void SH1106_InvertDisplay(bool invert)
{
        if (invert)
        {
                SH1106_Write(true, 0xA7);
        }
        else
        {
                SH1106_Write(true, 0xA6);
        }
}

void SH1106_SetBrightness(uint8_t brightness)
{
        SH1106_Write(true, 0x81);
        SH1106_Write(true, brightness);
}

void SH1106_StartScrollHorizontal(bool left_scroll, uint8_t start, uint8_t end)
{
        if (left_scroll)
        {
                SH1106_Write(true, 0x27);
        }
        else
        {
                SH1106_Write(true, 0x26);
        }

        SH1106_Write(true, 0x00);
        SH1106_Write(true, start);
        SH1106_Write(true, 0x00);
        SH1106_Write(true, end);
        SH1106_Write(true, 0x00);
        SH1106_Write(true, 0xFF);
        SH1106_Write(true, 0x2F);
}

void SH1106_StartScrollVerticalHorizontal(bool left_scroll, uint8_t start, uint8_t end, uint8_t vertical_area, uint8_t rows)
{
        SH1106_Write(true, 0xA3);
        SH1106_Write(true, 0x00);
        SH1106_Write(true, vertical_area);

        if (left_scroll)
        {
                SH1106_Write(true, 0x2A);
        }
        else
        {
                SH1106_Write(true, 0x29);
        }
        
        SH1106_Write(true, 0x00);
        SH1106_Write(true, start);
        SH1106_Write(true, 0x00);
        SH1106_Write(true, end);
        SH1106_Write(true, rows);
        SH1106_Write(true, 0x2F);
}

void SH1106_DeactivateScroll(void)
{
        SH1106_Write(true, 0x2E);
}

void SH1106_Fill(uint8_t data)
{
        for (unsigned int i = 0; i < (SH1106_MAX_SEG * (SH1106_MAX_LINE + 1)); i++)
        {
                SH1106_Write(false, data);
        }
}

void SH1106_ClearDisplay(void)
{
        SH1106_SetCursor(0, 0);
        SH1106_Fill(0x00);
}

int SH1106_DisplayInit(void)
{
        int ret = SH1106_ResetDCInit();

        if (ret >= 0)
        {
                gpio_set_value(SH1106_RES, 0);
                msleep(100);
                gpio_set_value(SH1106_RES, 1);
                msleep(100);

                SH1106_Write(true, 0xAE); // Entire Display OFF
                SH1106_Write(true, 0xD5); // Set Display Clock Divide Ratio and Oscillator Frequency
                SH1106_Write(true, 0x80); // Default Setting for Display Clock Divide Ratio and Oscillator Frequency that is recommended
                SH1106_Write(true, 0xA8); // Set Multiplex Ratio
                SH1106_Write(true, 0x3F); // 64 COM lines
                SH1106_Write(true, 0xD3); // Set display offset
                SH1106_Write(true, 0x00); // 0 offset
                SH1106_Write(true, 0x40); // Set first line as the start line of the display
                SH1106_Write(true, 0x8D); // Charge pump
                SH1106_Write(true, 0x14); // Enable charge dump during display on
                SH1106_Write(true, 0x20); // Set memory addressing mode
                SH1106_Write(true, 0x00); // Horizontal addressing mode
                SH1106_Write(true, 0xA1); // Set segment remap with column address 127 mapped to segment 0
                SH1106_Write(true, 0xC8); // Set com output scan direction, scan from com63 to com 0
                SH1106_Write(true, 0xDA); // Set com pins hardware configuration
                SH1106_Write(true, 0x12); // Alternative com pin configuration, disable com left/right remap
                SH1106_Write(true, 0x81); // Set contrast control
                SH1106_Write(true, 0x80); // Set Contrast to 128
                SH1106_Write(true, 0xD9); // Set pre-charge period
                SH1106_Write(true, 0xF1); // Phase 1 period of 15 DCLK, Phase 2 period of 1 DCLK
                SH1106_Write(true, 0xDB); // Set Vcomh deselect level
                SH1106_Write(true, 0x20); // Vcomh deselect level ~ 0.77 Vcc
                SH1106_Write(true, 0xA4); // Entire display ON, resume to RAM content display
                SH1106_Write(true, 0xA6); // Set Display in Normal Mode, 1 = ON, 0 = OFF
                SH1106_Write(true, 0x2E); // Deactivate scroll
                SH1106_Write(true, 0xAF); // Display ON in normal mode

                // Clear the display
                SH1106_ClearDisplay();
        }        
}