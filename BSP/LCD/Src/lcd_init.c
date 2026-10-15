#include "lcd_init.h"
//#include "tim.h"

/*
 * @brief delay_ms funciton
 */
void delay_ms(uint32_t ms) {
	HAL_Delay(ms);
}

/******************************************************************************
      函数说明：LCD端口初始化
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure = {0};
	
	__HAL_RCC_GPIOC_CLK_ENABLE();
 	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStructure.Pin = BLK_PIN;	 
 	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP; 		 //推挽输出
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;//速度50MHz
 	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);	  //初始化GPIOA
 	HAL_GPIO_WritePin(GPIOA, BLK_PIN, GPIO_PIN_SET);
	
	GPIO_InitStructure.Pin = RES_PIN;	 
 	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP; 		 //推挽输出
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;//速度50MHz
 	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);	  //初始化GPIOB
 	HAL_GPIO_WritePin(GPIOB, RES_PIN, GPIO_PIN_SET);

	GPIO_InitStructure.Pin = DC_PIN;	 
 	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP; 		 //推挽输出
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;//速度50MHz
 	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);	  //初始化GPIOC
 	HAL_GPIO_WritePin(GPIOC, DC_PIN, GPIO_PIN_SET);

	GPIO_InitStructure.Pin = CS_PIN;	 
 	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP; 		 //推挽输出
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;//速度50MHz
 	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);	  //初始化GPIOD
 	HAL_GPIO_WritePin(GPIOD, CS_PIN, GPIO_PIN_SET);

}


/******************************************************************************
      函数说明：LCD串行数据写入函数(software SPI)
      入口数据：dat  要写入的串行数据
      返回值：  无
******************************************************************************/
void LCD_Writ_Bus(u8 dat) 
{	
	//hard SPI
	HAL_SPI_Transmit(&hspi1,&dat,1,100);
	
	//soft SPI

	// u8 i;
	// for(i=0;i<8;i++)
	// {
	// 	LCD_SCLK_Clr();
	// 	if(dat&0x80)
	// 	{
	// 	   LCD_MOSI_Set();
	// 	}
	// 	else
	// 	{
	// 	   LCD_MOSI_Clr();
	// 	}
	// 	LCD_SCLK_Set();
	// 	dat<<=1;
	// }

}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
//	LCD_Writ_Bus(dat>>8);
//	LCD_Writ_Bus(dat);
	uint8_t temp[2];
	temp[0]=(dat>>8)&0xff;
	temp[1]=dat&0xff;

	HAL_SPI_Transmit(&hspi1,temp,2,1);
	
}


/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	LCD_DC_Clr();//写命令
	LCD_Writ_Bus(dat);
	LCD_DC_Set();//写数据
}


/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	LCD_WR_REG(CASET);//列地址设置
	LCD_WR_DATA(x1);
	LCD_WR_DATA(x2);
	LCD_WR_REG(RASET);//行地址设置
	LCD_WR_DATA(y1);
	LCD_WR_DATA(y2);
	LCD_WR_REG(RAMWR);//储存器写
}


/******************************************************************************
      函数说明：LCD调节背光
      入口数据：dc,占空比,5%~100%
      返回值：  无
******************************************************************************/
void LCD_Set_Light(uint8_t dc)
{
//	if(dc>=5 && dc<=100)
//		__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_3,dc*PWM_PERIOD/100);
}


/******************************************************************************
      函数说明：LCD关闭背光
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_Close_Light(void)
{
	//__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_3,0);
	//HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_3);
}


/******************************************************************************
      函数说明：LCD开启背光
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_Open_Light(void)
{
	//HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
}

/******************************************************************************
      函数说明：ST7789 SLEEP IN
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_ST7789_SleepIn(void)
{
	LCD_WR_REG(0x10);
	delay_ms(100);
}


/******************************************************************************
      函数说明：ST7789 SLEEP OUT
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_ST7789_SleepOut(void)
{
	LCD_WR_REG(0x11);
	delay_ms(100);
}


/******************************************************************************
      函数说明：LCD初始化
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_Init(void)
{
	LCD_GPIO_Init();//初始化GPIO
	LCD_CS_Clr();		//chip select
	
	LCD_RES_Clr();	//复位
	delay_ms(100);
	LCD_RES_Set();
	delay_ms(100);
	
	LCD_WR_REG(0x11);	//0x11 SLEEP OUT寄存器，控制驱动IC退出休眠
	delay_ms(120); 
	LCD_WR_REG(0x36); 
	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
	else LCD_WR_DATA8(0xA0);

	LCD_WR_REG(0x3A);
	// LCD_WR_DATA8(0x05);		//使用RGB565颜色数据格式
	LCD_WR_DATA8(0x55);

	//在SPI模式下，设置画面与画面之间的缓冲间隙，一下参数保持默认即可
	LCD_WR_REG(0xB2);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x33);

	//调节屏幕内部升压电路的栅极驱动电压。屏幕上电默认即可，别去改它，改不对会把屏幕烧坏
	 LCD_WR_REG(0xB7);
	 LCD_WR_DATA8(0x35);

	//屏幕模组的公共电压，厂家已经调好的参数，别去改变就好
	LCD_WR_REG(0xBB);
	LCD_WR_DATA8(0x19);

	//DeepSeek推荐这个寄存器的值写0x00,防止屏幕方向和颜色偷偷互换
	LCD_WR_REG(0xC0);
	LCD_WR_DATA8(0x2C);
	// LCD_WR_DATA8(0x00);

	//VDV和VRH的写使能开关，这里将其打开，否则后续有些设置的调节会不起作用
	LCD_WR_REG(0xC2);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0xFF);

	//设置屏幕内部源极驱动电压
	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x12);   

	//设置屏幕内部基准偏移电压
	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x20);  

	//正常模式帧率控制 设置点反转模式，60Hz帧率上限
	LCD_WR_REG(0xC6); 
	LCD_WR_DATA8(0x0F);    

	//控制屏幕内部电源升压的电路的输出电压，厂商给的参数，别改即可
	LCD_WR_REG(0xD0); 
	LCD_WR_DATA8(0xA4);
	LCD_WR_DATA8(0xA1);


	LCD_WR_REG(0xE0);
	LCD_WR_DATA8(0xD0);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x11);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x2B);
	LCD_WR_DATA8(0x3F);
	LCD_WR_DATA8(0x54);
	LCD_WR_DATA8(0x4C);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x0B);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x23);

	LCD_WR_REG(0xE1);
	LCD_WR_DATA8(0xD0);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x11);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x2C);
	LCD_WR_DATA8(0x3F);
	LCD_WR_DATA8(0x44);
	LCD_WR_DATA8(0x51);
	LCD_WR_DATA8(0x2F);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x20);
	LCD_WR_DATA8(0x23);

	//开启颜色反转 开启后，颜色才正常
	LCD_WR_REG(0x21);

	//开启显示，只有开启后内存里的图像才会显示到屏幕
	LCD_WR_REG(0x29);
}



