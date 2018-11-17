/* 
 * File:   OutputControl.h
 * Author: jenny
 *
 * Created on October 31, 2018, 6:57 PM
 */

#ifndef OUTPUTCONTROL_H
#define	OUTPUTCONTROL_H

//#define debug
#include <string>
#include <iostream>
using namespace std;

class OutputControl {
public:
    OutputControl();
    OutputControl(const OutputControl& orig);
    virtual ~OutputControl();
#ifdef debug    
    void print(string output);
    void print(string output, int value);
    void print(string output, double value);
#else
    void print(string output) {};
    void print(string output, int value) {};
    void print(string output, double value) {};
#endif
    
private:

};

#endif	/* OUTPUTCONTROL_H */

