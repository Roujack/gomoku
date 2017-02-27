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
//�û����С
const int hashSize = 1 << 20;
const int unknown = -10000000000;

class point
{
public:
	int x, y, val;
	point(int i = 0, int j = 0, int v = 0) 
	{ x = i, y = j, val = v; }
};

// ��ϣ��ṹ
class Hashe {
public:
	U64 key;
	int depth;
	int val;
	Hashe() { depth = -1; }
};

//��������
class Gomoku
{
private:

public:
	static const int size = 15;
	//��ά����������̣� 0|�� 1|�� -1|��
	int board[size][size];
	//��ѡ��
	int candidate[size][size];
	//����λ�÷������� Խ�������ķ���Խ��
	int posValue[size][size];
	//�û���
	int transportationTable[3][size][size];
	//��Ϸ�������� 0|û�н��� 1|��ʤ 2|��ʤ 3|����
	int gameOver;
	//bool isStart;
	//��ǰ�����һ�� 0|�� 1|��
	bool currentState;
	bool music;
	///��������������
	int piecesCnt;
	/// �����Ѷȣ�����С���������������
	int  searchDepth;
	int searchLimit;
	vector<point> steps;
	// ��ʱ�洢�����ŷ�(����������)
	point candidateMoves[256];
	/**< ƥ���ַ����������ж�����
	�������Ͱ��������壬���ģ����ģ�������������������߶�*/
	string pattern[39] = {
		"aaaaa",///0:����
		"oaaaao",///1:����
		"baaaao","oaaaab",///2~3����
		"aoaaab","baaaoa","aaoaa","boaaaoa","aoaaaob",///4~8�ͼ�����
		"oaaaoo","ooaaao",///9~10����
		"oaaoao","oaoaao",///11~12������
		"baaaoo","ooaaab","baaoao","oaoaab","oaaoab","baoaao","aooaa","aaooa","aoaoa","boaaaob",///13~22����
		"ooaaoo","oooaao","oaaooo",///23~25���
		"ooaoao","oaoaoo","oaooao",///26~28�ͼ����
		"aaooo","oooaa","baoaoo","ooaoab","oaooab","baooao","aoooa","boaoaob","boaaoob","booaaob"///29~38�߶�
	};
	/**< ��������
	���ݲ�ͬ�ĳ������и��֣��ο�http://www.cnblogs.com/songdechiu/p/5768999.html
	*/
	int scores[13] = { 1000000,100000,50000,10000,5000,4000,1000,900,500,100,90,50,20 };
	// ��ʾ��ǰ�����zobristKey
	U64 zobristKey;
	// zobrist��ֵ��
	U64 zobrist[2][size][size];
	Hashe hashTable[hashSize];
	//�û���������
	int hit;
	Gomoku::Gomoku();
	//�������64λ�����������rand���������15λ������Ҫ���ü���rand��������λ���õ�
	U64 Rand64();
	// ��������
	void sort(point * a, int n);
	//��֦���������ȿ��ǳ������ϵĵ�
	int cutCandidateMoves(point * bestMoves, point * candidateMoves, int csize);
	///���ɺϷ��·�,���غϷ�������㼯��
	int generateBestMoves(int limit, point * bestMoves);
	///��̬�������ֺ���,���ÿ����������������
	int getValue(int x, int y, int State);
	/** \brief :������ֺ���,Ĭ�����Ϸ������������м��
	*�Ե�ǰҪ�ߵ�һ�����й�ֵ��ռ�ŷ�����ֵ�����򷵻ظ�ֵ
	* \param ��x��������
	* \param ��y���������
	* \return ��ϵ���˫���ۺ����۷���
	*
	*/
	int evaluate();
	int negaMax(int depth, int &x, int &y, int alpha, int beta);
	//��ȡ��������
	void getBestPos(int &x, int &y);
	//�ж���Ϸ�Ƿ����
	void isGameOver(int x, int y);
	//�ڷ�����λ��
	bool placePiece(int x, int y);
	/** \brief :��������㺯��
	*
	* \param ��x��������
	* \param ��y���������
	* \return
	*
	*/
	void removePiece(int x, int y);
	void printChessBoard();
	/** \brief �Ե�ǰ��ֽ��д���*/
	void saveSteps();
	/** \brief �ָ����һ�����*/
	void loadBoard();
	//////////////////////////////////////////////////////////////////////////
	// ����������Ϸ
	void restart();
	//�����Ѷ�
	void changeDifficulty(int type);
	///�û������ʵ�ֺ���
	//д���û���
	void writeToHashTable(int val, int depth);
	//��ѯ�û���
	int searchHashTable(int depth);
};