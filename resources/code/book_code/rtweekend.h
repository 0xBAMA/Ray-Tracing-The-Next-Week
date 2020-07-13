#ifndef RTWEEKEND_H
#define RTWEEKEND_H
//==============================================================================================
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>


// Usings

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

inline double random_double() {
   // return rand() / (RAND_MAX + 1.0);

	// long unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
	// std::default_random_engine engine{seed};
	// std::uniform_real_distribution<double> distribution{0.0, 1.0};
	// return distribution(engine);

	static std::random_device rd;  //Will be used to obtain a seed for the random number engine
   	static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
   	std::uniform_real_distribution<> dis(0.0, 1.0);

	return dis(gen);

}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();



	// long unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
	// std::default_random_engine engine{seed};
	// std::uniform_real_distribution<double> distribution{min, max};
	// return distribution(engine);



	// std::uniform_real_distribution<double> distribution(min, max);
	// std::mt19937 generator;
	// return distribution(generator);
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double(min, max+1));
}

// Common Headers

#include "ray.h"
#include "vec3.h"


#endif
