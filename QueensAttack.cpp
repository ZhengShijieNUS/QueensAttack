// QueensAttack.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <time.h>
#include <stdlib.h>

#include <Windows.h>

typedef unsigned char BYTE;
#ifdef WIN32

#else
typedef enum { FALSE, TRUE } BOOL;
#endif


#define BLOCKSIZE 8
#define BOARD BYTE
#define EMPTY_CELL "[ ]"
#define PIECE_CELL "[*]"


typedef struct {
	unsigned int pieces;
	BOARD Board[];
};

typedef enum {
	NONE = 0b0000,
	EDGE_RIGHT = 0b0001,
	EDGE_BOTTOM = 0b0010,
	EDGE_LEFT = 0b0100,
	EDGE_TOP = 0b1000,
	CORNER_RIGHTBOTTOM = 0b0011,
	CORNER_LEFTBOTOM = 0b0110,
	CORNER_RIGHTTOP = 0b1001,
	CORNER_LEFTTOP = 0b1100,

} EDGE;


/// <summary>
/// the size of a side of the board
/// </summary>
unsigned long long side = 0;

/// <summary>
/// total size of the board
/// </summary>
unsigned long long size = 0;

/// <summary>
/// the numer of pieces that every queen must attack
/// </summary>
unsigned int attack = 0;

/// <summary>
/// wraparound
/// </summary>
BOOL wraparound = FALSE;

/// <summary>
/// the number of blocks to store a board, each block equals a byte
/// </summary>
unsigned long long blocks = 0;

/// <summary>
/// the number of pieces on the board
/// </summary>
unsigned long long pieces = 0;

/// <summary>
/// Timer
/// </summary>
BOOL TimerEnabled = FALSE;

void PrintBoard(BOARD board[], unsigned int blocks, unsigned int size, unsigned int side);
BOOL CheckBoard(BOARD board[], unsigned int size, unsigned int side, BOOL wraparound);
BOARD* AddPieceToBoard(BOARD board[], unsigned int blocks);
BOARD* RemovePieceFromBoard(BOARD board[], unsigned int blocks);
unsigned int GetPiecesCount(BOARD board[], unsigned int blocks, unsigned int size);
BOARD* InitializeBoard(unsigned int blocks, unsigned int size);
void ReleaseBoard(BOARD board[]);


int main(int argc, char* argv[])
{
	if (argc == 5)
	{
		memset(&side, 0xFF, sizeof(side));
		side = (unsigned long long)atoll(argv[1]);
		if (side > 4294967295)
		{
			printf("N must be less than 4294967295\r\n");
			exit(FALSE);
		}

		size = side * side;
		attack = (unsigned int)atoi(argv[2]);

		/*
		* calcuate how many bytes are needed that store the whole board, 如果不能整除8，则再加一个字节
		*/
		blocks = size / BLOCKSIZE + (size % BLOCKSIZE != 0 ? 1 : 0);
	}
	else
	{
		printf("Usage: %s N k l w\r\n", argv[0]);
		printf("\t*N is the size of a side of the board\r\n");
		printf("\t*k is the numer of pieces that every queen must attack\r\n");
		printf("\t*l = 0 is short format N, k:number: and l = 1 is long format N, k : number : pc1, pc2, pc3...\r\n");
		printf("\t*w = 0 is no wraparound, w = 1 is wraparound\r\n");

	}

	printf("Run with %lld(%lld*%lld) board in %lld blocks(bytes),every queen must attack %d.\r\n", size, side, side, blocks, attack);


	BOARD* Board = InitializeBoard(blocks, size);

	/*
	* 循环直到棋子布满棋盘
	*/
	while (pieces < size)
	{
		AddPieceToBoard(Board, blocks);
		pieces = GetPiecesCount(Board, blocks, size);
		//PrintBoard(Board, blocks, size, side);
		//printf("pieces:%lld\r\n",  pieces);
		BOOL result = CheckBoard(Board, size, side, FALSE);
		printf("result: %d.\r\n", result);
	}
	printf("pieces on board: %lld\r\n", pieces);
	ReleaseBoard(Board);
	exit(TRUE);
}

BOARD* InitializeBoard(unsigned int blocks, unsigned int size)
{
	BOARD* board = (BOARD*)calloc(blocks, sizeof(BOARD));


	return board;
}

void ReleaseBoard(BOARD board[])
{
	free(board);
}

BOOL CheckBoard(BOARD board[], unsigned int size, unsigned int side, BOOL wraparound)
{
	//current position
	unsigned int p = size;
	do
	{
		p--;

		/*
		*	08 07 06
		*	05 04 03
		*	02 01 00
		*
		*	15 14 13 12
		*	11 10 09 08
		*	07 06 05 04
		*	03 02 01 00
		*
		*	24 23 22 21 20
		*	19 18 17 16 15
		*	14 13 12 11 10
		*	09 08 07 06 05
		*	04 03 02 01 00
		*/

		/*
		*	当前位置的余数m=p%side
		*	当前位置的商n=p/side
		*
		*	m==side-1				在左边界
		*	m==0					在右边界
		*	n==(size-1)/side		在上边界
		*	n==0					在下边界
		*/

		int m = p % side;
		int n = p / side;

		EDGE edge = NONE;

		if (m == side - 1) edge = (EDGE)(EDGE_LEFT | edge);
		if (m == 0) edge = (EDGE)(EDGE_RIGHT | edge);
		if (n == (size - 1) / side) edge = (EDGE)(EDGE_TOP | edge);
		if (n == 0) edge = (EDGE)(EDGE_BOTTOM | edge);

		int step = 1;
		for (step = 1; step < side; step++)
		{

			/*
			* p[0]:	left		p+step				wraparound:	p+step-side*wraparound
			* p[1]:	right		p-step				wraparound:	p-step+side*wraparound
			* p[2]:	top			p+step*side			wraparound:	p+step*side-(size-1)*wraparound
			* p[3]:	bottom		p-step*side			wraparound:	p-step*side+(size-1)*wraparound
			*
			* p[4]:	lefttop		p+step+step*side	
			* p[5]:	righttop	p-step+step*side
			* p[6]:	leftbottom	p+step-step*side
			* p[7]:	rightbottom	p-step-step*side
			*/
			unsigned int p[8];
			BOOL cell[8];

			//left
			if ((EDGE)(edge & EDGE_LEFT) == EDGE_LEFT)
			{
				cell[0] = FALSE;
			}
			
			//right
			if ((EDGE)(edge & EDGE_RIGHT) == EDGE_RIGHT)
			{
				cell[1] = FALSE;
			}



		}
	} while (p > 0);

	return FALSE;
}

BOOL GetPieceOnBoard(BOARD board[], unsigned int blocks, unsigned int size, unsigned int cell)
{
	/*
	* locate a piece on board
	*/
	int bit = (size - cell) % 8;
	int block = (size - cell) / 8;

	return (board[block] & 1 << bit) ? TRUE : FALSE;
}

/// <summary>
/// Add a piece on board sequentially 
/// </summary>
/// <param name="board">board address</param>
/// <param name="size">board size</param>
/// <returns>board address</returns>
BOARD* AddPieceToBoard(BOARD board[], unsigned int blocks)
{
	BOOL carry = FALSE;
	unsigned int block = 0;

	if (++board[0] == 0)
	{
		carry = TRUE;
	}
	else
	{
		carry = FALSE;
	}


	for (block = 1; block < blocks; block++)
	{
		if (carry == TRUE)
		{
			if (++board[block] == 0)
			{
				carry = TRUE;
			}
			else
			{
				carry = FALSE;
			}
		}
		else
		{
			break;
		}

	}
	return board;
}

BOARD* RemovePieceFromBoard(BOARD board[], unsigned int blocks)
{
	BOOL borrowed = FALSE;


	return board;
}

unsigned int GetPiecesCount(BOARD board[], unsigned int blocks, unsigned int size)
{
	unsigned int pieces = 0;
	do
	{
		blocks--;
		unsigned int bit = 0;
		do
		{
			bit = --size % 8;
			if ((board[blocks] & 1 << bit) > 0)
			{
				pieces++;
			}
			else
			{

			}

		} while (bit > 0);
	} while (blocks > 0);
	return pieces;
}

void PrintBoard(BOARD board[], unsigned int blocks, unsigned int size, unsigned int side)
{
	/*
	* 假设有如下5*5棋盘
	* 10110
	* 01110
	* 00001
	* 11011
	* 10010
	* 总共25个位置，需要25个bit存储，但每8bit为一字节，所以需要4个字节(32个bit)
	* 高位在左，低位在右，右对齐
	* 76543210 size除以8的余数，左移相应位数，表示当前需要获取的bit
	*  byte[3]  byte[2]	 byte[1]  byte[0]
	* 01234567 01234567 01234567 01234567	//每个字节内表示棋子状态
	* -------1 01100111 00000111 01110010	//棋子
	*
	* (1 << 0) = 00000001
	* (1 << 1) = 00000010
	* (1 << 2) = 00000100
	* (1 << 3) = 00001000
	* (1 << 4) = 00010000
	* (1 << 5) = 00100000
	* (1 << 6) = 01000000
	* (1 << 7) = 10000000
	*/

	do
	{

		//从0开始，所以需要最大数减一
		blocks--;
		unsigned int bit = 0;
		do
		{
			//size也是从0开始，计算时需要最大数减一
			bit = --size % 8;
			//通过按位与运算得知该位是否为1
			printf((board[blocks] & 1 << bit) > 0 ? PIECE_CELL : EMPTY_CELL);
			//如果当前bit刚好可以整除边长，说明需要换行
			if (size % side == 0) printf("\r\n");
		} while (bit > 0);	//当bit大于0时，说明仍需要取下一位，
	} while (blocks > 0);	//当blocks大于0时，说明仍然需要取下一字节
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
