

//
//  Wavelet.cpp
//  Wavelet Compression
//
//  Created by Michael Rotondo on 11/17/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#include <iostream>

#include "Wavelet.h"
#include "STColor3f.h"
#include <assert.h>

STImage *createImageFromGrid(const Grid<STVector3> *grid)
{
    STImage *newImage = new STImage((*grid).Cols(), (*grid).Rows());
    for (int i = 0; i < newImage->GetWidth(); i++)
    {
        for (int j = 0; j < newImage->GetHeight(); j++)
        {
            STVector3 v = (*grid)(j, i);
            newImage->SetPixel(i, j, STImage::Pixel(STColor3f(v.x, v.y, v.z)));
        }
    }
    return newImage;
}

Grid<STVector3> *createGridFromImage(const STImage *image)
{
    Grid<STVector3> *newGrid = new Grid<STVector3>(image->GetHeight(), image->GetWidth());
    for (int i = 0; i < newGrid->Cols(); i++)
    {
        for (int j = 0; j < newGrid->Rows(); j++)
        {
            STColor3f c = STColor3f(image->GetPixel(i, j));
            (*newGrid)(j, i) = STVector3(c.r, c.g, c.b);
        }
    }    
    return newGrid;
}

void printGrid(Grid<STVector3> &grid)
{
    for (int i = 0; i < grid.Rows(); i++)
    {
        for (int j = 0; j < grid.Cols(); j++)
        {
            STVector3 v = grid(i, j);
            printf("%f, %f, %f :: ", v.x, v.y, v.z);
        }
        printf("\n");
    }
}

Wavelet::Wavelet(const std::string &filename)
{
    original = new STImage(filename);
    transformed = new STImage(filename);
    reconstructed = new STImage(filename);
    difference = new STImage(filename);
    
    assert(fmodf(log2f(original->GetWidth()), 1.0f) == 0.0f);
    assert(fmodf(log2f(original->GetHeight()), 1.0f) == 0.0f);
    assert(original->GetWidth() == original->GetHeight());
    
    int size = 4;
    Grid<STVector3> signal(size, size);
    Grid<STVector3> transformed(size, size);
    Grid<STVector3> reconstructed(size, size);
    for (int i = 0; i < signal.Rows(); i++)
    {
        for (int j = 0; j < signal.Cols(); j++)
        {
            signal(i, j) = i + 2 * j;
        }
    }
    printGrid(signal);
    
    HaarTransformForward2D(signal, transformed);
    
    HaarTransformInverse2D(transformed, reconstructed);
    
//    Grid<STVector3> *signal = createGridFromImage(original);
//    Grid<STVector3> transformed(signal->Rows(), signal->Cols());
//    
//    HaarTransformForward2D(*signal, transformed);
//    STImage *output1 = createImageFromGrid(&transformed);
//    output1->Save("output1.png");
//    
//    HaarTransformInverse2D(transformed, *signal);
//    STImage *output2 = createImageFromGrid(signal);
//    output2->Save("output2.png");
    
    
    
//    delete signal;
//    delete output1;
//    delete output2;
}

void Wavelet::HaarTransformForward1D(const std::vector<STVector3> &Signal, std::vector<STVector3> &Transformed)
{
    size_t size = Signal.size();  // Would be nicer to store order and derive size
    for (int i = 0; i < size; i++)
    {
        Transformed[i] = Signal[i];
    }
    HaarTransformForward1DHelper(size, Signal, Transformed, true);
}

void Wavelet::HaarTransformForward1DHelper(size_t size, const std::vector<STVector3> &Signal, std::vector<STVector3> &Transformed, bool recurse)
{
    if (size == 1)
    {
        return;
    }

    printf("HERE COMES FORWARD SIZE: %d\n", size);
    
    std::vector<STVector3> newDifferences;
    
    for (int i = 0; i < size / 2; i++)
    {
        const STVector3 &a = Transformed[i * 2];
        const STVector3 &b = Transformed[i * 2 + 1];
        
        const STVector3 &average = (a + b) / 2.0f;
        const STVector3 &difference = (a - b) / 2.0f + STVector3(0.5f, 0.5f, 0.5f);
        
        Transformed[i] = average;
        newDifferences.push_back(difference);
    }
    
    for (int i = 0; i < size / 2; i++)
    {
        Transformed[size / 2 + i] = newDifferences[i];
    }

    if (recurse)
        HaarTransformForward1DHelper(size / 2, Signal, Transformed, true);
}

void Wavelet::HaarTransformInverse1D(const std::vector<STVector3> &Transformed, std::vector<STVector3> &Signal)
{
    size_t size = 1;
    size_t finalSize = Transformed.size();
    
    for (int i = 0; i < finalSize; i++)
    {
        Signal[i] = Transformed[i];
    }
    
    HaarTransformInverse1DHelper(size, finalSize, Transformed, Signal, true);
}

void Wavelet::HaarTransformInverse1DHelper(size_t size, size_t finalSize, const std::vector<STVector3> &Transformed, std::vector<STVector3> &Signal, bool recurse)
{
    if (size == finalSize)
    {
        return;
    }
    
    printf("HERE COMES INVERSE SIZE: %lu\n", size);
    
    std::vector<STVector3> newSignal;
    
    for (int i = 0; i < size; i++)
    {
        const STVector3 &average = Signal[i];
        const STVector3 &difference = Signal[size + i];
        const STVector3 a = average + (difference - STVector3(0.5f, 0.5f, 0.5f));
        const STVector3 b = average - (difference - STVector3(0.5f, 0.5f, 0.5f));
        
        printf("S: %f %f %f, D: %f %f %f\n", average.x, average.y, average.z, difference.x, difference.y, difference.z);
        printf("A: %f %f %f, B: %f %f %f\n", a.x, a.y, a.z, b.x, b.y, b.z);
        
        newSignal.push_back(a);
        newSignal.push_back(b);
    }
    
//    printf("About to write %d items into Signal which is of size %d\n", size * 2, Signal.size());
    
    for (int i = 0; i < size * 2; i++)
    {
//        printf("%f %f %f\n", newSignal[i].x, newSignal[i].y, newSignal[i].z);
        Signal[i] = newSignal[i];
    }
    
    if (recurse)
        HaarTransformInverse1DHelper(size * 2, finalSize, Transformed, Signal, true);
}

void Wavelet::HaarTransformForward2D(const Grid<STVector3> &Signal, Grid<STVector3> &Transformed)
{
    int h = Signal.Cols();

    for (int i = 0; i < Transformed.Cols(); i++)
    {
        for (int j = 0; j < Transformed.Rows(); j++)
        {
            Transformed(i, j) = Signal(i, j);
        }
    }
    
//    // Normalize
    Grid<STVector3> normalizedSignal = Signal;
//    Grid<STVector3> normalizedSignal(Signal.Rows(), Signal.Cols());
//    for (int i = 0; i < Signal.Rows(); i++)
//    {
//        std::vector<STVector3> normalizedRow;
//        for (int j = 0; j < Signal.Cols(); j++)
//        {
//            normalizedRow.push_back(Signal(i, j) / (float)h);
//        }
//        normalizedSignal.WriteRow(i, Signal.Cols(), normalizedRow);
//    }
    
    while (h > 1)
    {
        printf("Forward 2d, size: %d\n", h);
        
        for (int row = 0; row < h; row++)
        {
            std::vector<STVector3> signalRow;
            normalizedSignal.ReadRow(row, h, signalRow);
            std::vector<STVector3> transformedRow;
            Transformed.ReadRow(row, h, transformedRow);
            
//            for (int i = 0; i < signalRow.size(); i++)
//            {
//                transformedRow[i] = signalRow[i];
//            }
            
//            HaarTransformForward1D(signalRow, transformedRow);
            HaarTransformForward1DHelper(h, signalRow, transformedRow);
            Transformed.WriteRow(row, h, transformedRow);
        }
        
        for (int col = 0; col < h; col++)
        {
            std::vector<STVector3> signalCol;
            normalizedSignal.ReadColumn(col, h, signalCol);
            std::vector<STVector3> transformedCol;
            Transformed.ReadColumn(col, h, transformedCol);
            
//            for (int i = 0; i < signalCol.size(); i++)
//            {
//                transformedCol[i] = signalCol[i];
//            }
            
//            HaarTransformForward1D(signalCol, transformedCol);
            HaarTransformForward1DHelper(h, signalCol, transformedCol);
            Transformed.WriteColumn(col, h, transformedCol);
        }
        h /= 2;
    }
}

void Wavelet::HaarTransformInverse2D(const Grid<STVector3> &Transformed, Grid<STVector3> &Signal)
{
    int h = 1;
    
    for (int i = 0; i < Transformed.Cols(); i++)
    {
        for (int j = 0; j < Transformed.Rows(); j++)
        {
            Signal(i, j) = Transformed(i, j);
        }
    }
    
    //    // Normalize
    //    Grid<STVector3> normalizedSignal(Signal.Rows(), Signal.Cols());
    //    for (int i = 0; i < Signal.Rows(); i++)
    //    {
    //        std::vector<STVector3> normalizedRow;
    //        for (int j = 0; j < Signal.Cols(); j++)
    //        {
    //            normalizedRow.push_back(Signal(i, j) / (float)h);
    //        }
    //        normalizedSignal.WriteRow(i, Signal.Cols(), normalizedRow);
    //    }
    
    while (h < Signal.Cols())
    {
        for (int col = 0; col < h; col++)
        {
            std::vector<STVector3> signalCol;
            Signal.ReadColumn(col, h * 2, signalCol);
            std::vector<STVector3> transformedCol;
            Transformed.ReadColumn(col, h, transformedCol);
            
            HaarTransformInverse1DHelper(h, h + 1, transformedCol, signalCol);
            Signal.WriteColumn(col, h * 2, signalCol);
        }
        
        for (int row = 0; row < h; row++)
        {
            std::vector<STVector3> signalRow;
            Signal.ReadRow(row, h * 2, signalRow);
            std::vector<STVector3> transformedRow;
            Transformed.ReadRow(row, h, transformedRow);
        
            HaarTransformInverse1DHelper(h, h + 1, transformedRow, signalRow);
            Signal.WriteRow(row, h * 2, signalRow);
        }

        h *= 2;
    }
    printf("DONE\n");
    printGrid(Signal);
}
