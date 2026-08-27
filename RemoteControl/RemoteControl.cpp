//服务端（服务器）：提供服务（响应请求）的机器
#include <stdio.h>
#include <windows.h> // Windows编程要包含的头文件

#pragma comment(lib,"ws2_32.lib") //加载 ws2_32.dll

#pragma pack(push,1) //设置结构体对齐方式为1字节对齐
struct PacketHeader
{
	int magic;//4字节的包头
	int cmd;//4字节的命令字
	int body_len;//4字节的包体长度


};

#pragma pack(pop)
struct Packet
{
	PacketHeader header;//包头
	char body[];//包数据，不固定长度

};

Packet* ParsePacket(char* buffer, int len);

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
	while (true)
	{
		//永久的接受数据
		//返回接受到的数据长度 recv阻塞等待接受数据
		int len = recv(clientSocket, buffer, sizeof(buffer), 0);
		//依照协议解析数据
		Packet* packet = ParsePacket(buffer, len);


		if (len > 0)
		{
			printf("服务器接受的数据：%s\n", packet->body);
			printf("-----------------------------\n");
		}


		//返回值是堆上的内存，使用完后要释放
		free(packet);	
		Sleep(1000);
	}
	closesocket(clientSocket);
	closesocket(serverSocket);
	//清除网络环境
	WSACleanup();
	return 0;
}
 
Packet* ParsePacket(char* buffer,int len) 
{
	//第一个char地址转为int指针，因为转为int*，一次取4个字节，判断包头是否正确

	Packet pck;
	Packet* rPck;
	//四字节的包头，四字节的命令字，四字节的包体长度+包体数据
	int i = 0;
	for (; i < len; i++)
	{
		//判断包头是否正确
		if (*(int*)(buffer+i) == 0x12345678)
		{
			pck.header.magic = *(int*)(buffer + i);
			i += 4;
			break;
		}
	}
	pck.header.cmd = *(int*)(buffer + i);
	i += 4;

	pck.header.body_len = *(int*)(buffer + i);
	i += 4;

	//获取包体数据
	if (pck.header.body_len > 0)
	{
		rPck = (Packet*)malloc(sizeof(PacketHeader) + pck.header.body_len);	//分配内存
		//拷贝数据(包体，包头)
		memcpy(rPck->body, buffer + i, pck.header.body_len);
		memcpy(&rPck->header, &pck.header, sizeof(PacketHeader));

		return rPck;
	}
	return nullptr;
}