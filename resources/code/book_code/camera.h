#ifndef CAMERA_H
#define CAMERA_H
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

#include "rtweekend.h"


class camera {
    public:
        camera() : camera(glm::dvec3(0,0,-1), glm::dvec3(0,0,0), glm::dvec3(0,1,0), 40, 1, 0, 10) {}

        camera(
            glm::dvec3 lookfrom,
            glm::dvec3 lookat,
            glm::dvec3   vup,
            double vfov, // vertical field-of-view in degrees
            double aspect_ratio,
            double aperture,
            double focus_dist,
            double t0 = 0,
            double t1 = 0
        ) {
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;

            w = glm::normalize(lookfrom - lookat);
            u = glm::normalize(glm::cross(vup, w));
            v = glm::cross(w, u);

            origin = lookfrom;
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            lower_left_corner = origin - horizontal/2.0 - vertical/2.0 - focus_dist*w;

            lens_radius = aperture / 2.0;
            time0 = t0;
            time1 = t1;
        }

        ray get_ray(double s, double t) const {
            glm::dvec3 rd = lens_radius * random_in_unit_disk();
            glm::dvec3 offset = u * rd.x + v * rd.y;
            return ray(
                origin + offset,
                lower_left_corner + s*horizontal + t*vertical - origin - offset,
                random_double(time0, time1)
            );
        }

    private:
        glm::dvec3 origin;
        glm::dvec3 lower_left_corner;
        glm::dvec3 horizontal;
        glm::dvec3 vertical;
        glm::dvec3 u, v, w;
        double lens_radius;
        double time0, time1;  // shutter open/close times
};

#endif
