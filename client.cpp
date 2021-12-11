#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<WinSock2.h>
#include<stdio.h>
#include<iostream>
#include<algorithm>
#include<string.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")
#define MAXSIZE 128
using namespace std;

/*
定义消息格式：
0:第一位:0自己摸到牌消息,需回复
  1出牌后回复自己的牌,无需回复
  2是否副露消息，需回复
  3别家出牌消息，无需回复
  4牌河消息变动，无需回复

*/
int inTile = 0;
static int myNum = 0;
int playernum = 0;
int IDnum = 0;
int countNum = 0;
const char* ptr[4] = { "↓","→","↑","←" };
int drop[4] = { 0 };
int tilesRemaining = 0;

const char* Mahjong[50] = { "0","东","南","西","北","中","发","0","0","0","0","1m","2m","3m","4m","5m","6m","7m","8m","9m","0","1s","2s","3s","4s","5s","6s","7s","8s","9s" ,"0","1p","2p","3p","4p","5p","6p","7p","8p","9p","0","白","春","夏","秋","冬","梅","兰","竹","菊" };

struct myTiles
{
	int numOfFlowers=0;
	int flowers[12] = {0};
	int numOfMeld=0;
	int melds[16] = {0};
	int numOfTiles=0;
	int tilesInHand[14];
};
void outMyTiles(myTiles* mytiles);
struct dropTile
{
	int num=0;
	int tile[30] = {0};
};

void outOneTile(int tile)
{
	cout << Mahjong[tile] << endl;
}

void colour(int x)
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, x);
}

void outFile(dropTile* droptile, myTiles* mytiles, string* ID, int inputtile,int tilesRemaining)
{
	system("cls");
	cout << "剩余牌数:"<<tilesRemaining<<endl;
	for (int k = 1; k < 4; k++)
	{
		if (countNum == k)
		{
			colour(6);
			cout << "→ ";
		}
		else colour(15);
		cout << ID[k].c_str() << ": 花:";
		for (int i = 0; i < mytiles[k].numOfFlowers; i++)
		{
			cout << Mahjong[mytiles[k].flowers[i]];
		}
		cout << "   牌:";
		for (int i = 0; i < mytiles[k].numOfTiles; i++) cout << "口";
		cout << "   副露:";
		for (int i = 0; i < mytiles[k].numOfMeld; i++)
		{
			cout << "|";
			for (int j = i * 4; j < i * 4 + 4; j++)
			{
				if (Mahjong[mytiles[k].melds[j]] != "0")
					cout << Mahjong[mytiles[k].melds[j]] << " ";
			}
		}		
		cout << "|" << endl;

		if (drop[k] != 0)
		{
			cout << endl;
			cout << endl;
			cout << "出牌：" << Mahjong[drop[k]] << endl;
			cout << endl;
		}

		cout << endl;
		cout << "牌河:";
		for (int j = 0; j < droptile[k].num; j++)
		{
			cout << Mahjong[droptile[k].tile[j]] << "  ";
		}
		cout << endl;
		cout << endl;
		cout << endl;
	}
	cout << endl;
	colour(15);
	cout <<ID[0].c_str() << "(我)的牌河:";
	for (int j = 0; j < droptile[0].num; j++)
	{
		cout << Mahjong[droptile[0].tile[j]] << "  ";
	}
	cout << endl;
	cout << endl;
	outMyTiles(mytiles);


}
/*
void outAll(dropTile* droptile, myTiles* mytiles, string* ID, int inputtile)
{
	system("cls");

	//对家牌 
	cout << "                                             " << ID[2].c_str() << endl;
	cout << "                                          ";
	for (int i = 0; i < mytiles[2].numOfFlowers; i++)
	{
		cout << Mahjong[mytiles[2].flowers[i]] << " ";
	}
	cout << endl;
	cout << "                                          ";
	for (int i = 0; i < mytiles[2].numOfTiles; i++) cout << "口";
	cout << "      ";
	for (int i = 0; i < mytiles[2].numOfMeld; i++)
	{
		cout << "|";
		for (int j = i * 4; j < i * 4 + 4; j++)
		{
			if (Mahjong[mytiles[2].melds[j]] != "0")
				cout << Mahjong[mytiles[2].melds[j]] << " ";
		}
	}
	cout << "|";

	//对家牌河
	for (int j = 0; j < droptile[2].num; j++)
	{
		if (j % 8 == 0) cout << endl<< "                                          ";
		cout << Mahjong[droptile[2].tile[j]] << "  ";
	}
	cout << endl;

	if(drop[2]!=0) cout << "                          " << Mahjong[drop[2]];
	cout << endl;

	for (int i = 0; i < 5; i++) cout << endl;

	//上家
	cout << " " << ID[3].c_str() << endl;
	for (int i = 0; i < mytiles[3].numOfFlowers; i++)
	{
		cout << Mahjong[mytiles[3].flowers[i]] << " ";
	}
	cout << endl;
	for (int i = 0; i < mytiles[3].numOfTiles; i++) cout << "口";
	cout << "      ：";
	for (int i = 0; i < mytiles[3].numOfMeld; i++)
	{
		cout << "|";
		for (int j = i * 4; j < i * 4 + 4; j++)
		{
			if (Mahjong[mytiles[3].melds[j]] != "0")
				cout << Mahjong[mytiles[3].melds[j]] << " ";
		}
	}
	cout << "|";

	cout << endl;
	if (drop[3] != 0) cout << "               " << Mahjong[drop[3]];
	cout << endl;

	//上家牌河
	for (int j = 0; j < droptile[3].num; j++)
	{
		if (j % 8 == 0) cout << endl;
		cout << Mahjong[droptile[3].tile[j]] << "  ";
	}
	for(int i=0;i<5;i++) cout << endl;
	cout << "                                                " << ptr[countNum];

	for (int i = 0; i < 5; i++) cout << endl;

	//下家
	cout << "                                                                  " << ID[1].c_str() << endl;
	cout << "                                                                  ";
	for (int i = 0; i < mytiles[1].numOfFlowers; i++)
	{
		cout << Mahjong[mytiles[1].flowers[i]] << " ";
	}
	cout << endl;
	cout << "                                                                  ";
	for (int i = 0; i < mytiles[1].numOfTiles; i++) cout << "口";
	cout << "      ";
	for (int i = 0; i < mytiles[1].numOfMeld; i++)
	{
		cout << "|";
		for (int j = i * 4; j < i * 4 + 4; j++)
		{
			if (Mahjong[mytiles[1].melds[j]] != "0")
				cout << Mahjong[mytiles[1].melds[j]] << " ";
		}
	}
	cout << "|"<<endl;
	cout << endl;
	if (drop[1] != 0) cout << "                                                " << Mahjong[drop[1]];
	cout << endl;
	

	//下家牌河
	for (int j = 0; j < droptile[1].num; j++)
	{ 
		if (j % 8 == 0) cout << endl<<"                                                           ";
		cout << Mahjong[droptile[1].tile[j]] << "  ";
	}

	for (int i = 0; i < 3; i++) cout << endl;


	cout << endl;
	if(drop[0]!=0) cout << "                 " << Mahjong[drop[0]];
	cout << endl;

	//自家牌河
	for (int j = 0; j < droptile[0].num; j++)
	{
		if (j % 8 == 0) cout << endl;
		cout << Mahjong[droptile[1].tile[j]] << "  ";
	}
	cout << endl;

	//自家
	for (int i = 0; i < mytiles[0].numOfFlowers; i++)
	{
		cout << Mahjong[mytiles[0].flowers[i]] << " ";
	}
	cout << endl;
	for (int i = 0; i < mytiles[0].numOfTiles; i++)
	{
		cout << Mahjong[mytiles[0].tilesInHand[i]] << " ";
	}

	if (inputtile!= 0)  cout << "   " << Mahjong[inputtile] << "   ";
	else cout << "   ";
	for (int i = 0; i < mytiles[0].numOfMeld; i++)
	{
		cout << "|";
		for (int j = i * 4; j < i * 4 + 4; j++)
		{
			if (Mahjong[mytiles[0].melds[j]] != "0")
				cout << Mahjong[mytiles[0].melds[j]] << " ";
		}
	}
	cout << "|" << endl;
	cout << "                  " << ID[0].c_str() << endl;
}
*/
void outTileRiver(dropTile* droptile)
{
	for (int i = 0; i < 4; i++)
	{
		cout << "玩家" << i << "牌河:";
		for (int j = 0; j < droptile[i].num; j++)
		{
			cout << Mahjong[droptile[i].tile[j]]<< "  ";
		}
		cout << endl;
	}
		
}

void outMyTiles(myTiles *mytiles)
{

	cout << "花：";
	for (int i = 0; i < mytiles[0].numOfFlowers; i++)
	{
		cout << Mahjong[mytiles[0].flowers[i]]<<" ";
	}
	cout << endl;
	for (int i = 0; i < mytiles[0].numOfTiles; i++)
	{
		cout << Mahjong[mytiles[0].tilesInHand[i]]<< " ";
	}

	cout << "     副露：";
	for (int i = 0; i < mytiles[0].numOfMeld; i++)
	{
		cout << "|";
		for (int j = i * 4; j < i * 4 + 4; j++)
		{
			if (Mahjong[mytiles[0].melds[j]] != "0")
				cout << Mahjong[mytiles[0].melds[j]] << " ";
		}
	}
	cout << "|" << endl;

}

int todigit(char* num)
{
	return (int)((num[0] - '0') * 10 + num[1] - '0');
}

int parse(char* buffRecv, int tilesRemaining_lost, myTiles * mytiles, int& intile,dropTile *droptile,string *ID)
{
	int point = 0;
	switch (buffRecv[0])
	{
//摸到的牌
/*
如果第一位0：
1-2：进的牌
3:0:无效
  1:正常
  3:暗杠
  4:明杠
  9:胡牌
*/
	case '0':
	{
		intile = todigit(buffRecv + 1 * sizeof(char));
		if (intile > 40)
		{
			outOneTile(intile);
			return 0;
		}
		else if(intile>0)
		{
			outOneTile(intile);
			return (int)(buffRecv[3] - '0');
		}
		else return (int)(buffRecv[3] - '0');
		break;
	}

	//变更的牌
/*
如果第一位1：
1-2:剩余牌的数量
3-4:花的数量
5-28:花的名称代码
29-30：副露数量n
31-62:副露牌
63-64：牌数量
65-90:自己的牌
91:回复位
*/
	case '1':
	{
		tilesRemaining = todigit(buffRecv + 1 * sizeof(char));
		mytiles[0].numOfFlowers = todigit(buffRecv + 3 * sizeof(char));
		for (int i = 5; i < 28; i+=2)
		{
			if (buffRecv[i] > '0' && buffRecv[i] <= '9')
			{
				mytiles[0].flowers[point] = todigit(buffRecv + i * sizeof(char));
				point++;
			}
			else
			{
				point = 0;
				break;
			}
		}

		mytiles[0].numOfMeld = todigit(buffRecv + 29 * sizeof(char));
		for (int i = 65; i < 90; i += 2)
		{
			mytiles[0].tilesInHand[point] = todigit(buffRecv + i * sizeof(char));
			point++;
		}

		point = 0;
		mytiles[0].numOfTiles = todigit(buffRecv + 63 * sizeof(char));
		for (int i = 31; i < 62; i += 2)
		{
			mytiles[0].melds[point] = todigit(buffRecv + i * sizeof(char));
			point++;
		}
		return (int)(buffRecv[91]- '0');
		break;
	}

	//副露确认消息
/*
如果第一位2：
1:7吃
  6碰
  5明杠
  9胡
*/
	case '2':	
	{
		return (int)(buffRecv[1] - '0');
		break; 
	}

//出牌消息
/*
1:玩家：ID
2-3:出的牌
*/
	case '3':	
	{
		playernum = buffRecv[1] - '0';
		IDnum = (playernum - myNum + 4) % 4;
		for (int i = 0; i < 4; i++)
		{
			if (i == IDnum) drop[IDnum] = todigit(buffRecv + 2 * sizeof(char));
			else drop[i] = 0;
		}
		outFile(droptile, mytiles, ID, inTile,tilesRemaining);
		return 0; break;
	}

//舍牌消息
/*
1:玩家：ID
2-3:加入舍牌河的牌
*/
	case '4':
		
	{
		playernum = buffRecv[1] - '0';
		IDnum = (playernum - myNum + 4) % 4;
		droptile[IDnum].tile[droptile[IDnum].num] = todigit(buffRecv + 2 * sizeof(char));
		droptile[IDnum].num++;
		return 0; break;
	}

//其他家牌消息
/*
1 - 2:剩余牌的数量1
3-4  : 花数
5-28：花
29-30 : 副露数
31-62: 副露
63:玩家号
*/
	case'5':
	{
		playernum = buffRecv[63] - '0';
		countNum=IDnum = (playernum - myNum + 4) % 4;
		if (IDnum != 0)
		{
			mytiles[IDnum].numOfTiles = todigit(buffRecv + 1 * sizeof(char));
			mytiles[IDnum].numOfFlowers = todigit(buffRecv + 3 * sizeof(char));
			point = 0;
			for (int i = 5; i < 28; i += 2)
			{
				if (buffRecv[i] > '0' && buffRecv[i] <= '9')
				{
					mytiles[IDnum].flowers[point] = todigit(buffRecv + i * sizeof(char));
					point++;
				}
				else
				{
					point = 0;
					break;
				}
			}

			mytiles[IDnum].numOfMeld = todigit(buffRecv + 29 * sizeof(char));
			point = 0;
			for (int i = 31; i < 62; i += 2)
			{
				mytiles[IDnum].melds[point] = todigit(buffRecv + i * sizeof(char));
				point++;
			}
			tilesRemaining= todigit(buffRecv + 64 * sizeof(char));
		}
		outFile(droptile, mytiles, ID,inTile,tilesRemaining);
		return 0;
		break;
	}

	case '6':
	{
		int i = 0;
		playernum = buffRecv[1] - '0';
		IDnum = (playernum - myNum + 4) % 4;
		while (buffRecv[i+2] != '\0')
		{
			ID[IDnum][i] = buffRecv[i + 2];
			i++;
		}
		ID[IDnum][i] = '\0';
		cout << "玩家 " << playernum << ": " << ID[IDnum].c_str() <<" 已经加入游戏。" << endl;
		return 0; break;
	}

	case '7':return -1; break;
	case '8':return 11; break;
	case '9':
	{
		myNum = buffRecv[1] - '0';
		cout << "连接服务器成功，您是" << myNum << "号玩家，请输入您的ID：";
		cin >> ID[0];
		cout << endl;
		cout << "尊敬的" << ID[0] << ",欢迎来到常熟麻将教室！请等待其他人游戏开始..." << endl;

		return 0; break;
	}
	default:cout << "error message!"; cout<<buffRecv<<endl; return 0; break;

	}
}

void complie(char* buff, char* buffSend)
{
	if (buff[0] == '0' || buff[0] == 'n'|| buff[0] == 'N')
	{
		buffSend[0] = '0';
		buffSend[1] = '\0';
	}
	else if (buff[0] == 'y'|| buff[0] == 'Y')
	{
		buffSend[0] = '1';
		buffSend[1] = '\0';
	}

	//舍牌类消息
	else
	{
		if (buff[1]<'0' || buff[1]>'9')
		{
			buffSend[0] = '2';
			buffSend[1] = buff[0];
			buffSend[2] = '\0';
		}

		else
		{
			buffSend[0] = '3';
			buffSend[1] = buff[0];
			buffSend[2] = buff[1];
			buffSend[3] = '\0';
		}

	}

}



void main()
{
	/* 1. 加载 WinSock 库 */
	WSADATA wsaData;
	char buffRecv[128];
	char buffSend[128];
	char buff[128];
	int recvSize;
	memset(buffRecv, 0, sizeof(buffRecv));
	myTiles mytiles[4];
	int reply = 0;
	dropTile droptile[4];
	string ID[4];
	bool begin = 0;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("failed to load Winsock");
		return;
	}

	/* 2. 创建用于通讯的套接字 socket() */
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == sockClient) {
		printf("Socket() error:%d", WSAGetLastError());
		return;
	}

	/* 3. 设置要连接的服务端的ip地址，端口等属性*/
	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(6666);
	addrSrv.sin_addr.S_un.S_addr = inet_addr("121.5.178.37");

	/* 4. 向服务器发送连接请求*/
	if (connect(sockClient, (struct sockaddr*)&addrSrv, sizeof(addrSrv)) == INVALID_SOCKET) {
		printf("Connect falied:%d", WSAGetLastError());
		return;
	}
	else
	{
		cout << "正在连接服务器...";
	
	}

	//服务器返回ID信息
	recv(sockClient, buffRecv, sizeof(buffRecv), 0);
	//分析ID信息
	parse(buffRecv, tilesRemaining, mytiles, inTile, droptile, ID);
	strcpy_s(buffSend, ID[0].c_str());
	if (send(sockClient,buffSend, sizeof(buffSend), 0) < 0)
	{
		printf("write Error");
		exit(0);
	}

	//游戏未开始
	while (begin == false)
	{
		if (recvSize = recv(sockClient, buffRecv, sizeof(buffRecv), 0) < 0)
		{
			printf("read Error");
			exit(0);
		}
		else
		{
			if (parse(buffRecv, tilesRemaining, mytiles, inTile, droptile, ID) < 0)
				begin = true;
		}
	}
	cout << "人数已满，游戏开始";

	/* 5. 接收，发送数据*/
	while (1)
	{


		//接收
		if (recvSize = recv(sockClient, buffRecv, sizeof(buffRecv), 0) < 0)
		{
			printf("read Error");
			exit(0);
		}
		else
		{
			reply=parse(buffRecv,tilesRemaining,mytiles,inTile,droptile,ID);
			//outAll(droptile,mytiles,ID);
			if (reply != 0)
			{
				//发送数据
				switch (reply)
				{
				case 1:
				{
					cout << "请输入牌的序号,打出一张牌："; break;
				}
				case 7:
				{
					cout << "是否吃牌（y/n）："; break;
				}
				case 3:
				{
					cout << "是否暗杠（y/n）："; break;
				}
				case 4:
				{
					cout << "是否明杠（y/n）："; break;
				}
				case 5:
				{
					cout << "是否明杠（y/n）："; break;
				}
				case 6:
				{
					cout << "是否碰牌（y/n）："; break;
				}
				case 9:
				{
					cout << "是否胡牌（y/n）："; break;
				}
				case 2:
				{
					cout << "请依次输入要吃的两张牌："; break;
				}
				case 8:
				{
					cout << "请输入杠牌："; break;
				}
				case 11:
				{
					cout << "恭喜胡牌！" << endl; break;
				}

				case 12:
				{
					break;
				}

				case 21:
				{
					break;
				}
				default:cout << "error!"<<reply<<endl; break;
				}
				fgets(buff, MAXSIZE, stdin);
				complie(buff, buffSend);
				if (send(sockClient, buffSend, sizeof(buffSend), 0) < 0)
				{
					printf("write Error");
					exit(0);
				}
				inTile = 0;
			}

			
			memset(buffRecv, 0, sizeof(buffRecv));
			memset(buffSend, 0, sizeof(buffSend));
		}


	}
	

	/* 6. 关闭socket*/
	closesocket(sockClient);
	WSACleanup();
	system("pause");
}

