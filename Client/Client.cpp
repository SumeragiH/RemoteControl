#include <stdio.h>
#include <Windows.h>

#pragma comment(lib,"ws2_32.lib") //加载 ws2_32.dll

int main()
{
	//让控制台以UTF-8编码输出，避免中文乱码
	SetConsoleOutputCP(CP_UTF8);
	//客户端网络编程步骤：初始化网络环境，创建socket，连接服务器，发送数据，接收数据
	// 初始化网络环境
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	//创建客户端Socket
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket==INVALID_SOCKET)
	{
		printf("创建客户端Socket失败，错误码：%d\n", WSAGetLastError());
		return 0;
	}

	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port=htons(9999);
	serverAddr.sin_addr.S_un.S_addr = inet_addr("192.168.18.203");

	//连接服务器
	if (connect(serverSocket, (sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		printf("连接服务器失败，错误码：%d\n", WSAGetLastError());
		return 0;
	}

	//给服务器发送数据
	char buffer[1024] = "hello world";
	char recvBuffer[1024] = { 0 };
	send(serverSocket, buffer, strlen(buffer), 0);

	//等待接受服务器返回的数据
	int len = recv(serverSocket, recvBuffer, sizeof(recvBuffer), 0);
	if (len>0)
	{
		printf("客户端接受的数据：%s\n", recvBuffer);
	}

	WSACleanup();
	return 0;	
}

