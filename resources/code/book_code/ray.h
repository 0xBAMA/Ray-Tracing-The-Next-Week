#ifndef RAY_H
#define RAY_H
//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "vec3.h"


class ray
{
    public:
        ray() {}
        ray(const glm::dvec3& origin, const glm::dvec3& direction)
            : orig(origin), dir(direction), tm(0)
        {}

        ray(const glm::dvec3& origin, const glm::dvec3& direction, double time)
            : orig(origin), dir(direction), tm(time)
        {}

        glm::dvec3 origin() const  { return orig; }
        glm::dvec3 direction() const { return dir; }
        double time() const    { return tm; }

        glm::dvec3 at(double t) const {
            return orig + t*dir;
        }

    public:
        glm::dvec3 orig;
        glm::dvec3 dir;
        double tm;
};

#endif
