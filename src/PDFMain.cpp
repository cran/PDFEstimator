
#include <vector>
#include "callPDF.h"

extern "C" { 
    void estimatePDF(double *sampleData, int *sampleLength, 
                     double *low, double *high, int *isLow, int *isHigh, 
                     int *points, int *fail, double *x, double *pdf, double *cdf, double *sqr, double *lagrange){     
        callPDF pd;
        pd.makeCall(sampleLength[0], sampleData, low[0], high[0], isLow[0], isHigh[0], points[0]);
        
        vector <double> Vx = pd.Vx;
        vector <double> Vpdf = pd.Vpdf;
        vector <double> Vcdf = pd.Vcdf;      
        vector <double> Vsqr = pd.Vsqr;        
        vector <double> Vlagrange = pd.lagrange;
        
        if (pd.solutionFailed) {
            fail[0] = 1;
        }
        
        for (int i = 0; i < Vx.size(); i++) {
            cdf[i] = Vcdf[i];
            pdf[i] = Vpdf[i];
            x[i] = Vx[i];
            sqr[i] = Vsqr[i];
        }    
        for (int i = 0; i < Vlagrange.size(); i++) {
            lagrange[i] = Vlagrange[i];
        }
        return;    
    }   
} 
/*
static const R_CMethodDef CEntries[] = {
    {"estimatePDF", (DL_FUNC) &estimatePDF, 13},
    {NULL, NULL, 0}
};

void R_init_estimatePDF(DllInfo *dll)
{
    R_registerRoutines(dll, CEntries, NULL, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
 */
 
