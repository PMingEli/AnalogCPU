/*************************************************************************
    > File Name: AnalogCPU.c
    > Author: nian
    > Blog: https://whoisnian.com
    > Mail: zhuchangbao2017@gmail.com
    > Created Time: 2018��07��10�� ���ڶ� 23ʱ54��29��
 ************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<windows.h>

void PrintfColorfulMessage(HANDLE cmd, char *message, int type);

int main(void)
{
    // ������ cmd �е���ɫ��ʾ
    HANDLE cmd = GetStdHandle(STD_OUTPUT_HANDLE);
    // �򿪴���ͨ�Ŷ˿�
    HANDLE uart = CreateFileA("COM3", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if(uart == INVALID_HANDLE_VALUE)
    {
        PrintfColorfulMessage(cmd, "Open COM3 failed.\n\n", 1);
        system("pause");
        return 1;
    }
    else
    {
        PrintfColorfulMessage(cmd, "Open COM3 successfully.\n\n", 0);
    }

    // ���û�������С
    SetupComm(uart, 1024, 1024);
    // ���ó�ʱ
    COMMTIMEOUTS timeout;
    timeout.ReadIntervalTimeout = 1000;
    timeout.ReadTotalTimeoutMultiplier = 500;
    timeout.ReadTotalTimeoutConstant = 5000;
    timeout.WriteTotalTimeoutMultiplier = 500;
    timeout.WriteTotalTimeoutConstant = 2000;
    SetCommTimeouts(uart, &timeout);

    // ���ô���ͨ�Ų����� 9600bps��8 λ����λ����У��λ��1 λֹͣλ
    DCB dcb;
    GetCommState(uart, &dcb);
    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;
    dcb.fParity = FALSE;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    SetCommState(uart, &dcb);

    // ���ָ��ת��Ϊ�����Ʋ�����
    int num = 0;
    char ram[32];
    char command[5], temp[5];
    while(scanf("%s", command) != EOF)
    {
        if(!strcmp(command, "mov"))
        {
            scanf("%s", temp);
            if(!strcmp(temp, "ax"))
            {
                scanf("%s", temp);
                if(!strcmp(temp, "bx"))
                {
                    ram[num++] = 0x03;
                }
                else
                {
                    ram[num++] = 0x01;
                    if(strtoul(temp, NULL, 0) > 255)
                        ram[num++] = 0x00;
                    else
                        ram[num++] = strtoul(temp, NULL, 0);
                }
            }
            else if(!strcmp(temp, "bx"))
            {
                scanf("%s", temp);
                if(!strcmp(temp, "ax"))
                {
                    ram[num++] = 0x04;
                }
                else
                {
                    ram[num++] = 0x02;
                    if(strtoul(temp, NULL, 0) > 255)
                        ram[num++] = 0x00;
                    else
                        ram[num++] = strtoul(temp, NULL, 0);
                }
            }
            else
            {
                PrintfColorfulMessage(cmd, "\nInvalid parameter.\n\n", 1);
                system("pause");
                return 1;
            }
        }
        else if(!strcmp(command, "add"))
        {
            scanf("%s", temp);
            if(!strcmp(temp, "ax"))
            {
                scanf("%s", temp);
                if(!strcmp(temp, "bx"))
                {
                    ram[num++] = 0x07;
                }
                else
                {
                    ram[num++] = 0x05;
                    if(strtoul(temp, NULL, 0) > 255)
                        ram[num++] = 0x00;
                    else
                        ram[num++] = strtoul(temp, NULL, 0);
                }
            }
            else if(!strcmp(temp, "bx"))
            {
                scanf("%s", temp);
                if(!strcmp(temp, "ax"))
                {
                    ram[num++] = 0x08;
                }
                else
                {
                    ram[num++] = 0x06;
                    if(strtoul(temp, NULL, 0) > 255)
                        ram[num++] = 0x00;
                    else
                        ram[num++] = strtoul(temp, NULL, 0);
                }
            }
            else
            {
                PrintfColorfulMessage(cmd, "\nInvalid parameter.\n\n", 1);
                system("pause");
                return 1;
            }
        }
        else if(!strcmp(command, "shr"))
        {
            scanf("%s", temp);
            if(!strcmp(temp, "ax"))
            {
                ram[num++] = 0x09;
            }
            else if(!strcmp(temp, "bx"))
            {
                ram[num++] = 0x0a;
            }
            else
            {
                PrintfColorfulMessage(cmd, "\nInvalid parameter.\n\n", 1);
                system("pause");
                return 1;
            }
        }
        else if(!strcmp(command, "shl"))
        {
            scanf("%s", temp);
            if(!strcmp(temp, "ax"))
            {
                ram[num++] = 0x0b;
            }
            else if(!strcmp(temp, "bx"))
            {
                ram[num++] = 0x0c;
            }
            else
            {
                PrintfColorfulMessage(cmd, "\nInvalid parameter.\n\n", 1);
                system("pause");
                return 1;
            }
        }
        else if(!strcmp(command, "xchg"))
        {
            ram[num++] = 0x0d;
        }
        else if(!strcmp(command, "halt"))
        {
            ram[num++] = 0x0e;
        }
        else
        {
            PrintfColorfulMessage(cmd, "\nCommand not found.\n\n", 1);
            system("pause");
            return 1;
        }
        if(num > 16)
        {
            PrintfColorfulMessage(cmd, "\nToo many commands.\n\n", 1);
            system("pause");
            return 1;
        }
        if(!strcmp(command, "halt"))
        {
            break;
        }
    }
    PrintfColorfulMessage(cmd, "\nBEGIN\n\n", 0);

    // ���η������������ָ��
    char data[2];
    DWORD WriteNum;
    for(int i = 0;i < num;i++)
    {
        PurgeComm(uart, PURGE_TXCLEAR);
        data[0] = i;
        data[1] = ram[i];
        WriteNum = 0;
        if(WriteFile(uart, data, sizeof(data), &WriteNum, 0))
        {
            printf("%2d/%-2d Writing to RAM: %02x\n", i, num-1, ram[i]);
        }
        PurgeComm(uart, PURGE_TXCLEAR);
        // �������� 8 λ���ݺ�ȴ� 200 ���룬���ڴ洢���ӽ���ģ���ȡ��д������
        Sleep(200);
    }

    PrintfColorfulMessage(cmd, "\nEND\n\n", 0);
    system("pause");

    CloseHandle(uart);
    return 0;
}

void PrintfColorfulMessage(HANDLE cmd, char *message, int type)
{

    if(type == 0)
        SetConsoleTextAttribute(cmd, FOREGROUND_GREEN | FOREGROUND_INTENSITY);          // ����
    else if(type == 1)
        SetConsoleTextAttribute(cmd, FOREGROUND_RED | FOREGROUND_INTENSITY);            // ����
    printf("%s", message);
    SetConsoleTextAttribute(cmd, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}
