#include "display.hpp"

void setup()
{
    SERIAL.begin(115200);
    MatrixDisplayType type = MatrixDisplayType::Message;
    changeDisplayTypeML(type);
    setupLEDMatrix();
    setupLEDChain();
    createDisplayTasks();
    //createPingTasks();
    for (;;)
        ;
}

void loop()
{
}