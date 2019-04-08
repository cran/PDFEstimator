/* 
 * File:   callPDF.cpp
 * Author: jenny
 * 
 * Created on September 18, 2018, 1:20 PM
 */

#include "callPDF.h"

callPDF::callPDF() {
}

callPDF::callPDF(const callPDF& orig) {
}

callPDF::~callPDF() {
}

void callPDF::makeCall(int sampleLength, double * sampleData, double low, double high, int isLow, int isHigh, int points) {
    InputParameters *input = new InputParameters;
    if (isLow) {
        input->lowerBoundSpecified = true;
        input->lowerBound = low;
    } else {
         input->lowerBoundSpecified = false;
    }
    if (isHigh) {
        input->upperBoundSpecified = true;
        input->upperBound = high;
    } else {
         input->upperBoundSpecified = false;
    }
    
    input->integrationPoints = points - 1;
   
    Score *score = new Score(input->SURDTarget, input->SURDMinimum, input->SURDMaximum);    
    MinimizeScore *minimumPDF = new MinimizeScore();
    InputData *data = new InputData(*input);  
    vector <double> inputData;
    for (int i = 0; i < sampleLength; i++) {
        inputData.push_back(sampleData[i]);
    }
    input->writeHeader = false;
    input->writeFile = false;
    data->setData(inputData);     
    if (data->processData()) {        
        solutionFailed = minimumPDF->minimize(input, *data, *score);
        WriteResults write;        
        write.createSolution(input, data, minimumPDF, score);  

        if (!solutionFailed) {
            write.createQQ(minimumPDF->trialRandom, data->N);   
            Vsqr = write.SQR;
        }
        Vcdf = write.CDF;
        Vpdf = write.PDF;
        Vx   = write.x;     
        delete data;
        delete score;
        delete minimumPDF;
    } else {
        solutionFailed = true;
    }
    
    delete input;
}

