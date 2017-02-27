#include "chess.h"
#include "openGL.h"

Gomoku::Gomoku()
{
	srand(time(NULL));
	for (int i = 0; i<size; ++i)
		for (int j = 0; j<size; ++j)
		{
			board[i][j] = 0;
			candidate[i][j] = false;
			posValue[i][j] = min(min(i - 0, size - 1 - i), min(j - 0, size - 1 - j));
			//初始化zobrist键值表
			zobrist[0][i][j] = Rand64();
			zobrist[1][i][j] = Rand64();
		}
	gameOver = 0;
	//开始局面为0
	zobristKey = 0;
	//黑子先行
	currentState = 0;
	piecesCnt = 0;
	hit = 0;
	searchDepth = 1;
	searchLimit = 20;

}
//随机生成64位随机数，由于rand函数最大是15位，故需要调用几次rand函数并移位异或得到
U64 Gomoku::Rand64() {
	return rand() ^ ((U64)rand() << 15) ^ ((U64)rand() << 30) ^ ((U64)rand() << 45) ^ ((U64)rand() << 60);
}
// 插入排序
void Gomoku::sort(point * a, int n)
{
	int i, j;
	point key;
	for (i = 1; i < n; i++) {
		key = a[i];
		for (j = i; j >0 && a[j - 1].val < key.val; j--) {
			a[j] = a[j - 1];
		}
		a[j] = key;
	}
}
//剪枝函数，优先考虑冲四以上的点
int Gomoku::cutCandidateMoves(point * bestMoves, point * candidateMoves, int csize)
{
	int Msize = 0;
	if (csize > 0 && candidateMoves[0].val >= 100000)
	{
		bestMoves[0] = candidateMoves[0];
		Msize = 1;
	}
	else if (csize > 0 && candidateMoves[0].val >= 50000)
	{
		bestMoves[0] = candidateMoves[0];
		Msize = 1;
		if (csize > 1 && candidateMoves[1].val >= 50000) {
			bestMoves[1] = candidateMoves[1];
			Msize = 2;
		}
		for (int i = Msize; i < csize; ++i) {
			if (candidateMoves[i].val >= 4000) {
				bestMoves[Msize] = candidateMoves[i];
				++Msize;
			}
		}
	}
	return Msize;
}
///生成合法下法,返回合法的下棋点集合
int Gomoku::generateBestMoves(int limit, point * bestMoves)
{
	int val, csize = 0;
	for (int i = 0; i<size; ++i)
		for (int j = 0; j < size; ++j)
		{
			if (candidate[i][j] && !board[i][j])
			{
				int score[2];
				if (currentState)
				{
					board[i][j] = 1;
					score[0] = getValue(i, j, WHITE);
					board[i][j] = -1;
					score[1] = getValue(i, j, BLACK);
					board[i][j] = 0;
				}
				else
				{
					board[i][j] = -1;
					score[0] = getValue(i, j, BLACK);
					board[i][j] = 1;
					score[1] = getValue(i, j, WHITE);
					board[i][j] = 0;
				}

				if (score[0] >= 900 || score[1] >= 900)
				{
					val = (score[0] >= score[1]) ? score[0] * 3 : score[1] * 2;
				}
				else
				{
					val = score[0] * 2 + score[1];
				}
				if (val > size)
				{
					candidateMoves[csize].x = i;
					candidateMoves[csize].y = j;
					candidateMoves[csize].val = val;
					++csize;
				}
			}
		}
	//排序
	sort(candidateMoves, csize);
	csize = csize > limit ? limit : csize;

	int msize = cutCandidateMoves(bestMoves, candidateMoves, csize);
	if (msize == 0)
	{
		msize = csize;
		for (int k = 0; k < msize; ++k)
			bestMoves[k] = candidateMoves[k];
	}
	return msize;
}
///静态棋子评分函数,针对每个棋子落点进行评分
int Gomoku::getValue(int x, int y, int State)
{
	int value = posValue[x][y];
	if (steps.size() == 1)
		return value;
	int offsetX[4] = { 0,1,1,1 }, offsetY[4] = { 1 , 0 ,1,-1 };

	/**< 棋型判断数组 */
	int patternCnt[10] = { 0,0,0,0,0,0,0,0,0,0 };
	int x1, y1;
	int k;
	///从四个方向考虑
	for (int i = 0; i<4; ++i)
	{
		string t;
		x1 = x;
		y1 = y;
		int k = 4;
		while (k--)
		{
			x1 -= offsetX[i];
			y1 -= offsetY[i];

			if (x1 > -1 && x1 < size && y1 > -1 && y1 < size)
			{
				if (!board[x1][y1])
					t.insert(0, 1, 'o');
				else if (board[x1][y1] == 1 && State == 1)
					t.insert(0, 1, 'a');
				else if (board[x1][y1] == 1 && State == 0)
					t.insert(0, 1, 'b');
				else if (board[x1][y1] == -1 && State == 0)
					t.insert(0, 1, 'a');
				else if (board[x1][y1] == -1 && State == 1)
					t.insert(0, 1, 'b');
			}
			else
				break;
		}
		t.push_back('a');
		k = 4;
		x1 = x;
		y1 = y;
		while (k--)
		{
			x1 += offsetX[i];
			y1 += offsetY[i];
			if (x1 > -1 && x1 < size && y1 > -1 && y1 < size)
			{
				if (!board[x1][y1])
					t.push_back('o');
				else if (board[x1][y1] == 1 && State == 1)
					t.push_back('a');
				else if (board[x1][y1] == 1 && State == 0)
					t.push_back('b');
				else if (board[x1][y1] == -1 && State == 0)
					t.push_back('a');
				else if (board[x1][y1] == -1 && State == 1)
					t.push_back('b');
			}
			else
				break;
		}

		for (k = 0; k<39; ++k)
		{
			if (t.find(pattern[k]) != string::npos)
			{
				break;
			}

		}
		if (k<37)
		{
			if (k == 0)
				++patternCnt[0];
			else if (k == 1)
				++patternCnt[1];
			else if (k>1 && k<4)
				++patternCnt[2];
			else if (k>3 && k<9)
				++patternCnt[3];
			else if (k>8 && k<11)
				++patternCnt[4];
			else if (k>10 && k<13)
				++patternCnt[5];
			else if (k>12 && k<23)
				++patternCnt[6];
			else if (k>22 && k<26)
				++patternCnt[7];
			else if (k>25 && k<29)
				++patternCnt[8];
			else if (k>28 && k<39)
				++patternCnt[9];
		}
	}
	///综合四个方向的评分
	if (patternCnt[0] >= 1)
		value += scores[0];
	else if (patternCnt[1] > 0 || patternCnt[2] + patternCnt[3] > 1 || ((patternCnt[2] + patternCnt[3]) == 1 && (patternCnt[4] + patternCnt[5]) == 1))
		value += scores[1];
	else if (patternCnt[4] + patternCnt[5] > 1)
		value += scores[2];
	else if (patternCnt[4] >= 1 && patternCnt[6] >= 1)
		value += scores[3];
	else if (patternCnt[2] >= 1)
		value += scores[4];
	else if (patternCnt[3] >= 1)
		value += scores[5];
	else if (patternCnt[4] >= 1)
		value += scores[6];
	else if (patternCnt[5] >= 1)
		value += scores[7];
	else if (patternCnt[7] + patternCnt[8] > 1)
		value += scores[8];
	else if (patternCnt[7] > 0)
		value += scores[9];
	else if (patternCnt[8] > 0)
		value += scores[10];
	else if (patternCnt[6] > 0)
		value += scores[11];
	else if (patternCnt[9] > 0)
		value += scores[12];
	return value;
}
/** \brief :棋局评分函数,默认落点合法，不对落点进行检查
*对当前要走的一方进行估值，占优返回正值，否则返回负值
* \param ：x落点横坐标
* \param ：y落点纵坐标
* \return 结合敌我双方综合评价分数
*
*/
point  bestMoves[40];
int Gomoku::evaluate()
{
	int myScore, hisScore, legalMovesCnt;

	if (currentState)
		hisScore = getValue(steps[steps.size() - 1].x, steps[steps.size() - 1].y, BLACK);
	else
	{
		hisScore = getValue(steps[steps.size() - 1].x, steps[steps.size() - 1].y, WHITE);
	}
	///对当前局面进行估值
	int moveCnt = generateBestMoves(searchLimit, bestMoves);
	int val, bestVal = -999999999;
	for (int i = 0; i<moveCnt; ++i)
	{
		if (currentState)
		{
			board[bestMoves[i].x][bestMoves[i].y] = 1;
			val = getValue(bestMoves[i].x, bestMoves[i].y, WHITE);
		}

		else
		{
			board[bestMoves[i].x][bestMoves[i].y] = -1;
			val = getValue(bestMoves[i].x, bestMoves[i].y, BLACK);
		}
		board[bestMoves[i].x][bestMoves[i].y] = 0;
		if (val > bestVal)
			bestVal = val;

	}
	myScore = bestVal;
	return myScore - hisScore;
}
//极大极小搜索，使用负极大函数实现
int Gomoku::negaMax(int depth, int &x, int &y, int alpha, int beta)
{

	int best = -99999999, val;
	//如果对方已连五，就直接返回
	isGameOver(steps[piecesCnt - 1].x, steps[piecesCnt - 1].y);
	if (gameOver)
		return -1000000;

	//如果之前计算出这个局面的值就直接返回
	val = searchHashTable(depth);
	if (val != unknown)
	{
		++hit;
		return val;
	}

	if (depth <= 0)
		return evaluate();

	point * move;
	move = new point[40];
	int moveCnt;
	//动态生成可行点,根据局面判断要考虑的节点数
	if (piecesCnt<6)
		moveCnt = generateBestMoves(8, move);
	else if (piecesCnt<12)
		moveCnt = generateBestMoves(10, move);
	else
		moveCnt = generateBestMoves(searchLimit, move);
	int x1, y1;

	for (int i = 0; i<moveCnt; ++i)
	{
		placePiece(move[i].x, move[i].y);
		x1 = move[i].x;
		y1 = move[i].y;
		val = -negaMax(depth - 1, x1, y1, -beta, -alpha);
		removePiece(move[i].x, move[i].y);
		if (val >= beta) {
			writeToHashTable(beta, depth);
			return beta;
		}
		if (val > best) {
			best = val;
			if (depth == searchDepth)
			{
				x = x1;
				y = y1;
			}
			if (val > alpha)
				alpha = val;
		}
	}
	delete[] move;
	writeToHashTable(best, depth);
	return best;
}
void Gomoku::getBestPos(int &x, int &y)
{
	clock_t startTime, endTime;
	startTime = clock();
	///第一步下中心；如果第一步对方下边界，第二步也下中心
	if (steps.size() == 0 || steps.size() == 1 && getValue(steps[0].x, steps[0].y, BLACK)<3)
	{
		x = y = (size - 1) >> 1;
		return;
	}
	else if (steps.size() == 1 && steps[0].x == 7 && steps[0].y == 7)
	{
		int a[] = { 6,6,6,7,7,8,8,8 };
		int b[] = { 6,7,8,6,8,6,7,8 };
		srand(time(NULL));
		int ran = rand() % 8;
		x = a[ran];
		y = b[ran];
	}
	else
	{
		negaMax(searchDepth, x, y, -9999999999, 9999999999);
	}
	endTime = clock();
	cout << "time :" << double(endTime - startTime) << "ms." << endl;
}
//判断游戏是否结束
void Gomoku::isGameOver(int x, int y)
{
	int offset[4] = { 1,size - 1,size,size + 1 };
	int checkValue = currentState ? -1 : 1;
	int cnt;
	for (int i = 0; i<4; ++i)
	{
		int offsetX[4] = { 0,1,1,1 }, offsetY[4] = { 1 , 0 ,1,-1 };
		cnt = 1;
		//检查左边的连子数
		int k = 4;
		int x1 = x, y1 = y;
		while (k--)
		{
			x1 -= offsetX[i];
			y1 -= offsetY[i];
			if (x1 > -1 && x1 < size && y1 > -1 && y1 < size && board[x1][y1] == checkValue)
			{
				++cnt;
			}
			else
				break;
		}
		k = 4;
		x1 = x;
		y1 = y;
		//检查右边的连子数
		while (k--)
		{
			x1 += offsetX[i];
			y1 += offsetY[i];
			if (x1 > -1 && x1 < size && y1 > -1 && y1 < size && board[x1][y1] == checkValue)
			{
				++cnt;
			}
			else
				break;
		}
		if (cnt >= 5)
		{
			gameOver = currentState ? 2 : 1;
			break;
		}
		else if (cnt <5 && piecesCnt == size*size)
		{
			gameOver = 3;
			break;
		}

	}

}
//摆放棋子位置
bool Gomoku::placePiece(int x, int y)
{

	if (x >= size || y >= size || x<0 || y <0)
	{
		cout << "error,pos out of bound!" << endl;
		return false;
	}
	else if (board[x][y])
	{
		cout << x << ' ' << y << ' ' << board[x][y] << endl;
		cout << "error,invalid pos!" << endl;
		return false;
	}
	else
	{
		///更新棋盘
		board[x][y] = (currentState == 0 ? -1 : 1);
		++piecesCnt;
		point oneStep(x, y);
		steps.push_back(oneStep);
		///更新局面键值
		zobristKey ^= zobrist[currentState][x][y];
		///更新候选点
		for (int i = x - 2; i <= x + 2; i++) {
			if (i >= 0 && i<size)
				for (int j = y - 2; j <= y + 2; j++)
					if (j >= 0 && j<size)
						candidate[i][j]++;
		}
		///更新下棋方
		currentState = !currentState;
		updateSquarePoints( y * 50 + 40 - 189.5, 750.5 - x * 50 - 40);
		generateEllipsePoints(currentState, 68 + (piecesCnt - 1)*100, 100, vec2(y * 50 + 40 - 189.5, 750.5 - x * 50 - 40), 24, 1);
		return true;
	}
}

/** \brief :撤销下棋点函数
*
* \param ：x落点横坐标
* \param ：y落点纵坐标
* \return
*
*/
void Gomoku::removePiece(int x, int y)
{
	if (!board[x][y])
		return;
	else
	{
		//cout << "removePiece:" << x << ' ' << y << endl;
		board[x][y] = 0;
		--piecesCnt;
		steps.pop_back();

		currentState = !currentState;
		zobristKey ^= zobrist[currentState][x][y];

		//撤销候选点
		for (int i = x - 2; i <= x + 2; i++) {
			if (i >= 0 && i<size)
				for (int j = y - 2; j <= y + 2; j++)
					if (j >= 0 && j<size)
						candidate[i][j]--;
		}
		//重新定位最后落子点,正方形方框的位置
		if (steps.size() > 0)
			updateSquarePoints(steps[steps.size() - 1].y * 50 + 40 - 189.5, 750.5 - steps[steps.size() - 1].x * 50 - 40);
		gameOver = 0;
	}
}
/** \brief 对当前棋局进行存盘*/
void Gomoku::saveSteps()
{
	fstream fin;
	fin.open("c:\\steps.txt", fstream::ate | fstream::in | fstream::out);
	if (fin.bad())
		MessageBox(NULL, ("无法打开文件"), ("操作提示"), MB_OK);
	else
	{
		int x, y;
		///保存每一步
		for (auto step : steps)
		{

			fin << setw(2) << step.x << ' ' << setw(2) << step.y << endl;

		}
		///当前所走的步数
		fin << setw(3) << (int)steps.size();
		fin << endl;
		fin.close();
		MessageBox(NULL, ("保存成功"), ("操作提示"), MB_OK);
	}
}

/** \brief 恢复最后一盘棋局*/
void Gomoku::loadBoard()
{
	fstream fin;
	fin.open("c:\\steps.txt", fstream::ate | fstream::in | fstream::out);
	if (fin.bad())
		MessageBox(NULL, ("无法打开文件"), ("操作提示"), MB_OK);
	else
	{
		int stepsCnt, x, y;
		fin.seekg(-5, fstream::end);
		fin >> stepsCnt;
		cout << stepsCnt << endl;
		restart();
		for (int i = stepsCnt; i>0; --i)
		{
			fin.seekg(-5 - 7 * i, fstream::end);
			fin >> x >> y;
			placePiece(x, y);
		}
		fin.close();
		MessageBox(NULL, ("成功恢复棋盘"), ("操作提示"), MB_OK);
		fin.close();
	}
}

//////////////////////////////////////////////////////////////////////////
// 重新启动游戏
void Gomoku::restart()
{
	for (int i = 0; i<size; ++i)
		for (int j = 0; j<size; ++j)
		{
			board[i][j] = 0;
			posValue[i][j] = min(min(i - 0, size - 1 - i), min(j - 0, size - 1 - j));
		}
	gameOver = 0;
	//黑子先行
	currentState = 0;
	piecesCnt = 0;
	steps.clear();
}

//更改难度
void Gomoku::changeDifficulty(int type)
{
	searchDepth = type;

}

///置换表相关实现函数
//写入置换表
void Gomoku::writeToHashTable(int val, int depth)
{
	Hashe * e = &hashTable[zobristKey % hashSize];
	e->key = zobristKey;
	e->val = val;
	e->depth = depth;
}
//查询置换表
int  Gomoku::searchHashTable(int depth)
{
	Hashe * e = &hashTable[zobristKey % hashSize];
	if (e->key == zobristKey && e->depth >= depth)
	{
		return e->val;
	}
	return unknown;
}
//命令行界面
void Gomoku::printChessBoard()
{
	int i, j;
	char c;
	cout << "------------我是友好的分界线-----------" << endl;
	cout << "   ";
	for (i = 0; i != size; ++i)
		cout << setw(2) << i << ' ';
	cout << endl;
	for (i = 0; i != size; ++i)
	{
		cout << setw(2) << i << ' ';
		for (j = 0; j != size; ++j)
		{
			switch (board[i][j])
			{
			case -1:
				c = 'o';
				break;
			case 0:
				c = '-';
				break;
			case 1:
				c = '*';
				break;

			}
			cout << setw(2) << c << ' ';
		}
		cout << endl;
	}
	cout << "------------我是友好的分界线-----------" << endl;
}

