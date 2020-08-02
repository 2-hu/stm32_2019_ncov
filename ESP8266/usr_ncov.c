#include "usr_ncov.h"
#include "time.h"

struct ncov_data dataChina = {12345678, -100, 23456789, 456, 4567890, -920, 123445, 12324, 12324, 123412, "06-13 16:22"};
struct ncov_data dataGlobal = {12345678, -100, 23456789, 456, 4567890, -920, 0, 0, 0, 0, NULL};

extern void gui_show_ncov_data(struct ncov_data china, struct ncov_data global);


uint8_t build_ssl_connect(char *type, char *ip, char *port)
{
    char str_tmp[200];
    uint8_t TryConnectTime = 0;
    atk_8266_send_cmd("AT+CIPMUX=0", "OK", 100);   
    atk_8266_send_cmd("AT+CIPSSLSIZE=4096", "OK", 100);  

    sprintf((char *)str_tmp, "AT+CIPSTART=\"%s\",\"%s\",%s", type, ip, port); 
    
    while (atk_8266_send_cmd((uint8_t *)str_tmp, "OK", 200))
    {
        printf("api����ʧ��,���ڳ��Ե� %d ������\r\r\n", TryConnectTime++);
        delay_ms(100);
        if (TryConnectTime >= 10)
        {
            return 1;
        }
    };  
    
    delay_ms(300);
    atk_8266_send_cmd("AT+CIPMODE=1", "OK", 100);    //����ģʽΪ��͸��

    USART2_RX_STA = 0;
    atk_8266_send_cmd("AT+CIPSEND", "OK", 100);       //��ʼ͸��
    return 0;
}

uint8_t get_ncov_api(char *api_addr, uint8_t (*parse_fun)(void))
{
    printf("��ȡ��������:GET %s\r\r\n", api_addr);
    u2_printf("GET %s\r\n\r\n", api_addr);

    delay_ms(20);
    USART2_RX_STA = 0;
    delay_ms(1000);

    if (USART2_RX_STA & 0X8000)     
    {
        USART2_RX_BUF[USART2_RX_STA & 0X7FFF] = 0; //��ӽ�����
    }
   
//	printf("���յ�������:%d-%d\r\r\n%s\r\r\n", sizeof(USART2_RX_BUF), strlen((const char*)USART2_RX_BUF),USART2_RX_BUF);	//JSONԭʼ����
   
    parse_fun();
    USART2_RX_STA = 0;
    memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
//    atk_8266_quit_trans();
//    printf("�˳�͸��\r\n");
    return 0;
}

uint8_t parse_ncov_data(void)
{
    int ret = 0;
    cJSON *root, *result_arr;
    cJSON *result, *global;
    time_t updateTime;
    struct tm *time;

    //root = cJSON_Parse((const char *)str);   //����JSON�������󣬷���JSON��ʽ�Ƿ���ȷ
    printf("���յ�������:%d\r\r\n", strlen((const char*)USART2_RX_BUF));	//JSONԭʼ����
    root = cJSON_Parse((const char*)USART2_RX_BUF);
    
    if (root != 0)
    {
        printf("JSON format ok, start parse!!!\r\n");
        result_arr = cJSON_GetObjectItem(root, "results");
        if(result_arr->type == cJSON_Array)
        {
            printf("result is array\r\n");
            result = cJSON_GetArrayItem(result_arr, 0);
            if(result->type == cJSON_Object)
            {
                printf("result_arr[0] is object\r\n");

                /* china data parse */
                dataChina.currentConfirmedCount = cJSON_GetObjectItem(result, "currentConfirmedCount")->valueint;
                dataChina.currentConfirmedIncr = cJSON_GetObjectItem(result, "currentConfirmedIncr")->valueint;
                dataChina.confirmedCount = cJSON_GetObjectItem(result, "confirmedCount")->valueint;
                dataChina.confirmedIncr = cJSON_GetObjectItem(result, "confirmedIncr")->valueint;
                dataChina.curedCount = cJSON_GetObjectItem(result, "curedCount")->valueint;
                dataChina.curedIncr = cJSON_GetObjectItem(result, "curedIncr")->valueint;
                dataChina.deadCount = cJSON_GetObjectItem(result, "deadCount")->valueint;
                dataChina.deadIncr = cJSON_GetObjectItem(result, "deadIncr")->valueint;

                printf("------------��������-------------\r\n");
                printf("�ִ�ȷ��:   %5d, ������:%3d\r\n", dataChina.currentConfirmedCount, dataChina.currentConfirmedIncr);
                printf("�ۼ�ȷ��:   %5d, ������:%3d\r\n", dataChina.confirmedCount, dataChina.confirmedIncr);
                printf("�ۼ�����:   %5d, ������:%3d\r\n", dataChina.curedCount, dataChina.curedIncr);
                printf("�ۼ�����:   %5d, ������:%3d\r\n", dataChina.deadCount, dataChina.deadIncr);
                printf("�ִ���֢״: %5d, ������:%3d\r\n\r\n", dataChina.seriousCount, dataChina.seriousIncr);

                global = cJSON_GetObjectItem(result, "globalStatistics");
                if(global->type == cJSON_Object)
                {
                    dataGlobal.currentConfirmedCount = cJSON_GetObjectItem(global, "currentConfirmedCount")->valueint;
                    dataGlobal.currentConfirmedIncr = cJSON_GetObjectItem(global, "currentConfirmedIncr")->valueint;
                    dataGlobal.confirmedCount = cJSON_GetObjectItem(global, "confirmedCount")->valueint;
                    dataGlobal.confirmedIncr = cJSON_GetObjectItem(global, "confirmedIncr")->valueint;
                    dataGlobal.curedCount = cJSON_GetObjectItem(global, "curedCount")->valueint;
                    dataGlobal.curedIncr = cJSON_GetObjectItem(global, "curedIncr")->valueint;
                    dataGlobal.deadCount = cJSON_GetObjectItem(global, "deadCount")->valueint;
                    dataGlobal.deadIncr = cJSON_GetObjectItem(global, "deadIncr")->valueint;

                    printf("\r\n**********global ncov data**********\r\n");

                    printf("------------ȫ������-------------\r\n");
                    printf("�ִ�ȷ��: %8d, ������:%5d\r\n", dataGlobal.currentConfirmedCount, dataGlobal.currentConfirmedIncr);
                    printf("�ۼ�ȷ��: %8d, ������:%5d\r\n", dataGlobal.confirmedCount, dataGlobal.confirmedIncr);
                    printf("�ۼ�����: %8d, ������:%5d\r\n", dataGlobal.deadCount, dataGlobal.deadIncr);
                    printf("�ۼ�����: %8d, ������:%5d\r\n\r\n", dataGlobal.curedCount, dataGlobal.curedIncr);

                } else return 1;
                
                
                /* ���뼶ʱ���ת�ַ��� */
                updateTime = (time_t )(cJSON_GetObjectItem(result, "updateTime")->valuedouble / 1000);
                updateTime += 8 * 60 * 60; /* UTC8У�� */
                time = localtime(&updateTime);
                /* ��ʽ��ʱ�� */
                strftime(dataChina.updateTime, 20, "%m-%d %H:%M", time);
                printf("������:%s\r\n", dataChina.updateTime);/* 06-24 11:21 */
            } else return 1;
        } else return 1;
        printf("\r\nparse complete \r\n");
        gui_show_ncov_data(dataChina, dataGlobal);
    }
    else
    {
        printf("JSON format error:%s\r\n", cJSON_GetErrorPtr()); //���json��ʽ������Ϣ
        return 1;
    }
    cJSON_Delete(root);

    return ret;
}

