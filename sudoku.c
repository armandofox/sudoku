/* I wrote this program, and placed it in the public domain.
 * -- Scott Hemphill  5 September 2005
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int grid[81];                   /* The puzzle grid: contains numbers 1 */
                                /*   through 9, or 0 if a number hasn't */
                                /*   been chosen. */
int bits[81];                   /* Bitmap for each square: 0x01 means */
                                /*   that "1" is not possible, 0x02 means */
                                /*   that "2" is not possible, 0x04 means */
                                /*   that "3" is not possible, ..., 0x100 */
                                /*   that "9" is not possible. */

/* Count of number of 1-bits in a bitmap */

const int count[512] = {
  0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8,
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8,
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8,
  4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8,5,6,6,7,6,7,7,8,6,7,7,8,7,8,8,9,
};

/* For each square, an array of 20 "other" */
/*   locations affect by this square.  For */
/*   example, if there is a "6" in square 0, */
/*   there are 20 other squares we know */
/*   can't contain a six: */
/*   1,2,3,4,5,6,7,8 (horizontal row) */
/*   9,18,27,36,45,54,63,72 (vertcal column) */
/*   1,2,9,10,11,18,19,20 (3x3 square) */
/*   That's a total of 24 square numbers, */
/*   but 4 of them are duplicates.  The */
/*   "others" array is big enough to hold */
/*   24 numbers while it is being built, but */
/*   will only 20 numbers will be used when */
/*   it is complete. */

const int others[81][20] = {
  { 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,18,19,20,27,36,45,54,63,72},
  { 0, 2, 3, 4, 5, 6, 7, 8, 9,10,11,18,19,20,28,37,46,55,64,73},
  { 0, 1, 3, 4, 5, 6, 7, 8, 9,10,11,18,19,20,29,38,47,56,65,74},
  { 0, 1, 2, 4, 5, 6, 7, 8,12,13,14,21,22,23,30,39,48,57,66,75},
  { 0, 1, 2, 3, 5, 6, 7, 8,12,13,14,21,22,23,31,40,49,58,67,76},
  { 0, 1, 2, 3, 4, 6, 7, 8,12,13,14,21,22,23,32,41,50,59,68,77},
  { 0, 1, 2, 3, 4, 5, 7, 8,15,16,17,24,25,26,33,42,51,60,69,78},
  { 0, 1, 2, 3, 4, 5, 6, 8,15,16,17,24,25,26,34,43,52,61,70,79},
  { 0, 1, 2, 3, 4, 5, 6, 7,15,16,17,24,25,26,35,44,53,62,71,80},
  { 0, 1, 2,10,11,12,13,14,15,16,17,18,19,20,27,36,45,54,63,72},
  { 0, 1, 2, 9,11,12,13,14,15,16,17,18,19,20,28,37,46,55,64,73},
  { 0, 1, 2, 9,10,12,13,14,15,16,17,18,19,20,29,38,47,56,65,74},
  { 3, 4, 5, 9,10,11,13,14,15,16,17,21,22,23,30,39,48,57,66,75},
  { 3, 4, 5, 9,10,11,12,14,15,16,17,21,22,23,31,40,49,58,67,76},
  { 3, 4, 5, 9,10,11,12,13,15,16,17,21,22,23,32,41,50,59,68,77},
  { 6, 7, 8, 9,10,11,12,13,14,16,17,24,25,26,33,42,51,60,69,78},
  { 6, 7, 8, 9,10,11,12,13,14,15,17,24,25,26,34,43,52,61,70,79},
  { 6, 7, 8, 9,10,11,12,13,14,15,16,24,25,26,35,44,53,62,71,80},
  { 0, 1, 2, 9,10,11,19,20,21,22,23,24,25,26,27,36,45,54,63,72},
  { 0, 1, 2, 9,10,11,18,20,21,22,23,24,25,26,28,37,46,55,64,73},
  { 0, 1, 2, 9,10,11,18,19,21,22,23,24,25,26,29,38,47,56,65,74},
  { 3, 4, 5,12,13,14,18,19,20,22,23,24,25,26,30,39,48,57,66,75},
  { 3, 4, 5,12,13,14,18,19,20,21,23,24,25,26,31,40,49,58,67,76},
  { 3, 4, 5,12,13,14,18,19,20,21,22,24,25,26,32,41,50,59,68,77},
  { 6, 7, 8,15,16,17,18,19,20,21,22,23,25,26,33,42,51,60,69,78},
  { 6, 7, 8,15,16,17,18,19,20,21,22,23,24,26,34,43,52,61,70,79},
  { 6, 7, 8,15,16,17,18,19,20,21,22,23,24,25,35,44,53,62,71,80},
  { 0, 9,18,28,29,30,31,32,33,34,35,36,37,38,45,46,47,54,63,72},
  { 1,10,19,27,29,30,31,32,33,34,35,36,37,38,45,46,47,55,64,73},
  { 2,11,20,27,28,30,31,32,33,34,35,36,37,38,45,46,47,56,65,74},
  { 3,12,21,27,28,29,31,32,33,34,35,39,40,41,48,49,50,57,66,75},
  { 4,13,22,27,28,29,30,32,33,34,35,39,40,41,48,49,50,58,67,76},
  { 5,14,23,27,28,29,30,31,33,34,35,39,40,41,48,49,50,59,68,77},
  { 6,15,24,27,28,29,30,31,32,34,35,42,43,44,51,52,53,60,69,78},
  { 7,16,25,27,28,29,30,31,32,33,35,42,43,44,51,52,53,61,70,79},
  { 8,17,26,27,28,29,30,31,32,33,34,42,43,44,51,52,53,62,71,80},
  { 0, 9,18,27,28,29,37,38,39,40,41,42,43,44,45,46,47,54,63,72},
  { 1,10,19,27,28,29,36,38,39,40,41,42,43,44,45,46,47,55,64,73},
  { 2,11,20,27,28,29,36,37,39,40,41,42,43,44,45,46,47,56,65,74},
  { 3,12,21,30,31,32,36,37,38,40,41,42,43,44,48,49,50,57,66,75},
  { 4,13,22,30,31,32,36,37,38,39,41,42,43,44,48,49,50,58,67,76},
  { 5,14,23,30,31,32,36,37,38,39,40,42,43,44,48,49,50,59,68,77},
  { 6,15,24,33,34,35,36,37,38,39,40,41,43,44,51,52,53,60,69,78},
  { 7,16,25,33,34,35,36,37,38,39,40,41,42,44,51,52,53,61,70,79},
  { 8,17,26,33,34,35,36,37,38,39,40,41,42,43,51,52,53,62,71,80},
  { 0, 9,18,27,28,29,36,37,38,46,47,48,49,50,51,52,53,54,63,72},
  { 1,10,19,27,28,29,36,37,38,45,47,48,49,50,51,52,53,55,64,73},
  { 2,11,20,27,28,29,36,37,38,45,46,48,49,50,51,52,53,56,65,74},
  { 3,12,21,30,31,32,39,40,41,45,46,47,49,50,51,52,53,57,66,75},
  { 4,13,22,30,31,32,39,40,41,45,46,47,48,50,51,52,53,58,67,76},
  { 5,14,23,30,31,32,39,40,41,45,46,47,48,49,51,52,53,59,68,77},
  { 6,15,24,33,34,35,42,43,44,45,46,47,48,49,50,52,53,60,69,78},
  { 7,16,25,33,34,35,42,43,44,45,46,47,48,49,50,51,53,61,70,79},
  { 8,17,26,33,34,35,42,43,44,45,46,47,48,49,50,51,52,62,71,80},
  { 0, 9,18,27,36,45,55,56,57,58,59,60,61,62,63,64,65,72,73,74},
  { 1,10,19,28,37,46,54,56,57,58,59,60,61,62,63,64,65,72,73,74},
  { 2,11,20,29,38,47,54,55,57,58,59,60,61,62,63,64,65,72,73,74},
  { 3,12,21,30,39,48,54,55,56,58,59,60,61,62,66,67,68,75,76,77},
  { 4,13,22,31,40,49,54,55,56,57,59,60,61,62,66,67,68,75,76,77},
  { 5,14,23,32,41,50,54,55,56,57,58,60,61,62,66,67,68,75,76,77},
  { 6,15,24,33,42,51,54,55,56,57,58,59,61,62,69,70,71,78,79,80},
  { 7,16,25,34,43,52,54,55,56,57,58,59,60,62,69,70,71,78,79,80},
  { 8,17,26,35,44,53,54,55,56,57,58,59,60,61,69,70,71,78,79,80},
  { 0, 9,18,27,36,45,54,55,56,64,65,66,67,68,69,70,71,72,73,74},
  { 1,10,19,28,37,46,54,55,56,63,65,66,67,68,69,70,71,72,73,74},
  { 2,11,20,29,38,47,54,55,56,63,64,66,67,68,69,70,71,72,73,74},
  { 3,12,21,30,39,48,57,58,59,63,64,65,67,68,69,70,71,75,76,77},
  { 4,13,22,31,40,49,57,58,59,63,64,65,66,68,69,70,71,75,76,77},
  { 5,14,23,32,41,50,57,58,59,63,64,65,66,67,69,70,71,75,76,77},
  { 6,15,24,33,42,51,60,61,62,63,64,65,66,67,68,70,71,78,79,80},
  { 7,16,25,34,43,52,60,61,62,63,64,65,66,67,68,69,71,78,79,80},
  { 8,17,26,35,44,53,60,61,62,63,64,65,66,67,68,69,70,78,79,80},
  { 0, 9,18,27,36,45,54,55,56,63,64,65,73,74,75,76,77,78,79,80},
  { 1,10,19,28,37,46,54,55,56,63,64,65,72,74,75,76,77,78,79,80},
  { 2,11,20,29,38,47,54,55,56,63,64,65,72,73,75,76,77,78,79,80},
  { 3,12,21,30,39,48,57,58,59,66,67,68,72,73,74,76,77,78,79,80},
  { 4,13,22,31,40,49,57,58,59,66,67,68,72,73,74,75,77,78,79,80},
  { 5,14,23,32,41,50,57,58,59,66,67,68,72,73,74,75,76,78,79,80},
  { 6,15,24,33,42,51,60,61,62,69,70,71,72,73,74,75,76,77,79,80},
  { 7,16,25,34,43,52,60,61,62,69,70,71,72,73,74,75,76,77,78,80},
  { 8,17,26,35,44,53,60,61,62,69,70,71,72,73,74,75,76,77,78,79},
};

void usage(char *prog)
{
  fprintf(stderr, "usage: %s [sudoko input file]\n", prog);
  exit(1);
}

/*
 * "setbox" is used to place a number in the grid, whether it is known,
 * or a guess.  It also updates all the bitmasks for all the associated
 * squares to indicate that they can't contain this number.
 */

void setbox(int n, int x, int bit)
{
  int i;
  grid[n] = x;
  for (i = 0; i < 20; i++) bits[others[n][i]] |= bit;
}

/*
 * "unsetbox" is used to remove a number from the grid.  Zero is stored
 * to indicate an unused square.  It is too complicated to calculate
 * what the bitmask ought to now contain, so it is up to the calling
 * program to restore the bitmask from a previously saved value.
 */

void unsetbox(int n)
{
  grid[n] = 0;
  // it's up to calling program to restore bits
}

/*
 * "readgrid" is used to read the initial data for the grid from a file.
 * The initial bitmask is calculated at the same time (by calling "setbox").
 */

void readgrid(FILE *f)
{
  char buf[BUFSIZ];
  int i, j;
  for (i = 0; i < 9; i++) {
    if (!fgets(buf, BUFSIZ, f)) {
      fprintf(stderr, "premature end of input\n");
      exit(1);
    }
    if (strlen(buf) < 10) {
      fprintf(stderr, "invalid input line %d\n", i+1);
      exit(1);
    }
    for (j = 0; j < 9; j++) {
      if ('1' <= buf[j] && buf[j] <= '9') {
        setbox(9*i+j, buf[j]-'0', 1<<(buf[j]-'1'));
      }
    }
  }
  for (i = 0; i < 81; i++) {
    if (!grid[i]) continue;
    if (bits[i] & (1<<(grid[i]-1))) {
      fprintf(stderr, "inconsistent input data for box (%d,%d)\n", (i/9)+1,
(i%9)+1);
      exit(1);
    }
  }
}

static long long int num = 0;   /* count of number of solutions found */

void printsolution(void)
{
  int i, j;
  if (num++) printf("\n");      /* if more than one solution, separate them */
  for (i = 0; i < 9; i++) {
    for (j = 0; j < 9; j++) {
      printf("%2d", grid[9*i+j]);
    }
    printf("\n");
  }
}

/* "selectbox" is the heart of the algorithm.  It works this way:
 * First, the grid is searched for an empty square.  If there isn't one,
 * then the grid contains a solution, so it is printed.  If an empty
 * square is found, then it is compared with all other empty squares to
 * see which one of them has the maximum number of 1-bits in its bitmask.
 * That square is the one selected, and each of the possible values are
 * tried in return, calling "selectbox" recursively to search for a
 * solution.  Note that if a square has 9 1-bits in its bitmask, then
 * there are no possible values, and "selectbox" will return without any
 * further recursion.  Otherwise, if a square has 8 1-bits in its bitmask,
 * then it has a forced choice, and only that choice will be taken at this
 * level in the recursion.
 *
 * Note: compile with -DONEONLY to stop recursion if a solution has already
 * been found.
 */

void selectbox(void)
{
  int bitsave[81];
  int i, j;
  int best, bestcount;
#ifdef ONEONLY
  if (num == 0) {           /* allow recursion only if no solutions so far */
#endif
  for (best = 0; best < 81; best++) { /* see if there are any empty squares */
    if (!grid[best]) break;
  }
  if (best == 81) {             /* 81 indicates no empty squares */
    printsolution();
    return;
  }
  bestcount = count[bits[best]]; /* count of bits for the square we found */
  for (i = best+1; i < 81; i++) { /* find the biggest count over all squares */
    if (grid[i]) continue;      /* only consider empty squares */
    j = count[bits[i]];
    if (j > bestcount) {
      best = i;
      bestcount = j;
    }
  }
  for (i = 1, j = 1; i <= 9; i++, j<<=1) { /* Try all numbers for this square */
    if ((bits[best] & j) == 0) { /* if the number is possible, recurse */
      memcpy(bitsave, bits, sizeof bits); /* save bitmask */
      setbox(best, i, j);       /* place number in square, update bitmask */
      selectbox();              /* recurse */
      unsetbox(best);           /* unplace number in square */
      memcpy(bits, bitsave, sizeof bits); /* restore bitmask */
    }
  }
#ifdef ONEONLY
  }
#endif
}

/*
 * main program.  Calls "init" to initialize "count" and "others" arrays.
 * Calls "readgrid" to read initial grid from a file, and calculate initial
 * bitmask.  Calls "selectbox" to recursively enumerate solution(s).  Outputs
 * summary to "stderr".
 */

int main(int argc, char *argv[])
{
  FILE *f = stdin;
  if (argc > 2) usage(argv[0]);
  if (argc == 2) f = fopen(argv[1], "r");
  if (!f) usage(argv[0]);
  readgrid(f);
  selectbox();
#ifdef ONEONLY
  if (num == 0) fprintf(stderr, "no solutions found\n");
#else
  fprintf(stderr, "%lld solution%s found\n", num, num==1?"":"s");
#endif
  return 0;
}
