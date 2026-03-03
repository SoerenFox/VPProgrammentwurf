/******************************************************************************
 * @file Filter.c
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com)
 * @date   03.01.2026
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief Implementation file for Filter library
 *
 *
 *****************************************************************************/

 /***** INCLUDES **************************************************************/
#include "Filter.h"

/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/


/***** PUBLIC FUNCTIONS ******************************************************/

int32_t filterInitEMA(EMAFilterData_t* pEMA, int32_t scalingFactor, int32_t alpha, bool resetFilter)
{
    if (!pEMA) return FILTER_ERR_INVALID_PTR;
    if (scalingFactor <= 0) return FILTER_ERR_INVALID_PARAM;
    if (alpha < 0 || alpha > scalingFactor) return FILTER_ERR_INVALID_PARAM;

    pEMA->scalingFactor = scalingFactor;
    pEMA->alpha = alpha;

    if (resetFilter) return filterResetEMA(pEMA);
    return FILTER_ERR_OK;
}

int32_t filterResetEMA(EMAFilterData_t* pEMA)
{
    if (!pEMA) return FILTER_ERR_INVALID_PTR;
    pEMA->firstValueAvailable = false;
    pEMA->previousValue = 0;
    return FILTER_ERR_OK;
}

int32_t filterEMA(EMAFilterData_t* pEMA, int32_t sensorValue)
{
    if (!pEMA) return FILTER_ERR_INVALID_PTR;
    if (pEMA->scalingFactor <= 0) return FILTER_ERR_INVALID_PARAM;
    if (pEMA->alpha < 0 || pEMA->alpha > pEMA->scalingFactor) return FILTER_ERR_INVALID_PARAM;

    // erster Wert: Filter "startet" auf dem Eingang
    if (!pEMA->firstValueAvailable) {
        pEMA->previousValue = sensorValue;
        pEMA->firstValueAvailable = true;
        return sensorValue;
    }

    // y = (alpha*x + (S-alpha)*y_prev) / S
    int64_t S     = (int64_t)pEMA->scalingFactor;
    int64_t a     = (int64_t)pEMA->alpha;
    int64_t one_a = S - a;

    int64_t y = (a * (int64_t)sensorValue) + (one_a * (int64_t)pEMA->previousValue);

    // Optional: Runden statt Trunkieren (nur wenn y >= 0; sonst separat behandeln)
    if (y >= 0) y += (S / 2);

    y /= S;

    pEMA->previousValue = (int32_t)y;
    return pEMA->previousValue;
