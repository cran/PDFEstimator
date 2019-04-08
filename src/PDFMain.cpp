
#include <vector>
#include "callPDF.h"

extern "C" { 
    void estimatePDF(double *sampleData, int *sampleLength, 
                     double *low, double *high, int *isLow, int *isHigh, 
                     int *points, int *fail, double *x, double *pdf, double *cdf, double *sqr){     
        callPDF pd;
        pd.makeCall(sampleLength[0], sampleData, low[0], high[0], isLow[0], isHigh[0], points[0]);
        
        vector <double> Vx = pd.Vx;
        vector <double> Vpdf = pd.Vpdf;
        vector <double> Vcdf = pd.Vcdf;      
        vector <double> Vsqr = pd.Vsqr;        
        
        if (pd.solutionFailed) {
            fail[0] = 1;
        } else {            
            fail[0] = 0;
        }
        
        for (unsigned i = 0; i < Vx.size(); i++) {
            cdf[i] = Vcdf[i];
            pdf[i] = Vpdf[i];
            x[i] = Vx[i];
        }
        for (unsigned i = 0; i < Vsqr.size(); i++) {
            sqr[i] = Vsqr[i];
        }           
        return;    
    }   
} 
