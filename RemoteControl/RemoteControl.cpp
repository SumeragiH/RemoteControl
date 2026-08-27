//服务端（服务器）：提供服务（响应请求）的机器
#include <stdio.h>
#include <windows.h> // Windows编程要包含的头文件

#pragma comment(lib,"ws2_32.lib") //加载 ws2_32.dll

//程序入口函数：程序从这里开始
int main()
{
	//让控制台以UTF-8编码输出，避免中文乱码
	SetConsoleOutputCP(CP_UTF8);
	//服务器网络编程步骤：
	//1.初始化网络环境
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//2.创建服务器Socket AF_INET:使用IPv4  SOCK_STREAM:使用TCP  0:使用默认协议
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM,0);
	if (serverSocket == INVALID_SOCKET)
	{
		printf("创建服务器Socket失败，错误码：%d\n", WSAGetLastError());
		return 0;
	}

	//3.给服务器绑定地址
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET; //使用IPv4的协议
	serverAddr.sin_port = htons(9999); //绑定端口号
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY; //绑定本机所有IP地址（因为电脑上可能不止一个网卡/可能有多个IP地址）
	if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		printf("绑定服务器失败，错误码：%d\n", WSAGetLastError());
		return 0;
	}

	//4.开启服务器socket监听
	if(listen(serverSocket, 5)==SOCKET_ERROR) //5表示允许完成三次握手的客户端的数量
	{
		printf("开启服务器监听失败，错误码：%d\n", WSAGetLastError());
		return 0;
	}

	//5.等待客户端连接，会返回客户端的socket
	SOCKADDR_IN clientAddr;
	int clientAddrLen = sizeof(SOCKADDR_IN);

	printf("等待客户端连接\r\n");
	//阻塞，等待客户端连接，连接成功后会返回客户端的socket

	SOCKET clientSocket = accept(serverSocket,(sockaddr*)&clientAddr, &clientAddrLen);
	printf("客户端连接成功\r\n");

	//6.等待客户端发送数据，接收数据
	char buffer[1024] = { 0 };
	//返回接受的数据的长度
	int len = recv(clientSocket, buffer , sizeof(buffer), 0);
	if (len>0)
	{
		printf("客户端接受的数据：%s\n", buffer);

	}
	//7.给客户端发送数据
	send(clientSocket, "Hello, Client!", strlen("Hello, Client!"), 0);

	//清除网络环境
	WSACleanup();
	return 0;
}
 
