./findAQ N k l w
•N is the size of a side of the board
•k is the numer of pieces that every queen must attack
•l=0 is short format N,k:number:and l=1 is long format N,k:number:pc1,pc2,pc3...
•w=0 is no wraparound, w=1 is wraparound



1001
1100
0011
1001


10011100 00111001

10110
01110
00001
11011
10010


	BOARD Board[] = {
		(
			(FALSE << 7) +
			(TRUE << 6) +
			(TRUE << 5) +
			(TRUE << 4) +
			(FALSE << 3) +
			(FALSE << 2) +
			(TRUE << 1) +
			(FALSE << 0)
		),
		(
			(FALSE << 7) +
			(FALSE << 6) +
			(FALSE << 5) +
			(FALSE << 4) +
			(FALSE << 3) +
			(TRUE << 2) +
			(TRUE << 1) +
			(TRUE << 0)
		),
		(
			(FALSE << 7) +
			(TRUE << 6) +
			(TRUE << 5) +
			(FALSE << 4) +
			(FALSE << 3) +
			(TRUE << 2) +
			(TRUE << 1) +
			(TRUE << 0)
		),
		(
			(FALSE << 7) +
			(FALSE << 6) +
			(FALSE << 5) +
			(FALSE << 4) +
			(FALSE << 3) +
			(FALSE << 2) +
			(FALSE << 1) +
			(TRUE << 0)
		),
	};


87654321
01234567 01234567 01234567 01234567
-------1 01100111 00000111 01110010

bit=(size-p)%8
block=(size-p)/8

piece p:
n=side, s=step

left:	p-s,
right:	p+s,
top:	p-s*n,
bottom:	p+s*n,

lefttop:p-s-s*n,
righttop:p+s-s*n,
leftbottom:p-s+s*n
rightbottom:p+s+s*n

