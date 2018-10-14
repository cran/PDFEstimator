/* 
 * PDF Estimator:  A non-parametric probability density estimation tool based on maximum entropy
 * File:   Score.hpp
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

#ifndef SCORE_HPP
#define	SCORE_HPP

#include <fstream>
#include <vector>
#include <iostream>
#include <math.h>
#include "OutputControl.h"
#include "Partition.h"

using namespace std;

class Score {
public:    
    double targetScore;
    double minimumScore;
    double maximumScore;
    
    Score(string scoreFile, double confidenceTarget, double confidenceMin, double confidenceMax, bool boundaryPenalty);
    Score(const Score& orig);
    virtual ~Score();    
    void setFactorials (int N, int p);
    int * getIndices(){return indices;};
    double getLikelihood() {return likelihood;};
    double getConfidence(double score);
    double getPenalty() {return penaltyScore;};
    double calculateScore(double r[], int N, int p);
private:
    string  scoreFile;
    vector <double> scores;
    vector <double> SURDs;
    double likelihood;
    double * factorials;
    int    * indices;
    bool     penalty;
    double * uniformL;
    int      nEndpoints; 
    double penaltyScore;
    
    bool   readFile();
    void   getValues();
    double smartFactorial (int x);
    double factorial (int n);
    double stirlingApproxLn(double x);
    double getTargetScore(double SURD);
};

#endif	/* SCORE_HPP */

