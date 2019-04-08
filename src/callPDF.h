/* 
 * File:   callPDF.hpp
 * Author: jenny
 *
 * Created on September 18, 2018, 1:20 PM
 */

#include "R.h"
#include "Rmath.h"
#include <vector>
#include "InputParameters.h"
#include "InputData.h"
#include "Score.h"
#include "MinimizeScore.h"
#include "WriteResults.h"


#ifndef CALLPDF_HPP
#define	CALLPDF_HPP

class callPDF {
public:
    callPDF();
    callPDF(const callPDF& orig);
    virtual ~callPDF();
    void makeCall(int sampleLength, double * sampleData, double low, double high, int isLow, int isHigh, int points);
    
    vector <double> Vcdf;
    vector <double> Vpdf;
    vector <double> Vx;
    vector <double> Vsqr;
    
    bool solutionFailed;
        
private:

};

#endif	/* CALLPDF_HPP */

