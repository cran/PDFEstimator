/* 
 * File:   Score.hpp
 * Author: jenny
 *
 * Created on February 8, 2019, 8:33 PM
 */

#ifndef SCORE_HPP
#define	SCORE_HPP

#include <fstream>
#include <vector>
#include <iostream>
#include "Partition.h"
#include <math.h>

class Score {
public:
    double targetScore;
    double minimumScore;
    double maximumScore;
    
    Score() {};
    Score(const Score& orig) {};
    Score(double confidenceTarget, double confidenceMin, double confidenceMax); 
    virtual ~Score();
    
    double getLikelihood() {return likelihood;};
    int * getIndices(){return indices;}
    void setVarianceMin(bool qzVar) {minimizeVariance = qzVar;}
    void setIndices (int N, int p);
    double getConfidence(double score);    
    double calculateScore(double r[], int N, int p);
    double SURD;    
    double QZVariance;
    
private:
    vector <double> scores;
    vector <double> SURDs;
    double likelihood;
    int    * indices;
    
    bool minimizeVariance;
    
    double getTargetScore(double SURD);  
    void getValues();
    
};

#endif	/* SCORE_HPP */

