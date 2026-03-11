/******************************************************************************
 * @file GlobalObjects.h
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com)
 * @date   03.01.2026
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief Implementatiopn file for global objects used across different modules
 * (based on access functions)
 *
 *
 *****************************************************************************/

/***** INCLUDES **************************************************************/
#include "GlobalObjects.h"

/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/
EMAFilterData gEmaPot1;
EMAFilterData gEmaPot2;

GasSensor gGasSensor1;
GasSensor gGasSensor2;

DebounceButton gButtonSW1 = { BUTTON_RELEASED, BUTTON_RELEASED, 0};
DebounceButton gButtonB1  = { BUTTON_RELEASED, BUTTON_RELEASED, 0};

RadioConnect gRadioConnect;

/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/


/***** PUBLIC FUNCTIONS ******************************************************/


/***** PRIVATE FUNCTIONS *****************************************************/



