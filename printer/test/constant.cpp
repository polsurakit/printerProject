#include <string>
#if !defined(MYLIB_CONSTANTS_H)
#define MYLIB_CONSTANTS_H 1




const bool isSimulation = true;
const bool isAruco = true;

const int DPI = 100;
const bool isBlur = false;



const int fieldSize = 7000; //1.5m x 1.5m field


const int TOPLEFTX = 500;
const int TOPLEFTY = 500;
const int BOTTOMRIGHTX = fieldSize-TOPLEFTX;
const int BOTTOMRIGHTY = fieldSize-TOPLEFTY;

const int smallWindowSize = 250;
const int bigWindowSize = 500;
const int moveStep = 1000;				//can be calculate
const int firstPosition = 1050;



const int printFieldSize = 1300; // 25cm. x 25cm. size that we need to print
const int printSize = 1500; // 25cm. x 25cm. size that printer can print

const int MERGESIZE = printFieldSize-moveStep;
const int HALF_PURE_SIZE = printFieldSize/2-MERGESIZE;

#endif

//how 300 in algor6 come
//how 200 in showresult come