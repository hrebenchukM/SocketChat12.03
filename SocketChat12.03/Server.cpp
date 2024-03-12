#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "WinSock2.h" 
#include <ws2tcpip.h> 
#pragma comment(lib, "Ws2_32.lib") 
#include "tchar.h"
#include "resource.h"
using namespace std;



class ServerSocket
{
private:
	WSADATA wsaData;
	SOCKET _socket;
	SOCKET acceptSocket;
	sockaddr_in addr;

public:
	ServerSocket();
	~ServerSocket();
	void Listen();
	void Bind(int port);
	void StartHosting(int port);
	bool SendData(char* buffer);
	bool ReceiveData(char* buffer, int size);
	void CloseConnection();
	void SendDataMessage();
};



ServerSocket::ServerSocket()
{
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
    {
        cout << "WSAStartup error\n";
        system("pause");
        WSACleanup();
        exit(10);
    }

    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

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
    WSACleanup();
}

void ServerSocket::Listen()
{
    cout << "Waiting for client..." << endl;

    if (listen(_socket, 1) == SOCKET_ERROR)
    {
        cout << "Listening error\n";
        system("pause");
        WSACleanup();
        exit(15);
    }

    acceptSocket = accept(_socket, NULL, NULL);
    while (acceptSocket == SOCKET_ERROR)
    {
        acceptSocket = accept(_socket, NULL, NULL);
    }
    _socket = acceptSocket;
}

void ServerSocket::Bind(int port)
{
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
    addr.sin_port = htons(port);

    cout << "Binding to port:  " << port << endl;

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
    send(_socket, buffer, strlen(buffer), 0);

  
    return true;
}

bool ServerSocket::ReceiveData(char* buffer, int size)
{
    int i = recv(_socket, buffer, size, 0);
    buffer[i] = '\0';
    return true;


}


void ServerSocket::CloseConnection()
{
    closesocket(_socket);
}

void ServerSocket::SendDataMessage()
{
    char message[255];
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
            server.StartHosting(port);
            break;
        case IDC_SEND:
            GetDlgItemTextA(hwnd, IDC_EDIT1, sendMessage, 255);
            server.SendData(sendMessage);
        
            SendMessageA(hList, LB_ADDSTRING, 0, (LPARAM)sendMessage);
            break;


        case IDC_RECEIVE:
            server.ReceiveData(receiveMessage, 255);


           
            SendMessageA(hList, LB_ADDSTRING, 0, (LPARAM)receiveMessage);
            if (strcmp(receiveMessage, "end") == 0) {
                server.CloseConnection();
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