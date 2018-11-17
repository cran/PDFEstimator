/* 
 * File:   OutputControl.cpp
 * Author: jenny
 * 
 * Created on October 31, 2018, 6:57 PM
 */

#include "OutputControl.h"

OutputControl::OutputControl() {
}

OutputControl::OutputControl(const OutputControl& orig) {
}

OutputControl::~OutputControl() {
}

#ifdef debug
void OutputControl::print(string output) {
    cout << output << "\n";
}

void OutputControl::print(string output, int value) {
    cout << output << ": " << value << "\n";
}

void OutputControl::print(string output, double value) {
    cout << output << ": " << value << "\n";
}
#endif
