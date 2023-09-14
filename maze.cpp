//
// Created by Yusuf Pisan on 4/18/18.
//

#include "maze.h"
#include <algorithm>
#include <cassert>
#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

// print maze object
ostream &operator<<(ostream &out, const Maze &maze) {
  for (int row = 0; row < maze.height; ++row) {
    for (int col = 0; col < maze.width; ++col) {
      out << maze.field[row][col];
    }
    out << endl;
  }
  out << endl;
  return out;
}

// default constructor
Maze::Maze() = default;

// Load maze file from current directory
bool Maze::load(const string &fileName) {
  ifstream inFile;
  inFile.open(fileName);
  if (!inFile.is_open()) {
    cerr << "Unable to open file: " << fileName << endl;
    // next line is just to satisfy clang-tidy
    // otherwise it complains saying function can be const
    width = height = 0;
    return false;
  }
  inFile >> width >> height;
  inFile >> exitRow >> exitColumn;
  inFile >> startRow >> startColumn;
  // ignore leftover charcaters up to newline
  inFile.ignore(INT_MAX, '\n');
  string line;
  for (int row = 0; row < height; ++row) {
    getline(inFile, line);
    field.push_back(line);
  }
  return true;
}

// true if maze can be solved
bool Maze::solve() {
  // make the current row and column have the same values as startColumn and
  // startRow
  curCol = startColumn;
  curRow = startRow;

  // marking the starting point as path
  markAsPath(curRow, curCol);

  // checking whether we are at the exit already
  // and returning true if that's the case
  if (atExit(curRow, curCol)) {
    return true;
  }

  // calling all of the starting from the specific direction maze-solving
  // functions to see whether any of them returns true and storing this bool
  // value in success
  bool success = (goNorth() || goEast() || goSouth() || goWest());

  // if there was no path found marking the starting point as visited
  if (!success) {
    markAsVisited(startRow, startColumn);
  }

  // returning whether the maze was solved or not
  return success;
}

// path to exit
string Maze::getPath() {
  // path += 'N';
  // reverse(path.begin(), path.end());
  return path;
}

// true if row, column is the exit
bool Maze::atExit(int row, int column) const {
  return row == exitRow && column == exitColumn;
}

// true if row, column is inside the maze
bool Maze::isInside(int row, int col) const {
  return row >= 0 && col >= 0 && row < field.size() && col < field[row].size();
}

// true if row, column is clear to move
bool Maze::isClear(int row, int col) const {
  assert(row >= 0 && col >= 0 && row < field.size() && col < field[row].size());
  return field[row][col] == CLEAR;
}

// mark location as part of the path to exit
void Maze::markAsPath(int row, int col) {
  assert(row >= 0 && col >= 0 && row < field.size() && col < field[row].size());
  field[row][col] = PATH;
}

// mark location as visited, not part of the path to exit
void Maze::markAsVisited(int row, int col) {
  assert(row >= 0 && col >= 0 && row < field.size() && col < field[row].size());
  field[row][col] = VISITED;
}

// go North method checks whether north to the current location is "clear" and
// "inside" then solves the maze by calling the other methods (goWest, goSouth,
// goEast) or itself recursively and ultimately returns a boolean based on
// whether it was able to solve the maze
bool Maze::goNorth() {
  // making a bool called success and assigning it to false because maze hasn't
  // been solved yet
  bool success = false;

  // checking whether the point to the north is inside the maze and clear
  if (isInside(curRow - 1, curCol) && isClear(curRow - 1, curCol)) {
    // going to the north by decreasing one from the current row
    curRow--;

    // marking this place as path
    markAsPath(curRow, curCol);

    // adding 'N' to path
    path += 'N';

    // checking whether we are at the exit already
    if (atExit(curRow, curCol)) {
      success = true;
    } else {
      // trying going to North again
      success = goNorth();
      if (!success) // if going north again wasn't successful
      {
        // trying to go to West
        success = goWest();
        if (!success) // if going west wasn't successful
        {
          // trying to go to East
          success = goEast();
          if (!success) // if going to East wasn't successful
          {
            if (isInside(curRow, curCol)) {
              // if we are here it means going to no directions was succesful so
              // that means we are at a dead end so this point was wrong we
              // gotta mark it at visited and backtrack
              markAsVisited(curRow, curCol);
              curRow++;
              path.pop_back();
            }
          }
        }
      }
    }
  }

  // returning success
  return success;
}

// go West method checks whether west to the current location is "clear" and
// "inside" then solves the maze by calling the other methods (goNorth, goSouth,
// goEast) or itself recursively and ultimately returns a boolean based on
// whether it was able to solve the maze
bool Maze::goWest() {
  // making a boolean called success and assigning it to false because maze
  // hasn't been solved yet
  bool success = false;

  // checking whether the point to the west is inside the maze and clear
  if (isInside(curRow, curCol - 1) && isClear(curRow, curCol - 1)) {
    curCol--;
    markAsPath(curRow, curCol);
    path += 'W';
    if (atExit(curRow, curCol)) {
      success = true;
    } else {
      success = goNorth();
      if (!success) {
        success = goWest();
        if (!success) {
          success = goSouth();
          if (!success) {
            if (isInside(curRow, curCol)) {
              // if we are here it means going to no directions was succesful so
              // that means we are at a dead end so this point was wrong we
              // gotta mark it at visited and backtrack
              markAsVisited(curRow, curCol);
              curCol++;
              path.pop_back();
            }
          }
        }
      }
    }
  }

  return success;
}

// go East method checks whether west to the current location is "clear" and
// "inside" then solves the maze by calling the other methods (goNorth, goSouth,
// goNorth) or itself recursively and ultimately returns a boolean based on
// whether it was able to solve the maze
bool Maze::goEast() {
  // making a boolean called success and assigning it to false because maze
  // hasn't been solved yet
  bool success = false;

  // checking whether the point to the east is inside the maze and clear
  if (isInside(curRow, curCol + 1) && isClear(curRow, curCol + 1)) {
    curCol++;
    markAsPath(curRow, curCol);
    path += 'E';
    if (atExit(curRow, curCol)) {
      success = true;
    } else {
      success = goNorth();
      if (!success) {
        success = goEast();
        if (!success) {
          success = goSouth();
          if (!success) {
            if (isInside(curRow, curCol)) {
              // if we are here it means going to no directions was succesful so
              // that means we are at a dead end so this point was wrong we
              // gotta mark it at visited and backtrack
              markAsVisited(curRow, curCol);
              curCol--;
              path.pop_back();
            }
          }
        }
      }
    }
  }

  return success;
}

// go South method checks whether west to the current location is "clear" and
// "inside" then solves the maze by calling the other methods (goNorth, goWest,
// goEast) or itself recursively and ultimately returns a boolean based on
// whether it was able to solve the maze
bool Maze::goSouth() {
  // making a boolean called success and assigning it to false because maze
  // hasn't been solved yet
  bool success = false;

  // checking whether the point to the south is inside the maze and clear
  if (isInside(curRow + 1, curCol) && isClear(curRow + 1, curCol)) {
    curRow++;
    markAsPath(curRow, curCol);
    path += 'S';
    if (atExit(curRow, curCol)) {
      success = true;
    } else {
      success = goSouth();
      if (!success) {
        success = goEast();
        if (!success) {
          success = goWest();
          if (!success) {
            if (isInside(curRow, curCol)) {
              // if we are here it means going to no directions was succesful so
              // that means we are at a dead end so this point was wrong we
              // gotta mark it at visited and backtrack
              markAsVisited(curRow, curCol);
              curRow--;
              path.pop_back();
            }
          }
        }
      }
    }
  }

  return success;
}