/**
  ******************************************************************************
  * @file    lcd_screen.h
  * @author  粤嵌温工+FZetc飞贼整理
  * @version V0.0.1
  * @date    2026.04.19
  * @brief   说明：lcd相关参数、变量、函数功能等
  *
  ******************************************************************************
  * @attention
  *
  * 本文档只供学习使用，不得商用，违者必究
  *
  * 微信公众号：    FZetc飞贼
  * 全视频平台：    FZetc飞贼
  * CSDN博客：      https://blog.csdn.net/qq_58629108?type=blog
  * 有疑问或者建议：FZetcSnitch@163.com
  *
  ******************************************************************************
  */
 
#ifndef __LCD_SCREEN_H		// 定义以防止递归包含
#define __LCD_SCREEN_H


// 一、其它头文件
#include "stm32f4xx.h"                
#include "my_config.h"
#include "my_lib.h"

// 二、宏定义(函数、变量、常量)
#define	LCD_LED PBout(15)  				// LCD背光    		 PB15 	 

// 使用NOR/SRAM的 Bank1.sector4,地址位HADDR[27,26]=11 A6作为数据命令区分线 
// 注意设置时STM32内部会右移一位对其! 111 1110=0X7E			    
#define LCD_BASE        ((u32)(0x6C000000 | 0x0000007E))
#define LCD             ((LCD_TypeDef *) LCD_BASE)
	 
// 扫描方向定义
#define L2R_U2D  0 						// 从左到右,从上到下
#define L2R_D2U  1 						// 从左到右,从下到上
#define R2L_U2D  2 						// 从右到左,从上到下
#define R2L_D2U  3 						// 从右到左,从下到上
#define U2D_L2R  4 						// 从上到下,从左到右
#define U2D_R2L  5 						// 从上到下,从右到左
#define D2U_L2R  6 						// 从下到上,从左到右
#define D2U_R2L  7 						// 从下到上,从右到左	 

#define DFT_SCAN_DIR  L2R_U2D  			// 默认的扫描方向

// 画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 		// 棕色
#define BRRED 			 0XFC07 		// 棕红色
#define GRAY  			 0X8430 		// 灰色

// GUI颜色
#define DARKBLUE      	 0X01CF			// 深蓝色
#define LIGHTBLUE      	 0X7D7C			// 浅蓝色  
#define GRAYBLUE       	 0X5458 		// 灰蓝色
#define LIGHTGREEN     	 0X841F 		// 浅绿色
//#define LIGHTGRAY      0XEF5B 		// 浅灰色(PANNEL)
#define LGRAY 			 0XC618 		// 浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 		// 浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 		// 浅棕蓝色(选择条目的反色)

// LCD分辨率设置
#define SSD_HOR_RESOLUTION		800		// LCD水平分辨率
#define SSD_VER_RESOLUTION		480		// LCD垂直分辨率

// LCD驱动参数设置
#define SSD_HOR_PULSE_WIDTH		1		// 水平脉宽
#define SSD_HOR_BACK_PORCH		46		// 水平前廊
#define SSD_HOR_FRONT_PORCH		210		// 水平后廊
#define SSD_VER_PULSE_WIDTH		1		// 垂直脉宽
#define SSD_VER_BACK_PORCH		23		// 垂直前廊
#define SSD_VER_FRONT_PORCH		22		// 垂直前廊

// 如下几个参数，自动计算
#define SSD_HT	(SSD_HOR_RESOLUTION+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
#define SSD_HPS	(SSD_HOR_BACK_PORCH)
#define SSD_VT 	(SSD_VER_RESOLUTION+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH)
#define SSD_VPS (SSD_VER_BACK_PORCH)

// 三、自定义的数据类型(结构体、联合体、枚举等)
// LCD重要参数集
typedef struct  
{										    
	u16 width;			// LCD 宽度
	u16 height;			// LCD 高度
	u16 id;				// LCD ID
	u8  dir;			// 横屏还是竖屏控制：0，竖屏；1，横屏。	
	u16	wramcmd;		// 开始写gram指令
	u16  setxcmd;		// 设置x坐标指令
	u16  setycmd;		// 设置y坐标指令 
}_lcd_dev; 	

// LCD地址结构体
typedef struct
{
	vu16 LCD_REG;
	vu16 LCD_RAM;
} LCD_TypeDef;


// 四、全局变量声明
// LCD参数
extern _lcd_dev lcddev;		// 管理LCD重要参数
// LCD的画笔颜色和背景色	   
extern u16  POINT_COLOR;	// 默认红色    
extern u16  BACK_COLOR; 	// 背景颜色.默认为白色

// 五、函数声明
extern void LCD_Init(void);													   	// 初始化
extern void LCD_DisplayOn(void);												// 开显示
extern void LCD_DisplayOff(void);												// 关显示
extern void LCD_Clear(u16 Color);	 											// 清屏
extern void LCD_SetCursor(u16 Xpos, u16 Ypos);									// 设置光标
extern void LCD_DrawPoint(u16 x,u16 y);											// 画点
extern void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color);							// 快速画点
extern u16  LCD_ReadPoint(u16 x,u16 y); 										// 读点 
extern void LCD_Draw_Circle(u16 x0,u16 y0,u8 r);						 		// 画圆
extern void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);						// 画线
extern void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);		   			// 画矩形
extern void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);		   			// 填充单色
extern void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);				// 填充指定颜色
extern void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);					// 显示一个字符
extern void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);  					// 显示一个数字
extern void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);			// 显示 数字
extern void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);		// 显示一个字符串,12/16字体
extern void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue);
extern u16  LCD_ReadReg(u16 LCD_Reg);
extern void LCD_WriteRAM_Prepare(void);
extern void LCD_WriteRAM(u16 RGB_Code);
extern void LCD_SSD_BackLightSet(u8 pwm);										// SSD1963 背光控制
extern void LCD_Scan_Dir(u8 dir);												// 设置屏扫描方向
extern void LCD_Display_Dir(u8 dir);											// 设置屏幕显示方向
extern void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height);					// 设置窗口		
extern void LCD_BackLight(uint8_t sta);
// 以下代码是额外添加的函数，粤嵌.温工
extern void lcd_show_string(uint32_t x, uint32_t y,const char *str, uint32_t fc, uint32_t bc,uint32_t font_size);
extern void lcd_draw_image(uint16_t x_s, uint16_t y_s, uint16_t width, uint16_t height, const uint8_t *image);

	
// 六、静态变量、静态函数定义

#endif 	/* __LCD_SCREEN_H */
