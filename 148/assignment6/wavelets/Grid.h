/*
Grid.h

Arbitrary-type 2D array.  Implemented via std::vector

*/

#ifndef __GRID_H
#define __GRID_H

#include <vector>

template <class type> class Grid {
 public:
  Grid();
  Grid(unsigned int Rows, unsigned int Cols);
  Grid(const Grid<type> &G);

  ~Grid();
  void FreeMemory();

  Grid<type>& operator = (const Grid<type> &G);

  //
  // Allocate memory to the grid (2d-array) of the specified Rows and Cols 
  //
  void Allocate(unsigned int Rows, unsigned int Cols);

	//
  //Copy ElementsToCopy number of data from the top of column Col on the Grid to the ValuesOut vector.
  //
  void ReadColumn(unsigned int Col, unsigned int ElementsToCopy, std::vector<type> &ValuesOut) const;

  //
  //Copy ElementsToCopy number of data from ValuesIn to column Col on the Grid (starting from the top).
  //
  void WriteColumn(unsigned int Col, unsigned int ElementsToCopy, const std::vector<type> &ValuesIn);

  //
  //Copy ElementsToCopy number of data from the left of row Row on the Grid to the ValuesOut vector.
  //
  void ReadRow(unsigned int Row, unsigned int ElementsToCopy, std::vector<type> &ValuesOut) const;

  //
  //Copy ElementsToCopy number of data from ValuesIn to row Row on the Grid (starting from the left).
  //
  void WriteRow(unsigned int Row, unsigned int ElementsToCopy, const std::vector<type> &ValuesIn);
 
  //
  // Read a data value given its Row, Col indices.
  //
  type& operator()(unsigned int Row, unsigned int Col) {
    return Data[Row * _Cols + Col];
  }
  
  //
  // Read a data value given its Row, Col indices.
  //
  const type& operator()(unsigned int Row, unsigned int Col) const {
    return Data[Row * _Cols + Col];
  }

  //
  // Get the number of rows of the Grid.
  //
  unsigned int Rows() const {
    return _Rows;
  }

  //
  // Get the number of columns of the Grid.
  //
  unsigned int Cols() const {
    return _Cols;
  }
  
 protected:
  std::vector<type> Data;
  unsigned int _Rows, _Cols;
};

#include "Grid.cpp"

#endif
