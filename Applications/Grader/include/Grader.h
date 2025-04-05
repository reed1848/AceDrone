#ifndef _GRADER_H
#define _GRADER_H

#include <apexType.h>
#include <stdbool.h>
#include "Parameters.h"

#define STATEQUEUESIZE 10

typedef struct CorrectRatio
{
    int correctCount;
    int totalCount;
} CorrectRatio;

typedef struct GradeReport
{
    CorrectRatio configChallenge;
    CorrectRatio fuelChallenge;
    CorrectRatio stateChallenge;
} GradeReport;

void gradeConfigResponse( char *grader, char *student );
void receiveConfigData();
void sendConfigData();
void printGradeReport( GradeReport aReport );
void printChallengeScore( const char* aTitle, CorrectRatio aRatio );
void updateCRPass( CorrectRatio* aRatio );
void updateCRFail( CorrectRatio* aRatio );
RETURN_CODE_TYPE initalizePorts();

#endif
