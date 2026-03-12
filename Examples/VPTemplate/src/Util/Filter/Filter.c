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

// Initializes the EMA filter with the specified parameters
int32_t filterInitEMA(EMAFilterData* pEMA, int32_t scalingFactor, int32_t alpha, bool resetFilter)
{
    if (!pEMA) return FILTER_ERR_INVALID_PTR;
    if (scalingFactor <= 0) return FILTER_ERR_INVALID_PARAM;
    if (alpha < 0 || alpha > scalingFactor) return FILTER_ERR_INVALID_PARAM;

    pEMA->scalingFactor = scalingFactor;
    pEMA->alpha = alpha;

    if (resetFilter) return filterResetEMA(pEMA);
    return FILTER_ERR_OK;
}

// Resets the EMA filter to its initial state
int32_t filterResetEMA(EMAFilterData* pEMA)
{
    if (!pEMA) return FILTER_ERR_INVALID_PTR;
    pEMA->firstValueAvailable = false;
    pEMA->previousValue = 0;
    return FILTER_ERR_OK;
}

// Applies the EMA filter to the input value and returns the filtered output
int32_t filterEMA(EMAFilterData* pEMA, int32_t x)
{
    if (!pEMA) return FILTER_ERR_INVALID_PTR;
    if (pEMA->scalingFactor <= 0) return FILTER_ERR_INVALID_PARAM;
    if (pEMA->alpha < 0 || pEMA->alpha > pEMA->scalingFactor) return FILTER_ERR_INVALID_PARAM;

    if (!pEMA->firstValueAvailable) {
        pEMA->previousValue = x;
        pEMA->firstValueAvailable = true;
        return x;
    }

    // y = (a*x + (S-a)*y_prev) / S
    int32_t S = (int32_t)pEMA->scalingFactor;
    int32_t a = (int32_t)pEMA->alpha;

    int32_t y = (a * (int32_t)x) + ((S - a) * (int32_t)pEMA->previousValue);

    pEMA->previousValue = (int32_t)y;
    return pEMA->previousValue;
}
