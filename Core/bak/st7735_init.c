#include "st7735_init.h"
//#include "delay.h"

void LCD_SCLK_Clr(void) { };
void LCD_SCLK_Set(void) { };
void LCD_MOSI_Clr(void) { };
void LCD_MOSI_Set(void) { };
void LCD_RES_Clr(void) { };
void LCD_RES_Set(void) { };
void LCD_DC_Clr(void) { };
void LCD_DC_Set(void) { };
void LCD_CS_Clr(void) { };
void LCD_CS_Set(void) { };
void LCD_BLK_Clr(void) { };
void LCD_BLK_Set(void) { };

void LCD_GPIO_Init(void) {
	//GPIO_InitTypeDef  GPIO_InitStructure;
 	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��A�˿�ʱ��
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
 	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	//GPIO_Init(GPIOA, &GPIO_InitStructure);	  //��ʼ��GPIOA
 	//GPIO_SetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5);
}

void st7735_gpioinit(st7735_t *d, swgpio_t *cs, swgpio_t *dc, swgpio_t *rst, swgpio_t *blk) {
	if(!d) return;
	d->init = 0;
	d->blk = NULL; d->cs = NULL; d->dc = NULL; d->rst = NULL;
	d->d.curX = 0;
	d->d.curY = 0;
	if(cs) { d->cs=cs; swgp_gpmode(cs, 1); }
	if(dc) { d->dc=dc; swgp_gpmode(dc, 1); }
	if(rst) { d->rst=rst; swgp_gpmode(rst, 1); }
	if(blk) { d->blk=blk; swgp_gpmode(blk, 1); }
}
/******************************************************************************
      ����˵����LCD��������д�뺯��
      ������ݣ�dat  Ҫд��Ĵ�������
      ����ֵ��  ��
******************************************************************************/
void LCD_Writ_Bus(uint8_t dat) 
{	
	uint8_t i;
	LCD_CS_Clr();
	for(i=0;i<8;i++)
	{			  
		LCD_SCLK_Clr();
		if(dat&0x80)
		{
		   LCD_MOSI_Set();
		}
		else
		{
		   LCD_MOSI_Clr();
		}
		LCD_SCLK_Set();
		dat<<=1;
	}	
  LCD_CS_Set();	
}

void st7735_writeN(st7735_t *d, uint8_t *val, int size) {
	swgp_gpo(d->cs, 0);
	swspi_write(d->dev, val, size);
	swgp_gpo(d->cs, 1);
}

/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA8(uint8_t dat)
{
	LCD_Writ_Bus(dat);
}

void st7735_write8(st7735_t *d, uint8_t val) { st7735_writeN(d, &val, 1); }
/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA(uint16_t dat)
{
	LCD_Writ_Bus(dat>>8);
	LCD_Writ_Bus(dat);
}

void st7735_write16(st7735_t *d, uint16_t val) { uint8_t *t=(uint8_t*)&val;
	st7735_writeN(d, t+1, 1); st7735_writeN(d, t+0, 1); }

/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_REG(uint8_t dat)
{
	LCD_DC_Clr();//д����
	LCD_Writ_Bus(dat);
	LCD_DC_Set();//д����
}

void st7735_w_reg(st7735_t *d, uint8_t val) {
	swgp_gpo(d->dc, 0);
	st7735_write8(d, val);
	swgp_gpo(d->dc, 1);
}
/******************************************************************************
      ����˵����������ʼ�ͽ�����ַ
      ������ݣ�x1,x2 �����е���ʼ�ͽ�����ַ
                y1,y2 �����е���ʼ�ͽ�����ַ
      ����ֵ��  ��
******************************************************************************/
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	if(USE_HORIZONTAL==0)
	{
		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1+26);
		LCD_WR_DATA(x2+26);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);//������д
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1+26);
		LCD_WR_DATA(x2+26);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);//������д
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1+1);
		LCD_WR_DATA(x2+1);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1+26);
		LCD_WR_DATA(y2+26);
		LCD_WR_REG(0x2c);//������д
	}
	else
	{
		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1+1);
		LCD_WR_DATA(x2+1);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1+26);
		LCD_WR_DATA(y2+26);
		LCD_WR_REG(0x2c);//������д
	}
}

void st7735_window(st7735_t *d, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
#if	((USE_HORIZONTAL == 0) || (USE_HORIZONTAL == 1))
		st7735_w_reg(d, 0x2a);//�е�ַ����
		st7735_write16(d, x1+26);
		st7735_write16(d, x2+26);
		st7735_w_reg(d, 0x2b);//�е�ַ����
		st7735_write16(d, y1+1);
		st7735_write16(d, y2+1);
		st7735_w_reg(d, 0x2c);//������д
#else
		st7735_w_reg(d, 0x2a);//�е�ַ����
		st7735_write16(d, x1+1);
		st7735_write16(d, x2+1);
		st7735_w_reg(d, 0x2b);//�е�ַ����
		st7735_write16(d, y1+26);
		st7735_write16(d, y2+26);
		st7735_w_reg(d, 0x2c);//������д
#endif
}

void st7789_window(st7735_t *d, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
#if	((USE_HORIZONTAL == 0) || (USE_HORIZONTAL == 2))
		st7735_w_reg(d, 0x2a);//�е�ַ����
		st7735_write16(d, x1);
		st7735_write16(d, x2);
		st7735_w_reg(d, 0x2b);//�е�ַ����
		st7735_write16(d, y1);
		st7735_write16(d, y2);
		st7735_w_reg(d, 0x2c);//������д
#elif (USE_HORIZONTAL == 1)
		st7735_w_reg(d, 0x2a);//�е�ַ����
		st7735_write16(d, x1);
		st7735_write16(d, x2);
		st7735_w_reg(d, 0x2b);//�е�ַ����
		st7735_write16(d, y1+80);
		st7735_write16(d, y2+80);
		st7735_w_reg(d, 0x2c);//������д
#elif (USE_HORIZONTAL == 3)
		st7735_w_reg(d, 0x2a);//�е�ַ����
		st7735_write16(d, x1+80);
		st7735_write16(d, x2+80);
		st7735_w_reg(d, 0x2b);//�е�ַ����
		st7735_write16(d, y1);
		st7735_write16(d, y2);
		st7735_w_reg(d, 0x2c);//������д
#endif
}


void LCD_Init(void)
{
	LCD_GPIO_Init();//��ʼ��GPIO
	
	LCD_RES_Clr();//��λ
	swgp_delay_ms(100);
	LCD_RES_Set();
	swgp_delay_ms(100);
	
	LCD_BLK_Set();//�򿪱���
	swgp_delay_ms(100);
	
	LCD_WR_REG(0x11);     //Sleep out
	swgp_delay_ms(120);                //Delay 120ms
	LCD_WR_REG(0xB1);     //Normal mode
	LCD_WR_DATA8(0x05);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_REG(0xB2);     //Idle mode
	LCD_WR_DATA8(0x05);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_REG(0xB3);     //Partial mode
	LCD_WR_DATA8(0x05);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_DATA8(0x05);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_REG(0xB4);     //Dot inversion
	LCD_WR_DATA8(0x03);   
	LCD_WR_REG(0xC0);     //AVDD GVDD
	LCD_WR_DATA8(0xAB);   
	LCD_WR_DATA8(0x0B);   
	LCD_WR_DATA8(0x04);   
	LCD_WR_REG(0xC1);     //VGH VGL
	LCD_WR_DATA8(0xC5);   //C0
	LCD_WR_REG(0xC2);     //Normal Mode
	LCD_WR_DATA8(0x0D);   
	LCD_WR_DATA8(0x00);   
	LCD_WR_REG(0xC3);     //Idle
	LCD_WR_DATA8(0x8D);   
	LCD_WR_DATA8(0x6A);   
	LCD_WR_REG(0xC4);     //Partial+Full
	LCD_WR_DATA8(0x8D);   
	LCD_WR_DATA8(0xEE);   
	LCD_WR_REG(0xC5);     //VCOM
	LCD_WR_DATA8(0x0F);   
	LCD_WR_REG(0xE0);     //positive gamma
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x0E);   
	LCD_WR_DATA8(0x08);   
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x10);   
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x02);   
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x09);   
	LCD_WR_DATA8(0x0F);   
	LCD_WR_DATA8(0x25);   
	LCD_WR_DATA8(0x36);   
	LCD_WR_DATA8(0x00);   
	LCD_WR_DATA8(0x08);   
	LCD_WR_DATA8(0x04);   
	LCD_WR_DATA8(0x10);   
	LCD_WR_REG(0xE1);     //negative gamma
	LCD_WR_DATA8(0x0A);   
	LCD_WR_DATA8(0x0D);   
	LCD_WR_DATA8(0x08);   
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x0F);   
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x02);   
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x09);   
	LCD_WR_DATA8(0x0F);   
	LCD_WR_DATA8(0x25);   
	LCD_WR_DATA8(0x35);   
	LCD_WR_DATA8(0x00);   
	LCD_WR_DATA8(0x09);   
	LCD_WR_DATA8(0x04);   
	LCD_WR_DATA8(0x10);
		 
	LCD_WR_REG(0xFC);    
	LCD_WR_DATA8(0x80);  
		
	LCD_WR_REG(0x3A);     
	LCD_WR_DATA8(0x05);   
	LCD_WR_REG(0x36);
	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x08);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC8);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x78);
	else LCD_WR_DATA8(0xA8);   
	LCD_WR_REG(0x21);     //Display inversion
	LCD_WR_REG(0x29);     //Display on
	LCD_WR_REG(0x2A);     //Set Column Address
	LCD_WR_DATA8(0x00);   
	LCD_WR_DATA8(0x1A);  //26  
	LCD_WR_DATA8(0x00);   
	LCD_WR_DATA8(0x69);   //105 
	LCD_WR_REG(0x2B);     //Set Page Address
	LCD_WR_DATA8(0x00);   
	LCD_WR_DATA8(0x01);    //1
	LCD_WR_DATA8(0x00);   
	LCD_WR_DATA8(0xA0);    //160
	LCD_WR_REG(0x2C); 
}

void st7735_init(st7735_t *d, uint16_t width, uint16_t height, void *pvport, void *pvFontDef) {
	d->dev = pvport;
	d->d.parent = d;
	d->d.frameWidth = width;
	d->d.frameHeight = height;
	d->d.oneLineOffsetSize = (width >> 3);
	d->d.pFrameBuf = NULL;
	d->d.pFont = pvFontDef;
	d->init = 0;
	d->d.curX = 0;
	d->d.curY = 0;
	d->colorf = 0xffff;
	d->colorb = 0;
	swgp_gpo(d->blk, 1);
	swgp_gpo(d->rst, 0);
	swgp_delay_ms(100);
	swgp_gpo(d->rst, 1);	
	swgp_delay_ms(100);
	swgp_gpo(d->blk, 0);
	swgp_gpo(d->dc, 1);

	st7735_w_reg(d, 0x11);//sleep-out
	swgp_delay_ms(120);
	st7735_w_reg(d, 0xb1);//normal-mode
	st7735_write8(d, 0x05);
	st7735_write8(d, 0x3c);
	st7735_write8(d, 0x3c);
	st7735_w_reg(d, 0xb2);//idle-mode
	st7735_write8(d, 0x05);
	st7735_write8(d, 0x3c);
	st7735_write8(d, 0x3c);
	st7735_w_reg(d, 0xb3);//partical-mode
	st7735_write8(d, 0x05);
	st7735_write8(d, 0x3c);
	st7735_write8(d, 0x3c);
	st7735_write8(d, 0x05);
	st7735_write8(d, 0x3c);
	st7735_write8(d, 0x3c);
	st7735_w_reg(d, 0xb4);//dot-inversed
	st7735_write8(d, 0x03);
	st7735_w_reg(d, 0xc0);//avdd-gvdd
	st7735_write8(d, 0xab);
	st7735_write8(d, 0x0b);
	st7735_write8(d, 0x04);
	st7735_w_reg(d, 0xc1);//vgh-vgl
	st7735_write8(d, 0xc5);
	st7735_w_reg(d, 0xc2);//normal-mode
	st7735_write8(d, 0x0d);
	st7735_write8(d, 0x00);
	st7735_w_reg(d, 0xc3);//idle
	st7735_write8(d, 0x8d);
	st7735_write8(d, 0x6a);
	st7735_w_reg(d, 0xc4);//partical+full
	st7735_write8(d, 0x8d);
	st7735_write8(d, 0xee);
	st7735_w_reg(d, 0xc5);//vcom
	st7735_write8(d, 0x0f);
	st7735_w_reg(d, 0xe0);//positive gamma
	st7735_write8(d, 0x07);
	st7735_write8(d, 0x0e);
	st7735_write8(d, 0x08);
	st7735_write8(d, 0x07);
	st7735_write8(d, 0x10);
	st7735_write8(d, 0x07);
	st7735_write8(d, 0x02);
	st7735_write8(d, 0x07);
	st7735_write8(d, 0x09);
	st7735_write8(d, 0x0f);
	st7735_write8(d, 0x25);
	st7735_write8(d, 0x36);
	st7735_write8(d, 0x00);
	st7735_write8(d, 0x08);
	st7735_write8(d, 0x04);
	st7735_write8(d, 0x10);
	st7735_w_reg(d, 0xe1);//negative gamma
	st7735_write8(d, 0x0a);
	st7735_write8(d, 0x0d);
	st7735_write8(d, 0x08);
	st7735_write8(d, 0x07);
	st7735_write8(d, 0x0f);
	st7735_write8(d, 0x07);
	st7735_write8(d, 0x02);
	st7735_write8(d, 0x07);
	st7735_write8(d, 0x09);
	st7735_write8(d, 0x0f);
	st7735_write8(d, 0x25);
	st7735_write8(d, 0x35);
	st7735_write8(d, 0x00);
	st7735_write8(d, 0x09);
	st7735_write8(d, 0x04);
	st7735_write8(d, 0x10);
	st7735_w_reg(d, 0xfc);//dot-inversed

	st7735_write8(d, 0x80);
	st7735_w_reg(d, 0x3a);
	st7735_write8(d, 0x05);
	st7735_w_reg(d, 0x36);
#if (USE_HORIZONTAL==0)
	st7735_write8(d, 0x08);
#elif (USE_HORIZONTAL==1)
	st7735_write8(d, 0xc8);
#elif (USE_HORIZONTAL==2)
	st7735_write8(d, 0x78);
#else
	st7735_write8(d, 0xa8);
#endif
	st7735_w_reg(d, 0x21);//display inversion
	st7735_w_reg(d, 0x29);//display on
	st7735_w_reg(d, 0x2a);//set column address
	st7735_write8(d, 0x00);
	st7735_write8(d, 0x1a);
	st7735_write8(d, 0x00);
	st7735_write8(d, 0x69);
	st7735_w_reg(d, 0x2b);//set page address
	st7735_write8(d, 0x00);
	st7735_write8(d, 0x01);
	st7735_write8(d, 0x00);
	st7735_write8(d, 0xa0);
	st7735_w_reg(d, 0x2c);

	d->init = 1;
}







