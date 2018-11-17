/* 
 * PDF Estimator:  A non-parametric probability density estimation tool based on maximum entropy
 * File:   MinimizeScore.cpp
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

#include "MinimizeScore.h"
#include "WriteResults.h"

//#define POWER

MinimizeScore::MinimizeScore() {
    
    normalize = 0;    
    useLast = false;
    y2 = 0;
    seed = 12345678;
    nPoints = 0;
    N = 0;
    maxLagrange = 0; 
    mode = 0; 
    duration = 0; 
    bestScore = 0; 
}

MinimizeScore::MinimizeScore(const MinimizeScore& orig) {
}

MinimizeScore::~MinimizeScore() {
     delete [] trialRandom;
     delete [] bestLagrange;
}

vector <double> MinimizeScore::getLagrange() {
    vector <double> lagrange;
    for (int i = 0; i < maxLagrange; i++) {
        lagrange.push_back(bestLagrange[i]);
    }
    return lagrange;
}

bool MinimizeScore::minimize(InputParameters *input, const InputData& data, Score& score) {         
    
#ifdef debug
    clock_t algorithmTime;                                                  
    algorithmTime = clock();
#endif
  
    int minLagrange = input->minLagrange;
    maxLagrange = input->maxLagrange;
    int nLagrangeAdd = input->nLagrangeAdd;
    double fractionLagrangeAdd = input->fractionLagrangeAdd;
    int loopMax = input->loopMax;
    double initSigma = input->initSigma;
    double finalSigma = input->finalSigma;
    double decayFactor = input->decayFactor;   
    int partitionSize = input->initPartitionSize;
    int targetPartition = partitionSize;
    bool incPartition = false;
      
    this->inverse = data.inverse;
    this->dz = data.dz;
    this->doubleInverse = data.doubleInverse;
    this->xUntransform = data.xUntransform;
    double * transformedZeroOne = data.transformedZeroOne;
    this->cheby = data.cheby;
    this->nPoints = data.nPointsAdjust;
    this->N = data.N;
    if (partitionSize > N) {
        partitionSize = N;
        targetPartition = N;
    }
    if (partitionSize == 0) {
        partitionSize = N;
        targetPartition = N;
    }      
    
    int loopCount = 0;
    mode = minLagrange;
    int inc;
    double sigmaFactor = sqrt(partitionSize*1.0/targetPartition)*sqrt(1.0/mode); 
    double originalFinalSigma = finalSigma;
    double originalInitSigma = initSigma;
    double currentSigma = initSigma*sigmaFactor;
    finalSigma *= sigmaFactor;
    initSigma *= sigmaFactor;
    double * trialLagrange = new double[maxLagrange]; 
    bestLagrange = new double[maxLagrange];  
    
    for (int i = 0; i < minLagrange; i++) {
        trialLagrange[i] = 0;
        bestLagrange[i] = 0;
    }    

    T = cheby.getAllTerms(maxLagrange);
       
    double lastLagrangeScore = 0;
    int lagrangeAddCount = 0;
    
    trialRandom = new double[N];
    bestRandom = new double [N];    
    rawDataPartition = new double [N];
    
    for (int c = 0; c < N; c++) {
        trialRandom[c] = 0;
        bestRandom[c] = 0;    
        rawDataPartition[c] = 0;
    
    }
    
    double trialScore = 0;
    bestScore = -numeric_limits<double>::max();
    double targetScore = score.targetScore;
    double minimumScore = score.minimumScore;   
    double maximumScore = score.maximumScore;    
    
    bool funnelFinished = false;
    bool solutionNotFound = false;
        
       
    vector <int> indices = Partition::getIndices(N, partitionSize);  
    for (int i = 0; i < partitionSize; i++) {
        rawDataPartition[i] = transformedZeroOne[indices[i]]; 
    }
    
    score.setFactorials(partitionSize, partitionSize);   
    double * cdf;
    cdf = new double[nPoints];
#ifdef POWER
    calculatePDF(cdf, 0);                             
#else
    calculatePDF(cdf, trialLagrange, mode);                          
#endif
    map(trialRandom, cdf, rawDataPartition, partitionSize);
    bool continueLooking = true;       
    
    bestScore = score.calculateScore(trialRandom, targetPartition, partitionSize);   
    out.print("initial score", bestScore);
    out.print("partition size", partitionSize);
    out.print("target size", targetPartition);
    
    if (bestScore > targetScore) {                        
        continueLooking = false;       
    }
    else if (mode == 1) {
        if (maxLagrange > 1) {
            trialLagrange[0] = 0;
            bestLagrange[0] = 0;
            trialLagrange[1] = 0;
            bestLagrange[1] = 0;
            mode = 2;
        } else {           
            out.print("*Maximum number of lagrange multipliers exceeded", maxLagrange);
            return true;
        }
    }
    
#ifdef POWER
    bestLagrange[1] = 1.0;                                                    
    trialLagrange[1] = bestLagrange[1];
    trialLagrange[1] = funnelDiffusion(bestLagrange[1], currentSigma);   
#else    
    funnelDiffusion(bestLagrange, trialLagrange, mode, currentSigma);   
#endif
    
    bestRandom[0] = trialRandom[0];         
    while (partitionSize <= N) {    
        while (continueLooking) {
            loopCount++;     
            delete [] cdf;
            cdf = new double[nPoints];
#ifdef POWER
            calculatePDF(cdf, trialLagrange[1]);       
#else
            calculatePDF(cdf, trialLagrange, mode);                       
#endif
            map(trialRandom, cdf, rawDataPartition, partitionSize);
                        
            trialScore = score.calculateScore(trialRandom, targetPartition, partitionSize);    
            if (trialScore > bestScore) {
#ifdef debug
                ostringstream strOut;
                strOut << "SURD score: " << score.getLikelihood() << ";  total score: " << trialScore << ";  partition size: " << partitionSize << "; target:  " << targetPartition;                                      
                out.print(strOut.str());
#endif
                if (score.getLikelihood() < maximumScore) {
                    bestScore = trialScore;     
                }
                for (int k = 0; k < mode; k++) {
                    bestLagrange[k] = trialLagrange[k];
                }
                bestRandom[0] = trialRandom[0];
                if ((score.getLikelihood() > targetScore) && (score.getLikelihood() < maximumScore)) { 
                    out.print("*Solution found");
                    break;       
                }    
            }
            if (loopCount > loopMax) {          
                currentSigma /= decayFactor;
                if (currentSigma < finalSigma) funnelFinished = true;
                loopCount = 0;
            }
            if (funnelFinished) {
                if (mode < 5) inc = 1;
                else inc = 2;
                mode += inc;    
                if (mode > maxLagrange) {            
                    if (score.getLikelihood() > minimumScore) {
                        out.print("*Lower threshold accepted", score.getLikelihood());                            
                    } else {
                        out.print("*Maximum number of lagrange multipliers exceeded", maxLagrange);
                        solutionNotFound = true;
                        continueLooking = false;
                    }
                    mode -= inc;
                    break;
                }   
                out.print("*Adding lagrange", mode);
                for (int i = (mode-inc); i < mode; i++) {
                    trialLagrange[i] = 0;
                    bestLagrange[i] = 0;
                }
                funnelFinished = false;                   
                   
                if (fabs(bestScore-lastLagrangeScore)/fabs(lastLagrangeScore) < fractionLagrangeAdd) { 
                    if (lagrangeAddCount > nLagrangeAdd) { 
                        out.print("*Improvement not found in required number of attempts");
                        if ((score.getLikelihood() > minimumScore) && (score.getLikelihood() < maximumScore)) {
                            out.print("*Lower threshold accepted", score.getLikelihood());
                            targetScore = score.getLikelihood();
                        } else {
                            solutionNotFound = true;
                        }
                        break;
                    } else {
                        lagrangeAddCount++;   
                    }
                } else {
                    lastLagrangeScore = bestScore;
                    lagrangeAddCount = 0;
                }                  
                currentSigma = initSigma;
#ifdef POWER
                trialLagrange[1] = funnelDiffusion(bestLagrange[1], currentSigma);  
#else
                funnelDiffusion(bestLagrange, trialLagrange, mode, currentSigma, 1); 
#endif
            } else {
                
#ifdef POWER
                trialLagrange[1] = funnelDiffusion(bestLagrange[1], currentSigma);   
#else
                funnelDiffusion(bestLagrange, trialLagrange, mode, currentSigma, 1);   
#endif
            }                 
        }   
         
        if (!continueLooking) {
            break;
        }
        if (incPartition) {
            if (partitionSize == N) break;
            partitionSize = (partitionSize - 1) * 2 + 1;
            if (partitionSize > N) partitionSize = N;  
            vector <int> indices = Partition::getIndices(N, partitionSize);  
            for (int i = 0; i < partitionSize; i++) {
                rawDataPartition[i] = transformedZeroOne[indices[i]]; 
            }
            score.setFactorials(targetPartition, partitionSize); 
            bestScore = -numeric_limits<double>::max();
        } else {
            if (partitionSize == N) break; //added 11/3/2018
            targetPartition = (targetPartition - 1) * 2 + 1;
            if (targetPartition >= N) {
                targetPartition = N;
                incPartition = true;
            }
            score.setFactorials(targetPartition, partitionSize);             
            bestScore = -numeric_limits<double>::max();
        }     
                                      
        double sigmaFactor = sqrt(partitionSize*1.0/targetPartition)*sqrt(1.0/mode);       
        initSigma = originalInitSigma*sigmaFactor;
        finalSigma = originalFinalSigma*sigmaFactor;
        currentSigma = initSigma;
    }  
        
#ifdef debug
    algorithmTime = clock() - algorithmTime;  
    duration = ((float) algorithmTime)/CLOCKS_PER_SEC;
#endif
    
    if (input->writeQQ) {
        WriteResults write;
        string filename;
        filename = input->outputPath + input->qqFile;
        write.writeQQ(filename, trialRandom, partitionSize, false);
    }
    if (input->writeSQR) {
        WriteResults write; 
        string filename;
        filename = input->outputPath  + input->sqrFile;
        write.writeQQ(filename, trialRandom, partitionSize, true);
    }
    delete [] trialLagrange;
    delete [] bestRandom;
    delete [] rawDataPartition;
    delete [] cdf;
    

#ifdef POWER   
    out.print("power exponent", bestLagrange[1]);
#else
    out.print("Normalize", normalize);
#endif

    return solutionNotFound;
 
}

void MinimizeScore::calculatePDF(double cdf[], double power) {
    int pdfPoints = nPoints*2 - 1;   
    double * pdf;  
    pdf = new double[pdfPoints];
    double * x;
    x = new double[pdfPoints];
    for (int i = 0; i < pdfPoints; i++) {
        x[i] = 0;
    }
 
    for (int k = 0; k < pdfPoints; k++) { 
        pdf[k] = 1.0 / pow(xUntransform[k], power);
    }          
    
    
    double * max = max_element(x, x + pdfPoints);
    normalize = -*max;  
    
    int count = 1;             
    cdf[0] = (pdf[0])*dz[0]/2;
             
    for (int k = 1; k < nPoints; k++) {
        cdf[k] = cdf[k-1] + (pdf[count-1] + 4*pdf[count] + pdf[count+1])*dz[k-1]/6.0;                 
        count += 2;
    }   
        
    double constant = cdf[nPoints - 1];
    normalize = constant;
    for (int k = 0; k < nPoints; k++) {                      
        cdf[k] /= constant;
    }
    
    delete [] x;
    delete [] pdf;    
    
}

void MinimizeScore::calculatePDF (double cdf[], double lagrange[], int modes) {     
        
    int pdfPoints = nPoints*2 - 1;   
    double * pdf;  
    pdf = new double[pdfPoints];
    double * x;
    x = new double[pdfPoints];
    for (int i = 0; i < pdfPoints; i++) {
        x[i] = 0;
    }
    
    for (int k = 0; k < pdfPoints; k++) { 
        for (int n = 0; n < modes; n++) {
            x[k] += lagrange[n]*T[n][k];
        }
        pdf[k] = exp(x[k] + normalize);
    }              
    
    double * max = max_element(x, x + pdfPoints);
    normalize = -*max;  
    
    int count = 1;             
    cdf[0] = (pdf[0])*dz[0]/2;
             
    for (int k = 1; k < nPoints; k++) {
        cdf[k] = cdf[k-1] + (pdf[count-1] + 4*pdf[count] + pdf[count+1])*dz[k-1]/6.0;                 
        count += 2;
    }   
        
    double constant = cdf[nPoints - 1];
    for (int k = 0; k < nPoints; k++) {                      
        cdf[k] /= constant;
    }
        
    delete [] x;
    delete [] pdf;
}



void MinimizeScore::map (double r[], double cdf[], double rawDataPartition[], int partitionSize) {        
    double z1;
    double z2;
    double zCalc1;
    double endpointCDF;       
    int startPoint = 0;
    
    for (int k = 0; k < partitionSize; k++) {                
        int j = startPoint;
        for (j = startPoint; j < nPoints; j++) {
            if (rawDataPartition[k] < inverse[j])
                break;
        }
        startPoint = j;
        if (j==0) {
            z1 = 0;
        } else {                                
            z1 = inverse[j-1];
        }
                 
        if (j >= nPoints) {     
            z2 = 1.0;
            endpointCDF = 1.0;
        } else {
            z2 = inverse[j];
            endpointCDF = cdf[j];
        }                             
        zCalc1 = (rawDataPartition[k] - z1)/(z2 - z1);
        if (j==0) {
            r[k] = zCalc1*(endpointCDF);     
        } else {                    
            r[k] = cdf[j-1] + zCalc1*(endpointCDF - cdf[j-1]);     
        }       
        if (r[k] < 0 ) {
//            cout << "ERROR: random number is negative\n";
        }
    }
}           


void MinimizeScore::funnelDiffusion(double original[], double updated[], int arraySize, double currentSigmaMu, int startIndex) {
    for (int j=startIndex; j < arraySize; j++) {
        updated[j] = random(original[j], (currentSigmaMu*(0.1 * fabs(original[j]) + 1.0)/2));
    }       
}

double MinimizeScore::funnelDiffusion(double original, double currentSigmaMu) {
    return random(original, (currentSigmaMu*(0.1 * fabs(original + 1.0)/2)));
}

void MinimizeScore::funnelDiffusion(double original[], double updated[], int arraySize, double currentSigmaMu) {
    funnelDiffusion(original, updated, arraySize, currentSigmaMu, 1);     
}

double MinimizeScore::random(double m, double s) {
    double x1, x2, w = 2, y1;

    
    if (useLast) {
        y1 = y2;
        useLast = false;
    } else {
        do {  
            x1 = 2.0 * ranX() - 1;
            x2 = 2.0 * ranX() - 1;
            w = x1 * x1 + x2 * x2;
        } while ( w >= 1.0 );

        w = sqrt((-2.0 * log(w))/w);
        y1 = x1 * w;
        y2 = x2 * w;
        useLast = true;
    }
    return(m + y1 * s);
}

double MinimizeScore::ranX() {
    double x;
    seed=seed*1566083941 + 1;
    x=seed*2.328306e-10 + 0.5;
    
    return x;
}  
