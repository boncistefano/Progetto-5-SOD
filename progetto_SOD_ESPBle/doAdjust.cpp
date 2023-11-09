#include "doAdjust.h"

void doAdjust(RTC_PCF8523 rtc, String a)
{
    char charArray[a.length() + 1];  
    a.toCharArray(charArray, sizeof(charArray));
    uint32_t b = static_cast<uint32_t>(atol(charArray));
    rtc.adjust(DateTime(b));
}
