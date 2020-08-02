#include "delay.h"
#include "drv_usart2.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "drv_timer.h"

//���ڽ��ջ�����
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN];              //���ջ���,���USART2_MAX_RECV_LEN���ֽ�.
u8  USART2_TX_BUF[USART2_MAX_SEND_LEN];             //���ͻ���,���USART2_MAX_SEND_LEN�ֽ�

//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
vu16 USART2_RX_STA = 0;


void USART2_IRQHandler(void)
{
    u8 res;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //���յ�����
    {
        res = USART_ReceiveData(USART2);
        if ((USART2_RX_STA & (1 << 15)) == 0) //�������һ������,��û�б�����,���ٽ�����������
        {
            if (USART2_RX_STA < USART2_MAX_RECV_LEN) //�����Խ�������
            {
                TIM_SetCounter(TIM3, 0); //���������                         //���������
                if (USART2_RX_STA == 0)             //ʹ�ܶ�ʱ��3���ж�
                {
                    TIM_Cmd(TIM3, ENABLE); //ʹ�ܶ�ʱ��3
                }
                USART2_RX_BUF[USART2_RX_STA++] = res; //��¼���յ���ֵ
            }
            else
            {
                USART2_RX_STA |= 1 << 15;           //ǿ�Ʊ�ǽ������
            }
        }
    }
}


//��ʼ��IO ����3
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������
void usart2_init(u32 bound)
{
    GPIO_InitTypeDef gpio_initstruct;
    USART_InitTypeDef usart_initstruct;
    NVIC_InitTypeDef nvic_initstruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    //PA2   TXD
    gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_initstruct.GPIO_Pin = GPIO_Pin_2;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_initstruct);
    //PA3   RXD
    gpio_initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_initstruct.GPIO_Pin = GPIO_Pin_3;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_initstruct);

    usart_initstruct.USART_BaudRate = bound;
    usart_initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;        //��Ӳ������
    usart_initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                        //���պͷ���
    usart_initstruct.USART_Parity = USART_Parity_No;                                    //��У��
    usart_initstruct.USART_StopBits = USART_StopBits_1;                             //1λֹͣλ
    usart_initstruct.USART_WordLength = USART_WordLength_8b;                            //8λ����λ
    USART_Init(USART2, &usart_initstruct);
    USART_Cmd(USART2, ENABLE);                                                      //ʹ�ܴ���
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                                  //ʹ�ܽ����ж�

    nvic_initstruct.NVIC_IRQChannel = USART2_IRQn;
    nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_initstruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvic_initstruct);

    TIM3_Int_Init(1000 - 1, 7200 - 1);  //10ms�ж�
    USART2_RX_STA = 0;      //����
    TIM_Cmd(TIM3, DISABLE);         //�رն�ʱ��7
}

//����3,printf ����
//ȷ��һ�η������ݲ�����USART2_MAX_SEND_LEN�ֽ�
void u2_printf(char *fmt, ...)
{
    u16 i, j;
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char *)USART2_TX_BUF, fmt, ap);
    va_end(ap);
    i = strlen((const char *)USART2_TX_BUF);    //�˴η������ݵĳ���
    for (j = 0; j < i; j++)                     //ѭ����������
    {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET); //ѭ������,ֱ���������
        USART_SendData(USART2, USART2_TX_BUF[j]);
    }
}
void Usart_SendString(USART_TypeDef *USARTx, u8 *str)
{
    while (*str)
    {
        USART_SendData(USARTx, *str);                                   //��������
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);    //�ȴ��������
        str++;
    }
}


