#ifndef DISTANCECALC_H
#define DISTANCECALC_H
#define NUM_OF_OBSTACLES 5
#define NUM_OF_DISTANCES 10
#include <unordered_map>

class DistanceCalculator {
public:
    // Public members
    typedef enum { ASTROID_BELT, MOUNTAIN, SHOOTING_STAR, BLACK_HOLE, EXPLODING_STAR } OBSTACLE_TYPE;  
    typedef struct 
    {
        float aRate;
        float mRate;
        float sRate;
        float bRate;
        float eRate;
        int c1;
        int c2; 
        int maxTime;
    }IntialConfig;
    
    DistanceCalculator(IntialConfig* config);
    ~DistanceCalculator();
    int* GetDistance(OBSTACLE_TYPE obstacleType);

private:
    std::unordered_map<OBSTACLE_TYPE, int*> calculatedObstacleDistance;
    void precalculateDistance(int* arrayOfDistances, IntialConfig* config, OBSTACLE_TYPE obstacleType);
};

#endif