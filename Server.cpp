#include <iostream>
#include "Server.h"
using namespace std;

//构造函数
Server::Server()
{
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	listener = 0;
	epfd = 0;
}

//初始化函数
void Server::Init()
{
	cout<<"Init Mahjong Server..."<<endl;
	//创建套接字
	listener = socket(PF_INET,SOCK_STREAM,0);
	if(listener < 0)
	{
		perror("listener");
		exit(-1);
	}

	//绑定服务端地址结构
	if(bind(listener,(struct sockaddr*)&serverAddr,sizeof(serverAddr)) < 0)
	{
		perror("bind error");
		exit(-1);
	}

	//设置同时与服务器建立连接的上限数
	int ret = listen(listener,5);
	if(ret < 0)
	{
		perror("listen error");
		exit(-1);
	}

	cout<<"Start to listen: "<<SERVER_IP<<endl;

	//创建监听红黑数
	epfd = epoll_create(EPOLL_SIZE);
	if(epfd < 0)
	{
		perror("epfd error");
		exit(-1);
	}

	//调用addfd函数，将listener挂到树上
	addfd(epfd,listener,true);
}

//关闭文件描述符
void Server::Close()
{
	close(listener);
	close(epfd);

}

//发送广播信息
int Server::SendBroadcastMessage(int clientfd)
{
	char recv_buf[BUF_SIZE];//存放从客户端接收到的信息
	char send_buf[BUF_SIZE];//存放发送给客户端的信息
	Msg msg;//信息结构体
	bzero(recv_buf,BUF_SIZE);//清空recv_buf

	cout<<"read from ClientID="<<clientfd<<endl<<endl;
	int len = recv(clientfd,recv_buf,BUF_SIZE,0);//将客户端发送的信息存放到recv_buf
	memset(&msg,0,sizeof(msg));//清空信息结构体
	memcpy(&msg,recv_buf,sizeof(msg));//将recv_buf中信息存放到信息结构体中

	msg.fromID=clientfd;//将与对端建立连接的文件描述符赋值给fromID
	/*若信息结构体中内容成员的第一个字符为‘\’
	  并且第二个字符为‘数字字符’（对端连接描述符），则代表私聊*/
	if(msg.content[0]=='\\'&&isdigit(msg.content[1]))
	{
		msg.type = 1;//标志位为1代表私聊
		msg.toID = msg.content[1]-'0';//减‘0’是将数字字符转为数字
		//将信息结构体中的内容成员除去前两个代表私聊的字符，重新赋值给内容成员
		memcpy(msg.content,msg.content+2,sizeof(msg.content));
	}
	else
		msg.type = 0;//若前两个字符不符合则将标志位置0，表示广播

	//如果从对端读到的字节数为0，代表对端已经关闭
	if(len == 0)
	{
		//关闭对端文件描述符
		close(clientfd);
		//将这个文件描述符从链表中移除
		clients_list.remove(clientfd);
		cout<<"ClientID="<<clientfd<<" closed."<<endl
			<<"Now we have "<<clients_list.size()<<" in our chat room"
			<<endl<<endl;
	}
	//若读到字节数不为0
	else
	{
		//如果链表中只有一个成员
		if(clients_list.size() == 1)
		{
			//将CAUTION放到发送缓冲区发送给这个唯一的对端
			memcpy(&msg.content,CAUTION,sizeof(msg.content));
            bzero(send_buf, BUF_SIZE);
            memcpy(send_buf,&msg,sizeof(msg));
            send(clientfd, send_buf, sizeof(send_buf), 0);
            return len;
		}

		char format_message[BUF_SIZE];//定义一个消息数组
		//如果是广播的形式
		if(msg.type == 0)
		{
			//将SERVER_MESSAGE赋给msg.content
			sprintf(format_message,SERVER_MESSAGE,clientfd,msg.content);
			memcpy(msg.content,format_message,BUF_SIZE);

			//遍历整个链表
			list<int>::iterator it;
			for(it = clients_list.begin();it != clients_list.end();it++)
			{
				if(*it != clientfd )//发给除他本身外所有人
				{
					bzero(send_buf,BUF_SIZE);
					memcpy(send_buf,&msg,sizeof(msg));
					if( send(*it,send_buf,sizeof(send_buf),0) < 0 )
					{
						return -1;
					}
				}
			}
		}

		//如果为私聊形式
		if(msg.type == 1)
		{
			//先将标志私信对方是否下线标志位置为true
			bool private_offline = true;
			sprintf(format_message,SERVER_PRIVATE_MESSAGE,clientfd,msg.content);
			memcpy(msg.content,format_message,BUF_SIZE);

			list<int>::iterator it;
			for(it = clients_list.begin();it != clients_list.end();it++)
			{
				//将content中内容发给指定的私信对端
				if(*it == msg.toID)
				{
					//将标志位置false
					private_offline = false;
					bzero(send_buf,BUF_SIZE);
					memcpy(send_buf,&msg,sizeof(msg));
					if( send(*it,send_buf,sizeof(send_buf),0) < 0 )
					{
						return -1;
					}
				}
			}

			if(private_offline)//没有发过去，对方已经下线，将反馈信息发给发送端
			{
				sprintf(format_message,SERVER_PRIVATE_ERROR_MESSAGE,msg.toID);
				memcpy(msg.content,format_message,BUF_SIZE);
				bzero(send_buf,BUF_SIZE);
				memcpy(send_buf,&msg,sizeof(msg));
				if( send(msg.fromID,send_buf,sizeof(send_buf),0) < 0 )
				{
					return -1;
				}
			}
			
		}
	}
	return len;
}

//服务器运作函数
void Server::Start()
{
	//定义结构体存放事件
	static struct epoll_event events[EPOLL_SIZE];

	//调用初始化函数
	Init();

	while(1)
	{
		//获取事件个数
		int epoll_events_count = epoll_wait(epfd,events,EPOLL_SIZE,-1);
		if(epoll_events_count < 0)
		{
			perror("epoll failure");
			break;
		} 

		cout<<"epoll_events_count = "<<epoll_events_count<<endl;

		//处理事件
		for(int i = 0 ; i < epoll_events_count; i++)
		{
			int sockfd = events[i].data.fd;

			//若是监听事件，则继续监听，等待连接
			if(sockfd == listener)
			{
				struct sockaddr_in client_address;
				socklen_t client_addrLenth = sizeof(struct sockaddr_in);
				int clientfd = accept( listener,(struct sockaddr*)&client_address,&client_addrLenth);

				cout<<"client connection from:"
					<<inet_ntoa(client_address.sin_addr)<<":"
					<<ntohs(client_address.sin_port)<<", clientfd :"
					<<clientfd<<endl;

				//将监听到的新文件描述符挂到树上
				addfd(epfd,clientfd,true);

				//并将文件描述符插入到链表中
				clients_list.push_back(clientfd);
				cout<<"Add new clientfd = "<<clientfd<<" to epoll"<<endl;
				cout<<"Now we have "<<clients_list.size()<<" clients in our chat room."<<endl<<endl;

				//将SERVER_WELCOME发送到连接端
				char message[BUF_SIZE];
				bzero(message,BUF_SIZE);
				sprintf(message,SERVER_WELCOME,clientfd);
				int ret = send(clientfd,message,BUF_SIZE,0);
				if(ret < 0)
				{
					perror("send error");
					Close();
					exit(-1);
				}
			}
			//否则是有对端读写事件发生，调用发送广播函数
			else
			{
				int ret = SendBroadcastMessage(sockfd);
				if(ret < 0)
				{
					perror("error");
					Close();
					exit(-1);
				}
			}
		}

	}
	//结束关闭所有文件描述符
	Close();

}
