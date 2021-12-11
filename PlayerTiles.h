#pragma once

struct meld
{
	int type[4];//³Ô0¡¢Åö1¡¢Ã÷¸Ü2¡¢°µ¸Ü3
	int tile[4][4] = { {0} };
	int num=0;//¸±Â¶ÊýÁ¿
};

class PlayerTiles
{
private:
	const char* Mahjong[50] = { "0","East","South","West","North","Red","Green","0","0","0","0","1m","2m","3m","4m","5m","6m","7m","8m","9m","0","1s","2s","3s","4s","5s","6s","7s","8s","9s" ,"0","1p","2p","3p","4p","5p","6p","7p","8p","9p","0","White","Spring","Summer","Autumn","Winter","Plum","Orchid","Bamboo","Chrysanthemum"};
	int ready_num6[3][6] = { {1, 1, 2, 2, 3, 3},{1,2,2,2,2,3},{1,2,2,3,3,4} };
	int ready_num9[2][9] = { {1, 1, 2, 2,2, 3, 3,3,4},{1,2,2,2,3,3,3,4,4} };
	int ready_num12[3][12] = { {1, 1, 2, 2,2,2, 3, 3,3,3,4,4},{1, 1, 2, 2,2, 3, 3,3,3,4,4,5},{1,2,2,2,3,3,3,3,4,4,4,5} };



	int readyTiles[10];
	int num_ready;

	bool ready2(const int* tiles, int num);
	bool ready3(const int* tiles, int num);
	bool isEqual(int* A, int* B, int num);
	bool find(int tile);

public:
	int tilesInHand[14];
	int numOfTypes[4];
	int num_tiles;

	int flower[12] = {0};
	int num_flower=0;

	meld	 playermeld;


	int num_compensate;
	int state;//0:ÃþÅÆ 1:´òÅÆ 2:²¹ÅÆ 3:°µ¸Ü 4:Ã÷¸Ü-×Ô 5:Ã÷¸Ü 6:Åö  7:³Ô  9:ºú 10:µÈ´ý
	PlayerTiles(int* begintiles);
	bool discard(int tile);
	void getcompensate(int tile);
	void getdraw(int draw);
	void outputTiles();
	void outoneTile(int draw);
	bool del(int* array, int len, int val);
	void sort();
	void sort(int *temptile,int num);
	void generateReady();

	bool handwin();
	bool handwin(int *tilesInHand,int* numOfTypes);
	bool chongwin(int tile);

	int eat(int tile);
	void eatInMeld_1(int tile);
	void eatInMeld_2(int tile, int tile1, int tile2);

	bool pen(int tile);
	void penInMeld(int tile);

	bool exkong(int tile);
	void exkongInMeld(int tile);

	int exkong_self();
	int exkong_selftile();
	bool exkong_self(int tile);
	void exkong_selfInMeld(int tile);

	int clkong();
	int clkong_tile();
	bool clkong(int tile);
	void clkongInMeld(int tile);
};

