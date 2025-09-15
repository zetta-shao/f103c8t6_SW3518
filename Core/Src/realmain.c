#include <stdio.h>
#include "main.h"
#include "gpiodef.h"
#include "swgp.h"
#include "swi2c.h"
#include "swspi.h"
#include "st7735.h"
#include "lcd_fonts.h"
#include "sw35xx_s.h"

extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi2;

swi2c_t si2c1={0};
swspi_t sspi1={0};
//swspi_t sspi2={0};
swgpio_t tSPI_CLK = { SPI3CLKP, SPI3CLK };
swgpio_t tSPI_MOSI = { SPI3MOP, SPI3MOSI };
swgpio_t tCS = { LCD_CS_P, LCD_CS };
swgpio_t tCD = { LCD_CD_P, LCD_CD };
swgpio_t tRST = { LCD_RST_P, LCD_RST };
swgpio_t tBKL = { EVB_LED2_P, EVB_LED2 };
swgpio_t tLED = { EVB_LED_P, EVB_LED };
swgpio_t tCSF = { SPI2SSP, SPI2SS };
st7735_t st7735 = { 0 };
sw35xx_t sw3518 = { 0 };

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) { }
#endif /* USE_FULL_ASSERT */
#define ADV_VREF_PREAMP (4096 * 1210)
uint8_t color_index = 0;
int8_t timeout = 120;
uint16_t oldsta[5];

void update_st7735_color(void) {
		color_index &= 7;
		st7735.d.colorf = st7735_colorpalette[color_index];
		color_index++;
}

void init_adc(ADC_HandleTypeDef *d, void *out) {
	uint32_t idx, res = 0;
	//int vref; (void)vref;
	ADC_ChannelConfTypeDef sConfig = {0};

	sConfig.Channel = ADC_CHANNEL_VREFINT;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) { Error_Handler(); }

	HAL_ADC_Start(d);
	for(idx=0; idx<4; idx++) {
		HAL_ADC_PollForConversion(d, 1); //wsit for 1mS
		res += HAL_ADC_GetValue(d);
	}
	HAL_ADC_Stop(d);
	res >>= 2;

	//vref = ADV_VREF_PREAMP / res;

	sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) { Error_Handler(); }
}

void update_lcd_static(lcddev_t *d) {
	int bakx, lblw, lblh, _x, _y;
	FontDef *pF = &deffont;
	FontDef *p1 = &Font_6x8;

	bakx = 0;
	lblw = p1->FontWidth;
	lblh = (p1->FontHeight*9)+8;
	fontdraw_set_string_dir(d, 1);
	d->curX = 0; d->curY = 0; 
	_x = d->curX; _y = d->curY;
	fontdraw_stringFont(d, (char*)"out", 0, p1);
	d->curY += 4;
	fontdraw_stringFont(d, (char*)"c/A", 0, p1);
	d->curY += 4;
	fontdraw_stringFont(d, (char*)"p/A", 0, p1);
	st7735_update_window(d->parent, _x, _y, lblw, lblh);

	d->curX = d->frameWidth - p1->FontWidth; d->curY = 0;
	_x = d->curX; _y = d->curY;
	fontdraw_stringFont(d, (char*)"vin", 0, p1);
	d->curY += 4;
	fontdraw_stringFont(d, (char*)"c/C", 0, p1);
	d->curY += 4;
	fontdraw_stringFont(d, (char*)"p/C", 0, p1);
	fontdraw_set_string_dir(d, 0);
	st7735_update_window(d->parent, _x, _y, lblw, lblh);

	bakx = (d->frameWidth >> 1) - (pF->FontWidth >> 1);
	d->curX = bakx; d->curY = 0;
	_x = d->curX; _y = d->curY;
	fontdraw_stringFont(d, "V", 1, pF);
	d->curX = bakx; d->curY += pF->FontHeight + 4;
	fontdraw_stringFont(d, "A", 1, pF);
	d->curX = bakx; d->curY += pF->FontHeight + 4;
	fontdraw_stringFont(d, "W", 1, pF);
	st7735_update_window(d->parent, _x, _y, pF->FontWidth, (pF->FontHeight*3)+8);
}

void update_smalllabel(lcddev_t *d) {
	FontDef *p1 = &Font_6x8;
	FontDef *pF = &deffont;
	int _x, _y;
	d->curX = (d->frameWidth >> 1) - ((p1->FontWidth*13) >> 1);
	d->curY = pF->FontHeight - 1;
	_x = d->curX; _y = d->curY;
	fontdraw_stringFont(d, "Vout <-> Vin", 1, p1);
	st7735_update_window(d->parent, _x, _y, p1->FontWidth*12, p1->FontHeight );

	d->curX = (d->frameWidth >> 1) - ((p1->FontWidth*15) >> 1);
	d->curY = (pF->FontHeight << 1) + 3;
	_x = d->curX; _y = d->curY;
	fontdraw_stringFont(d, "portA <-> portC", 1, p1);
	st7735_update_window(d->parent, _x, _y, p1->FontWidth*12, p1->FontHeight );
	update_st7735_color();
}

void update_sw3518(sw35xx_t *swt, lcddev_t *d) {
	int bakx, lblw, lblh, _x, _y;
	char str[8];
	FontDef *pF = &Font_12x16N;
	FontDef *p1 = &Font_6x8;

	bakx = (int)sw35xx_version(swt);
	if(! swt->pDev) { return; }
	lblw = (pF->FontWidth * 4) + pF->dotwidth;
	lblh = pF->FontHeight;	
	SW35xx_readStatus(swt, 0);

	d->curX = 8; d->curY = 4; //Vout
	_x = d->curX; _y = d->curY;
	if(oldsta[0]!=swt->vout_mV) {
		str_4digitL(swt->vout_mV, str);
		fontdraw_stringFont(d, str, 1, pF);		
		oldsta[0]=swt->vout_mV; 
		st7735_update_window(d->parent, _x, _y, lblw, lblh);
		update_st7735_color();
	}
	d->curX = 8; d->curY += pF->FontHeight + 12; //c/A
	_x = d->curX; _y = d->curY;
	if(oldsta[1]!=swt->iout_usba_mA) {
		str_4digitL(swt->iout_usba_mA, str); 
		fontdraw_stringFont(d, str, 1, pF);		
		oldsta[1]=swt->iout_usba_mA; oldsta[4] |= 1;
		st7735_update_window(d->parent, _x, _y, lblw, lblh);
		update_st7735_color();
	} else { oldsta[4] &= ~1; }
	d->curX = 8; d->curY += pF->FontHeight + 12; //p/A
	_x = d->curX; _y = d->curY;
	if(oldsta[4]&1) {
		str_4digitL((swt->iout_usba_mA * swt->vout_mV)/1000, str);
		fontdraw_stringFont(d, str, 1, pF);
		st7735_update_window(d->parent, _x, _y, lblw, lblh);
		update_st7735_color();
	}	

	bakx = d->frameWidth - p1->FontWidth;
	bakx -= (pF->FontWidth * 4) + pF->dotwidth;
	d->curX = bakx; d->curY = 4;
	_x = d->curX; _y = d->curY;
	if(oldsta[3]!=swt->vin_mV) {
		str_4digitL(swt->vin_mV, str);
		fontdraw_stringFont(d, str, 1, pF);
		oldsta[3]=swt->vin_mV; 
		st7735_update_window(d->parent, _x, _y, lblw, lblh);
		update_st7735_color();
	}
	d->curX = bakx; d->curY += pF->FontHeight + 12;
	_x = d->curX; _y = d->curY;
	if(oldsta[2]!=swt->iout_usbc_mA) {
		str_4digitL(swt->iout_usbc_mA, str);
		fontdraw_stringFont(d, str, 1, pF);
		oldsta[2]=swt->iout_usbc_mA; oldsta[4]|=2;
		st7735_update_window(d->parent, _x, _y, lblw, lblh);
		update_st7735_color();
	} else { oldsta[4] &= ~2; }
	d->curX = bakx; d->curY += pF->FontHeight + 12;
	_x = d->curX; _y = d->curY;

	if(oldsta[4]&2) {
		str_4digitL((swt->vout_mV * swt->iout_usbc_mA)/1000, str);
		fontdraw_stringFont(d, str, 1, pF);
		st7735_update_window(d->parent, _x, _y, lblw, lblh);
		update_st7735_color();
	}

	if((oldsta[4] & 3) == 0) {
		if(timeout > 0) { timeout--; }
		else { timeout=0; st7735_set_idle(d->parent, 1); }
	} else { timeout = 120; st7735_set_idle(d->parent, 0); }
	oldsta[4] = 0;
}

int realmain(void) {
	uint8_t _val_ = 0;
    lcddev_t *d = &(st7735.d);

	swi2c_HWinit(&si2c1, &hi2c1, NULL);
	swspi_HWinit(&sspi1, &hspi2);
	//swspi_SWinit(&sspi2, &tSPI_CLK, &tSPI_MOSI, NULL);
	swgp_gpmode(&tCSF, 1); swgp_gpo(&tCSF, 1);
	//HAL_ADCEx_Calibration_Start(&hadc1);
	swgp_delay_ms(50); //HAL_Delay(50);

	st7735_gpioinit(&st7735, &tCS, &tCD, &tRST, &tBKL);
	st7735_init(&st7735, 160, 80, &sspi1, &deffont);
	//fontdraw_fill(d, 0);
	st7735_fill(&st7735, 0, 0, st7735.d.frameWidth, st7735.d.frameHeight, 0);
	//st7735_setcolor(&st7735, WHITE, BLACK);

    	//fontdraw_stringFont(d, (char*)"--test--", 0, &deffont);
	update_lcd_static(d);
	update_smalllabel(d);

	oldsta[0]=0xffff; oldsta[1]=0xffff; oldsta[2]=0xffff; oldsta[3]=0xffff; oldsta[4]=0xffff; oldsta[5]=0xffff;
	sw35xx_init(&sw3518, &si2c1);
	update_sw3518(&sw3518, d);

	//st7735.d.update(d);
	//init_adc(&hadc1, NULL);
	swgp_gpmode(&tLED, 1);
	while (1) {
		_val_++;
		_val_ &= 7;
		if(!_val_) { 
			update_lcd_static(d);
		} else {
			swgp_gpo(&tCSF, 1);
			swgp_gpo(&tLED, 1);
			update_sw3518(&sw3518, d);
			//update_smalllabel(d);
			swgp_delay_ms(500);

			swgp_gpo(&tCSF, 1);
			swgp_gpo(&tLED, 0);
			update_sw3518(&sw3518, d);
			//update_smalllabel(d);
			//st7735.d.update(d);
			swgp_delay_ms(500);

			//update_st7735_color();
		}
	}
}
