#include "main.h"

int main(void)
{
    extern struct ncov_data dataChina;
    extern struct ncov_data dataGlobal;

    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_2);
    led_init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ�
    delay_init();	    //��ʱ������ʼ��
    uart_init(115200);
    usart2_init(115200);
    lcd_init(WHITE);
    gui_show_bar();
    mem_init();             //��ʼ���ڴ��

    atk_8266_wifista_config();
    build_ssl_connect(api_type, api_ip, api_port);
    get_ncov_api(api_addr, parse_ncov_data);

    while(1)
    {
        switch (key_scan(0))
        {
        case 1:
//            build_ssl_connect(api_type, api_ip, api_port);
            get_ncov_api(api_addr, parse_ncov_data);
            break;
        case 2:
            break;
        default:
            break;
        }
        LD0 = 0;
        LD1 = 0;
        delay_ms(100);
        LD0 = 1;
        LD1 = 1;
        delay_ms(100);
    }
}
