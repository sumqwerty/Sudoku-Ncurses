#include <cstdlib>
#include <cstring>
#include <curses.h>
#include <iostream>
#include <ncurses.h>
#include <stdlib.h>
#include <string>
#include <utility>
#include <vector>

#define BLUE_BLACK 1
#define WHITE_CYAN 2
#define YELLOW_CYAN 3
#define WHITE_MAGENTA 4
#define YELLOW_MAGENTA 5

#define RED_BLACK 6
#define RED_CYAN 7
#define RED_MAGENTA 8

using namespace std;

class Sudoku {
private:
  const int BoardSize = 9;
  const int BoxSize = 3;
  int difficulty; // empty places

public:
  int board[9][9]{};
  int original[9][9]{};
  int solution[9][9]{};
  vector<pair<int, int>> invalidNum;

  void init(int _difficulty) {
    difficulty = _difficulty;
    fillBoard();    // filling board with solution
    fillSolution(); // copying the solution to a secondary board before emptying
                    // the board
    emptyCells();   // omit random cells from the board
    fillOriginal();
  }

  const int getBoardSize() { return BoardSize; }

  const int getBoxSize() { return BoxSize; }

  int getRandom() { return (1 + rand() % BoardSize); }

  // to check if the number can be placed in the box
  bool canSitInBox(int k, int l, int num) {
    for (int i = 0; i < BoxSize; ++i)
      for (int j = 0; j < BoxSize; ++j)
        if (board[k + i][l + j] == num)
          return false;

    return true;
  }

  // to check if the number can be placed in the row
  bool canSitInRow(int i, int num) {
    for (int j = 0; j < BoardSize; j++) {
      if (board[i][j] == num)
        return false;
    }
    return true;
  }

  // to check if the number can be placed in the col
  bool canSitInCol(int j, int num) {
    for (int i = 0; i < BoardSize; i++) {
      if (board[i][j] == num)
        return false;
    }
    return true;
  }

  // to check if the number can be placed in the cell
  bool canSit(int i, int j, int num) {
    return (canSitInBox(i - i % BoxSize, j - j % BoxSize, num) &&
            canSitInRow(i, num) && canSitInCol(j, num));
  }

  // filling free boxes
  void fillBox(int k, int l) {
    int num;
    for (int i = 0; i < BoxSize; ++i) {
      for (int j = 0; j < BoxSize; ++j) {
        do {
          num = getRandom();
        } while (!canSitInBox(k, l, num));
        board[k + i][k + j] = num;
      }
    }
  }

  // filling non free boxes
  bool backTracking(int i, int j) {
    if (i >= BoardSize && j >= BoardSize) {
      return true;
    }
    if (j >= BoardSize && i < BoardSize - 1) {
      i = i + 1;
      j = 0;
    }
    if (i < BoxSize) {
      if (j < BoxSize) {
        j = BoxSize;
      }
    } else if (i < BoardSize - BoxSize) {
      if (j == (int)(i / BoxSize) * BoxSize) {
        j = j + BoxSize;
      }
    } else {
      if (j == BoardSize - BoxSize) {
        i = i + 1;
        j = 0;
        if (i >= BoardSize) {
          return true;
        }
      }
    }
    for (int num = 1; num <= BoardSize; ++num) {
      if (canSit(i, j, num)) {
        board[i][j] = num;
        if (backTracking(i, j + 1))
          return true;
        board[i][j] = 0;
      }
    }
    return false;
  }

  // omitting cells from the board
  void emptyCells() {
    int cnt = difficulty;
    while (cnt != 0) {
      int i = getRandom() - 1;
      int j = getRandom() - 1;
      if (board[i][j] != 0) {
        board[i][j] = 0;
        cnt--;
      }
    }
  }

  // function to fill the board with a valid solution
  void fillBoard() {
    placeFree();
    backTracking(0, BoxSize);
  }

  // driver for filling free boxes
  void placeFree() {
    int i = 0;
    while (i < BoardSize) {
      fillBox(i, i);
      i += BoxSize;
    }
  }

  // copying the solution before omiting the board
  void fillSolution() {
    for (int i = 0; i < BoardSize; ++i) {
      for (int j = 0; j < BoardSize; ++j) {
        solution[i][j] = board[i][j];
      }
    }
  }

  void fillOriginal() {
    for (int i = 0; i < BoardSize; ++i) {
      for (int j = 0; j < BoardSize; ++j) {
        original[i][j] = board[i][j];
      }
    }
  }

  int getPos(int y, int x) {
    for (int i = 0; i < invalidNum.size(); ++i) {
      if (invalidNum[i].first == y && invalidNum[i].second == x) {
        return i;
      }
    }

    return -1;
  }

  void printInvalid() {
    // cout<<"Invalid: "<<invalidNum.size();
    for (int i = 0; i < invalidNum.size(); ++i) {
      cout << "(" << invalidNum[i].first << "," << invalidNum[i].second << ")"
           << invalidNum.size() << " ";
    }
    cout << endl;
  }

  bool setCell(int i, int j, int num) {
    if (original[i][j] != 0)
      return false;

    if (num != 48 && !canSit(i, j, num % 48)) {
      pair<int, int> temp;
      temp.first = i;
      temp.second = j;
      invalidNum.push_back(temp);

    } else {
      // if the number edited is valid search for it in the invalid array and
      // remove the position
      int pos = getPos(i, j);
      if (pos != -1) {
        invalidNum.erase(invalidNum.begin() + pos);
      }
    }
    board[i][j] = num;
    return true;
  }

  bool checkSolution() {
    for (int i = 0; i < BoardSize; ++i) {
      for (int j = 0; j < BoardSize; ++j) {
        if (solution[i][j] != board[i][j])
          return false;
      }
    }
    return true;
  }

  void fillAns() {
    for (int i = 0; i < BoardSize; ++i) {
      for (int j = 0; j < BoardSize; ++j) {
        board[i][j] = solution[i][j];
      }
    }
  }

  void resetBoard() {
    invalidNum.clear();
    for (int i = 0; i < BoardSize; ++i) {
      for (int j = 0; j < BoardSize; ++j) {
        board[i][j] = original[i][j];
      }
    }
  }

  void cleanBoard() {
    for (int i = 0; i < BoardSize; ++i) {
      for (int j = 0; j < BoardSize; ++j) {
        board[i][j] = 0;
      }
    }
  }

  void newBoard() {
    cleanBoard();
    invalidNum.clear();
    init(difficulty);
  }

  // void disp(){
  // 	for (int i=0; i<BoardSize; i++) {
  // 		for (int j=0; j<BoardSize; j++) {
  // 			if(board[i][j]!=0)cout <<board[i][j];
  // 			else cout<<"-";
  // 			cout<<" ";
  // 		}
  // 		cout<<endl;
  // 	}
  // 	cout<<endl;
  // }
};

class Game {
private:
  int difficulty;
  Sudoku sudoku;
  bool gameOver;
  bool won;
  int stX = 1;
  int stY = 0;
  int incX = 1;
  int incY = 1;
  bool clearBuffer = false;
  bool printedMessage = false;
  int row, col;

public:
  Game(int _difficulty) {
    gameOver = false;
    won = false;
    difficulty = _difficulty;
    sudoku.init(difficulty);
  }

  void changeInput(int num) {
    if (num >= 48 && num <= 57) {
      sudoku.setCell(stY, stX - 1, num % 48);
    }
  }

  void checkFin() {
    if (sudoku.checkSolution()) {
      gameOver = true;
      won = true;
    } else {
      char mesg[] =
          "NOT A VALID SOLUTION! KEEP SOLVING AND USE YOUR HEAD THIS TIME";
      move((row / 2) + 6, 2 + ((col - strlen(mesg)) / 2));
      printw("%s", mesg);
      printedMessage = true;
      move(0, 0);
    }
  }

  void giveAns() { sudoku.fillAns(); }

  void cheat() { gameOver = true; }

  void reset() { sudoku.resetBoard(); }

  void newGame() { sudoku.newBoard(); }

  void printInvalid() { sudoku.printInvalid(); }

  void inpt() {
    int ch;
    while ((ch = getch()) != 'e') {
      if (printedMessage) {
        printedMessage = false;
        clearBuffer = true;
      }
      switch (ch) {
      case KEY_UP:
        stY -= incY;
        break;

      case KEY_DOWN:
        stY += incY;
        break;

      case KEY_LEFT:
        stX -= incX;
        break;

      case KEY_RIGHT:
        stX += incX;
        break;

      case 'f':
        checkFin();
        break;

      case 'm':
        cheat(); // just to test the exit functionality of the game
        break;

      case 'a':
        giveAns();
        break;

      case 'c':
        reset();
        break;

      case 'n':
        newGame();
        break;

      case 'x':
        printInvalid();
        break;

      default:
        changeInput(ch);
      }
      break;
    }

    // code to bring the selector back when it crosses the board borders
    if (stX < 1)
      stX = sudoku.getBoardSize();
    else if (stX > sudoku.getBoardSize())
      stX = 1;

    if (stY < 0)
      stY = sudoku.getBoardSize() - 1;
    else if (stY > sudoku.getBoardSize() - 1)
      stY = 0;
  }

  void disp() {

    int bSize = sudoku.getBoardSize();
    int bxSize = sudoku.getBoxSize();
    bool flg = false;
    int crx = col / 2 - sudoku.getBoardSize() - 3,
        cry = row / 2 - sudoku.getBoardSize() + 3;
    move(cry, crx);

    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_CYAN);
    init_pair(4, COLOR_YELLOW, COLOR_CYAN);
    init_pair(3, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(5, COLOR_YELLOW, COLOR_MAGENTA);

    // invalid number
    init_pair(6, COLOR_RED, COLOR_BLACK);
    init_pair(7, COLOR_RED, COLOR_CYAN);
    init_pair(8, COLOR_RED, COLOR_CYAN);
    init_pair(9, COLOR_RED, COLOR_MAGENTA);
    init_pair(10, COLOR_RED, COLOR_MAGENTA);

    for (int i = 0; i < bSize; ++i) {

      for (int j = 0; j < bSize; ++j) {

        // store if current is wrong
        int wy, wx;
        if (sudoku.getPos(i, j) != -1) {
          wy = i;
          wx = j;
        }

        // ncurses attributes
        if ((i / 3 == j / 3) ||
            (i / 3 == 0 && j / 3 == sudoku.getBoxSize() - 1) ||
            (i / 3 == sudoku.getBoxSize() - 1 && j / 3 == 0)) {
          if (i == wy && j == wx)
            attron(COLOR_PAIR(RED_CYAN));
          else
            attron(COLOR_PAIR(WHITE_CYAN));
        } else {
          attron(COLOR_PAIR(YELLOW_CYAN));
        }

        // hightlight selection //////////////
        if (i == stY && j == stX - 1) {
          attron(A_STANDOUT);
          if (i == wy && j == wx)
            attron(COLOR_PAIR(RED_BLACK));
          else
            attron(COLOR_PAIR(BLUE_BLACK));
          flg = true;
        }
        /////////////////////////////////////

        if (sudoku.board[i][j] != 0) {
          if (sudoku.original[i][j] != 0) {
            if ((i / 3 == j / 3) ||
                (i / 3 == 0 && j / 3 == sudoku.getBoxSize() - 1) ||
                (i / 3 == sudoku.getBoxSize() - 1 && j / 3 == 0)) {
              if (i == wy && j == wx)
                attron(COLOR_PAIR(RED_MAGENTA));
              else
                attron(COLOR_PAIR(WHITE_MAGENTA));
            } else {
              attron(COLOR_PAIR(YELLOW_MAGENTA));
            }
          }
          attron(A_BOLD);
          printw(" %d ", sudoku.board[i][j]);
          attroff(A_BOLD);
          attroff(COLOR_PAIR(WHITE_MAGENTA));
          attroff(COLOR_PAIR(YELLOW_MAGENTA));
          attroff(COLOR_PAIR(RED_MAGENTA));

        } else
          printw(" - ");

        if (flg) {
          attroff(A_STANDOUT);
          attroff(COLOR_PAIR(BLUE_BLACK));
          attroff(COLOR_PAIR(RED_BLACK));
          flg = false;
        }
        attroff(COLOR_PAIR(WHITE_CYAN));
        attroff(COLOR_PAIR(YELLOW_CYAN));
        attroff(COLOR_PAIR(RED_CYAN));
        attroff(COLOR_PAIR(RED_CYAN));
      }
      printw("\n");
      move(++cry, crx);
    }
  }

  void gameLoop() {
    initscr();
    curs_set(0);
    getmaxyx(stdscr, row, col);
    noecho();
    keypad(stdscr, TRUE);
    while (!gameOver) {
      disp();
      inpt();
      refresh();
      if (clearBuffer) {
        clear();
        clearBuffer = false;
      }
      move(0, 0);
    }
    if (won) {
      char mesg[] = "!!YOU WON THE GAME!!";
      move((row / 2) + 6, 2 + ((col - strlen(mesg)) / 2));
      printw("%s", mesg);
    }

    move(sudoku.getBoardSize() + 10, 0);

    char mesg[] = "**GAME FINISHED**";
    move((row / 2) + 8, 2 + ((col - strlen(mesg)) / 2));
    printw("%s", mesg);

    getch();

    endwin();
  }
};

int main() {
  srand(time(0));
  Game game = Game(40);
  game.gameLoop();
  return 0;
}
