#include "DistanceCalculator.h"


// Constructor
DistanceCalculator::DistanceCalculator(IntialConfig* config)
{
    // Intialize hashMap and precompute values
    for (int i = 0; i < NUM_OF_OBSTACLES; i++)
    {
        // Create a new array for each obstacle and precompute the distance
        int* obstacleDistanceCalc = new int [NUM_OF_DISTANCES]{0};
        precalculateDistance(obstacleDistanceCalc, config, (OBSTACLE_TYPE)i);\
        calculatedObstacleDistance.insert({(OBSTACLE_TYPE)i, obstacleDistanceCalc});
    }
}

// Destructor
DistanceCalculator::~DistanceCalculator()
{
    for (int i = 0; i < NUM_OF_OBSTACLES; i++)
    {
        int* array = calculatedObstacleDistance.at((OBSTACLE_TYPE)i);
        delete[] array;
    }
    calculatedObstacleDistance.clear();
}

// Public Methods
int* DistanceCalculator::GetDistance(OBSTACLE_TYPE obstacleType)
{
    if(calculatedObstacleDistance.find(obstacleType) != calculatedObstacleDistance.end())
    {
        return calculatedObstacleDistance.at(obstacleType);
    }
    else
    {
        return nullptr;
    }
}
// Private Methods
void DistanceCalculator::precalculateDistance(int* arrayOfDistances, IntialConfig* config, OBSTACLE_TYPE obstacleType)
{
    double obstacleRate = 0.0;
    switch (obstacleType)
    {
        case ASTROID_BELT:
            obstacleRate = config->aRate;
            break;
        case MOUNTAIN:
            obstacleRate = config->mRate;
            break;
        case SHOOTING_STAR:
            obstacleRate = config->sRate;
            break;
        case BLACK_HOLE:
            obstacleRate = config->bRate;
            break;
        case EXPLODING_STAR:
            obstacleRate = config->eRate;
            break;
        default:
            obstacleRate = 0.0;
            break;
    }
    printf("Obstacle rate: %0.1f. Obstacle Type: %d\n", obstacleRate, obstacleType);

    for (int i = 0; i < NUM_OF_DISTANCES; i++)
    {
        arrayOfDistances[i] = (int) ceilf(fmod(((double)i * obstacleRate) * config->c1 / config->c2, config->maxTime));
    }
}

