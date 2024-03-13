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



class ClientSocket
{
private:
	WSADATA wsaData;//структура для хранения информации о инициализации сокетов
	SOCKET _socket; //дескриптор слушающего сокета
	sockaddr_in addr; //локальный адрес и порт
public:
	ClientSocket();
	~ClientSocket();
	//метод для подключения к серверу
	void ConnectToServer(const char* ipAddress, int port);
	bool SendData(char*); //метод для отправки данных в сеть
	bool ReceiveData(char*, int);//метод для получения данных
	void CloseConnection(); //метод для закрытия соединения
	void SendDataMessage();//метод для отправки сообщения пользователя
};



ClientSocket::ClientSocket()
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

ClientSocket::~ClientSocket()
{
	WSACleanup();//очищаем ресурсы
}

void ClientSocket::ConnectToServer(const char* ipAddress, int port)
{
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, ipAddress, &addr.sin_addr);
	addr.sin_port = htons(port);
	//при неудачном подключении к серверу выводим сообщение про ошибку
	if (connect(_socket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		cout << "Failed to connect to server\n";
		system("pause");
		WSACleanup();
		exit(13);
	}
}

bool ClientSocket::SendData(char* buffer)
{

	/*Отправляем сообщение на указанный сокет*/
	send(_socket, buffer, strlen(buffer), 0);
	return true;
}

bool ClientSocket::ReceiveData(char* buffer, int size)
{
	/*Получаем сообщение и сохраняем его в буфере.
	Метод является блокирующим!*/
	int i = recv(_socket, buffer, size, 0);
	buffer[i] = '\0';
	return true;
}

void ClientSocket::CloseConnection()
{
	//Закрываем сокет
	closesocket(_socket);
}

void ClientSocket::SendDataMessage()
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

























ClientSocket client;
string ipAddress = "127.0.0.1"; //Адрес сервера
int port = 24242; //выбираем порт
char sendMessage[255];
char receiveMessage[255];
 
BOOL CALLBACK ClientDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_CLIENTDIALOG), NULL, (DLGPROC)ClientDlgProc);
	return 0;
}



BOOL CALLBACK ClientDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	HWND hList = GetDlgItem(hwnd, IDC_LIST1);
	switch (message) {
	case WM_INITDIALOG:

		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_CONNECT:
			
			SetDlgItemTextA(hwnd, IDC_IP_EDIT2, ipAddress.c_str());
			//подключаемся к серверу
			client.ConnectToServer(ipAddress.c_str(), port);
			break;
		case IDC_SEND:
			GetDlgItemTextA(hwnd, IDC_EDIT1, sendMessage, 255);
			//отправляем сообщение
			client.SendData(sendMessage);
			
			SendMessageA(hList, LB_ADDSTRING, 0, (LPARAM)sendMessage);
			//Если есть сообщение "end", завершаем работу
			if (strcmp(receiveMessage, "end") == 0 ||
				strcmp(sendMessage, "end") == 0){
				client.CloseConnection();
				EndDialog(hwnd, 0);
			}
			break;
		case IDC_RECEIVE:
			//получаем ответ
			client.ReceiveData(receiveMessage, 255); 

			
			SendMessageA(hList, LB_ADDSTRING, 0, (LPARAM)receiveMessage);
			
			break;

		}
		break;
	case WM_CLOSE:
		//Закрываем соединение
		client.CloseConnection();
		EndDialog(hwnd, 0);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}


		
		
	
	
	
	