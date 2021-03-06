#include <vector>
#include <numeric>      // std::partial_sum
#include "Vec3.h"
#include "HemisphereSampling.h"

class HemisphereMapping : public HemisphereSampling {
public:
    HemisphereMapping(int resX, int resY): resX(resX), resY(resY), grid(resX*resY, 1.f) {}

    std::size_t size() {
        return grid.size();
    }

    const float getValue(int idx) {
        return grid[idx];
    }

    Vec3<float> getDir(int idx) {
        return normalize(mapIndexToDirection(idx));
    }

    void updateByIndex(int idx, float update) {
        grid[idx] = update;
    }

    void sampleDirection(Sample& s) const override {
        // Random float between 0. and 1.
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

        // Normalize the grid: floats -> floats between 0. and 1.
        //    - q /= (sum of all q's)
        //    - try a softmax approach ?
        std::vector<float> normalizedGrid(grid.size());
        float sum = 0.f;
        for (std::size_t i = 0; i < grid.size(); i++)
            sum += grid[i];
        for (std::size_t i = 0; i < grid.size(); i++)
            normalizedGrid[i] = grid[i] / sum;
        // Cumulative sum
        std::vector<float> cumulative(grid.size());
        std::partial_sum(normalizedGrid.begin(), normalizedGrid.end(), cumulative.begin());

        // Pick the right index
        s.index = grid.size() - 1;
        for (int i = 0; i < (int) cumulative.size(); i++) {
            if (r < cumulative[i]) {
                s.index = i;
                break;
            }
        }
        s.direction = normalize(mapIndexToDirection(s.index));
        s.probability = ((float) grid.size() * normalizedGrid[s.index]) / (2.f * M_PI);
    }

private:
    Vec3<float> mapIndexToDirection(int dirIndex) const {
        int idxX = dirIndex % resX;
        int idxY = dirIndex / resX;

        //float x = idxX / (float) (resX - 1);
        //float y = idxY / (float) (resY - 1);

        float sizeX = 1.f / (float) (resX);
        float sizeY = 1.f / (float) (resY);
        float randomShiftX = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float randomShiftY = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

        float x = ((float) idxX + randomShiftX) * sizeX;
        float y = ((float) idxY + randomShiftY) * sizeY;

        //return advancedMap(x, y);
        return simpleMap(x, y);
    }

    Vec3<float> simpleMap(float x, float y) const {
        // Bad implementation according to https://mathworld.wolfram.com/SpherePointPicking.html
        //float theta = M_PI * x * 0.5f;
        //float phi = M_PI * y * 2.f;
        //return Vec3<float>(std::sin(theta)*std::cos(phi), std::sin(theta)*std::sin(phi), std::cos(theta));

        // Better implementation according to https://mathworld.wolfram.com/SpherePointPicking.html
        //float phi = M_PI * x * 2.f;
        //float theta = std::acos(y);
        //return Vec3<float>(std::sin(theta)*std::cos(phi), std::sin(theta)*std::sin(phi), std::cos(theta));

        // Similar to uniform sample
        return HemisphereSampling::uniformSample(x, y);
    }

    /*
    * source: https://legacy.cs.indiana.edu/ftp/techreports/TR411.pdf
    *
    * This function takes a point in the unit square,
    * and maps it to a point on the unit hemisphere.
    */
    Vec3<float> advancedMap(float x, float y) const {
        Vec3<float> p;
        float xx, yy, offset, theta, phi;
        x = 2.f * x - 1.f;
        y = 2.f * y - 1.f;
        if (y > -x) { // Above y = -x
            if (y < x) { // Below y = x
                xx = x;
                if (y > 0) { // Above x-axis
                    /*
                    * Octant 1
                    */
                    offset = 0;
                    yy = y;
                } else { // Below and including x-axis
                    /*
                    * Octant 8
                    */
                    offset = (7.f * M_PI) / 4.f;
                    yy = x + y;
                }
            } else { // Above and including y = x
                xx = y;
                if (x > 0) { // Right of y-axis
                    /*
                    * Octant 2
                    */
                    offset = M_PI / 4.f;
                    yy = (y - x);
                } else { // Left of and including y-axis
                    /*
                    * Octant 3
                    */
                    offset = (2.f * M_PI) / 4.f;
                    yy = -x;
                }
            }
        } else { // Below and including y = -x
            if (y > x) { // Above y = x
                xx = -x;
                if (y > 0) { // Above x-axis
                    /*
                    * Octant 4
                    */
                    offset = (3.f * M_PI) / 4.f;
                    yy = -x - y;
                } else { // Below and including x-axis
                    /*
                    * Octant 5
                    */
                    offset = (4.f * M_PI) / 4.f;
                    yy = -y;
                }
            } else { // Below and including y = x
                xx = -y;
                if (x > 0) { // Right of y-axis
                    /*
                    * Octant 7
                    */
                    offset = (6.f * M_PI) / 4.f;
                    yy = x;
                } else { // Left of and including y-axis
                    if (y != 0) {
                        /*
                        * Octant 6
                        */
                        offset = (5.f * M_PI) / 4.f;
                        yy = x - y;
                    } else {
                        /*
                        * Origin
                        */
                        p[0] = 0.f;
                        //p[1] = 1.f;
                        //p[2] = 0.f;
                        p[1] = 0.f;
                        p[2] = 1.f;
                        return p;
                    }
                }
            }
        }
        theta = acos(1.f - xx * xx);
        phi = offset + (M_PI / 4.f) * (yy / xx);
        p[0] = sin(theta) * cos(phi);
        //p[1] = std::max(cos(theta), 0.f); // necessary, sometimes theta = pi/2 but a bit less and we get p[1] negative
        //p[2] = sin(theta) * sin(phi);
        p[1] = sin(theta) * sin(phi);
        p[2] = std::max(cos(theta), 0.f); // necessary, sometimes theta = pi/2 but a bit less and we get p[1] negative
        return p;
    }

    int resX;
    int resY;
    std::vector<float> grid;
};
