/*
Grid.cpp

Arbitrary-type 2D array.  Implemented via std::vector
*/

#ifndef __GRID_CPP
#define __GRID_CPP

#include "Grid.h"

template <class type> void Grid<type>::Allocate(unsigned int Rows, unsigned int Cols) {
  _Rows = Rows;
  _Cols = Cols;
  Data.clear();
  Data.resize(Rows * Cols);
}

template <class type> Grid<type>::Grid() {
  _Rows = 0;
  _Cols = 0;
}

template <class type> Grid<type>::Grid(unsigned int Rows, unsigned int Cols) {
  _Rows = Rows;
  _Cols = Cols;
  Data.resize(Rows * Cols);
}

template <class type> Grid<type>::Grid(const Grid<type> &G) {
  Data = G.Data;
  _Rows = G._Rows;
  _Cols = G._Cols;
}

template <class type> Grid<type>::~Grid() {
  FreeMemory();
}

template <class type> void Grid<type>::FreeMemory() {
  _Rows = 0;
  _Cols = 0;
  Data.resize(0);
  Data.clear();
}

template <class type> Grid<type>& Grid<type>::operator = (const Grid<type> &G) {
  Data = G.Data;
  _Rows = G._Rows;
  _Cols = G._Cols;
  return *this;
}

template <class type> void Grid<type>::ReadColumn(unsigned int Col,
						  unsigned int ElementsToCopy,
						  std::vector<type> &ValuesOut) const {
  ValuesOut.resize(ElementsToCopy);
  for(unsigned int Row = 0; Row < ElementsToCopy; Row++)
    ValuesOut[Row] = (*this)(Row, Col);
}

template <class type> void Grid<type>::WriteColumn(unsigned int Col,
						   unsigned int ElementsToCopy,
						   const std::vector<type> &ValuesIn) {
  for(unsigned int Row = 0; Row < ElementsToCopy; Row++)
    (*this)(Row, Col) = ValuesIn[Row];
}

template <class type> void Grid<type>::ReadRow(unsigned int Row,
					       unsigned int ElementsToCopy,
					       std::vector<type> &ValuesOut) const {
  ValuesOut.resize(ElementsToCopy);
  for(unsigned int Col = 0; Col < ElementsToCopy; Col++)
    ValuesOut[Col] = (*this)(Row, Col);
}

template <class type> void Grid<type>::WriteRow(unsigned int Row,
						unsigned int ElementsToCopy,
						const std::vector<type> &ValuesIn) {
  for(unsigned int Col = 0; Col < ElementsToCopy; Col++)
    (*this)(Row, Col) = ValuesIn[Col];
}

#endif
