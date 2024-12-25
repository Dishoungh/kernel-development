#define SPI_BUS_NUM             0

#define SH1106_RES              24
#define SH1106_DC               23

#define SH1106_MAX_SEG          128
#define SH1106_MAX_LINE         7
#define SH1106_DEF_FONT_SIZE    5

int SH1106_DisplayInit(void);
int SH1106_DisplayDeinit(void);

int SH1106_SPI_Write(uint8_t data);
int SH1106_Write(uint8_t data);

void SH1106_SetCursor(uint8_t line, uint8_t cursor);
void SH1106_NextLine(void);
void SH1106_PrintChar(unsigned char c);
void SH1106_String(char *str);

void SH1106_InvertDisplay(bool invert);
void SH1106_SetBrightness(uint8_t brightness);
void SH1106_StartScrollHorizontal(bool left_scroll, uint8_t start, uint8_t end);
void SH1106_StartScrollVerticalHorizontal(bool left_scroll, uint8_t start, uint8_t end, uint8_t vertical_area, uint8_t rows);
void SH1106_DeactivateScroll(void);
void SH1106_Fill(uint8_t data);
void SH1106_ClearDisplay(void);
