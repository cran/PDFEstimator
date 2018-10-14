/* 
 * PDF Estimator:  A non-parametric probability density estimation tool based on maximum entropy
 * File:   inputData.cpp
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

#include <numeric>

#include "InputData.h"
#include "WriteResults.h"


InputData::InputData(const InputParameters& input) {
    this->input = input;
    
    nRightOutliers = 0;
    nLeftOutliers = 0;
    
    leftOutliers = false;
    rightOutliers = false;
    useLast = false;
    y2 = 0;    
}

InputData::InputData(const InputData& orig) {
}

InputData::~InputData() {
   
}

bool InputData::readData() {
    
    ifstream fin;
    string line;
    fin.open((input.inputPath + input.inputFile).c_str());

    if(!fin.is_open()){
#ifndef R
        cout << "Failed to open data file " <<  input.inputFile.c_str() << "\n";
#endif
        return false;
    }
	
    while (getline(fin, line)) {
        double test = atof(line.c_str());
        rawData.push_back(test);
    }
    if (rawData.size() == 0) {
#ifndef R
        cout << "No data in " <<  input.inputFile.c_str() << "\n";
#endif
        return false;        
    }
   
    fin.close();   
    processData();
    return true;
}
    
void InputData::setData(vector<double> data) {
    rawData.reserve(data.size());
    rawData = data;
}

void InputData::processData() {   
    nPoints = input.integrationPoints;
    if (nPoints == -1) {
        nPoints = floor(200 + rawData.size()/200.0);
        if (nPoints > 1500) nPoints = 1500;
    }
    
    sort(rawData.begin(), rawData.end());
    minimumRaw = rawData[0];
    maximumRaw = rawData[rawData.size() - 1];
    identifyOutliers();
    transformData();  
    setAdaptiveDz();  
    cheby.initialize(doubleInverse, 2*nPointsAdjust-1);
    return;
}

void InputData::fuzzData() {
    int growthFactor = 1;
    int countFuzz = 0;
    int flagFuzz = 1;  //jacobs - 15??                
    double ratio_0 = 1.0e-10;
    int nScramble = 0;       
    int binNs = 101;
    
    N = rawData.size();
    
    while (flagFuzz > 0) { 
        vector <double> dx;
        int k1;
        int k2;
        if (N == 2*floor(N/2)) {
            k1 = N/2;
            k2 = k1 + 1;
        } else {
            k1 = (N-1)/2;
            k2 = k1 + 2;
            int kmid = k1 + 1;
            dx.push_back((rawData[k2] - rawData[k1]) / 2);
        }
        
        for (int k = 0; k < k1; k++) {
            dx.push_back(rawData[k+1] - rawData[k]);
            
        }
        for (int k = k2; k < N; k++) {
            dx.push_back(rawData[k] - rawData[k-1]);
        }
       
        double t1 = *min_element(dx.begin(), dx.end());
        double t2 = *max_element(dx.begin(), dx.end());
        double ratio = *min_element(dx.begin(), dx.end()) / (*max_element(dx.begin(), dx.end()) + 1.0e-40);  
        if (ratio > ratio_0/N) {                    
            break;                  
        } else {       
            double dxMin;
            ratio_0 = ratio_0/2;
            countFuzz = countFuzz + 1;
            double minNdx = round(0.75 * binNs) - 1; 
            vector <double> dxUnique = dx;
            sort(dxUnique.begin(), dxUnique.end());
            std::vector<double>::iterator it;
            
            it = unique(dxUnique.begin(), dxUnique.end());
            dxUnique.resize(distance(dxUnique.begin(), it));
            double nUnique = dxUnique.size();
            double k = min(nUnique, minNdx);
            if (k < 2) {
                dxMin = 0.01 * (*max_element(rawData.begin(), rawData.end()) - *min_element(rawData.begin(), rawData.end()));
                dxMin = max(dxMin, 1.0e-9);
            } else { 
                double avg = accumulate(dxUnique.begin() + 1, dxUnique.begin() + k, 0.0);
                dxMin = 0.01 * accumulate(dxUnique.begin() + 1, dxUnique.begin() + k, 0.0) / (k - 1.0); 
            }
            dxMin = growthFactor * dxMin;
            nScramble = 0;
            for (int k = 1; k <= N; k++) {
                if (dx[k] < dxMin) {
                    rawData[k] = rawData[k] + dxMin * random();
                    nScramble = nScramble + 1;
                }
            }                  
            sort(rawData.begin(), rawData.end());
            flagFuzz = flagFuzz - 1;
            growthFactor = 2*growthFactor;
        }    
    }
    WriteResults write;
    write.writeColumn("Fuzzed_" + input.inputFile, rawData, rawData.size());
}

double InputData::random() {
    double x1, x2, w, y1;   
    if (useLast) {
        y1 = y2;
        useLast = false;
    } else {
        do {                                    
#ifdef R
            x1 = 2.0 * ranX() - 1;
            x2 = 2.0 * ranX() - 1;
#else
            x1 = 2*(1.0*rand()/RAND_MAX) - 1;
            x2 = 2*(1.0*rand()/RAND_MAX) - 1;
#endif
            w = x1 * x1 + x2 * x2;
        } while ( w >= 1.0 );

        w = sqrt((-2.0 * log(w))/w);
        y1 = x1 * w;
        y2 = x2 * w;
        useLast = true;
    }
    return(y1);
}



  void InputData::identifyOutliers() {
         
        double q1 = 0;
        double q2 = 0;
        double q3 = 0;      
        
        int nValues = rawData.size();    
        
        int middle = floor(nValues/2);
        int quarter = floor(middle/2);
        
        if (nValues%2 == 0) {
            q2 = (rawData[(int)middle - 1] + rawData[(int)middle + 1])/2;
            if (middle%2 == 0) {
                q1 = (rawData[(int)quarter - 1] + rawData[(int)quarter])/2;
                q3 = (rawData[(int)quarter + (int)middle - 1] + rawData[(int)quarter + (int)middle])/2;
            } else {
                q1 = rawData[(int)quarter];
                q3 = rawData[(int)quarter + (int)middle];
            }
        } else {
            q2 = rawData[(int)quarter];
            if (middle%2 == 0) {
                q1 = (rawData[(int)quarter - 1] + rawData[(int)quarter])/2;
                q3 = (rawData[(int)quarter + (int)middle] + rawData[(int)quarter + (int)middle + 1])/2;
            } else {
                q1 = rawData[(int)quarter];
                q3 = rawData[(int)quarter + (int) middle] + 1;
            }
        }               
        double iqr = input.outlierCutoff*(q3 - q1);
        double leftOutlier = q1 - iqr;
        double rightOutlier = q3 + iqr;                   
        
         if (input.upperBoundSpecified) {  
            maximumCalc = input.upperBound;
        } else {
            double max = rawData[nValues - 1];
            maximumCalc = max + (max - rawData[rawData.size() - 5]);
            if (maximumCalc > rightOutlier) {
                maximumCalc = rightOutlier;
                rightOutliers = true;                
            }
        }
    
        if (input.lowerBoundSpecified) {  
            minimumCalc = input.lowerBound;
        } else {
            double min = rawData[0];
            minimumCalc = min + (min - rawData[4]);
            if (minimumCalc < leftOutlier) {
                minimumCalc = leftOutlier;
                leftOutliers = true;                
            }
        }
  }
    


void InputData::transformData() {                    
    
    
    int nValues = rawData.size();     
    
    if (input.fuzz) {
        fuzzData();
    }
        
    for (vector<double>::iterator iter = rawData.begin(); iter != rawData.end(); ++iter) {
        if (*iter >= minimumCalc) {
            if (*iter <= maximumCalc) {
                tempData.push_back(*iter);
            } else {
                nRightOutliers++;
            }
        } else {
            nLeftOutliers++;
        }            
    }
        
    nValues = tempData.size(); 
    if (nValues == 0) {
#ifndef R
        cout << "CRITICAL ERROR:  no data!\n";
#endif
        return;
    }
    
    
    transformedData.clear();      
    transformedData.reserve(nValues);
    transformedZeroOne = new double[nValues];
    int count = 0;
    for (vector<double>::iterator iter = tempData.begin(); iter != tempData.end(); ++iter) {
        transformedData.push_back((2*(*iter) - maximumCalc - minimumCalc)/(maximumCalc - minimumCalc));
        transformedZeroOne[count] = (transformedData[count] + 1)/2.0;
        count++;    
    }
}


void InputData::setAdaptiveDz() {
   
    vector <double> dzVector;   
    N = transformedData.size();
     
    double dzMax = 2.0/(nPoints - 1);
        
    int skip = floor(N/(nPoints - 1));
    if (skip==0) skip = 1;
                        
    double last = -1.0;
    double next;
        
    for (int b = skip; b <= (N + skip); b+=skip) {
        if (b >= (N)) {
            next = transformedData[N-1];
        }
        else {
            next = transformedData[b];
        }
        double test = next - last;
        double difference = fabs(test);
        if (difference > dzMax) {
            double steps =  difference/dzMax;
            int iSteps = (int) steps;
            for (int k = 0; k < (iSteps + 1); k++) {
                dzVector.push_back(difference/(iSteps + 1));
            }
        }
        else {             
            dzVector.push_back(difference); 
        }            
        last = next;
    }            
        
    int dzSize = dzVector.size();
    inverse = new double[dzSize + 1];
    inverse[0] = 0;
    for (int j = 1; j <= dzSize; j++) {
        inverse[j] = inverse[j-1] + dzVector[j-1]/2.0;
    }

    double difference = 1.0 - inverse[dzSize];
    if (difference > dzMax) {
        double steps =  (difference)/dzMax;
        int iSteps = (int) steps;
        for (int k = 0; k < 2*(iSteps + 1); k++) {
            dzVector.push_back(difference/(iSteps + 1));
        }        
    }
    else {
        dzVector.push_back(difference); 
    }
    dzSize = dzVector.size();
    dz = new double[dzSize];
    inverse = new double[dzSize];
    doubleInverse = new double[2*dzSize - 1];
    inverse[0] = dzVector[0]/2.0;
    dz[0] = dzVector[0]/2.0;
    int count = 0;
    for (int j = 1; j < dzSize; j++) {
        dz[j] = dzVector[j]/2.0;
        inverse[j] = inverse[j-1] + dzVector[j]/2.0;
        doubleInverse[count] = inverse[j-1];
        doubleInverse[count+1] = (inverse[j-1] + inverse[j])/2.0;
        count += 2;
    }        
    doubleInverse[count] = (inverse[dzSize-1] + 1.0)/2.0;
    
    xUntransform = new double[2*dzSize - 1];
        
    for (int i=0; i < 2*dzSize - 1; i++) {
        xUntransform[i] = doubleInverse[i]*2.0 - 1;
    }
    for (int i=0; i < 2*dzSize - 1; i++) {
        xUntransform[i] = (maximumCalc - minimumCalc)*xUntransform[i] + minimumCalc + maximumCalc;
        xUntransform[i] /= 2;
    }
        
    
    nPointsAdjust = dzSize;        
}
    
    

double InputData::ranX() {
    double x;
    seed=seed*1566083941 + 1;
    x=seed*2.328306e-10 + 0.5;
    
    return x;
}  
