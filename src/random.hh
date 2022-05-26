#pragma once
#include <stdlib.h>
#include <time.h>

class RandomGenerator {
public:
    RandomGenerator() {
        srand((unsigned)time(0));
    }

    float operator()(float min, float max) {
        return min + ((float)rand() / (float)RAND_MAX) * (max - min);
    }

    double operator()(double min, double max) {
        return min + ((double)rand() / (double)RAND_MAX) * (max - min);
    }

    int operator()(int min, int max) {
        return min + rand() % (max - min);
    }
};