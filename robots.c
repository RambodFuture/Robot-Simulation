#include <stdio.h>
#include <stdlib.h>

#define MIN_ROWS 12
#define MAX_ROWS 100
#define MIN_COLS 12
#define MAX_COLS 100
#define MIN_ROBOTS 1
#define MAX_ROBOTS 10
#define MIN_SEED 10
#define MAX_SEED 32767
#define MIN_ITERATIONS 5
#define MAX_ITERATIONS 2000

enum initTypeList { RANDOM_STRIPES = 1, CHECKERBOARD, ALL_MAGENTA };

struct Robot {
    int x;
    int y;
    int direction;
    int paintColour;
};

                                                                                   // Function to allocate memory for the floor
int **allocateFloor(int numRows, int numCols) {
    int **floor = malloc(numRows * sizeof(int *));
    if (!floor) {
        fprintf(stderr, "ERROR: Array of pointers for 2-D array could not be allocated\n");
        return NULL;
    }
    for (int i = 0; i < numRows; i++) {
        floor[i] = malloc(numCols * sizeof(int));
        if (!floor[i]) {
            fprintf(stderr, "ERROR: Array storage for the 2-D array could not be allocated\n");
            for (int j = 0; j < i; j++) free(floor[j]);                             // Free allocated rows
            free(floor);
            return NULL;
        }
    }
    return floor;
}

                                                                                    // Function to initialize all tiles as magenta
void InitFloorAllMagenta(int **floor, int numRows, int numCols) {
    for (int i = 0; i < numRows; i++)
        for (int j = 0; j < numCols; j++)
            floor[i][j] = 5;                                                        // Magenta color
}

                                                                                    // Function to initialize floor with a checkerboard pattern
void InitFloorChecker(int **floor, int numRows, int numCols) {
    for (int i = 0; i < numRows; i++)
        for (int j = 0; j < numCols; j++)
            floor[i][j] = ((i / 4 + j / 4) % 2 == 0) ? 6 : 5;                      // White and Magenta
}

                                                                                   // Function to initialize floor with random vertical stripes
void InitFloorRandStripe(int **floor, int numRows, int numCols, unsigned int seed) {
    srand(seed);
    for (int j = 0; j < numCols; j++)
        floor[0][j] = (rand() % 6) + 1;                                             // Random color (1-6)
    
    for (int i = 1; i < numRows; i++)
        for (int j = 0; j < numCols; j++)
            floor[i][j] = floor[0][j];                                             // Copy first row to all rows (stripes)
}

                                                                                   // Function to initialize robots
void initRobots(struct Robot robots[], int numRobots, int numRows, int numCols, unsigned int seed) {
    srand(seed);
    for (int i = 0; i < numRobots; i++) {
        robots[i].x = rand() % numRows;
        robots[i].y = rand() % numCols;
        robots[i].direction = rand() % 4;
        robots[i].paintColour = (rand() % 4) + 1;                                   // Paint color (1-4)
    }
}

                                                                                    // Function to move robots
void moveRobots(struct Robot robots[], int **floor, int numRobots, int numRows, int numCols, int numIterations, int interval) {
    for (int iter = 0; iter <= numIterations; iter++) {
        if (iter % interval == 0) {
            printf("Iteration %d:\n", iter);
            for (int i = 0; i < numRows; i++) {
                for (int j = 0; j < numCols; j++) {
                    printf("%d ", floor[i][j]);
                }
                printf("\n");
            }
        }
        
        for (int r = 0; r < numRobots; r++) {
            for (int step = 0; step < 4; step++) {
                switch (robots[r].direction) {
                    case 0: robots[r].x = (robots[r].x - 1 + numRows) % numRows; break;
                    case 1: robots[r].y = (robots[r].y + 1) % numCols; break;
                    case 2: robots[r].x = (robots[r].x + 1) % numRows; break;
                    case 3: robots[r].y = (robots[r].y - 1 + numCols) % numCols; break;
                }
                floor[robots[r].x][robots[r].y] = robots[r].paintColour;
            }
            switch (floor[robots[r].x][robots[r].y]) {
                case 1: case 5: robots[r].direction = (robots[r].direction + 1) % 4; break;
                case 2: case 6: robots[r].direction = (robots[r].direction + 2) % 4; break;
                case 3: robots[r].direction = (robots[r].direction + 3) % 4; break;
                case 4: break; // No change in direction on blue tile
            }
        }
    }
}

                                                                                            // Function to read parameters from a file
int readParameters(const char *filename, int *numRows, int *numCols, int *numRobots,
                   int *initType, unsigned int *initSeed, int *numIterations, int *interval, char *outputFilename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "ERROR: Could not open input file\n");
        return 1;
    }
    
    if (fscanf(file, "%d %d %d %d %u %d %d %s", numRows, numCols, numRobots, initType, initSeed, numIterations, interval, outputFilename) != 8) {
        fprintf(stderr, "ERROR: Corrupt or incomplete data in input file\n");
        fclose(file);
        return 1;
    }
    
    fclose(file);
    return 0;
}

int main() {
    int numRows, numCols, numRobots, initType, numIterations, interval;
    unsigned int initSeed;
    char outputFilename[50];

    //Change the name of the input files name here!
    if (readParameters("robots_input.txt", &numRows, &numCols, &numRobots, &initType, &initSeed, &numIterations, &interval, outputFilename) != 0) {
        return 1;
    }                                                       

    int **floor = allocateFloor(numRows, numCols);
    if (!floor) return 1;

    struct Robot *robots = malloc(numRobots * sizeof(struct Robot));
    if (!robots) {
        fprintf(stderr, "ERROR: Array of robots could not be allocated\n");
        return 1;
    }

    switch (initType) {
        case ALL_MAGENTA: InitFloorAllMagenta(floor, numRows, numCols); break;
        case CHECKERBOARD: InitFloorChecker(floor, numRows, numCols); break;
        case RANDOM_STRIPES: InitFloorRandStripe(floor, numRows, numCols, initSeed); break;
    }

    initRobots(robots, numRobots, numRows, numCols, initSeed);
    moveRobots(robots, floor, numRobots, numRows, numCols, numIterations, interval);

    free(robots);
    for (int i = 0; i < numRows; i++) free(floor[i]);
    free(floor);

    return 0;
}
