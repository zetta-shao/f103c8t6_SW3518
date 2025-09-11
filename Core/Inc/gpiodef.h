#ifndef __DEF_GPIO__
#define __DEF_GPIO__

#include "main.h" //for STM32 only

#define HI2C1L		GPIO_PIN_6	//1
#define HI2C1A		GPIO_PIN_7	//2
#define HI2C1P		GPIOB
#define HI2C2L		GPIO_PIN_11	//16 //PA11/PA9
#define HI2C2A		GPIO_PIN_12	//17 //PA12/PA10
#define HI2C2P		GPIOA
#define SPI2N		GPIO_PIN_12	// // SPI2 CS0
#define SPI2L		GPIO_PIN_13	// // SPI2 CLK
#define SPI2I		GPIO_PIN_14	// // SPI2 MISO
#define SPI2O		GPIO_PIN_15	// // SPI2 MOSI
#define SPI2P		GPIOB
#define TIM3C1		GPIO_PIN_3	//20 //PB3/PB4/PB5/PB6
#define TIM3P		GPIOB
#define SI2C2A		GPIO_PIN_0	//7
#define SI2C2L		GPIO_PIN_1	//8
#define SI2C2P		GPIOA
#define SI2C1A		GPIO_PIN_2	//9
#define SI2C1L		GPIO_PIN_3	//10
#define SI2C1P		GPIOA
#define BTN1		GPIO_PIN_13	//18
#define BTN2		GPIO_PIN_14	//19 //PA14/PA15
#define BTNP		GPIOA
#define LCD_CS		GPIO_PIN_1	//11
#define LCD_CS_P	GPIOB	//11
#define LCD_CD		GPIO_PIN_0	//13 //noused
#define LCD_CD_P	GPIOB	//13
#define LCD_RST		GPIO_PIN_7	//15 //PB0/PB1/PB2/PA8 noused
#define LCD_RST_P	GPIOA	//15
#define EVB_LED		GPIO_PIN_13	//15
#define EVB_LED_P	GPIOC	//15
#define EVB_LED2	GPIO_PIN_6	//11 //backlight
#define EVB_LED2_P	GPIOA	//15
#define SPI3CLK		GPIO_PIN_10	//13
#define SPI3CLKP	GPIOB
#define SPI3MOSI	GPIO_PIN_11	//15
#define SPI3MOP		GPIOB
#define SPI3SS		GPIO_PIN_1	//20
#define SPI3SSP		GPIOB
#define SPI2SS		GPIO_PIN_12	//20
#define SPI2SSP		GPIOB

//#define LCD_RS		GPIO_PIN_15	//20
//#define LCD_RSP		GPIOC

#endif
