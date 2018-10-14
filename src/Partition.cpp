/* 
 * PDF Estimator:  A non-parametric probability density estimation tool based on maximum entropy
 * File:   Partition.cpp
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

#include "Partition.h"

Partition::Partition() {
}

Partition::Partition(const Partition& orig) {
}

Partition::~Partition() {
}

vector <int> Partition::getIndices(int N, int p) {
    vector <int> indices;
    indices.reserve(p);
    if (N==p) {
        for (int i = 0; i < p; i++) {
            indices[i] = i;
        }
    } else {
        indices[0] = 0;
        double div = N*1.0/(p - 1);
        double index = div - 1;
        for (int i = 1; i < p; i++) {
            indices[i] = round(index);
            index += div;
        }
        indices[p-1] = N - 1;
    }
    return indices;
}

/*
vector <int> Partition::getIndices(int N, int p) {
    vector <int> indices;
    indices.reserve(p);
    if (N==p) {
        for (int i = 0; i < p; i++) {
            indices[i] = i;
        }
    } else {
        indices[0] = 0;
        int div = N/(p - 1);
        int index = div - 1;
        for (int i = 1; i < p; i++) {
            indices[i] = index;
            index += div;
        }
    }
    return indices;
}

*/
//vector <int>  Partition::calculateIndices (int totalSize, int subsetSize) {

/*vector <int>  Partition::getIndices (int totalSize, int subsetSize) {
      
    vector <int> partitionIndices;  
       
    if (subsetSize > totalSize) {
        subsetSize = totalSize;
    }
    if (subsetSize != totalSize) {
        subsetSize += 2;
    }              
    if (subsetSize == totalSize) {
        for (int p = 0; p < totalSize; p++) {
            partitionIndices.push_back(p);
        }
    } else {
        partitionIndices.push_back(0);
        partitionIndices.push_back(totalSize-1);
        subsetSize -= 2;           
        partitionIndices = getDataSubset(0, totalSize-1, subsetSize-2, partitionIndices);  
        sort(partitionIndices.begin(), partitionIndices.end());
    }
    //return &partitionIndices[0];
        return partitionIndices;
 }
*/
 
vector <int> Partition::getDataSubset(int lo, int hi, int size, vector <int> indices) {
    int size1 = 0,
    size2 = 0;
    double midPoint = (hi + lo)/2.0;// - 0.000000001; 
    
    midPoint = round(midPoint); 
    int iMidPoint = (int) midPoint;
    if ((iMidPoint == lo) || (iMidPoint == hi)) {
#ifndef R
        cout << "Error getting subset:  MidPoint is on Boundary\n";
#endif
        return indices;
    }
//    cout << "midpoint=" << iMidPoint << "\n";
    indices.push_back(iMidPoint);
    if (size == 1) {
        return indices;
    }
    if (size == 0) {
//        cout << "Error getting subset:  Size is Zero\n";
    }
    size--;
    if ((midPoint - iMidPoint) >= 0.5) {
        size1 = ceil(size/2.0);
        size2 = floor(size/2.0);
    } else {
        size1 = floor(size/2.0);
        size2 = ceil(size/2.0);
    }
    if (size1 != 0) {
        indices = getDataSubset(lo, iMidPoint, size1, indices);
    }
    if (size2 != 0) {
        indices = getDataSubset((int) iMidPoint, hi, size2, indices);
    }
    return indices;
 }
