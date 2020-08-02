#include "wifista.h"
#include "usart.h"
#include "delay.h"
#include "drv_usart2.h"
#include "malloc.h"
#include "stdlib.h"
#include "string.h"
#include "config.h"

//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������USART2_RX_STA;
//     1,����USART2_RX_STA;
void atk_8266_at_response(u8 mode)
{
    if (USART2_RX_STA & 0X8000)     //���յ�һ��������
    {
        USART2_RX_BUF[USART2_RX_STA & 0X7FFF] = 0; //��ӽ�����
        printf("%s", USART2_RX_BUF); //���͵�����
        if (mode)USART2_RX_STA = 0;
    }
}
//ATK-ESP8266���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
u8 *atk_8266_check_cmd(u8 *str)
{
    char *strx = 0;
    if (USART2_RX_STA & 0X8000)     //���յ�һ��������
    {
        USART2_RX_BUF[USART2_RX_STA & 0X7FFF] = 0; //��ӽ�����
        strx = strstr((const char *)USART2_RX_BUF, (const char *)str);
    }
    return (u8 *)strx;
}
//��ATK-ESP8266��������
//cmd:���͵������ַ���
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 atk_8266_send_cmd(u8 *cmd, u8 *ack, u16 waittime)
{
    u8 res = 0;
    USART2_RX_STA = 0;
    u2_printf("%s\r\n", cmd);   //��������
    if (ack && waittime)    //��Ҫ�ȴ�Ӧ��
    {
        while (--waittime)  //�ȴ�����ʱ
        {
            delay_ms(10);
            if (USART2_RX_STA & 0X8000) //���յ��ڴ���Ӧ����
            {
                if (atk_8266_check_cmd(ack))
                {
                    printf("���ͣ�%s ��Ӧ:%s\r\n", cmd, (u8 *)ack);
                    break;//�õ���Ч����
                }
                USART2_RX_STA = 0;
            }
        }
        if (waittime == 0)res = 1;
    }
    return res;
}
//��ATK-ESP8266����ָ������
//data:���͵�����(����Ҫ��ӻس���)
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)luojian
u8 atk_8266_send_data(u8 *data, u8 *ack, u16 waittime)
{
    u8 res = 0;
    USART2_RX_STA = 0;
    u2_printf("%s", data);  //��������
    if (ack && waittime)    //��Ҫ�ȴ�Ӧ��
    {
        while (--waittime)  //�ȴ�����ʱ
        {
            delay_ms(10);
            if (USART2_RX_STA & 0X8000) //���յ��ڴ���Ӧ����
            {
                if (atk_8266_check_cmd(ack))break; //�õ���Ч����
                USART2_RX_STA = 0;
            }
        }
        if (waittime == 0)res = 1;
    }
    return res;
}
//ATK-ESP8266�˳�͸��ģʽ
//����ֵ:0,�˳��ɹ�;
//       1,�˳�ʧ��
u8 atk_8266_quit_trans(void)
{
    while ((USART2->SR & 0X40) == 0); //�ȴ����Ϳ�
    USART2->DR = '+';
    delay_ms(15);                   //���ڴ�����֡ʱ��(10ms)
    while ((USART2->SR & 0X40) == 0); //�ȴ����Ϳ�
    USART2->DR = '+';
    delay_ms(15);                   //���ڴ�����֡ʱ��(10ms)
    while ((USART2->SR & 0X40) == 0); //�ȴ����Ϳ�
    USART2->DR = '+';
    delay_ms(500);                  //�ȴ�500ms
    return atk_8266_send_cmd("AT", "OK", 20); //�˳�͸���ж�.
}

//��ȡATK-ESP8266ģ�������״̬
//����ֵ:0,δ����;1,���ӳɹ�.
u8 atk_8266_consta_check(void)
{
    u8 *p;
    u8 res;
    if (atk_8266_quit_trans())return 0;         //�˳�͸��
    atk_8266_send_cmd("AT+CIPSTATUS", ":", 50); //����AT+CIPSTATUSָ��,��ѯ����״̬
    p = atk_8266_check_cmd("+CIPSTATUS:");
    res = *p;                               //�õ�����״̬
    return res;
}

//��ȡClient ip��ַ
//ipbuf:ip��ַ���������
void atk_8266_get_wanip(u8 *ipbuf)
{
    u8 *p, *p1;
    if (atk_8266_send_cmd("AT+CIFSR", "OK", 50)) //��ȡWAN IP��ַʧ��
    {
        ipbuf[0] = 0;
        return;
    }
    p = atk_8266_check_cmd("\"");
    p1 = (u8 *)strstr((const char *)(p + 1), "\"");
    *p1 = 0;
    sprintf((char *)ipbuf, "%s", p + 1);
}


//����ESP8266λstaģʽ�������ӵ�·����
u8 atk_8266_wifista_config(void)
{
    u8 p[200];
	char str[200];
    u8 TryConnectTime = 1;
	printf("׼������\r\n");
    while (atk_8266_send_cmd("AT", "OK", 20)) //���WIFIģ���Ƿ�����
    {
        printf("δ��⵽ģ��\r\n");
        atk_8266_quit_trans();//�˳�͸��
		delay_ms(1000);
    }
    atk_8266_send_cmd("AT+RESTORE", "OK", 200); //�ر�͸��ģʽ
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    
    atk_8266_send_cmd("AT+CIPMODE=0", "OK", 200); //�ر�͸��ģʽ
    while (atk_8266_send_cmd("ATE0", "OK", 20)); //�رջ���
    atk_8266_send_cmd("AT+CWMODE=1", "OK", 50);     //����WIFI STAģʽ
//    atk_8266_send_cmd("AT+RST", "OK", 20);      //DHCP�������ر�(��APģʽ��Ч)
	delay_ms(1000);         //��ʱ3S�ȴ������ɹ�
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    //�������ӵ���WIFI��������/���ܷ�ʽ/����,�⼸��������Ҫ�������Լ���·�������ý����޸�!!
    atk_8266_send_cmd("AT+CIPMUX=0", "OK", 20); //0�������ӣ�1��������
    sprintf((char *)p, "AT+CWJAP=\"%s\",\"%s\"", wifista_ssid, wifista_password); //�������߲���:ssid,����

	sprintf((char *)str, "%s", wifista_ssid);
//	LCD_ShowString(125, 20, 200, 16, 16, str);

	while (atk_8266_send_cmd(p, "WIFI GOT IP", 300))
    {
        printf("WiFi����ʧ��,���ڳ��Ե� %d ������\r\n", TryConnectTime++);
        if (TryConnectTime >= 250)
            TryConnectTime = 0;
        delay_ms(100);
    };                  //����Ŀ��·����,���һ��IP
    printf("WiFi���ӳɹ�\r\n");
//	LCD_ShowString(0,60,100, 16, 16, "WiFi Connect Success!");
    return 0;
}

