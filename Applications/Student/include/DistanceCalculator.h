#ifndef DISTANCECALC_H
#define DISTANCECALC_H
#define NUM_OF_OBSTACLES 5
#define NUM_OF_DISTANCES 10
#include <unordered_map>
#include <cmath>
#include <string.h>

class DistanceCalculator {
public:
    // Public members
    typedef enum { ASTROID_BELT, MOUNTAIN, SHOOTING_STAR, BLACK_HOLE, EXPLODING_STAR, OBSTACLE_ALL} OBSTACLE_TYPE;  
    typedef struct 
    {
        double aRate;
        double mRate;
        double sRate;
        double bRate;
        double eRate;
        int c1;
        int c2; 
        int maxTime;
    }IntialConfig;
    
    DistanceCalculator(IntialConfig* config);
    ~DistanceCalculator();
    int* GetDistance(OBSTACLE_TYPE obstacleType);
    void convertDistanceEnum(DistanceCalculator::OBSTACLE_TYPE obstacleEnum, char *enumStr);

private:
    std::unordered_map<OBSTACLE_TYPE, int*> calculatedObstacleDistance;
    void precalculateDistance(int* arrayOfDistances, IntialConfig* config, OBSTACLE_TYPE obstacleType);
};

#endif