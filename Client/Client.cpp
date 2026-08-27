#include <stdio.h>
#include <Windows.h>

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
	char buffer[1024] = {0};
	char recvBuffer[1024] = { 0 };
	int count = 0;//记录发送数据的次数
	while (true)
	{
		count++;
		//准备发送数据 sprintf：把格式化的数据写入字符串中
		sprintf_s(buffer,"packet:%d",count);
		//printf("请输入要发送的数据：");
		////接受用户输入,stdin标准输入
		//fgets(buffer,1024,stdin);

		//发送数据
		Packet* packet = (Packet*)malloc(sizeof(PacketHeader) + strlen(buffer)+1);//malloc：动态分配内存，返回指向分配内存的指针(c++的字节数计算要长度+1)

		packet->header.magic = 0x12345678;//包头
		packet->header.cmd = 2000;//命令字 
		packet->header.body_len = strlen(buffer) + 1;//包体长度

		//内存复制函数
		memcpy(packet->body, buffer, strlen(buffer) + 1);//把buffer中的数据拷贝到packet->body中

		send(serverSocket,(char*)packet,packet->header.body_len+sizeof(PacketHeader),0);
		//malloc是动态分配在堆上的内存，使用了要及时清理
		free(packet);//释放内存
		printf("客户端发送数据：%s\n", buffer);

		////等待接受服务器返回的数据
		//int len = recv(serverSocket, recvBuffer, sizeof(recvBuffer), 0);
		//if (len > 0)
		//{
		//	printf("客户端接受的数据：%s\n", recvBuffer);
		//}

		Sleep(10);
	}

	closesocket(serverSocket);
	WSACleanup();
	return 0;	
}

