#pragma once
#include <string>
#include <vector>
#include <iomanip>
#include<fstream> 
#include<ctime>
#include<cstdio>
#include<algorithm>

using namespace std;

typedef unsigned long long U64;
const int BLACK = 0;
const int WHITE = 1;
//置换表大小
const int hashSize = 1 << 20;
const int unknown = -10000000000;

class point
{
public:
	int x, y, val;
	point(int i = 0, int j = 0, int v = 0) 
	{ x = i, y = j, val = v; }
};

// 哈希表结构
class Hashe {
public:
	U64 key;
	int depth;
	int val;
	Hashe() { depth = -1; }
};

//五子棋类
class Gomoku
{
private:

public:
	static const int size = 15;
	//二维数组代表棋盘， 0|空 1|白 -1|黑
	int board[size][size];
	//候选点
	int candidate[size][size];
	//棋盘位置分数矩阵 越靠近中心分数越高
	int posValue[size][size];
	//置换表
	int transportationTable[3][size][size];
	//游戏结束变量 0|没有结束 1|白胜 2|黑胜 3|和棋
	int gameOver;
	//bool isStart;
	//当前下棋的一方 0|黑 1|白
	bool currentState;
	bool music;
	///棋盘上棋子总数
	int piecesCnt;
	/// 棋盘难度，极大极小博弈树搜索的深度
	int  searchDepth;
	int searchLimit;
	vector<point> steps;
	// 临时存储合理着法(两格内有子)
	point candidateMoves[256];
	/**< 匹配字符串，用来判断棋型
	基本棋型包括：连五，活四，冲四，活三，眠三，活二，眠二*/
	string pattern[39] = {
		"aaaaa",///0:连五
		"oaaaao",///1:活四
		"baaaao","oaaaab",///2~3眠四
		"aoaaab","baaaoa","aaoaa","boaaaoa","aoaaaob",///4~8低级死四
		"oaaaoo","ooaaao",///9~10活三
		"oaaoao","oaoaao",///11~12跳活三
		"baaaoo","ooaaab","baaoao","oaoaab","oaaoab","baoaao","aooaa","aaooa","aoaoa","boaaaob",///13~22眠三
		"ooaaoo","oooaao","oaaooo",///23~25活二
		"ooaoao","oaoaoo","oaooao",///26~28低级活二
		"aaooo","oooaa","baoaoo","ooaoab","oaooab","baooao","aoooa","boaoaob","boaaoob","booaaob"///29~38眠二
	};
	/**< 分数数组
	根据不同的场景进行给分，参考http://www.cnblogs.com/songdechiu/p/5768999.html
	*/
	int scores[13] = { 1000000,100000,50000,10000,5000,4000,1000,900,500,100,90,50,20 };
	// 表示当前局面的zobristKey
	U64 zobristKey;
	// zobrist键值表
	U64 zobrist[2][size][size];
	Hashe hashTable[hashSize];
	//置换表命中率
	int hit;
	Gomoku::Gomoku();
	//随机生成64位随机数，由于rand函数最大是15位，故需要调用几次rand函数并移位异或得到
	U64 Rand64();
	// 插入排序
	void sort(point * a, int n);
	//剪枝函数，优先考虑冲四以上的点
	int cutCandidateMoves(point * bestMoves, point * candidateMoves, int csize);
	///生成合法下法,返回合法的下棋点集合
	int generateBestMoves(int limit, point * bestMoves);
	///静态棋子评分函数,针对每个棋子落点进行评分
	int getValue(int x, int y, int State);
	/** \brief :棋局评分函数,默认落点合法，不对落点进行检查
	*对当前要走的一方进行估值，占优返回正值，否则返回负值
	* \param ：x落点横坐标
	* \param ：y落点纵坐标
	* \return 结合敌我双方综合评价分数
	*
	*/
	int evaluate();
	int negaMax(int depth, int &x, int &y, int alpha, int beta);
	//获取最佳下棋点
	void getBestPos(int &x, int &y);
	//判断游戏是否结束
	void isGameOver(int x, int y);
	//摆放棋子位置
	bool placePiece(int x, int y);
	/** \brief :撤销下棋点函数
	*
	* \param ：x落点横坐标
	* \param ：y落点纵坐标
	* \return
	*
	*/
	void removePiece(int x, int y);
	void printChessBoard();
	/** \brief 对当前棋局进行存盘*/
	void saveSteps();
	/** \brief 恢复最后一盘棋局*/
	void loadBoard();
	//////////////////////////////////////////////////////////////////////////
	// 重新启动游戏
	void restart();
	//更改难度
	void changeDifficulty(int type);
	///置换表相关实现函数
	//写入置换表
	void writeToHashTable(int val, int depth);
	//查询置换表
	int searchHashTable(int depth);
};