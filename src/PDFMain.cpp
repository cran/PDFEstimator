
#include <vector>
#include "callPDF.h"

extern "C" { 
    void estimatePDF(double *sampleData, int *sampleLength, 
                     double *low, double *high, int *isLow, int *isHigh, 
                     int *lagrangeMin, int *lagrangeMax, int *debug, int *outlierCutoff,
                     int *points, int *fail, double *threshold, double *x, double *pdf, double *cdf, double *sqr, double *sqrSize, double *lagrange){    
        callPDF pd;
        pd.makeCall(sampleLength[0], sampleData, low[0], high[0], isLow[0], isHigh[0], points[0], lagrangeMin[0], lagrangeMax[0], outlierCutoff[0], debug[0]);
        
        vector <double> Vx = pd.Vx;
        vector <double> Vpdf = pd.Vpdf;
        vector <double> Vcdf = pd.Vcdf;      
        vector <double> Vsqr = pd.Vsqr;  
        vector <double> Vlagrange = pd.Vlagrange;
        
        sqrSize[0] = pd.N;
        
        if (pd.solutionFailed) {
            fail[0] = 1;
        } else {            
            fail[0] = 0;
        }
        
        threshold[0] = pd.solutionThreshold;
        
        for (unsigned i = 0; i < Vx.size(); i++) {
            cdf[i] = Vcdf[i];
            pdf[i] = Vpdf[i];
            x[i] = Vx[i];
        }
        for (unsigned i = 0; i < Vsqr.size(); i++) {
            sqr[i] = Vsqr[i];
        }    

//        int resizeLagrange = pd.Vlagrange.size();
//        double * newLagrange = new double[resizeLagrange];
//        delete lagrange;
//        lagrange = new double[resizeLagrange];
//        for (int i = 0; i < resizeLagrange; i++) {
        for (unsigned i = 0; i < Vlagrange.size(); i++) {
            lagrange[i] = Vlagrange[i];
        }            
//        lagrange = newLagrange;
        return;    
    }   
} 
