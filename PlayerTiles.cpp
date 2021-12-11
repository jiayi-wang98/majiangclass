#include "PlayerTiles.h"
#include <iostream>
#include<algorithm>
#include<time.h>
#include<cstring>

using namespace std;

PlayerTiles:: PlayerTiles(int* begintiles)
{
    state=num_compensate = playermeld.num = num_flower = num_tiles = 0;
    numOfTypes[0] = numOfTypes[1] = numOfTypes[2] = numOfTypes[3] = 0;
    for (int i = 0; i < 13; i++)
    {
        if (begintiles[i] > 40)
        {
            flower[num_flower] = begintiles[i];
            num_flower++;
            num_compensate++;
        }
        else
        {
            tilesInHand[num_tiles] = begintiles[i];
            num_tiles++;
            numOfTypes[begintiles[i] / 10]++;
        }
    }
    num_ready = 0;
    state = 10;

};

bool PlayerTiles::find(int tile) 
{
    for (int i = 0; i < num_tiles; i++)
    {
        if (tilesInHand[i] == tile)
            return true;
    }
    return false;
}

bool PlayerTiles::handwin()
{
    int flag1 = 0;
    int str1 = 0;
    for (int i = 0; i < 4; i++)
    {
        //cout << " numOfTypes [" << i << "]" << numOfTypes[i]<<"\n";
        if (numOfTypes[i] % 3 == 0)
        {
            if (!ready3((tilesInHand + str1), numOfTypes[i]))
            {
                return false;
            }
            str1 += numOfTypes[i];
        }
        else if (numOfTypes[i] % 3 == 1)
        {
            return false;
        }
        else if (numOfTypes[i] % 3 == 2)
        {
            flag1++;

            if (flag1 > 1) { return false; }

            else if (!ready2(tilesInHand + str1, numOfTypes[i]))
            {
                return false;
            }
            else
            {
                str1 += numOfTypes[i];
            }
        }
    }
    return true;
}

bool PlayerTiles::handwin(int *tilesInHand, int* numOfTypes)
{
    int flag1 = 0;
    int str1 = 0;
    for (int i = 0; i < 4; i++)
    {
        //cout << " numOfTypes [" << i << "]" << numOfTypes[i]<<"\n";
        if (numOfTypes[i] % 3 == 0)
        {
            if (!ready3((tilesInHand + str1), numOfTypes[i]))
            {
                return false;
            }
            str1 += numOfTypes[i];
        }
        else if (numOfTypes[i] % 3 == 1)
        {
            return false;
        }
        else if (numOfTypes[i] % 3 == 2)
        {
            flag1++;

            if (flag1 > 1) { return false; }

            else if (!ready2(tilesInHand + str1, numOfTypes[i]))
            {
                return false;
            }
            else
            {
                str1 += numOfTypes[i];
            }
        }
    }
    return true;
}

void PlayerTiles::generateReady()
{
    num_ready = 0;
    int temptile[14];
    int numOfType_temp[4];
    const int alltiles[33] = {1,2,3,4,5,6,11,12,13,14,15,16,17,18,19,21,22,23,24,25,26,27,28,29,31,32,33,34,35,36,37,38,39};
    for (int i = 0; i < 33; i++)
    {
        memcpy(temptile, tilesInHand, num_tiles * sizeof(int));
        memcpy(numOfType_temp, numOfTypes, 4 * sizeof(int));

        temptile[num_tiles] = alltiles[i]; 
        numOfType_temp[alltiles[i] / 10]++;
        sort(temptile, num_tiles+1);
        if (handwin(temptile, numOfType_temp))
        {
            readyTiles[num_ready] = alltiles[i];
            num_ready++;
        }
    }

    cout << "Winning Tiles:";
    for (int i = 0; i < num_ready; i++)
    {
        cout << Mahjong[readyTiles[i]] << "   ";
    }
    cout << "\n";
}

void PlayerTiles::sort(int* temptile, int num)
{
    int i = 0, j = 0;
    int temp = 0;
    for (i = 0; i < num - 1; i++)
    {
        for (j = i + 1; j < num; j++)
        {
            if (temptile[j] < temptile[i])
            {
                temp = temptile[i];
                temptile[i] = temptile[j];
                temptile[j] = temp;
            }
        }
    }
}

bool PlayerTiles::chongwin(int tile)
{
    for (int i = 0; i < num_ready; i++)
    {
        if (tile == readyTiles[i])
        {
            return true;
        }
   }
    return false;
}

bool PlayerTiles::ready2(const int* tiles, int num)
{
    int removepairs[14] = { 0 };
    int pairtile;
    for (int i = 0; i < num - 1; i++)
    {
        memcpy(removepairs, tiles, num * sizeof(int));
        if (tiles[i] == tiles[i + 1])
        {
            i++;
            pairtile = tiles[i];
            if (!del(removepairs, num, pairtile)) return 0;
            if (!del(removepairs, num - 1, pairtile))  return 0;
            if (ready3(removepairs, num - 2))
                return true;
        }
    }
    return false;
}

bool PlayerTiles::ready3(const int* tiles, int num)
{
    if (num == 0) return true;
    else
    {
        while (num > 2)
        {
            //如果是字牌类
            if (tiles[0] < 10)
            {
                if (tiles[num - 1] == tiles[num - 2] && tiles[num - 1] == tiles[num - 3]) //如果每三个相等，则win
                    num -= 3;
                else return false;
            }

            //非字牌
            else
            {
                if ((tiles[num - 1] == tiles[num - 2] && tiles[num - 1] == tiles[num - 3]) || (tiles[num - 1] == tiles[num - 2] + 1 && tiles[num - 1] == tiles[num - 3] + 2))//如果刻子或顺子，则win
                    num -= 3;

                //校验特殊情况
                else
                {
                    if (num > 5) //检验6牌特殊情况
                    {
                        int diff = tiles[num - 6] - 1;
                        int temparray[12];
                        bool flag = 0;

                        for (int i = 0; i < 6; i++)
                        {
                            temparray[i] = tiles[num - 6 + i] - diff;
                        }

                        for (int i = 0; i < 3; i++)
                        {
                            if (isEqual(temparray, ready_num6[i], 6))
                            {
                                flag = 1;
                                break;
                            }
                        }
                        if (flag == 1)  //若符合，则继续
                        {
                            num -= 6;
                        }
                        else //否则继续检验9牌特殊情况
                        {
                            if (num > 8)
                            {
                                for (int i = 0; i < 9; i++)
                                {
                                    temparray[i] = tiles[num - 9 + i] - diff;
                                }
                                for (int i = 0; i < 2; i++)
                                {
                                    if (isEqual(temparray, ready_num9[i], 9))
                                    {
                                        flag = 1;
                                        break;
                                    }
                                }
                                if (flag == 1)  //若符合，则继续
                                {
                                    num -= 9;
                                }
                                else//继续检验12牌型
                                {
                                    if (num > 11)
                                    {
                                        for (int i = 0; i < 12; i++)
                                        {
                                            temparray[i] = tiles[num - 12 + i] - diff;
                                        }
                                        for (int i = 0; i < 3; i++)
                                        {
                                            if (isEqual(temparray, ready_num12[i], 12))
                                            {
                                                flag = 1;
                                                break;
                                            }
                                        }
                                        if (flag == 1)  //若符合，则继续
                                        {
                                            num -= 12;
                                        }
                                        else return false;
                                    }
                                    else return false;
                                }

                            }
                            //无牌，失败
                            else return false;
                        }

                    }

                    //如果没有牌了就失败
                    else return false;
                }
            }
        }
        return true;
    }

}
bool PlayerTiles::isEqual(int* A, int* B, int num)
{
    int i = num;
    while (i > 0)
    {
        if (A[num - i] != B[num - i]) return false;
        else i--;
    }
    return true;
}

bool PlayerTiles::del(int* array, int len, int val)
{
    for (int i = 0; i < len; i++)
    {
        if (array[i] == val)
        {
            for (int j = i; j < len - 1; j++)
            {
                array[j] = array[j + 1];
            }
            array[len - 1] = 0;

            /*
            for (int j = 0; j < len; j++)
            {
                cout << array[j]<<"  ";
            }
            cout << "\n";
             */
            return true;
        }
    }
    //cout << "delete error!\n";
    return false;
}
void PlayerTiles::sort()
{
    int i = 0, j = 0;
    int temp = 0;
    for (i = 0; i < num_tiles - 1; i++)
    {
        for (j = i + 1; j < num_tiles; j++)
        {
            if (tilesInHand[j] < tilesInHand[i])
            {
                temp = tilesInHand[i];
                tilesInHand[i] = tilesInHand[j];
                tilesInHand[j] = temp;
            }
        }
    }
}

void PlayerTiles::outputTiles()
{
    for (int i = 0; i < num_flower; i++)
    {
        cout << Mahjong[flower[i]];
    }
    cout <<"        "<<"Flowers:" << num_flower<<endl;

    for (int i = 0; i < num_tiles; i++)
    {

        cout << Mahjong[tilesInHand[i]] << "  ";
    }

    cout << "          Signaling:";
    for (int i = 0; i < playermeld.num; i++)
    {
        if (playermeld.type[i] < 2)
        {
            cout << "| ";
            for (int j = 0; j < 3; j++)
                cout << Mahjong[playermeld.tile[i][j]]<< " ";
            cout << " |";
        }
        else
        {
            cout << "| ";
            for (int j = 0; j < 4; j++)
                cout << Mahjong[playermeld.tile[i][j]] << " ";
            cout << " |";
        }
    }
    cout << endl;
}

void PlayerTiles::getdraw(int draw)
{
    if (draw > 40)
    {
        flower[num_flower] = draw;
        num_flower++;
        num_compensate++;
    }
    else
    {
        tilesInHand[num_tiles] = draw;
        numOfTypes[draw/10]++;
        num_tiles++;
    }
}

void PlayerTiles::getcompensate(int tile)
{
    if (tile > 40)
    {
        flower[num_flower] = tile;
        num_flower++;
        num_compensate++;
    }
    else
    {
        tilesInHand[num_tiles] = tile;
        numOfTypes[tile / 10]++;
        num_tiles++;
    }
    num_compensate--;
}

bool PlayerTiles::discard(int tile)
{
    if (!del(tilesInHand, num_tiles, tile)) return false;
    num_tiles--;
    numOfTypes[tile / 10]--;
    return true;
}

void PlayerTiles::outoneTile(int draw)
{
    cout << Mahjong[draw] << endl;
}

int PlayerTiles::eat(int tile)
{
    int eattype = 0;
    if (tile < 10 || tile>40)
    {
        return 0;
    }
    else
    {
        if (tile % 10 == 1)
        {
            if (find(tile + 1) && find(tile + 2))
                return 1;
        }
        else if (tile % 10 == 9)
        {
            if (find(tile - 1) && find(tile - 2))
                return 1;
        }
        else if (tile % 10 == 2)
        {
            if (find(tile - 1) && find(tile + 1))
                eattype++;
            if (find(tile + 1) && find(tile + 2))
                eattype++;
        }
        else if (tile % 10 == 8)
        {
            if (find(tile - 1) && find(tile + 1))
                eattype++;
            if (find(tile - 1) && find(tile - 2))
                eattype++;
        }
        else 
        {
            if (find(tile - 1) && find(tile - 2))
                eattype++;
            if (find(tile - 1) && find(tile + 1))
                eattype++;
            if (find(tile + 1) && find(tile +2))
                eattype++;
        }
        return eattype;
    }
}

void PlayerTiles::eatInMeld_1(int tile)
{
    if (find(tile - 1) && find(tile - 2))
    {
        playermeld.tile[playermeld.num][0] = tile;
        playermeld.tile[playermeld.num][1] = tile - 1;
        playermeld.tile[playermeld.num][2] = tile - 2;
        del(tilesInHand, num_tiles, tile - 1);
        numOfTypes[tile / 10]--;
        num_tiles--;
        del(tilesInHand, num_tiles, tile - 2);
        numOfTypes[tile / 10]--;
        num_tiles--;
    }
    else if (find(tile - 1) && find(tile + 1))
    {
        playermeld.tile[playermeld.num][0] = tile;
        playermeld.tile[playermeld.num][1] = tile - 1;
        playermeld.tile[playermeld.num][2] = tile + 1;
        del(tilesInHand, num_tiles, tile - 1);
        numOfTypes[tile / 10]--;
        num_tiles--;
        del(tilesInHand, num_tiles, tile + 1);
        numOfTypes[tile / 10]--;
        num_tiles--;
    }
    else if (find(tile + 1) && find(tile + 2))
    {
        playermeld.tile[playermeld.num][0] = tile;
        playermeld.tile[playermeld.num][1] = tile + 1;
        playermeld.tile[playermeld.num][2] = tile + 2;
        del(tilesInHand, num_tiles, tile + 1);
        numOfTypes[tile / 10]--;
        num_tiles--;
        del(tilesInHand, num_tiles, tile + 2);
        numOfTypes[tile / 10]--;
        num_tiles--;
    }

    playermeld.type[playermeld.num] = 0;
    playermeld.num++;
}

void PlayerTiles::eatInMeld_2(int tile, int tile1,int tile2)
{
    playermeld.tile[playermeld.num][0] = tile;
    playermeld.tile[playermeld.num][1] = tile1;
    playermeld.tile[playermeld.num][2] = tile2;
    del(tilesInHand, num_tiles, tile1);
    numOfTypes[tile / 10]--;
    num_tiles--;
    del(tilesInHand, num_tiles, tile2);
    numOfTypes[tile / 10]--;
    num_tiles--;
    playermeld.type[playermeld.num] = 0;
    playermeld.num++;
}


bool PlayerTiles::pen(int tile)
{
    int count = 0;
    for (int i = 0; i < num_tiles; i++)
    {
        if (tilesInHand[i] == tile)
        {
            count++;
            if (count > 1) return true;
        }
    }
    return false;
}

void PlayerTiles::penInMeld(int tile)
{
    playermeld.tile[playermeld.num][0] = tile;
    playermeld.tile[playermeld.num][1] = tile;
    playermeld.tile[playermeld.num][2] = tile;
    del(tilesInHand, num_tiles, tile);
    numOfTypes[tile / 10]--;
    num_tiles--;
    del(tilesInHand, num_tiles, tile);
    numOfTypes[tile / 10]--;
    num_tiles--;
    playermeld.type[playermeld.num] = 1;
    playermeld.num++;
}

bool PlayerTiles::exkong(int tile)
{
    int count = 0;
    for (int i = 0; i < num_tiles; i++)
    {
        if (tilesInHand[i] == tile)
        {
            count++;
            if (count > 2) return true;
        }
    }
    return false;
}

int PlayerTiles::exkong_selftile()
{
    int flag = 0;
    for (int i = 0; i < playermeld.num; i++)
    {
        if (playermeld.type[i] == 1)
        {
            if (find(playermeld.tile[i][0]))
                return playermeld.tile[i][0];
        }
    }
    cout << "error";
    return 0;
}

void PlayerTiles::exkongInMeld(int tile)
{
    playermeld.tile[playermeld.num][0] = tile;
    playermeld.tile[playermeld.num][1] = tile;
    playermeld.tile[playermeld.num][2] = tile;
    playermeld.tile[playermeld.num][3] = tile;
    del(tilesInHand, num_tiles, tile);
    num_tiles--;
    del(tilesInHand, num_tiles, tile);
    num_tiles--;
    del(tilesInHand, num_tiles, tile);
    num_tiles--;
    numOfTypes[tile / 10]-=3;
    playermeld.type[playermeld.num] = 2;
    playermeld.num++;
    num_compensate++;
}

int PlayerTiles::exkong_self()
{
    int flag = 0;
    for (int i = 0; i < playermeld.num; i++)
    {
        if (playermeld.type[i] == 1)
        {
            if (find(playermeld.tile[i][0]))
                flag++;
        }
    }
    return flag;
}

bool PlayerTiles::exkong_self(int tile)
{
    for (int i = 0; i < playermeld.num; i++)
    {
        if (playermeld.type[i] == 1 && playermeld.tile[i][0]==tile)
        {
                return true;
        }
    }
    return false;
}

void PlayerTiles::exkong_selfInMeld(int tile)
{
    for (int i = 0; i < playermeld.num; i++)
    {
        if (playermeld.type[i] == 1 && playermeld.tile[i][0] == tile)
        {
            playermeld.tile[i][3] = tile;
            playermeld.type[i] = 2;
        }
    }
    discard(tile);
    num_compensate++;
}


bool PlayerTiles::clkong(int tile)
{
    int count = 0;
    for (int i = 0; i < num_tiles; i++)
    {
        if (tilesInHand[i] == tile)
        {
            count++;
            if (count > 3) return true;
        }
    }
    return false;
}

int PlayerTiles::clkong()
{
    int count = 0;
    for (int i = 0; i < num_tiles; i++)
    {
        if ((tilesInHand[i] ==tilesInHand[i+1])&& (tilesInHand[i] == tilesInHand[i + 2]) && (tilesInHand[i] == tilesInHand[i + 3]))
        {
            count++;
        }
    }
    return count;
}

int PlayerTiles::clkong_tile()
{
    int count = 0;
    for (int i = 0; i < num_tiles; i++)
    {
        if ((tilesInHand[i] == tilesInHand[i + 1]) && (tilesInHand[i] == tilesInHand[i + 2]) && (tilesInHand[i] == tilesInHand[i + 3]))
        {
            return tilesInHand[i];
        }
    }
    cout << "error";
    return 0;
}

void PlayerTiles::clkongInMeld(int tile)
{
    playermeld.tile[playermeld.num][0] = tile;
    playermeld.tile[playermeld.num][1] = tile;
    playermeld.tile[playermeld.num][2] = tile;
    playermeld.tile[playermeld.num][3] = tile;
    del(tilesInHand, num_tiles, tile);
    num_tiles--;
    del(tilesInHand, num_tiles, tile);
    num_tiles--;
    del(tilesInHand, num_tiles, tile);
    num_tiles--;
    del(tilesInHand, num_tiles, tile);
    num_tiles--;
    numOfTypes[tile / 10]-=4;
    playermeld.type[playermeld.num] = 3;
    playermeld.num++;
    num_compensate++;
}
