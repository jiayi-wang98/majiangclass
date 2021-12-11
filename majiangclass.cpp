#include "PlayerTiles.h"
#include <iostream>
#include<algorithm>
#include<time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

using namespace std;

//Socket
#define MAXSIZE 128
#define IP_ADDR "121.5.178.37"
#define IP_PORT 6666

const int Stack[144] = {
                1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,41,41,41,41,42,43,44,45,46,47,48,49,
                11,11,11,11,12,12,12,12,13,13,13,13,14,14,14,14,15,15,15,15,16,16,16,16,17,17,17,17,18,18,18,18,19,19,19,19,
                21,21,21,21,22,22,22,22,23,23,23,23,24,24,24,24,25,25,25,25,26,26,26,26,27,27,27,27,28,28,28,28,29,29,29,29,
                31,31,31,31,32,32,32,32,33,33,33,33,34,34,34,34,35,35,35,35,36,36,36,36,37,37,37,37,38,38,38,38,39,39,39,39 };


int Rand(int i) { return rand() % i; }
int nrecvSize = 0;


//获取数据
int getMsg(int i_connfd)
{

    char msg[MAXSIZE];
    while(read(i_connfd, msg, MAXSIZE)<0);
    cout << msg << endl;
    int ret;
    if (msg[0] == '0')
        ret= 0;
    else if (msg[0] == '1')
        ret= 1;
    else if (msg[0] == '2')
        ret= msg[1] - '0';
    else if (msg[0]=='3')
        ret= ((msg[1] - '0') * 10 + msg[2] - '0');
    return ret;
}

void tochar(int num, char* buff)
{
    if (num < 10)
    {
        buff[0] = '0';
        buff[1] = '0' + num;
    }
    else
    {
        buff[0] = '0' + num / 10;
        buff[1] = '0' + num % 10;
    }
}

void sendAll(int* i_connfd, PlayerTiles player1,int num,int tilesRemaining)
{
    int point = 0;
    char buffSend[MAXSIZE];
    buffSend[0] = '5';
    tochar(player1.num_tiles, buffSend + 1 * sizeof(char));

    //发送花的信息
    tochar(player1.num_flower, buffSend + 3 * sizeof(char));
    for (int i = 5; i < 28; i += 2)
    {
        tochar(player1.flower[point], buffSend + i * sizeof(char));
        point++;
    }
    point = 0;
    tochar(player1.playermeld.num, buffSend + 29 * sizeof(char));
    //转换4*4数组为16*1数组
    int meld[16] = { 0 };
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            meld[i * 4 + j] = player1.playermeld.tile[i][j];

    //发送副露信息
    for (int i = 31; i < 62; i += 2)
    {
        tochar(meld[point], buffSend + i * sizeof(char));
        point++;
    }
    point = 0;
    buffSend[63] = '0' + num;
    tochar(tilesRemaining, buffSend + 64 * sizeof(char));
    buffSend[66] = '\0';
    for (int i = 0; i < 4; i++)
    {
        write(i_connfd[i], buffSend, MAXSIZE);
    }


}

//发送数据
void sendMsg(int i_connfd, PlayerTiles player1, int type = 1, int inTile = 0, int reply = 0, int TilesRemaining = 0, int meldType = 0,int playernum=0,int playerTile=0)
{
    char buffSend[MAXSIZE];
    switch (type)
    {
    case 0://发送摸到的牌
    {
        buffSend[0] = '0';
        tochar(inTile, buffSend + 1 * sizeof(char));
        buffSend[3] = '0' + reply; 
        buffSend[4] = '\0';
        break;
    }
    case 1://发送牌堆信息
    {
        int point = 0;
        buffSend[0] = '1';

        //数量
        tochar(TilesRemaining, buffSend + 1 * sizeof(char));

        //发送花的信息
        tochar(player1.num_flower, buffSend + 3 * sizeof(char));
        for (int i = 5; i < 28; i+=2)
        {
            tochar(player1.flower[point], buffSend + i * sizeof(char));
            point++;
        }
        point = 0;
        tochar(player1.playermeld.num, buffSend + 29 * sizeof(char));

        //转换4*4数组为16*1数组
        int meld[16] = { 0 };
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                meld[i*4+j] = player1.playermeld.tile[i][j];

        //发送副露信息
        for (int i = 31; i < 62; i += 2)
        {
            tochar(meld[point], buffSend + i * sizeof(char));
            point++;
        }
        point = 0;

        //发送手牌的信息
        tochar(player1.num_tiles, buffSend + 63 * sizeof(char));
        for (int i = 65; i < 90; i += 2)
        {
            tochar(player1.tilesInHand[point], buffSend + i * sizeof(char));
            point++;
        }

        //发送回复信息
        buffSend[91] = '0' + reply;
        buffSend[92] = '\0';
        break;
    }

    //副露消息
    case 2:
    {
        buffSend[0] = '2';
        buffSend[1] = meldType + '0';
        buffSend[2] = '\0';
        break;
    }

    //玩家出牌
    case 3:
    {
        buffSend[0] = '3';
        buffSend[1] = playernum + '0';
        tochar(playerTile, buffSend + 2 * sizeof(char));
        buffSend[4] = '\0';
        break;
    }
    case 4:

    {
        buffSend[0] = '4';
        buffSend[1] = playernum+'0';
        tochar(playerTile, buffSend + 2 * sizeof(char));
        buffSend[4] = '\0';
        break;
    }
    case 8:
    {
        buffSend[0] = '8';
        buffSend[1] = '\0';
        break;
    }

    case 9:
    {
        buffSend[0] = '9';
        buffSend[1] = '0'+playernum;
        buffSend[2] = '\0';
        break;
    }
    default:buffSend[0] = '\0'; cout << "default error!"; break;
    }

    
    cout << buffSend << endl;
    write(i_connfd, buffSend, MAXSIZE);
}

void sendMsg_9(int i_connfd, int playernum = 0)
{
    char buffSend[MAXSIZE];
    buffSend[0] = '9';
    buffSend[1] = '0' + playernum;
    buffSend[2] = '\0';

    cout << buffSend << endl;
    write(i_connfd, buffSend, MAXSIZE);
}

void sendMsg_7(int i_connfd)
{
    char buffSend[MAXSIZE];
    buffSend[0] = '7';
    buffSend[1] = '\0';
    cout << buffSend << endl;
    write(i_connfd, buffSend, MAXSIZE);
}

void sendMsg_6(int i_connfd,int playernum,char *msg)
{
    char buffSend[MAXSIZE];
    buffSend[0] = '6';
    buffSend[1] = '0'+playernum;
    int i = 0;
    while(msg[i] != '\0')
    {
        buffSend[i + 2] = msg[i];
        i++;
    }
    buffSend[i + 2] = '\0';
    //memcpy(buffSend + 2 * sizeof(char), msg, sizeof(msg));
    cout << buffSend << endl;
    write(i_connfd, buffSend, MAXSIZE);
}


int main()
{
    char ID[4][MAXSIZE];

    //Socket Connection
    int i_listenfd, i_connfd[4];
    struct sockaddr_in st_sersock;

    if ((i_listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)	//建立socket套接字
    {
        printf("socket Error: %s (errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    memset(&st_sersock, 0, sizeof(st_sersock));
    st_sersock.sin_family = AF_INET;  //IPv4协议
    st_sersock.sin_addr.s_addr = htonl(INADDR_ANY);	
    st_sersock.sin_port = htons(IP_PORT);

    if (bind(i_listenfd, (struct sockaddr*)&st_sersock, sizeof(st_sersock)) < 0) //将套接字绑定IP和端口用于监听
    {
        printf("bind Error: %s (errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    if (listen(i_listenfd, 5) < 0)	//设定可同时排队的客户端最大连接个数
    {
        printf("listen Error: %s (errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    printf("======waiting for client's request======\n");
    //准备接受客户端连接
    {

        for (int i = 0; i < 4; i++)
        {
            if ((i_connfd[i] = accept(i_listenfd, (struct sockaddr*)NULL, NULL)) < 0)	//阻塞等待客户端连接
            {
                printf("accept Error: %s (errno: %d)\n", strerror(errno), errno);
                //	continue;
            }
            else
            {
                sendMsg_9(i_connfd[i],i);
                char msg[MAXSIZE];
                read(i_connfd[i], msg, MAXSIZE);
                printf("Client[%d], welcome!\n", i_connfd[i]);
                memcpy(ID[i], msg, MAXSIZE);
                cout << "Name of ID:" << i << " is:" << ID[i] << endl;

                //告知用户房间内其他用户
                for (int j = 0; j < i; j++)
                {
                    sendMsg_6(i_connfd[i], j, ID[j]);
                }
                //告知其他用户新用户加入
                for (int k = 0; k < i; k++)
                {
                    sendMsg_6(i_connfd[k], i, ID[i]);
                }
            }
        }
    }

    //发送游戏开始信号
    for (int i = 0; i < 4; i++)
    {
        sendMsg_7(i_connfd[i]);
    }


    //begin Game 
    cout << "开始常熟麻将！" << endl;

    srand(unsigned(time(0)));

    int randomStack[144];
    memcpy(randomStack, Stack, 144 * sizeof(int));
    random_shuffle(randomStack, randomStack + 144, Rand);
    random_shuffle(randomStack, randomStack + 144, Rand);
    random_shuffle(randomStack, randomStack + 144, Rand);

    //四家发牌
    int discardtile=0;
    int tileRemain = 0;
    int tileRiver[4][30];
    int tileRiverNum[4] = {0,0,0,0};
    int begintile[4][13] = {0};
    int beginpoint = 0, endpoint = 143;
    for (int k = 0; k < 3; k++)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                begintile[i][j+k*4] = randomStack[beginpoint];
                beginpoint++;
            }
        }
    }

    for (int j = 0; j < 4; j++)
    {
        begintile[j][12] = randomStack[beginpoint];
        beginpoint++;
    }

    PlayerTiles player1[4] = {begintile[0],begintile[1], begintile[2], begintile[3]};//初始化13张牌

    //补花
    for (int i = 0; i < 4; i++)
    {
        while (player1[i].num_compensate > 0)
        {
            player1[i].getcompensate(randomStack[endpoint]);
            endpoint--;
        }
    }
    
    tileRemain = endpoint - beginpoint;

    //输出现有的牌
    for (int i = 0; i < 4; i++)
    {
        player1[i].sort();
        player1[i].outputTiles();
        //TCP
        sendMsg(i_connfd[i], player1[i], 1, 0, 0, tileRemain);
        sendAll(i_connfd, player1[i], i,tileRemain);
    }

    int count = 0;
    bool choose = 0;
    int inputtile=0;



    //state machine
    while (beginpoint<=endpoint)
    {
        switch (player1[count].state)
        {
        case 0://摸牌
        {
            inputtile = randomStack[beginpoint];
            beginpoint++;
            player1[count].getdraw(inputtile);
            player1[count].outoneTile(inputtile);
            tileRemain = endpoint - beginpoint;


            cout << "剩余牌数：" << endpoint - beginpoint << "\n";
            player1[count].sort();
            if (player1[count].handwin())
            {
                cout << "胡？";
                //TCP
                sendMsg(i_connfd[count], player1[count], 0, inputtile,9);
                choose = getMsg(i_connfd[count]);//自摸胡牌
                if (choose)
                {
                    cout << "你胡了！";
                    //TCP

                    player1[count].state = 9;//进入胡牌
                    break;
                }
            }

            if (inputtile > 40) //摸到花
            {
                player1[count].state = 2;
                break;//进入补牌状态
            }


            if (player1[count].clkong())//可以暗杠
            {
                
                cout << "暗杠？";
                //TCP
                sendMsg(i_connfd[count], player1[count], 0, inputtile, 3);
                choose = getMsg(i_connfd[count]);
                if (choose)
                {
                    player1[count].state = 3;//进入暗杠
                    break;
                }
            }

            if (player1[count].exkong_self()>0)//可以明杠
            {
                cout << "明杠？";
                //TCP
                sendMsg(i_connfd[count], player1[count], 0, inputtile, 4);
                choose = getMsg(i_connfd[count]);
                if (choose)
                {
                    player1[count].state = 4;//进入明杠
                    break;
                }
            }
            player1[count].state = 1;//进入打牌
            break;
        }

        case 1://打牌
        {
            cout << "选择你要出的牌:";
            //TCP
            sendMsg(i_connfd[count], player1[count], 0, inputtile, 1, 0, 1);
            discardtile = getMsg(i_connfd[count]);
            if (discardtile == 0)
            {
                player1[count].discard(inputtile);
                player1[count].sort();
                //system("clear");
                player1[count].outputTiles();
                discardtile = inputtile;
            }
            else if (player1[count].discard(discardtile))
            {
                player1[count].sort();
                //system("clear");
                player1[count].outputTiles();
            }
            else
            {
                int valid = 0;
                while (valid == 0)
                {
                    cout << "无效，请重新输入:";
                    //TCP
                    sendMsg(i_connfd[count], player1[count], 0, inputtile, 1, 0, 1);
                    discardtile = getMsg(i_connfd[count]);
                    if (player1[count].discard(discardtile))
                    {
                        player1[count].sort();
                        //system("clear");
                        player1[count].outputTiles();
                        valid = 1;
                    }
                }
            }
            sendMsg(i_connfd[count], player1[count],1,0,0, endpoint - beginpoint);
            sendAll(i_connfd, player1[count], count, endpoint-beginpoint);
            for (int i = 0; i < 4; i++)
            {
                sendMsg(i_connfd[i], player1[i], 3, 0,0 , 0, 0, count, discardtile);
            }
            player1[count].generateReady();
            player1[count].state = 10;//进入等待听牌
            break;

        }

        case 2://补牌
        {
            inputtile = randomStack[endpoint];
            endpoint--;
            player1[count].getdraw(inputtile);
            sendAll(i_connfd, player1[count], count, endpoint - beginpoint);
            player1[count].outoneTile(inputtile);
            
            if (player1[count].handwin())
            {
                cout << "胡？";
                //TCP
                sendMsg(i_connfd[count], player1[count], 0, inputtile, 1, 0, 9);
                choose = getMsg(i_connfd[count]);//自摸胡牌
                if (choose)
                {
                    cout << "你胡了！";
                    player1[count].state = 9;//进入胡牌
                    break;
                }
            }

            if (inputtile > 40) //摸到花
            {
                player1[count].state = 2;
                break;//进入补牌状态
            }


            if (player1[count].clkong())//可以暗杠
            {
                sendMsg(i_connfd[count], player1[count], 0, inputtile, 1, 0, 3);
                cout << "暗杠？";
                choose = getMsg(i_connfd[count]);
                if (choose)
                {
                    player1[count].state = 3;//进入暗杠
                    break;
                }
            }

            if (player1[count].exkong_self()>0)//可以明杠
            {
                cout << "明杠？";
                sendMsg(i_connfd[count], player1[count], 0, inputtile, 1, 0, 4);
                choose = getMsg(i_connfd[count]);
                if (choose)
                {
                    player1[count].state = 4;//进入明杠
                    break;
                }
            }
            player1[count].state = 1;//进入打牌
            break;
        }

        case 3://暗杠
        {
            int Kongtile;
            if (player1[count].clkong() > 1)
            {
                cout << "选择暗杠的牌:";
                sendMsg(i_connfd[count], player1[count],0,0,8);
                Kongtile= getMsg(i_connfd[count]);
                bool flag = 0;
                while (!player1[count].clkong(Kongtile))
                {
                    cout << "无效，请重新输入:";
                    sendMsg(i_connfd[count], player1[count], 0, 0,  8);
                    Kongtile = getMsg(i_connfd[count]);
                }
            }
            else
            {
                Kongtile = player1[count].clkong_tile();
            }
            player1[count].clkongInMeld(Kongtile);
            sendMsg(i_connfd[count], player1[count], 1, 0, 0, endpoint - beginpoint);
            sendAll(i_connfd, player1[count], count, endpoint - beginpoint);
            player1[count].state = 2;//进入补牌
            break;
        }

        case 4://明杠--自
        {
            int Kongtile;
            if (player1[count].exkong_self() > 1)
            {
                cout << "选择明杠的牌:";
                sendMsg(i_connfd[count], player1[count], 0, 0, 8);
                Kongtile = getMsg(i_connfd[count]);
                bool flag = 0;
                while (!player1[count].exkong_self(Kongtile))
                {
                    cout << "无效，请重新输入:";
                    sendMsg(i_connfd[count], player1[count], 0, 0,  8);
                    Kongtile = getMsg(i_connfd[count]);
                }
            }
            else
            {
                Kongtile = player1[count].exkong_selftile();
                sendAll(i_connfd, player1[count], count, endpoint - beginpoint);
            }
            player1[count].exkong_selfInMeld(Kongtile);
            sendMsg(i_connfd[count], player1[count],1,0,0, endpoint - beginpoint);
            sendAll(i_connfd, player1[count], count, endpoint - beginpoint);
            player1[count].state = 2;//进入补牌
            break;
        }
        
        case 5://明杠
        {
            player1[count].exkongInMeld(discardtile);
            sendAll(i_connfd, player1[count], count, endpoint - beginpoint);
            player1[count].state = 2;
            player1[count].sort();
            //system("clear");
            player1[count].outputTiles();
            sendMsg(i_connfd[count], player1[count],1,0,0, endpoint - beginpoint);
            sendAll(i_connfd, player1[count], count, endpoint - beginpoint);
            break;
        }

        case 6://碰
        {
            player1[count].penInMeld(discardtile);
            player1[count].state = 1;
            player1[count].sort();
            //system("clear");
            player1[count].outputTiles();
            sendMsg(i_connfd[count], player1[count],1,0,0, endpoint - beginpoint);
            sendAll(i_connfd, player1[count], count, endpoint - beginpoint);
            inputtile = 0;
            break;
        }

        case 7://吃
        {
            if (player1[count].eat(discardtile) == 1)
            {
                player1[count].eatInMeld_1(discardtile);
            }
            else if (player1[count].eat(discardtile) > 1)
            {
                int tile1, tile2,tiles[3];
                cout << "输入吃的牌：";
                sendMsg(i_connfd[count], player1[count], 0, 0, 2);
                tile1 = getMsg(i_connfd[count]);
                sendMsg(i_connfd[count], player1[count], 0, 0, 2);
                tile2 = getMsg(i_connfd[count]);
                tiles[0] = discardtile; tiles[1] = tile1; tiles[2] = tile2;
                player1[count].sort(tiles, 3);
                if (tiles[0] + 1 == tiles[1] && tiles[0] + 2 == tiles[2])
                {
                    player1[count].eatInMeld_2(discardtile, tile1, tile2);
                }
                else
                {
                    int valid = 0;
                    while (valid == 0)
                    {
                        cout << "无效，请重新输入";
                        sendMsg(i_connfd[count], player1[count], 0, 0, 2);
                        tile1 = getMsg(i_connfd[count]);
                        sendMsg(i_connfd[count], player1[count], 0, 0, 2);
                        tile2 = getMsg(i_connfd[count]);
                        tiles[0] = discardtile; tiles[1] = tile1; tiles[2] = tile2;
                        player1[count].sort(tiles, 3);
                        if (tiles[0] + 1 == tiles[1] && tiles[0] + 2 == tiles[2])
                        {
                            player1[count].eatInMeld_2(discardtile, tile1, tile2);
                            valid = 1;
                        }
                    }
                }
            }
            player1[count].sort();
            //system("clear");
            player1[count].outputTiles();
            inputtile = 0;
            sendMsg(i_connfd[count], player1[count],1,0,0, endpoint - beginpoint);
            sendAll(i_connfd, player1[count], count, endpoint - beginpoint);
            player1[count].state = 1;
            break;
        }

        case 9://胡牌
        {
            cout << "你胡了，恭喜你！";
            sendMsg(i_connfd[count], player1[count],8);
            sendAll(i_connfd, player1[count], count, endpoint - beginpoint);
            choose = getMsg(i_connfd[count]);
            return 0;//暂时结束
            break;
        }

        case 10://等待
        {   
            
            //判断吃、碰、杠、胡   

            int flag = 0;
            for (int i = 0; i < 4; i++)
            {
                if (i == count)
                    continue;
                if (player1[i].chongwin(discardtile))
                {
                    cout << "胡？";
                    sendMsg(i_connfd[i], player1[i], 2, 0, 1, 0, 9);
                    choose = getMsg(i_connfd[i]);//自摸胡牌
                    if (choose)
                    {
                        flag = 1;
                        count = i;
                        cout << "你胡了！";
                        player1[i].state = 9;//进入胡牌
                    }
                }
            }

            if (flag == 1)
            {
                break;
            }


            for (int i = 0; i < 4; i++)
            {
                if (i == count)
                    continue;
                if (player1[i].exkong(discardtile))
                {
                    cout << "杠？";
                    sendMsg(i_connfd[i], player1[i], 2, 0, 0, 0, 5);
                    choose = getMsg(i_connfd[i]);//是否gang牌
                    if (choose)
                    {
                        player1[count].state = 0;
                        count = i;
                        player1[count].state = 5;//进入杠牌
                        flag = 1;
                        break;
                    }
                }
            }
            if (flag == 1)
            {
                break;
            }

            for (int i = 0; i < 4; i++)
            {
                if (i == count)
                    continue;
                if (player1[i].pen(discardtile))
                {
                    cout << "碰？";
                    sendMsg(i_connfd[i], player1[i], 2, 0, 0, 0, 6);
                    choose = getMsg(i_connfd[i]);//是否碰牌
                    if (choose)
                    {
                        player1[count].state = 0;
                        count = i;
                        player1[count].state = 6;//进入碰牌
                        flag = 1;
                        break;
                    }
                }
            }

            if (flag == 1)
            {
                break;
            }


            if (player1[(count + 1) % 4].eat(discardtile) > 0)
            {
                cout << "吃？";
                sendMsg(i_connfd[(count + 1) % 4], player1[(count + 1) % 4], 2, 0, 0, 0, 7);
                choose = getMsg(i_connfd[(count + 1) % 4]);//是否吃牌
                if (choose)
                {
                    player1[count].state = 0;
                    count++;
                    count = count % 4;
                    player1[count].state = 7;//进入吃牌
                    break;
                }
            }
           

            if (discardtile != 0)
            {
                for (int i = 0; i < 4; i++)
                {
                    sendMsg(i_connfd[i], player1[0], 4, 0, 0, 0, 0, count, discardtile);
                }
            }
            player1[count].state = 0;
            count++;
            count = count % 4;
            break;
        }

        default:break;

        }
     }
     cout << "平局，无人胡牌";
     choose = getMsg(i_connfd[0]);
     close(i_connfd[0]);
     close(i_connfd[1]);
     close(i_connfd[2]);
     close(i_connfd[3]);
     close(i_listenfd);
     return 0;
          
 }
