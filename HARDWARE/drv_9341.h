#ifndef __DRV_9341_H__
#define __DRV_9341_H__

#include "sys.h"

/**
LCD_BL = PA8
LCD_CS = PC9
LCD_RS = PC8
LCD_WR = PC7
LCD_RD = PC6
LCD_DATA_INT = GPIOB->IDR
LCD_DATA_OUT(x) GPIOB->ODR=x; 
**/

#define LCD_BL  PAout(8)
#define LCD_CS  PCout(9)
#define LCD_RS  PCout(8)
#define LCD_WR  PCout(7)
#define LCD_RD  PCout(6)
#define LCD_DATA_OUT(data) GPIOB->ODR=data
#define LCD_DATA_IN     GPIOB->IDR

//LCD��Ҫ������
typedef struct
{
    uint16_t width;          //LCD ���
    uint16_t height;         //LCD �߶�
    uint16_t id;             //LCD ID
    uint8_t  dir;            //���������������ƣ�0��������1��������
    uint16_t wramcmd;        //��ʼдgramָ��
    uint16_t setxcmd;        //����x����ָ��
    uint16_t  setycmd;       //����y����ָ��
} _lcd_dev;

//LCD����
extern _lcd_dev lcddev; //����LCD��Ҫ����
//LCD�Ļ�����ɫ�ͱ���ɫ
extern uint16_t  POINT_COLOR;//Ĭ�Ϻ�ɫ
extern uint16_t  BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ

#define L2R_U2D  0 //������,���ϵ���
#define L2R_D2U  1 //������,���µ���
#define R2L_U2D  2 //���ҵ���,���ϵ���
#define R2L_D2U  3 //���ҵ���,���µ���

#define U2D_L2R  4 //���ϵ���,������
#define U2D_R2L  5 //���ϵ���,���ҵ���
#define D2U_L2R  6 //���µ���,������
#define D2U_R2L  7 //���µ���,���ҵ���


//#define DFT_SCAN_DIR  L2R_U2D  //��Ļ��ת180��
#define DFT_SCAN_DIR  R2L_D2U

//������ɫ
#define WHITE            0xFFFF
#define BLACK            0x0000
#define BLUE             0x001F
#define BRED             0XF81F
#define GRED             0XFFE0
#define GBLUE            0X07FF
#define RED              0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN            0XBC40 //��ɫ
#define BRRED            0XFC07 //�غ�ɫ
#define GRAY             0X8430 //��ɫ
//GUI��ɫ

#define DARKBLUE         0X01CF //����ɫ
#define LIGHTBLUE        0X7D7C //ǳ��ɫ  
#define GRAYBLUE         0X5458 //����ɫ
//������ɫΪPANEL����ɫ

#define LIGHTGREEN       0X841F //ǳ��ɫ 
#define LGRAY            0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)
#define LCD_SIZE_X 240
#define LCD_SIZE_Y 320

void lcd_init(uint16_t color);                                                     //��ʼ��
void lcd_clear(uint16_t color);                                                  //����
void lcd_setCursor(uint16_t Xpos, uint16_t Ypos);                                     //���ù��
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);                   //��䵥ɫ
void lcd_drawPoint(uint16_t x, uint16_t y, uint16_t color);

void lcd_write_reg(uint16_t LCD_Reg, uint16_t LCD_RegValue);
uint16_t lcd_read_reg(uint16_t LCD_Reg);
void lcd_setScanDir(uint8_t dir);                                  //������ɨ�跽��
void lcd_setDisplayDir(uint8_t dir);                               //������Ļ��ʾ����
void lcd_setBL(uint8_t pwm);

//д���ݺ���
#define lcd_write_data(data){\
LCD_RS = 1;\
LCD_CS = 0;\
LCD_DATA_OUT(data);\
LCD_WR = 0;\
LCD_WR = 1;\
LCD_CS = 1;\
}


#endif


