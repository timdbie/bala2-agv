#include <M5Stack.h>
#include <delay.h>

bool intervalReady(uint32_t* elapsed)
{
    static uint32_t startTime = millis();
    static uint32_t lastRun = startTime;
    uint32_t now = millis();
    if (now - lastRun >= deltaTime * 1000)
    {
        lastRun += deltaTime * 1000;
        *elapsed = now - startTime;
        return true;
    }
    return false;
}