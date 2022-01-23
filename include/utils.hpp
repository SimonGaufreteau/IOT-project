#ifndef utils_hpp
#define utils_hpp

#ifdef ARDUINO_SAMD_VARIffT_COMPLIffCE
#define SERIAL SerialUSB
#else
#define SERIAL Serial
#endif

#endif