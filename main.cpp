#pragma comment(lib,"libws2_32.a")

#include <winsock2.h>
#include <stdio.h>

/**
 * Кастомный клиент для TarifSV
 */

int main() {
    WORD ver = MAKEWORD(2,2);
    WSADATA wsaData;
    int retVal=0;

    WSAStartup(ver,(LPWSADATA)&wsaData);

    LPHOSTENT hostEnt;

    hostEnt = gethostbyname("localhost");

    if(!hostEnt)
    {
        printf("Unable to collect gethostbyname\n");
        WSACleanup();
        return 1;
    }

    //Создаем сокет
    SOCKET clientSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);


    if(clientSock == SOCKET_ERROR)
    {
        printf("Unable to create socket\n");
        WSACleanup();
        return 1;
    }

    SOCKADDR_IN serverInfo;

    serverInfo.sin_family = PF_INET;
    serverInfo.sin_addr.S_un.S_addr = inet_addr("192.168.28.2");
    serverInfo.sin_port = htons(5000);

    retVal=connect(clientSock,(LPSOCKADDR)&serverInfo, sizeof(serverInfo));
    if(retVal==SOCKET_ERROR)
    {
        printf("Unable to connect\n");
        WSACleanup();
        return 1;
    }

    printf("Connection made sucessfully\n");
    /**
     * конфигурирование и подключение к сокету и серверу
     */
    const char *pBuf = "sorokin3";

    printf("Sending request from client\n");

    retVal = send(clientSock, pBuf, strlen(pBuf), 0); //отправляем серверу логин

    if(retVal == WSAECONNRESET )
    {
        printf("Wsaeintr " + retVal);
        WSACleanup();
        return 1;
    }


    char szResponse[255];
    for (int i = 0; i < 255; ++i)
    {
        szResponse[i] = 0;
    }

    retVal = recv(clientSock, szResponse, 255, 0); //приходит время ответа от сервера(используется для пароля)

    if(retVal == SOCKET_ERROR)
    {
        printf("Unable to recv\n");
        WSACleanup();
        return 1;
    }

    printf("Got the response from server\n%s\n",szResponse);

    /**
     * тк в качестве пароля используется комбинация вида 5000  + время получения сервером логина, используем следующий кастыль
     * смещаем все символы на 1 правее и вставляем в 1 5
     */
    szResponse[3] = szResponse[2];
    szResponse[2] = szResponse[1];
    szResponse[1] = szResponse[0];

    szResponse[0] = '5';

    const  char *pBUf = szResponse;

    printf("Sending request from client\n");
    retVal = send(clientSock, pBUf, strlen(pBUf), 0); //отправляем пароль

    if(retVal == SOCKET_ERROR)
    {
        printf("Unable to send\n");
        WSACleanup();
        return 1;
    }

    retVal = recv(clientSock, szResponse, 255, 0); //получаем ответ от сервера(приходит вся информация с сервера)

    if(retVal == SOCKET_ERROR)
    {
        printf("Unable to recv\n");
        WSACleanup();
        return 1;
    }

    printf("Got the response from server\n%s\n",szResponse);

    closesocket(clientSock);
    WSACleanup(); //отключились и всё почистили

    return 0;
}