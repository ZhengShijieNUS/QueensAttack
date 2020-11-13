﻿// QueensAttack.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>

#ifdef WIN32

#else
typedef enum { FALSE, TRUE } BOOL;
#endif

#define BLOCKSIZE 8
//#define BOARD BYTE
#define EMPTY_CELL "[ ]"
#define PIECE_CELL "[*]"

typedef unsigned char BYTE;
typedef unsigned long long ULL;


typedef struct {
	ULL pieces;
	BYTE* board;
}BOARD;

typedef struct {
	unsigned int x;
	unsigned int y;
	ULL index;
}COORDINATE;


/// <summary>
/// the size of a side of the board
/// </summary>
ULL side = 0;

/// <summary>
/// total size of the board
/// </summary>
ULL size = 0;

/// <summary>
/// the numer of pieces that every queen must attack
/// </summary>
int attack = 0;

/// <summary>
/// whether to print the location
/// </summary>
BOOL printlocation = FALSE;

/// <summary>
/// wraparound
/// </summary>
BOOL wraparound = FALSE;

/// <summary>
/// the number of blocks to store a board, each block equals a byte
/// </summary>
ULL blocks = 0;

/// <summary>
/// the number of pieces on the board
/// </summary>
ULL pieces = 0;

void PrintBoard(BYTE board[], unsigned int blocks, unsigned int size, unsigned int side);
void PrintBoardIndex(BYTE board[], unsigned int blocks, unsigned int size, unsigned int side);
BOOL CheckBoard(BYTE board[], unsigned int size, unsigned int side, int attack, BOOL wraparound);
COORDINATE GetCoordinate(unsigned int locationIndex, unsigned int side);
ULL GetLocationIndex(COORDINATE* c, unsigned int side);
BYTE* AddPieceToBoard(BYTE board[], unsigned int blocks);
BYTE* RemovePieceFromBoard(BYTE board[], unsigned int blocks);
BOOL GetPieceOnBoard(BYTE board[], unsigned int blocks, unsigned int size, unsigned int cell);
unsigned int GetPiecesCount(BYTE board[], unsigned int blocks, unsigned int size);
BYTE* InitializeBoard(unsigned int blocks, unsigned int size);
void ReleaseBoard(BYTE board[]);

BOOL CheckUp(BYTE board[], COORDINATE coordinate, ULL blocks, ULL size, ULL side, BOOL pieceAttacked[], BOOL wraparound);
BOOL CheckDown(BYTE board[], COORDINATE coordinate, ULL blocks, ULL size, ULL side, BOOL pieceAttacked[], BOOL wraparound);
BOOL CheckLeft(BYTE board[], COORDINATE coordinate, ULL blocks, ULL size, ULL side, BOOL pieceAttacked[], BOOL wraparound);
BOOL CheckRight(BYTE board[], COORDINATE coordinate, ULL blocks, ULL size, ULL side, BOOL pieceAttacked[], BOOL wraparound);
BOOL CheckLefttop(BYTE board[], COORDINATE coordinate, ULL blocks, ULL size, ULL side, BOOL pieceAttacked[], BOOL wraparound);
BOOL CheckRighttop(BYTE board[], COORDINATE coordinate, ULL blocks, ULL size, ULL side, BOOL pieceAttacked[], BOOL wraparound);
BOOL CheckLeftbottom(BYTE board[], COORDINATE coordinate, ULL blocks, ULL size, ULL side, BOOL pieceAttacked[], BOOL wraparound);
BOOL CheckRightbottom(BYTE board[], COORDINATE coordinate, ULL blocks, ULL size, ULL side, BOOL pieceAttacked[], BOOL wraparound);

int main(int argc, char* argv[])
{
	if (argc == 5)
	{

		side = (ULL)atoll(argv[1]);	//the size of a side of the board
		if (side > 4294967295)
		{
			printf("N must be less than 4294967295\r\n");
			exit(FALSE);
		}

		size = side * side;
		attack = atoi(argv[2]);	//the numer of pieces that every queen must attack
		if (attack > 9 || attack < 0)
		{
			printf("k must be between 0 and 9.\r\n");
			exit(FALSE);
		}

		if (atoi(argv[3]) == 0)	//format
		{
			printlocation = FALSE;
		}
		else
		{
			printlocation = TRUE;
		}

		if (atoi(argv[4]) == 0)	//wraparound
		{
			wraparound = FALSE;
		}
		else
		{
			wraparound = TRUE;
		}

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


	BYTE* board = InitializeBoard(blocks, size);

	BOARD* boards = NULL;
	ULL boardsCount = 0;

	/*
	* 循环直到棋子布满棋盘
	*/
	while (pieces < size)
	{
		AddPieceToBoard(board, blocks);
		pieces = GetPiecesCount(board, blocks, size);

		BOOL result = CheckBoard(board, size, side, attack, wraparound);
		if (result == TRUE)
		{
			boardsCount++;

			boards = (BOARD*)realloc(boards, boardsCount * sizeof(BOARD));
			boards[boardsCount - 1].pieces = pieces;
			boards[boardsCount - 1].board = InitializeBoard(blocks, size);
			memcpy(boards[boardsCount - 1].board, board, blocks * sizeof(BYTE));

			//PrintBoard(boards[boardsCount - 1].board, blocks, size, side);
			//printf("pieces:%lld\r\n", boards[boardsCount - 1].pieces);
		}
	}

	printf("All boards exhausted.\r\n");

	//Export result
	ULL maxPieces = 0;
	ULL i = 0;
	for (i = 0; i < boardsCount; i++)
	{
		if (boards[i].pieces > maxPieces)
		{
			maxPieces = boards[i].pieces;
		}
	}

	printf("Max pieces is:%d.\r\n", maxPieces);

	for (i = 0; i < boardsCount; i++)
	{
		if (boards[i].pieces == maxPieces)
		{
			printf("%d, %d:%d:", side, attack, maxPieces);
			if (printlocation == TRUE)
			{
				PrintBoardIndex(boards[i].board, blocks, size, side);
			}

#ifdef DEBUG
			PrintBoard(boards[i].board, blocks, size, side);
			printf("--------------------------------------------\r\n");
#endif
		}
	}


	//Free memory
	ReleaseBoard(board);

	do
	{
		boardsCount--;
		free(boards[boardsCount].board);
	} while (boardsCount > 0);

	free(boards);

	//exit with success
	exit(TRUE);
}

BYTE* InitializeBoard(unsigned int blocks, unsigned int size)
{
	BYTE* board = (BYTE*)calloc(blocks, sizeof(BYTE));

	return board;
}

void ReleaseBoard(BYTE board[])
{
	free(board);
}

BOOL CheckBoard(BYTE board[], unsigned int size, unsigned int side, int attack, BOOL wraparound)
{
	//current position
	ULL p = size;
	do
	{
		/*													y
		*	08 07 06					(3,3)	(2,3)	(1,3)
		*	05 04 03					(3,2)	(2,2)	(1,2)
		*	02 01 00				x 	(3,1)	(2,1)	(1,1)
		*
		*															y
		*	15 14 13 12					(4,4)	(3,4)	(2,4)	(1,4)
		*	11 10 09 08					(4,3)	(3,3)	(2,3)	(1,3)
		*	07 06 05 04					(4,2)	(3,2)	(2,2)	(1,2)
		*	03 02 01 00				x	(4,1)	(3,1)	(2,1)	(1,1)
		*
		*																	y
		*	24 23 22 21 20				(5,5)	(4,5)	(3,5)	(2,5)	(1,5)
		*	19 18 17 16 15				(5,4)	(4,4)	(3,4)	(2,4)	(1,4)
		*	14 13 12 11 10				(5,3)	(4,3)	(3,3)	(2,3)	(1,3)
		*	09 08 07 06 05				(5,2)	(4,2)	(3,2)	(2,2)	(1,2)
		*	04 03 02 01 00			x	(5,1)	(4,1)	(3,1)	(2,1)	(1,1)
		*/
		p--;

		if (GetPieceOnBoard(board, blocks, size, p) == FALSE)
		{
			continue;
		}

		COORDINATE cc = GetCoordinate(p, side);

#ifdef DEBUG
		printf("origin:%d(%d,%d)\r\n", p, cc.x, cc.y);
#endif
		int k = 0;

		BOOL* pieceAttacked = (BOOL*)malloc(size * sizeof(BOOL));
		memset(pieceAttacked, FALSE, size * sizeof(BOOL));

		k = (int)CheckUp(board, cc, blocks, size, side, pieceAttacked, wraparound);
		k += (int)CheckDown(board, cc, blocks, size, side, pieceAttacked, wraparound);
		k += (int)CheckLeft(board, cc, blocks, size, side, pieceAttacked, wraparound);
		k += (int)CheckRight(board, cc, blocks, size, side, pieceAttacked, wraparound);

		k += (int)CheckLefttop(board, cc, blocks, size, side, pieceAttacked, wraparound);
		k += (int)CheckRighttop(board, cc, blocks, size, side, pieceAttacked, wraparound);
		k += (int)CheckLeftbottom(board, cc, blocks, size, side, pieceAttacked, wraparound);
		k += (int)CheckRightbottom(board, cc, blocks, size, side, pieceAttacked, wraparound);

		free(pieceAttacked);

		if (k != attack)
		{
#ifdef DEBUG
			printf("Piece %d(%d,%d) attacks %d more than %d, skip. \r\n", cc.index, cc.x, cc.y, k, attack);
#endif
			return FALSE;
		}
#ifdef DEBUG
		printf("-------------------------------------------------------------\r\n");
#endif

	} while (p > 0);

	return TRUE;
}
BOOL CheckRightbottom(BYTE board[], COORDINATE coordinate, ULL blocks, ULL size, ULL side, BOOL pieceAttacked[], BOOL wraparound)
{
	BOOL result = FALSE;
	ULL step = 1;
	for (step = 1; step < side; step++)
	{
		COORDINATE p;

		//rightbottom
		if (coordinate.x <= step)
		{
			if (wraparound)
			{
				p.x = coordinate.x + side - step;
			}
			else
			{
				break;
			}
		}
		else
		{
			p.x = coordinate.x - step;
		}

		if (coordinate.y <= step)
		{
			if (wraparound)
			{
				p.y = coordinate.y + side - step;
			}
			else
			{
				break;
			}
		}
		else
		{
			p.y = coordinate.y - step;
		}

		GetLocationIndex(&p, side);

		if (pieceAttacked[p.index] == TRUE)
		{
#ifdef DEBUG
			printf("CheckRightbottom piece %d(%d,%d) has been attacked.\r\n", p.index, p.x, p.y);
#endif
			break;
		}
#ifdef DEBUG
		printf("CheckRightbottom\tstep:%d, %d(%d,%d)\r\n", step, p.index, p.x, p.y);
#endif
		if (GetPieceOnBoard(board, blocks, size, p.index) == TRUE)
		{
			result = TRUE;
			pieceAttacked[p.index] = result;
			break;
		}
	}
	return result;
}

BOOL CheckLeftbottom(BYTE board[], COORDINATE coordinate, ULL blocks, ULL size, ULL side, BOOL pieceAttacked[], BOOL wraparound)
{
	BOOL result = FALSE;
	ULL step = 1;
	for (step = 1; step < side; step++)
	{
		COORDINATE p;

		//leftbottom
		p.x = coordinate.x + step;
		if (p.x > side)
		{
			if (wraparound)
			{
				p.x -= side;
			}
			else
			{
				break;
			}
		}


		if (coordinate.y <= step)
		{
			if (wraparound)
			{
				p.y = coordinate.y + side - step;
			}
			else
			{
				break;
			}
		}
		else
		{
			p.y = coordinate.y - step;
		}

		GetLocationIndex(&p, side);

		if (pieceAttacked[p.index] == TRUE)
		{
#ifdef DEBUG
			printf("CheckLeftbottom piece %d(%d,%d) has been attacked.\r\n", p.index, p.x, p.y);
#endif
			break;
		}
#ifdef DEBUG
		printf("CheckLeftbottom\t\tstep:%d, %d(%d,%d)\r\n", step, p.index, p.x, p.y);
#endif
		if (GetPieceOnBoard(board, blocks, size, p.index) == TRUE)
		{
			result = TRUE;
			pieceAttacked[p.index] = result;
			break;
		}
	}
	return result;
}

BOOL CheckRighttop(BYTE board[], COORDINATE coordinate, ULL blocks, ULL size, ULL side, BOOL pieceAttacked[], BOOL wraparound)
{
	BOOL result = FALSE;
	ULL step = 1;
	for (step = 1; step < side; step++)
	{
		COORDINATE p;

		//righttop
		if (coordinate.x <= step)
		{
			if (wraparound)
			{
				p.x = coordinate.x + side - step;
			}
			else
			{
				break;
			}
		}
		else
		{
			p.x = coordinate.x - step;
		}

		p.y = coordinate.y + step;
		if (p.y > side)
		{
			if (wraparound)
			{
				p.y -= side;
			}
			else
			{
				break;
			}
		}

		GetLocationIndex(&p, side);

		if (pieceAttacked[p.index] == TRUE)
		{
#ifdef DEBUG
			printf("CheckRighttop piece %d(%d,%d) has been attacked.\r\n", p.index, p.x, p.y);
#endif
			break;
		}

#ifdef DEBUG
		printf("CheckRighttop\t\tstep:%d, %d(%d,%d)\r\n", step, p.index, p.x, p.y);
#endif	

		if (GetPieceOnBoard(board, blocks, size, p.index) == TRUE)
		{
			result = TRUE;
			pieceAttacked[p.index] = result;
			break;
		}
	}
	return result;
}

BOOL CheckLefttop(BYTE board[], COORDINATE coordinate, ULL blocks, ULL size, ULL side, BOOL pieceAttacked[], BOOL wraparound)
{
	BOOL result = FALSE;
	ULL step = 1;
	for (step = 1; step < side; step++)
	{
		COORDINATE p;

		//lefttop
		p.x = coordinate.x + step;
		p.y = coordinate.y + step;
		if (p.x > side)
		{
			if (wraparound)
			{
				p.x -= side;
			}
			else
			{
				break;
			}
		}
		if (p.y > side)
		{
			if (wraparound)
			{
				p.y -= side;
			}
			else
			{
				break;
			}
		}

		GetLocationIndex(&p, side);

		if (pieceAttacked[p.index] == TRUE)
		{
#ifdef DEBUG
			printf("CheckLefttop piece %d(%d,%d) has been attacked.\r\n", p.index, p.x, p.y);
#endif
			break;
		}

#ifdef DEBUG
		printf("CheckLefttop\t\tstep:%d, %d(%d,%d)\r\n", step, p.index, p.x, p.y);
#endif

		if (GetPieceOnBoard(board, blocks, size, p.index) == TRUE)
		{
			result = TRUE;
			pieceAttacked[p.index] = result;
			break;
		}
	}
	return result;
}


BOOL CheckRight(BYTE board[], COORDINATE coordinate, ULL blocks, ULL size, ULL side, BOOL pieceAttacked[], BOOL wraparound)
{
	BOOL result = FALSE;
	ULL step = 1;
	for (step = 1; step < side; step++)
	{
		COORDINATE p;

		//right
		if (coordinate.x <= step)
		{
			if (wraparound)
			{
				p.x = coordinate.x + side - step;
			}
			else
			{
				break;
			}
		}
		else
		{
			p.x = coordinate.x - step;
		}

		p.y = coordinate.y;

		GetLocationIndex(&p, side);

		if (pieceAttacked[p.index] == TRUE)
		{
#ifdef DEBUG
			printf("CheckRight piece %d(%d,%d) has been attacked.\r\n", p.index, p.x, p.y);
#endif
			break;
		}
#ifdef DEBUG
		printf("CheckRight\t\tstep:%d, %d(%d,%d)\r\n", step, p.index, p.x, p.y);
#endif

		if (GetPieceOnBoard(board, blocks, size, p.index) == TRUE)
		{
			result = TRUE;
			pieceAttacked[p.index] = result;
			break;
		}
	}
	return result;
}

BOOL CheckLeft(BYTE board[], COORDINATE coordinate, ULL blocks, ULL size, ULL side, BOOL pieceAttacked[], BOOL wraparound)
{
	BOOL result = FALSE;
	ULL step = 1;
	for (step = 1; step < side; step++)
	{
		COORDINATE p;

		//left
		p.x = coordinate.x + step;
		if (p.x > side)
		{
			if (wraparound)
			{
				p.x -= side;
			}
			else
			{
				break;
			}
		}
		p.y = coordinate.y;

		GetLocationIndex(&p, side);

		if (pieceAttacked[p.index] == TRUE)
		{
#ifdef DEBUG
			printf("CheckLeft piece %d(%d,%d) has been attacked.\r\n", p.index, p.x, p.y);
#endif
			break;
		}

#ifdef DEBUG
		printf("CheckLeft\t\tstep:%d, %d(%d,%d)\r\n", step, p.index, p.x, p.y);
#endif
		if (GetPieceOnBoard(board, blocks, size, p.index) == TRUE)
		{
			result = TRUE;
			pieceAttacked[p.index] = result;
			break;
		}
	}
	return result;
}

BOOL CheckDown(BYTE board[], COORDINATE coordinate, ULL blocks, ULL size, ULL side, BOOL pieceAttacked[], BOOL wraparound)
{
	BOOL result = FALSE;
	ULL step = 1;
	for (step = 1; step < side; step++)
	{
		COORDINATE p;

		//bottom
		p.x = coordinate.x;

		if (coordinate.y <= step)
		{
			if (wraparound)
			{
				p.y = coordinate.y + side - step;
			}
			else
			{
				break;
			}
		}
		else
		{
			p.y = coordinate.y - step;
		}

		GetLocationIndex(&p, side);

		if (pieceAttacked[p.index] == TRUE)
		{
#ifdef DEBUG
			printf("CheckDown piece %d(%d,%d) has been attacked.\r\n", p.index, p.x, p.y);
#endif

			break;
		}

#ifdef DEBUG
		printf("CheckDown\t\tstep:%d, %d(%d,%d)\r\n", step, p.index, p.x, p.y);
#endif

		if (GetPieceOnBoard(board, blocks, size, p.index) == TRUE)
		{
			result = TRUE;
			pieceAttacked[p.index] = result;
			break;
		}
	}
	return result;
}

BOOL CheckUp(BYTE board[], COORDINATE coordinate, ULL blocks, ULL size, ULL side, BOOL pieceAttacked[], BOOL wraparound)
{
	BOOL result = FALSE;
	ULL step = 1;
	for (step = 1; step < side; step++)
	{
		COORDINATE p;

		//top
		p.x = coordinate.x;
		p.y = coordinate.y + step;
		if (p.y > side)
		{
			if (wraparound)
			{
				p.y -= side;
			}
			else
			{
				break;
			}
		}

		GetLocationIndex(&p, side);

		if (pieceAttacked[p.index] == TRUE)
		{
#ifdef DEBUG
			printf("CheckUp piece %d(%d,%d) has been attacked.\r\n", p.index, p.x, p.y);
#endif
			break;
		}

#ifdef DEBUG
		printf("CheckUp\t\t\tstep:%d, %d(%d,%d)\r\n", step, p.index, p.x, p.y);
#endif

		if (GetPieceOnBoard(board, blocks, size, p.index) == TRUE)
		{
			result = TRUE;
			pieceAttacked[p.index] = result;
			break;
		}

	}
	return result;
}



COORDINATE GetCoordinate(unsigned int locationIndex, unsigned int side) {
	COORDINATE c;
	c.index = locationIndex + 1;
	int m = c.index % side;
	if (m == 0) {
		c.y = floor(c.index / side);
		c.x = side;
	}
	else {
		c.y = floor(c.index / side) + 1;
		c.x = m;
	}

	return c;
}

ULL GetLocationIndex(COORDINATE* c, unsigned int side)
{
	c->index = (c->y * side - (side - 1)) + (c->x - 1) - 1;
	return c->index;
}

BOOL GetPieceOnBoard(BYTE board[], unsigned int blocks, unsigned int size, unsigned int cell)
{
	/*
	* locate a piece on board
	*/
	int bit = cell % 8;
	int block = cell / 8;
#ifdef DEBUG
	printf("cell:%d,bit:%d,block:%d\r\n",cell,bit,block);
#endif
	return (board[block] & 1 << bit) ? TRUE : FALSE;
}

/// <summary>
/// Add a piece on board sequentially 
/// </summary>
/// <param name="board">board address</param>
/// <param name="size">board size</param>
/// <returns>board address</returns>
BYTE* AddPieceToBoard(BYTE board[], unsigned int blocks)
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

BYTE* RemovePieceFromBoard(BYTE board[], unsigned int blocks)
{
	BOOL borrowed = FALSE;


	return board;
}

unsigned int GetPiecesCount(BYTE board[], unsigned int blocks, unsigned int size)
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

void PrintBoardIndex(BYTE board[], unsigned int blocks, unsigned int size, unsigned int side)
{
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
			if ((board[blocks] & 1 << bit) > 0)
			{
				int r = side * side - (blocks * 8 + bit);
				printf("%d,", r);
			}

		} while (bit > 0);	//当bit大于0时，说明仍需要取下一位，
	} while (blocks > 0);	//当blocks大于0时，说明仍然需要取下一字节
	printf("\r\n");
}

void PrintBoard(BYTE board[], unsigned int blocks, unsigned int size, unsigned int side)
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