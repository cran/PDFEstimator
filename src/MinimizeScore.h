/* 
 * PDF Estimator:  A non-parametric probability density estimation tool based on maximum entropy
 * File:   MinimizeScore.hpp
 * Copyright (C) 2018
 * Jenny Farmer jfarmer6@uncc.edu
 * Donald Jacobs djacobs1@uncc.edu
 * 
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. This program is distributed in 
 * the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with 
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MINIMIZESCORE_HPP
#define	MINIMIZESCORE_HPP

#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <string.h>

#include "ChebyShev.h"
#include "InputData.h"
#include "Score.h"
#include "Partition.h"
#include "OutputControl.h"
#include <limits>


using namespace std;

class MinimizeScore {
public:
    MinimizeScore();
    MinimizeScore(const MinimizeScore& orig);
    virtual ~MinimizeScore();
    bool minimize(InputParameters *input, const InputData& data, Score& score);
    vector <double> getLagrange();
    
    int    mode;
    float duration;
    double bestScore;
    double * bestLagrange;
    double  normalize;
    double * trialRandom;
private:
    int maxLagrange;
    int seed;
    bool useLast;
    double y2;    
    ChebyShev cheby;
    double * inverse;
    double * z;
    double * dz;    
    double * doubleInverse;
    double * xUntransform;
    int     nPoints;
    int     N;    
    double * bestRandom;
    double * rawDataPartition;
    vector < vector < double > > T;
    
    void funnelDiffusion(double * original, double * updated, int arraySize, double currentSigmaMu);
    void funnelDiffusion(double * original, double * updated, int arraySize, double currentSigmaMu, int startIndex);
    double funnelDiffusion(double original, double currentSigmaMu);
    double random(double m, double s);
    double ranX();
    void map(double r[], double cdf[], double rawDataPartition[], int partitionSize);
    void calculatePDF (double cdf[], double lagrange[], int modes);//, double ** Tarray);
    void calculatePDF (double cdf[], double power);
};

#endif	/* MINIMIZESCORE_HPP */

