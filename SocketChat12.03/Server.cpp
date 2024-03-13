#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "WinSock2.h" //здесь находятся объявления, необходимые
//для Winsock 2 API.
#include <ws2tcpip.h> //содержит функции для работы с адресами
//напр. inet_pton
#pragma comment(lib, "Ws2_32.lib")  //линкуем библиотеку Windows Sockets
#include "tchar.h"
#include "resource.h"
using namespace std;



class ServerSocket
{
private:
    WSADATA wsaData;//структура для хранения информации о инициализации сокетов
    SOCKET _socket; //дескриптор слушающего сокета
    SOCKET acceptSocket;//дескриптор сокета, который связан с клиентом 
    sockaddr_in addr; //локальный адрес и порт

public:
	ServerSocket();
	~ServerSocket();
		void Listen(); //метод для активации "слушающего" сокета
	void Bind(int port); //метод для привязки сокета к порту
	void StartHosting(int port); //объединяет вызов двух предыдущих методов
    bool SendData(char*); //метод для отправки данных в сеть
    bool ReceiveData(char*, int);//метод для получения данных
    void CloseConnection(); //метод для закрытия соединения
    void SendDataMessage();//метод для отправки сообщения пользователя
};



ServerSocket::ServerSocket()
{
    //если инициализация сокетов прошла неуспешно, выводим сообщение об
    //ошибке
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
    {
        cout << "WSAStartup error\n";
        system("pause");
        WSACleanup();
        exit(10);
    }

    //создаем потоковый сокет, протокол TCP
    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //при неуспешном создании сокета выводим сообщение об ошибке
    if (_socket == INVALID_SOCKET)
    {
        cout << "Socket create error." << endl;
        system("pause");
        WSACleanup();
        exit(11);
    }

}

ServerSocket::~ServerSocket()
{
    WSACleanup();//очищаем ресурсы
}

void ServerSocket::Listen()
{
    cout << "Waiting for client..." << endl;

    //При ошибке активации сокета в режиме прослушивания
    //выводим ошибку
    if (listen(_socket, 1) == SOCKET_ERROR)
    {
        cout << "Listening error\n";
        system("pause");
        WSACleanup();
        exit(15);
    }
    /*
    Метод является блокирующим. Ожидаем подключение клиента.
    Как только клиент подключился, функция accept возвращает
    новый сокет, через который происходит обмен данными.
    */
    acceptSocket = accept(_socket, NULL, NULL);
    while (acceptSocket == SOCKET_ERROR)
    {
        acceptSocket = accept(_socket, NULL, NULL);
    }
    _socket = acceptSocket;
}

void ServerSocket::Bind(int port)
{
    //Указываем семейство адресов IPv4
    addr.sin_family = AF_INET;
    /*Преобразуем адрес "0.0.0.0"в правильную
    структуру хранения адресов, результат сохраняем в поле sin_addr */
    inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
    //Указываем порт. 
    //Функиця htons выполняет преобразование числа в
    //сетевой порядок байт
    addr.sin_port = htons(port);

    cout << "Binding to port:  " << port << endl;
    //При неудачном биндинге к порту, выводим сообщение про ошибку
    if (bind(_socket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        cout << "Failed to bind to port\n";
        system("pause");
        WSACleanup();
        exit(14);
    }
}

void ServerSocket::StartHosting(int port)
{
    Bind(port);
    Listen();
}

bool ServerSocket::SendData(char* buffer)
{

    /*Отправляем сообщение на указанный сокет*/
    send(_socket, buffer, strlen(buffer), 0);
    return true;
}

bool ServerSocket::ReceiveData(char* buffer, int size)
{
    /*Получаем сообщение и сохраняем его в буфере.
     Метод является блокирующим!*/
    int i = recv(_socket, buffer, size, 0);
    buffer[i] = '\0';
    return true;
}


void ServerSocket::CloseConnection()
{
    //Закрываем сокет
    closesocket(_socket);
}

void ServerSocket::SendDataMessage()
{
    //Строка для сообщения пользователя
    char message[255];
    //Без этого метода из потока будет считан последний
    //ввод пользователя, выполняем сброс.
    cin.ignore();
    cout << "Input message: ";
    cin.get(message, 255);
    SendData(message);
}









ServerSocket server;
int port = 24242; //выбираем порт
char sendMessage[255];
char receiveMessage[255];

BOOL CALLBACK ServerDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_SERVERDIALOG), NULL, (DLGPROC)ServerDlgProc);
	return 0;
}


BOOL CALLBACK ServerDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {


    HWND hList = GetDlgItem(hwnd, IDC_LIST1);
	switch (message) {
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_START:
            //Запускаем сервер
            server.StartHosting(port);
            break;
        case IDC_SEND:

            GetDlgItemTextA(hwnd, IDC_EDIT1, sendMessage, 255);
            //Отправляем данные клиенту
            server.SendData(sendMessage);
        
            SendMessageA(hList, LB_ADDSTRING, 0, (LPARAM)sendMessage);
            break;


        case IDC_RECEIVE:
            //Получаем данные от клиента
        //и сохраняем в переменной receiveMessage
            server.ReceiveData(receiveMessage, 255);


           
            SendMessageA(hList, LB_ADDSTRING, 0, (LPARAM)receiveMessage);

            if (strcmp(receiveMessage, "end") == 0) {
                EndDialog(hwnd, 0);
            }
            break;
         

        }
        break;
    case WM_CLOSE:
       
        server.ReceiveData(receiveMessage, 255);
        SetDlgItemTextA(hwnd, IDC_LIST1, receiveMessage);
        server.CloseConnection();
        EndDialog(hwnd, 0);
        break;
	default:
		return FALSE;
	}
	return TRUE;
}