/*
 * step_detector.h
 *
 *  Created on: Jan 10, 2026
 *      Author: ksiaz
 */

#ifndef INC_STEP_DETECTOR_H_
#define INC_STEP_DETECTOR_H_

#include <stdint.h>
#include <math.h>

typedef struct
{
    float avgM;
    //float lastM;
    float lastZ;
    //float peak;
    float valley;
    uint8_t state;          // 0 = szukanie peak, 1 = szukanie valley
    uint32_t lastStepTime;
    uint8_t initialized;
    int stepsUp;
    int stepsDown;
    int stepsTotal;
} StepDetector_t;

void StepDetector_Init(StepDetector_t *dev);
void StepDetector_Process(StepDetector_t *dev, float x, float y, float z);
void StepDetector_Reset(StepDetector_t *dev);




#endif /* INC_STEP_DETECTOR_H_ */
