/* 
 * PDF Estimator:  A non-parametric probability density estimation tool based on maximum entropy
 * File:   WriteResults->cpp
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

#include "WriteResults.h"

WriteResults::WriteResults() {
}

WriteResults::WriteResults(const WriteResults& orig) {
}

WriteResults::~WriteResults() {
}


void WriteResults::writeSolution(InputParameters *input, InputData *data, MinimizeScore *solution, int solutionNumber, int trialNumber, Score *score, bool failed) {
        
    createSolution(input, data, solution, score);
    
    ofstream outFile;
    if (input->writeFile) {
        ostringstream solutionString; 
        solutionString << trialNumber;
        ostringstream surdString; 
        surdString << input->SURDTarget;  
        string filename;
        if (input->outputFile == "") {
            if (failed) {
                if (trialNumber > 1) {
                    filename = "FAILED_PDF_" + surdString.str() + "_" + solutionString.str() + "_" + input->inputFile;
                } else {
                    filename = "FAILED_PDF_" + surdString.str() + "_" + input->inputFile;
                }
            } else {
                if (trialNumber > 1) {
                    filename = "PDF_" + surdString.str() + "_" + solutionString.str() + "_" + input->inputFile;
                } else {
                    filename = "PDF_" + surdString.str() + "_" + input->inputFile;
                }
            }
        } else {
            filename = input->outputFile;
        }
        filename = input->outputPath + filename;
        outFile.open((filename).c_str());
        if(!outFile.is_open()){
            out.print("Failed to open data file " + filename);
            return;
        }
    }
    
    if ((input->writeFile) && (input->writeHeader)) {
        if (failed) {
            outFile << "***** FAILED SOLUTION **********\n\n";
        }
#ifdef clock
        float time = solution->duration;
        outFile << "#   calculation time: " << time << " seconds\n";
        if (time >= 60) {
            outFile << "#                    (" << time/60 << " minutes)\n";
        }
        outFile << "#\n";
#endif          
        outFile << "#   total # of trials: " << trialNumber << " \n";
        outFile << "#\n";            
        outFile << "# PARAMETERS\n";            
        outFile << "#\n";            
        outFile << "#   minimum confidence:           "  << input->SURDTarget << "%\n";    
        
        outFile << "#   minimum data value:           "  << data->minimumRaw << "\n";  
        outFile << "#   maximum data value:           "  << data->maximumRaw << "\n";   
            
        if (input->lowerBoundSpecified) {
            outFile << "#   requested left boundary:      " << input->lowerBound << "\n";                
        } else {
            outFile << "#   requested left boundary:      infinite\n" ;
        }
          
        if (input->upperBoundSpecified) {
            outFile << "#   requested right boundary:      " << input->upperBound << "\n";                
        } else {
            outFile << "#   requested right boundary:     infinite\n" ;
        }
                      
        outFile << "#   right outliers removed:       "  << data->nRightOutliers << "\n";  
        outFile << "#   left outliers removed:        "  << data->nLeftOutliers << "\n";        
        outFile << "#   recalculated left boundary:   "  << data->minimumCalc << "\n";                    
        outFile << "#   recalculated right boundary:  "  << data->maximumCalc << "\n";     
        outFile << "#   number of sample points used: "  << data->N << "\n";                                 
        outFile << "#   starting partition:           "  << input->initPartitionSize << "\n";  
        outFile << "#   max lagrange :                "  << input->maxLagrange << "\n";         
        outFile << "#   attempts per step size:       "  << input->loopMax << "\n";                                        
        outFile << "#   integration points:           "  << data->nPoints << "\n";      
        outFile << "#   integration points adjusted:  "  << data->nPointsAdjust << "\n";                
        outFile << "#\n";            
            
        outFile << "# SCORES\n";                    
        outFile << "#\n";               
        outFile << "#   total score :         " << solution->bestScore << "\n";  
//        outFile << "#   penalty score:        " << score->getPenalty() << "\n";
        outFile << "#   likelihood score:     " << score->getLikelihood() << "\n";
        outFile << "#   SURD threshold:       " << score->getConfidence(score->getLikelihood()) << "%\n";
        outFile << "#\n";           
        outFile << "#\n";           
          
    }
    
    
    if ((input->writeFile) && (input->writeHeader)) {  
        outFile << "# LAGRANGE MULTIPLIERS (" << solution->mode << ")\n";            
        outFile << "#\n";            
        for (int j = 0; j < solution->mode; j++) {
            outFile << "#   " << L[j] << "\n";     
        }
        outFile << "#\n";            
        outFile << "# PLOT POINTS\n";            
        outFile << "#\n";            
        outFile << "#  x                     PDF(x)                     CDF(x) \n";             
        
     }
    
    for (unsigned int k = 0; k < PDF.size(); k++) {
        if (input->writeFile) {
            outFile << x[k] << "   " <<  PDF[k] << "   " << CDF[k] << "\n";
        }
    }
    
    if (input->writeFile) {
        outFile.close();
    }
    
}


void WriteResults::createSolution(InputParameters *input, InputData *data, MinimizeScore *solution, Score *score) {
    bool power = false;
    
    double max = data->maximumCalc;
    double min = data->minimumCalc;      
    double normFactor = 1;   
    
    double dzSize = data->nPoints;
    double dzUniform = (max - min)*1.0/dzSize;
    dzSize++;
    double * dz;
    dz = new double[(int) dzSize];
    for (int i = 0; i < dzSize; i++) {
        dz[i] = dzUniform;
    }
        
    vector <double> termsT;;         
    vector <double> termsP; 
    double p = 0;
    double termsSum = 0;     
    double z = 0;
    double q = min;
    vector <double> lagrange = solution->getLagrange();
    for (int k = 0; k < dzSize; k++) {
        z = (2*q - max - min) / (max - min);   
       
        termsT.clear();
        termsT.push_back(1.0);
        termsT.push_back(z);
        
        p = lagrange[0];
        if (power) {
            p = 1.0 / pow(q, lagrange[1]); 
        } else {
            for (int t = 1; t < solution->mode; t++) {                
                p += termsT[t] * lagrange[t];
                termsT.push_back(2*z*termsT[t] - termsT[t-1]);
            }  
            p = exp(p);// + solution->normalize);
            p /= (max - min)/2;
        }
        termsP.push_back(p);
        termsSum += p * dz[k];
        q += dz[k];          
    }    
    termsSum /= normFactor;
    
    double lambdaZero =  -log(termsSum);//solution->normalize;//    
    
    L.push_back(lambdaZero);
    for (int j = 1; j < solution->mode; j++) {   
        L.push_back(lagrange[j]);
        out.print("Lagrange   ", L[j]);
    }
         
    for (int k = 0; k < dzSize; k++) {
        double pk = termsP[k] / termsSum;
        termsP[k] = pk;
        if (input->symmetry) {
            termsP[k] = pk/2.0;
        }
    }        
    vector <double> pdf;                      
    vector <double> dzBig; 
    termsSum = 0;
    int count = 0;
    if (input->symmetry) {
        for (int k = (dzSize - 1); k > 0; k--) {                
            termsSum += termsP[k]*dz[k];
            CDF.push_back(termsSum);           
            dzBig.push_back(dz[k]);
            pdf.push_back(termsP[k]);      
            count++;
        }            
    }
    for (int k = 0; k < dzSize; k++) {                
        termsSum += termsP[k]*dz[k];
        CDF.push_back(termsSum);           
        dzBig.push_back(dz[k]);
        pdf.push_back(termsP[k]);       
        count++;
    }   
                       
    q = min;   
    if (input->symmetry) {
        q = -max;
    }
    for (int k = 0; k < count; k++) {
        x.push_back(q);
        PDF.push_back(pdf[k]);
        if (k < count) {
            q += dzBig[k];
        }
    }    
    delete [] dz;
}



void WriteResults::writeColumn(string filename, vector <double> r, int length) {   
    ofstream outFile;
    outFile.open(filename.c_str());
    for (int i = 0; i < length; i++) {
        outFile << r[i] <<  "\n";
    }
    outFile.close(); 
}

void WriteResults::writeColumn(string filename, vector <int> r, int length) {   
    ofstream outFile;
    outFile.open(filename.c_str());
    for (int i = 0; i < length; i++) {
        outFile << r[i] <<  "\n";
    }
    outFile.close(); 
}

void WriteResults::writeColumn(string filename, double r[], int length) {
    ofstream outFile;
    outFile.open(filename.c_str());
    for (int i = 0; i < length; i++) {
        outFile << r[i] <<  "\n";
    }
    outFile.close();
}

void WriteResults::writeColumn(string filename, int r[], int length) {
    ofstream outFile;
    outFile.open(filename.c_str());
    for (int i = 0; i < length; i++) {
        outFile << r[i] <<  "\n";
    }
    outFile.close();
}

void WriteResults::writeQQ(string filename, double r[], int length, bool sqr) {
    ofstream outFile;
    outFile.open(filename.c_str());
    if(!outFile.is_open()){
        out.print("Failed to open data file " + filename);
        return;
    }
   
    for (int i = 0; i < length; i++) {
        double position = (i + 1) * 1.0 / (length + 1);
        if (sqr) {
            SQR.push_back(sqrt(length + 2) * (r[i] - position));
            outFile << setprecision(12) << position << " " << SQR[i] <<  "\n";
        } else {
            outFile << setprecision(12) << position << " " << r[i] <<  "\n";
        }
    }    
    outFile.close();
}

void WriteResults::createQQ(double r[], int length) {
   
    for (int i = 0; i < length; i++) {
        double position = (i + 1) * 1.0 / (length + 1);
        SQR.push_back(sqrt(length + 2) * (r[i] - position));
    }    
}
