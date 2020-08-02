#include "drv_9341.h"
#include "stdlib.h"
#include "stdio.h"
#include "delay.h"

//LCD�Ļ�����ɫ�ͱ���ɫ
uint16_t POINT_COLOR = 0x0000; //������ɫ
uint16_t BACK_COLOR = 0xFFFF; //����ɫ

//����LCD��Ҫ����
//Ĭ��Ϊ����
_lcd_dev lcddev;

void lcd_write_cmd(uint16_t data)
{
    LCD_RS = 0;//д��ַ
    LCD_CS = 0;
    LCD_DATA_OUT(data);
    LCD_WR = 0;
    LCD_WR = 1;
    LCD_CS = 1;
}
//д���ݺ���
//�������lcd_write_dataX��,��ʱ�任�ռ�.
//data:�Ĵ���ֵ
void lcd_write_dataX(uint16_t data)
{
    LCD_RS = 1;
    LCD_CS = 0;
    LCD_DATA_OUT(data);
    LCD_WR = 0;
    LCD_WR = 1;
    LCD_CS = 1;
}

uint16_t lcd_read_data(void)
{
    uint16_t t;
    GPIOB->CRL = 0X88888888; //PB0-7  ��������
    GPIOB->CRH = 0X88888888; //PB8-15 ��������
    GPIOB->ODR = 0X0000;   //ȫ�����0

    LCD_RS = 1;
    LCD_CS = 0;
    //��ȡ����(���Ĵ���ʱ,������Ҫ��2��)
    LCD_RD = 0;
    t = LCD_DATA_IN;
    LCD_RD = 1;
    LCD_CS = 1;

    GPIOB->CRL = 0X33333333; //PB0-7  �������
    GPIOB->CRH = 0X33333333; //PB8-15 �������
    GPIOB->ODR = 0XFFFF;  //ȫ�������
    return t;
}

void lcd_write_reg(uint16_t addr, uint16_t LCD_RegValue)
{
    lcd_write_cmd(addr);
    lcd_write_data(LCD_RegValue);
}

uint16_t lcd_read_reg(uint16_t addr)
{
    lcd_write_cmd(addr);  //д��Ҫ���ļĴ�����
    return lcd_read_data();
}

void opt_delay(uint8_t i)
{
    while (i--);
}

void lcd_displayON(void)
{
    lcd_write_cmd(0X29); //������ʾ
}

void lcd_displayOFF(void)
{
    lcd_write_cmd(0X28);
}

void lcd_setCursor(uint16_t Xpos, uint16_t Ypos)
{
    lcd_write_cmd(lcddev.setxcmd);
    lcd_write_data(Xpos >> 8);
    lcd_write_data(Xpos & 0XFF);
    lcd_write_cmd(lcddev.setycmd);
    lcd_write_data(Ypos >> 8);
    lcd_write_data(Ypos & 0XFF);
}

void lcd_setScanDir(uint8_t dir)
{
    uint16_t regval = 0;
    uint16_t dirreg = 0;
    uint16_t temp;

    switch (dir) //����ת��
    {
    case 0:
        dir = 6;
        break;
    case 1:
        dir = 7;
        break;
    case 2:
        dir = 4;
        break;
    case 3:
        dir = 5;
        break;
    case 4:
        dir = 1;
        break;
    case 5:
        dir = 0;
        break;
    case 6:
        dir = 3;
        break;
    case 7:
        dir = 2;
        break;
    }

    switch (dir)
    {
    case L2R_U2D://������,���ϵ���
        regval |= (0 << 7) | (0 << 6) | (0 << 5);
        break;
    case L2R_D2U://������,���µ���
        regval |= (1 << 7) | (0 << 6) | (0 << 5);
        break;
    case R2L_U2D://���ҵ���,���ϵ���
        regval |= (0 << 7) | (1 << 6) | (0 << 5);
        break;
    case R2L_D2U://���ҵ���,���µ���
        regval |= (1 << 7) | (1 << 6) | (0 << 5);
        break;
    case U2D_L2R://���ϵ���,������
        regval |= (0 << 7) | (0 << 6) | (1 << 5);
        break;
    case U2D_R2L://���ϵ���,���ҵ���
        regval |= (0 << 7) | (1 << 6) | (1 << 5);
        break;
    case D2U_L2R://���µ���,������
        regval |= (1 << 7) | (0 << 6) | (1 << 5);
        break;
    case D2U_R2L://���µ���,���ҵ���
        regval |= (1 << 7) | (1 << 6) | (1 << 5);
        break;
    }
    dirreg = 0X36;

    regval |= 0X08; //5310/5510/1963����ҪBGR

    lcd_write_reg(dirreg, regval);

    if (regval & 0X20)
    {
        if (lcddev.width < lcddev.height) //����X,Y
        {
            temp = lcddev.width;
            lcddev.width = lcddev.height;
            lcddev.height = temp;
        }
    }
    else
    {
        if (lcddev.width > lcddev.height) //����X,Y
        {
            temp = lcddev.width;
            lcddev.width = lcddev.height;
            lcddev.height = temp;
        }
    }

    lcd_write_cmd(lcddev.setxcmd);
    lcd_write_data(0);
    lcd_write_data(0);
    lcd_write_data((lcddev.width - 1) >> 8);
    lcd_write_data((lcddev.width - 1) & 0XFF);
    lcd_write_cmd(lcddev.setycmd);
    lcd_write_data(0);
    lcd_write_data(0);
    lcd_write_data((lcddev.height - 1) >> 8);
    lcd_write_data((lcddev.height - 1) & 0XFF);
}

void lcd_drawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_write_cmd(lcddev.setxcmd);
    lcd_write_data(x >> 8);
    lcd_write_data(x & 0XFF);
    lcd_write_cmd(lcddev.setycmd);
    lcd_write_data(y >> 8);
    lcd_write_data(y & 0XFF);

    LCD_RS = 0;
    LCD_CS = 0;
    LCD_DATA_OUT(lcddev.wramcmd);//дָ��
    LCD_WR = 0;
    LCD_WR = 1;
    LCD_CS = 1;
    lcd_write_data(color);     //д����
}
//SSD1963 ��������
//pwm:����ȼ�,0~100.Խ��Խ��.
void lcd_setBL(uint8_t pwm)
{
    lcd_write_cmd(0xBE);   //����PWM���
    lcd_write_data(0x05);  //1����PWMƵ��
    lcd_write_data(pwm * 2.55); //2����PWMռ�ձ�
    lcd_write_data(0x01);  //3����C
    lcd_write_data(0xFF);  //4����D
    lcd_write_data(0x00);  //5����E
    lcd_write_data(0x00);  //6����F
}
//����LCD��ʾ����
//dir:0,������1,����
void lcd_setDisplayDir(uint8_t dir)
{
    if (dir == 0)       //����
    {
        lcddev.dir = 0; //����
        lcddev.width = 240;
        lcddev.height = 320;
        lcddev.wramcmd = 0X2C;
        lcddev.setxcmd = 0X2A;
        lcddev.setycmd = 0X2B;
    }
    else                //����
    {
        lcddev.dir = 1; //����
        lcddev.width = 320;
        lcddev.height = 240;
        lcddev.wramcmd = 0X2C;
        lcddev.setxcmd = 0X2A;
        lcddev.setycmd = 0X2B;
    }
    lcd_setScanDir(DFT_SCAN_DIR); //Ĭ��ɨ�跽��
}

void lcd_init(uint16_t color)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); //ʹ��PORTB,Cʱ�Ӻ�AFIOʱ��
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //����SWD��ʧ��JTAG

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_7 | GPIO_Pin_6; ///PORTC6~10�����������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure); //GPIOC

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;    //��������PA8
    GPIO_Init(GPIOA, &GPIO_InitStructure); //GPIOC

    GPIO_SetBits(GPIOC, GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_7 | GPIO_Pin_6);

    GPIO_SetBits(GPIOA, GPIO_Pin_8);    //��������

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All; //  PORTB�������
    GPIO_Init(GPIOB, &GPIO_InitStructure); //GPIOB

    GPIO_SetBits(GPIOB, GPIO_Pin_All);

    delay_ms(50); // delay 50 ms
    lcd_write_reg(0x0000, 0x0001);
    delay_ms(50); // delay 50 ms
    lcddev.id = lcd_read_reg(0x0000);
    if (lcddev.id < 0XFF || lcddev.id == 0XFFFF || lcddev.id == 0X9300) //����ID����ȷ,����lcddev.id==0X9300�жϣ���Ϊ9341��δ����λ������»ᱻ����9300
    {
        //����9341 ID�Ķ�ȡ
        lcd_write_cmd(0XD3);
        lcd_read_data();              //dummy read
        lcd_read_data();              //����0X00
        lcddev.id = lcd_read_data();  //��ȡ93
        lcddev.id <<= 8;
        lcddev.id |= lcd_read_data(); //��ȡ41

    }
//    printf(" LCD ID:%x\r\n", lcddev.id); //��ӡLCD ID

    lcd_write_cmd(0xCF);
    lcd_write_dataX(0x00);
    lcd_write_dataX(0xC1);
    lcd_write_dataX(0X30);
    lcd_write_cmd(0xED);
    lcd_write_dataX(0x64);
    lcd_write_dataX(0x03);
    lcd_write_dataX(0X12);
    lcd_write_dataX(0X81);
    lcd_write_cmd(0xE8);
    lcd_write_dataX(0x85);
    lcd_write_dataX(0x10);
    lcd_write_dataX(0x7A);
    lcd_write_cmd(0xCB);
    lcd_write_dataX(0x39);
    lcd_write_dataX(0x2C);
    lcd_write_dataX(0x00);
    lcd_write_dataX(0x34);
    lcd_write_dataX(0x02);
    lcd_write_cmd(0xF7);
    lcd_write_dataX(0x20);
    lcd_write_cmd(0xEA);
    lcd_write_dataX(0x00);
    lcd_write_dataX(0x00);
    lcd_write_cmd(0xC0);    //Power control
    lcd_write_dataX(0x1B);   //VRH[5:0]
    lcd_write_cmd(0xC1);    //Power control
    lcd_write_dataX(0x01);   //SAP[2:0];BT[3:0]
    lcd_write_cmd(0xC5);    //VCM control
    lcd_write_dataX(0x30);      //3F
    lcd_write_dataX(0x30);      //3C
    lcd_write_cmd(0xC7);    //VCM control2
    lcd_write_dataX(0XB7);
    lcd_write_cmd(0x36);    // Memory Access Control
    lcd_write_dataX(0x48);
    lcd_write_cmd(0x3A);
    lcd_write_dataX(0x55);
    lcd_write_cmd(0xB1);
    lcd_write_dataX(0x00);
    lcd_write_dataX(0x1A);
    lcd_write_cmd(0xB6);    // Display Function Control
    lcd_write_dataX(0x0A);
    lcd_write_dataX(0xA2);
    lcd_write_cmd(0xF2);    // 3Gamma Function Disable
    lcd_write_dataX(0x00);
    lcd_write_cmd(0x26);    //Gamma curve selected
    lcd_write_dataX(0x01);
    lcd_write_cmd(0xE0);    //Set Gamma
    lcd_write_dataX(0x0F);
    lcd_write_dataX(0x2A);
    lcd_write_dataX(0x28);
    lcd_write_dataX(0x08);
    lcd_write_dataX(0x0E);
    lcd_write_dataX(0x08);
    lcd_write_dataX(0x54);
    lcd_write_dataX(0XA9);
    lcd_write_dataX(0x43);
    lcd_write_dataX(0x0A);
    lcd_write_dataX(0x0F);
    lcd_write_dataX(0x00);
    lcd_write_dataX(0x00);
    lcd_write_dataX(0x00);
    lcd_write_dataX(0x00);
    lcd_write_cmd(0XE1);    //Set Gamma
    lcd_write_dataX(0x00);
    lcd_write_dataX(0x15);
    lcd_write_dataX(0x17);
    lcd_write_dataX(0x07);
    lcd_write_dataX(0x11);
    lcd_write_dataX(0x06);
    lcd_write_dataX(0x2B);
    lcd_write_dataX(0x56);
    lcd_write_dataX(0x3C);
    lcd_write_dataX(0x05);
    lcd_write_dataX(0x10);
    lcd_write_dataX(0x0F);
    lcd_write_dataX(0x3F);
    lcd_write_dataX(0x3F);
    lcd_write_dataX(0x0F);
    lcd_write_cmd(0x2B);
    lcd_write_dataX(0x00);
    lcd_write_dataX(0x00);
    lcd_write_dataX(0x01);
    lcd_write_dataX(0x3f);
    lcd_write_cmd(0x2A);
    lcd_write_dataX(0x00);
    lcd_write_dataX(0x00);
    lcd_write_dataX(0x00);
    lcd_write_dataX(0xef);
    lcd_write_cmd(0x11); //Exit Sleep
    delay_ms(120);
    lcd_write_cmd(0x29); //display on
    lcd_setDisplayDir(1);         //1Ϊ����
    LCD_BL = 1;                //��������
    lcd_clear(color);
}

void lcd_clear(uint16_t color)
{
    u32 index = 0;
    u32 totalpoint = lcddev.width;
    totalpoint *= lcddev.height;        //�õ��ܵ���
    lcd_setCursor(0x00, 0x0000);   //���ù��λ��
    lcd_write_cmd(lcddev.wramcmd);             //��ʼд��GRAM
    for (index = 0; index < totalpoint; index++)
        lcd_write_data(color);
}

void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    uint16_t i, j;
    uint16_t xlen = 0;

    xlen = ex - sx + 1;
    for (i = sy; i <= ey; i++)
    {
        lcd_setCursor(sx, i);                   //���ù��λ��
        lcd_write_cmd(lcddev.wramcmd);                 //��ʼд��GRAM
        for (j = 0; j < xlen; j++)
            lcd_write_data(color); //���ù��λ��
    }
}
