/*
 * step_detector.c
 *
 *  Created on: Jan 10, 2026
 *      Author: ksiaz
 */

#include "step_detector.h"
#include "main.h"
#include <math.h>

void StepDetector_Init(StepDetector_t *dev)
{
    if (dev == NULL) return;//jakby nie istnial obiekt
    //inicjalizacja tych wartosci ze structa
    dev->avgM = 0;
    dev->lastZ = 0;
    dev->state = 0;
    dev->lastStepTime = 0;
    dev->initialized = 0;
    dev->stepsUp = 0;
    dev->stepsDown = 0;
    dev->stepsTotal = 0;
}

void StepDetector_Process(StepDetector_t *dev, float x, float y, float z)
{
    if (dev == NULL) return;//konczymy jezeli jest pusty struct
    //stale wzeite z eksperymentow
    const float ALPHA_AVG = 0.15f;//wspolczynnik do wyliczenia sredniej wartosci
    const float PEAK_THRESH = 0.15f;
    const float MIN_TIME = 450;

    uint32_t now = HAL_GetTick();
    float M = sqrtf(x * x + y * y + z * z);//wyliczenie magnitudy

    // Używamy -> bo dev to wskaźnik
    if(!dev->initialized)//przy 1 uruchomieni nie ma wartosci przypisanych wiec dlatego ten if jest zeby skopiowac obecne odczytane dane do pamieci
    {
        dev->avgM = M;
        dev->lastZ = z;
        dev->lastStepTime = now;
        dev->initialized = 1;
        return;
    }

    dev->avgM = dev->avgM + ALPHA_AVG * (M - dev->avgM);//wyliczenie sredniej wartosci M zeby byla do pozniejszych porownan
    float dM = M - dev->avgM;//odejmujemy wyliczona wartosc od aktualnie odczytanej najnowszej zeby miec sama zmiane

    switch(dev->state)
    {
        case 0:
            if(dM > PEAK_THRESH && (now - dev->lastStepTime > MIN_TIME))
            {
                float diffZ = z - dev->lastZ;//sprawdzamy odczyt z OZ zeby okreslic kierunke kroku Up/Down
                if(diffZ < -0.12f)
                {
                    dev->stepsDown++;
                    dev->stepsTotal++;
                    UART_Print("STEP DOWN\r\n");
                    dev->lastStepTime = now;
                    dev->state = 1;
                }
                else if(diffZ > 0.08f)
                {
                    dev->stepsUp++;
                    dev->stepsTotal++;
                    UART_Print("STEP UP\r\n");
                    dev->lastStepTime = now;
                    dev->state = 1; //zeby nie zliczac tego samego zdarzenia jako np. 2 kroki to wracamy do wartosci state = 1
                }
            }
            break;
           //po zmienie zliczeniowej sprawdzmy znowu wartosci dM czy pobrane probki wrocily do normy i mozna zliczac dalej
        case 1:
            if(dM < 0.05f) dev->state = 0;
            break;
    }
    //aktualizujemy wartosci z do porownania funkcja ktora
    if(fabsf(dM) < 0.05f)
    {
        dev->lastZ = z;
    }
}
