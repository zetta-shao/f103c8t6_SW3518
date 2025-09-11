#ifndef __ST7735_INIT_H
#define __ST7735_INIT_H
#ifdef __cplusplus
extern "C" {
#endif

#include "swgp.h"
#include "swspi.h"

#define USE_HORIZONTAL 2  //���ú�������������ʾ 0��1Ϊ���� 2��3Ϊ����

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 80
#define LCD_H 160
#else
#define LCD_W 160
#define LCD_H 80
#endif

//#define LCDFONT_INCLUDE_FONT_5x7
#define LCDFONT_INCLUDE_FONT_6x8
//#define LCDFONT_INCLUDE_FONT_8x16

//#define LCDFONT_INCLUDE_FONT_6x8_16
//#define LCDFONT_INCLUDE_FONT_7x10
//#define LCDFONT_INCLUDE_FONT_11x18
//#define LCDFONT_INCLUDE_FONT_16x26
//#define LCDFONT_INCLUDE_FONT_16x24

#include "lcd_fonts.h"

typedef struct st7735_t st7735_t;

struct st7735_t {
    uint8_t     init;
    uint8_t     unused[3];
    uint16_t    colorf; //front color
    uint16_t    colorb; //back color;
    lcddev_t    d;
    void        *dev;
    swgpio_t    *cs;
    swgpio_t    *dc; //data/command
    swgpio_t    *rst;
    swgpio_t    *blk;
};


//-----------------LCD�˿ڶ���---------------- 

//#define LCD_SCLK_Clr() GPIO_ResetBits(SPI3CLKP,SPI3CLK)//SCL=SCLK
//#define LCD_SCLK_Set() GPIO_SetBits(GPIOA,GPIO_Pin_0)

//#define LCD_MOSI_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_1)//SDA=MOSI
//#define LCD_MOSI_Set() GPIO_SetBits(GPIOA,GPIO_Pin_1)

//#define LCD_RES_Clr()  GPIO_ResetBits(GPIOA,GPIO_Pin_2)//RES
//#define LCD_RES_Set()  GPIO_SetBits(GPIOA,GPIO_Pin_2)

//#define LCD_DC_Clr()   GPIO_ResetBits(GPIOA,GPIO_Pin_3)//DC
//#define LCD_DC_Set()   GPIO_SetBits(GPIOA,GPIO_Pin_3)
 		     
//#define LCD_CS_Clr()   GPIO_ResetBits(GPIOA,GPIO_Pin_4)//CS
//#define LCD_CS_Set()   GPIO_SetBits(GPIOA,GPIO_Pin_4)

//#define LCD_BLK_Clr()  GPIO_ResetBits(GPIOA,GPIO_Pin_5)//BLK
//#define LCD_BLK_Set()  GPIO_SetBits(GPIOA,GPIO_Pin_5)
#if 0
void LCD_SCLK_Clr(void);
void LCD_SCLK_Set(void);
void LCD_MOSI_Clr(void);
void LCD_MOSI_Set(void);
void LCD_RES_Clr(void);
void LCD_RES_Set(void);
void LCD_DC_Clr(void);
void LCD_DC_Set(void);
void LCD_CS_Clr(void);
void LCD_CS_Set(void);
void LCD_BLK_Clr(void);
void LCD_BLK_Set(void);
#endif

void LCD_GPIO_Init(void);//��ʼ��GPIO
void LCD_Writ_Bus(uint8_t dat);//ģ��SPIʱ��
void LCD_WR_DATA8(uint8_t dat);//д��һ���ֽ�
void LCD_WR_DATA(uint16_t dat);//д�������ֽ�
void LCD_WR_REG(uint8_t dat);//д��һ��ָ��
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);//�������꺯��
void LCD_Init(void);//LCD��ʼ��

void st7735_gpioinit(st7735_t *d, swgpio_t *cs, swgpio_t *dc, swgpio_t *rst, swgpio_t *blk);
void st7735_writeN(st7735_t *d, uint8_t *val, int size);
void st7735_write8(st7735_t *d, uint8_t val);
void st7735_write16(st7735_t *d, uint16_t val);
void st7735_w_reg(st7735_t *d, uint8_t val);
void st7735_window(st7735_t *d, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void st7735_init(st7735_t *d, uint16_t width, uint16_t height, void *pvport, void *pvFontDef);

#ifdef __cplusplus
}
#endif
#endif




