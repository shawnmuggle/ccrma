//
//  Wavelet.h
//  Wavelet Compression
//
//  Created by Michael Rotondo on 11/17/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#ifndef Wavelet_Compression_Wavelet_h
#define Wavelet_Compression_Wavelet_h

#include "STVector3.h"
#include "Grid.h"
#include "STImage.h"

class Wavelet {
    STImage *original;
    STImage *transformed;
    STImage *reconstructed;
    STImage *difference;
    
public:
    Wavelet(const std::string &filename);
    void HaarTransformForward1D(const std::vector<STVector3> &Signal, std::vector<STVector3> &Transformed); 
    void HaarTransformForward1DHelper(size_t size, const std::vector<STVector3> &Signal, std::vector<STVector3> &Transformed, bool recurse = false);
    void HaarTransformInverse1D(const std::vector<STVector3> &Transformed, std::vector<STVector3> &Signal); 
    void HaarTransformInverse1DHelper(size_t size, size_t finalSize, const std::vector<STVector3> &Transformed, std::vector<STVector3> &Signal, bool recurse = false);
    void HaarTransformForward2D(const Grid<STVector3> &Signal, Grid<STVector3> &Transformed);
    void HaarTransformInverse2D(const Grid<STVector3> &Transformed, Grid<STVector3> &Signal);
    void Quantize(const Grid<STVector3> &ExactValues, STImage*QuantizedValues);
    void Unquantize(const STImage*QuantizedValues, Grid<STVector3> &ApproximateValues);
    float ComputeEntropy(const STImage*Image);
    float ComputePSNR();
    void ComputeDifferenceImage();
};

#endif
