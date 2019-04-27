#include <string>
#if !defined(MYLIB_CONSTANTS_H)
#define MYLIB_CONSTANTS_H 1



const bool isSimulation = false;

const std::string INPUT_NAME = "test.jpg";
const std::string OUTPUT_NAME = "algor3_new.jpg";
const int DPI = 100;
const bool isBlur = false;

const int TARGET_H_SIZE = 4000; //0.1 millimeter / pixel
const int TARGET_W_SIZE = 4000; //0.1 millimeter / pixel

const int fieldSize = 7000; //1.5m x 1.5m field


const int TOPLEFTX = 500;
const int TOPLEFTY = 500;
const int BOTTOMRIGHTX = fieldSize-TOPLEFTX;
const int BOTTOMRIGHTY = fieldSize-TOPLEFTY;

const int smallWindowSize = 250;
const int bigWindowSize = 500;
const int moveStep = 750;				//can be calculate
const int firstPosition = 800;



const int printFieldSize = 1000; // 25cm. x 25cm. size that we need to print
const int printSize = 1500; // 25cm. x 25cm. size that printer can print

const int MERGESIZE = printFieldSize-moveStep;
const int HALF_PURE_SIZE = printFieldSize/2-MERGESIZE;

#endif

//how 300 in algor6 come
//how 200 in showresult come