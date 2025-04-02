/***************************************************************************************************
** Copyright 2014 GE Aviation Systems LLC, U.S.A.
** -------------------------------------------------------------------------------------------------
**                       GE AVIATION SYSTEMS LLC PROPRIETARY INFORMATION
** The information contained in this document is GE Aviation Systems LLC proprietary information
** and is disclosed in confidence.  It is the property of GE Aviation Systems LLC and shall not
** be used, disclosed to others or reproduced without the express written consent of GE Aviation
** Systems LLC, including, but without limitation, it is not to be used in the creation,
** manufacture, development, or derivation of any repairs, modifications, spare parts, designs, or
** configuration changes or to obtain FAA or any other government or regulatory approval to do so.
** If consent is given for reproduction in whole or in part, this notice and the notice set forth
** on each page of this document shall appear in any such reproduction in whole or in part.
**
** This technical data is considered ITAR and/or EAR controlled pursuant to 22 CFR Part 120-130
** and 15 CFR Parts 730-774 respectively.  Transfer of this data by any means to a Non-US
** person, whether in the United States or abroad, without the proper U.S. Government
** authorization (e.g.,License, exemption, NLR, etc.) is strictly prohibited.
***************************************************************************************************/

#include <apexType.h>
#include <apexProcess.h>
#include <apexPartition.h>
#include <apexQueuing.h>
#include <apexSampling.h>
#include "apex/apexLib.h"

#include <scoeAMIOEnable.h>

#include "DroneController.h"
#include "DistanceCalculator.h"

/***************************************************************************************************
** droneController_main
**  The partition's entry point (called on cold and warm start). Creates processes/ports/etc then
**  transitions to NORMAL mode (NORMAL mode used for Embedded SW and Real-Time Systems Classes).
**
** Parameters:
**   None
**
** Returns:
**   Nothing
*/
DistanceCalculator::IntialConfig demo_config;
void droneController_main( void )
{
    scoeAmioEnable();

    //TODO: Remove after finishing testing
    demo_config.aRate = 1.0;
    demo_config.bRate = 0.5;
    demo_config.eRate = 1.5;
    demo_config.mRate = 2.0;
    demo_config.sRate = 2.5;
    demo_config.c1 = 4;
    demo_config.c2 = 7;
    demo_config.maxTime = 10;
    DistanceCalculator dc(&demo_config);
    printf("Done init\n");

    int* distancesAstroid = dc.GetDistance(DistanceCalculator::ASTROID_BELT);
    for (int i = 0; i < NUM_OF_DISTANCES; i++)
    {
        printf("Calculated distance %d\n", distancesAstroid[i]);
    }


}
