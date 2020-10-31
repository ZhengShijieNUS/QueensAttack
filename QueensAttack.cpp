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

/// <summary>
/// Timer thread
/// </summary>
HANDLE TimerThread;
DWORD WINAPI Timer(LPVOID lpParam);
void StartTimer();
void StopTimer();

void PrintBoard(BOARD board[], unsigned int blocks, unsigned int size, unsigned int side);
BOOL CheckBoard(BOARD board[], unsigned int size);
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

	printf("Run with %d(%d*%d) board in %d blocks(bytes),every queen must attack %d.\r\n", size, side, side, blocks, attack);

	StartTimer();

	BOARD* Board = InitializeBoard(blocks, size);

	/*
	* 循环直到棋子布满棋盘
	*/
	while (pieces < size)
	{
		AddPieceToBoard(Board, blocks);
		pieces = GetPiecesCount(Board, blocks, size);
		//i++;
		//PrintBoard(Board, blocks, size, side);
		//printf("i:%lld, pieces:%d\r\n", i, pieces);
	}
	printf("pieces on board: %d\r\n", pieces);
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
	do
	{
		size--;
		int step = 1;
		for (step = 1; step < side; step++)
		{
			/*求当前位置的余数，用余数判断是否出左右边界
			* m+step>side，出右边界
			* m-step<1，出左边界
			*/
			int m = size % side;
			
			/*
			* p1:	left		p-step
			* p2:	right		p+step
			* p3:	top			p-step*side
			* p4:	bottom		p+step*side
			*
			* p5:	lefttop		p-step-step*side
			* p6:	righttop	p+step-step*side
			* p7:	leftbottom	p-step+step*side
			* p8:	rightbottom	p+step+step*side
			*/
			unsigned int p1, p2, p3, p4, p5, p6, p7, p8, p9;

			

			p1 = size - step;



		}
	} while (size > 0);

	return FALSE;
}

BOOL GetPieceOnBoard(BOARD board[], unsigned int blocks, unsigned int size, unsigned int position)
{
	/*
	* locate a piece on board
	*/
	int bit = (size - position) % 8;
	int block = (size - position) / 8;

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

DWORD WINAPI Timer(LPVOID lpParam)
{
	while (TimerEnabled)
	{

		Sleep(1000);
	}

	return NULL;
}

void StartTimer()
{
	TimerEnabled = TRUE;
	TimerThread = CreateThread(NULL, 0, Timer, NULL, 0, NULL);

}

void StopTimer()
{
	TimerEnabled = FALSE;
	CloseHandle(TimerThread);
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
