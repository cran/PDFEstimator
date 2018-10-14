/* 
 * PDF Estimator:  A non-parametric probability density estimation tool based on maximum entropy
 * File:   InputParameters.cpp
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

#include "InputParameters.h"

InputParameters::InputParameters() {
    
    debug = false;    
   
    inputPath = "";
    inputFile = "specifyFilename.txt";
    writeFile = true;
    writeHeader = true;
    writeQQ = false;
    writeSQR = false;
    qqFile = "";
    sqrFile = "";
    
    scoreFile = "likelihood.txt";
    boundaryPenalty = true;
    lowerBoundSpecified = false;
    upperBoundSpecified = false;
    
    SURDMinimum = 5;
    SURDTarget  = 40;
    SURDMaximum = 100;
    initPartitionSize = 1025;
    startSolutionNumber = 0;
    integrationPoints = -1;
    numberSolutions = 1;
    maxLagrange = 200;//2 for power
    minLagrange = 1;
    nLagrangeAdd = 3;
    outlierCutoff = 7.0;    

    fuzz = false;
    fuzzFactor = 1;                                                     //not used yet
    
    fractionLagrangeAdd = 0.1;
    initSigma = 0.1;
    finalSigma = 0.001;
    decayFactor = 2.0/sqrt(2);
    loopMax = 30;
    
    symmetryPoint = 0;
    symmetry = false;   
    
    
}

InputParameters::InputParameters(const InputParameters& orig) {
}

InputParameters::~InputParameters() {
}

#ifndef R
bool InputParameters::userInput(int argc, char**  argv){
    
    int c;
    bool inputEntered = false;
    
    while ((c = getopt(argc, argv, "f:o:w:h:y:q:r:l:u:s:p:n:m:d:z:")) != -1)
    switch (c){        
        case 'f':
            inputFile = optarg;
            cout << "Input data file name:  " << inputFile << "\n";
            inputEntered = true;
            break; 
        case 'o':
            outputFile = optarg;
            cout << "Output data file name:  " << outputFile << "\n";
            break; 
        case 'w':
            writeOpt = optarg;
            if (writeOpt == "off") {
                writeFile = false;
                cout << "Write File:  off\n";
            }
            break;
        case 'h':
            headerOpt = optarg;
            if (headerOpt == "off") {
                writeHeader = false;
                cout << "Write File Header:  off\n";
            }
            break;
        case 'q':
            qqFile = optarg;
            writeQQ = true;
            cout << "Write QQ File:  " << qqFile << "\n";
            break;
        case 'r':
            sqrFile = optarg;
            writeSQR = true;
            cout << "Write SQR File:  " << sqrFile << "\n";
            break;
        case 'l': 
            lowerBound = atof(optarg);
            cout << "lower bound = " << optarg << "\n";
            lowerBoundSpecified = true;
            break; 
        case 'u':
            upperBound = atof(optarg);
            cout << "upper bound = " << optarg << "\n";
            upperBoundSpecified = true;
            break;        
        case 's':                                                               
            SURDTarget = atof(optarg);
            if (SURDTarget < 1) {
                cout << "WARNING: coverage must be between 1 and 100; setting to 1\n";
                SURDTarget = 1;
            } else if (SURDTarget > 100) {
                cout << "WARNING:  coverage must be between 1 and 100; setting to 100\n";
                SURDTarget = 100;
            } else {
                cout << "coverage = " << SURDTarget << "%\n";
            }
            break;
        case 'p':                                                               
            integrationPoints = atoi(optarg);
            cout << "integration points = " << optarg << "\n";
            break;
        case 'n':                                                               
            maxLagrange = atoi(optarg);
            cout << "maximum Lagrange = " << optarg << "\n";
            break;
        case 'm':                                                               
            minLagrange = atoi(optarg);
            cout << "minimum Lagrange = " << optarg << "\n";
            break;
        case 'y':
            penaltyOpt = optarg;
            if (penaltyOpt == "off") {
                boundaryPenalty = false;
                cout << "Penalty:  off\n";
            }
        case 'd':
            debugOpt = optarg;
            if (debugOpt == "on") {
                debug = true;
                cout << "Debug:  on\n";
            }
            break;
            break; 
        case 'z': 
            fuzzFactor = atof(optarg);
            cout << "fuzz factor = " << optarg << "\n";
            fuzz = true;
            break; 
        default:
            cout << "Invalid parameter flag: " << c << "\n\n";
            printUsage();            
            return false;
    }
    if (!inputEntered) {
        cout << "Input file name required\n\n";
        printUsage();
        return false;
    }
    return true;
}

  


void InputParameters::printUsage() {
    cout << "Usage:\n\n";
    cout << "getpdf -f <filename> [-option <argument>]\n\n";
    
    cout << "Options:\n\n";
    cout << " -f    input filename (REQUIRED)\n";
    cout << " -o    main output filename\n";
    cout << " -w    write main output file [on/off]\n";
    cout << " -h    include header info in main output file [on/off]\n";
    cout << " -q    QQ filename\n";
    cout << " -r    SQR filename\n";
    cout << " -l    lower bound\n";
    cout << " -u    upper bound\n";
    cout << " -s    score threshold percentage [1-100]\n";
    cout << " -p    minimum number of integration points\n";
    cout << " -n    maximum number of Lagrange multipliers\n";
    cout << " -m    minimum number of Lagrange multipliers\n";
    cout << " -y    penalty flag [on/off]\n";
    cout << " -d    debug [on/off]\n";
}

#endif
