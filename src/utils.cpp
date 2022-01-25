#include "utils.hpp"
#include <string>
#include "Arduino.h"

int extractHexa(const char *str)
{
    int length = strlen(str);
    int decimal = 0;
    int base = 1;
    for (int i = length--; i >= 0; i--)
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            decimal += (str[i] - 48) * base;
            base *= 16;
        }
        else if (str[i] >= 'A' && str[i] <= 'F')
        {
            decimal += (str[i] - 55) * base;
            base *= 16;
        }
        else if (str[i] >= 'a' && str[i] <= 'f')
        {
            decimal += (str[i] - 87) * base;
            base *= 16;
        }
    }
    return decimal;
}