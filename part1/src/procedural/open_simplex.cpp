#include "procedural/open_simplex.hpp"
#include <cmath>
#include <algorithm>
#include <random>
#include <numeric>
#include <thread>
#include <vector>

OpenSimplex::OpenSimplex(unsigned int seed)
{
    std::vector<int> p(256);
    // Fill p with values from 0 to 255
    std::iota(p.begin(), p.end(), 0);
    // Shuffle using the provided seed
    std::shuffle(p.begin(), p.end(), std::default_random_engine(seed));

    // Duplicate the permutation vector
    perm.insert(perm.end(), p.begin(), p.end());
    perm.insert(perm.end(), p.begin(), p.end());

    // Fill permMod12 for simplicity
    for (int i = 0; i < 512; i++) {
        permMod12.push_back(perm[i] % 12);
    }
}

double OpenSimplex::noise(double x, double y)
{
    double n0, n1, n2; // Noise contributions from the three corners

    // Skewing and unskewing factors for 2D
    double F2 = 0.5 * (sqrt(3.0) - 1.0);
    double G2 = (3.0 - sqrt(3.0)) / 6.0;

    double s = (x + y) * F2; // Hairy factor for 2D
    int i = fastFloor(x + s);
    int j = fastFloor(y + s);

    double t = (i + j) * G2;
    double X0 = i - t; // Unskew the cell origin back to (x,y) space
    double Y0 = j - t;
    double x0 = x - X0; // The x,y distances from the cell origin
    double y0 = y - Y0;

    // For the 2D case, the simplex shape is an equilateral triangle.
    // Determine which simplex we are in.
    int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
    if (x0 > y0) { i1 = 1; j1 = 0; } // lower triangle, XY order: (0,0)->(1,0)->(1,1)
    else { i1 = 0; j1 = 1; } // upper triangle, YX order: (0,0)->(0,1)->(1,1)

    // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
    // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
    // c = (3-sqrt(3))/6
    double x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
    double y1 = y0 - j1 + G2;
    double x2 = x0 - 1.0 + 2.0 * G2; // Offsets for last corner in (x,y) unskewed coords
    double y2 = y0 - 1.0 + 2.0 * G2;

    // Work out the hashed gradient indices of the three simplex corners
    int ii = i & 255;
    int jj = j & 255;
    int gi0 = permMod12[ii + perm[jj]];
    int gi1 = permMod12[ii + i1 + perm[jj + j1]];
    int gi2 = permMod12[ii + 1 + perm[jj + 1]];

    // Calculate the contribution from the three corners
    double t0 = 0.5 - x0 * x0 - y0 * y0;
    if (t0 < 0) n0 = 0.0;
    else {
        t0 *= t0;
        n0 = t0 * t0 * dot(grad3[gi0], x0, y0); // (x,y) of grad3 used for 2D gradient
    }

    double t1 = 0.5 - x1 * x1 - y1 * y1;
    if (t1 < 0) n1 = 0.0;
    else {
        t1 *= t1;
        n1 = t1 * t1 * dot(grad3[gi1], x1, y1);
    }

    double t2 = 0.5 - x2 * x2 - y2 * y2;
    if (t2 < 0) n2 = 0.0;
    else {
        t2 *= t2;
        n2 = t2 * t2 * dot(grad3[gi2], x2, y2);
    }

    // Add contributions from each corner to get the final noise value.
    // The result is scaled to return values in the interval [-1,1].
    return 70.0 * (n0 + n1 + n2);
}

HeightMap OpenSimplex::generateNoiseMap(int width, int height, double sensitivity, int seed)
{
    HeightMap map(width+1, std::vector<Cell>(height+1));

    const int numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    int rowsPerThread = (height+1) / numThreads;

    for (int i = 0; i < numThreads; ++i) {
        int yStart = i * rowsPerThread;
        int yEnd = (i + 1) * rowsPerThread;

        if (i == numThreads - 1) {
            yEnd = height;  // Ensure the last thread covers all remaining rows
        }

        threads.emplace_back(
            [this, yStart, yEnd, width, sensitivity, &map]() {
                this->generateSection(yStart, yEnd, width, sensitivity,0.9, map);
            }
        );
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    return map;
}

int OpenSimplex::fastFloor(double x) {
    int xi = (int)x;
    return x < xi ? xi - 1 : xi;
}

double OpenSimplex::dot(const int* g, double x, double y) {
    return g[0] * x + g[1] * y;
}

void OpenSimplex::generateSection(int yStart, int yEnd, int width, double sensitivity, double clamp, HeightMap &map)
{
    for (int y = yStart; y <= yEnd; ++y) {
        for (int x = 0; x <= width; ++x) {
            double nx = x / (double)width / sensitivity;
            double ny = y / (double)map.size() / sensitivity;
            double value = noise(nx, ny);
            value = (value + 1.0) / 2.0;
            if (clamp != 1.0) {
                if (value > clamp)
                    value = 1;
                else
                    value = 0;
            }
            map[y][x].height = value;  // Normalize to 0.0 to 1.0 *scale
        }
    }
}
