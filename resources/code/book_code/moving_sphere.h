#ifndef MOVING_SPHERE_H
#define MOVING_SPHERE_H
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

#include "hittable.h"


class moving_sphere : public hittable {
    public:
        moving_sphere() {}
        moving_sphere(
            glm::dvec3 cen0, glm::dvec3 cen1, double t0, double t1, double r, shared_ptr<material> m)
            : center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(m)
        {};

        virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;
        virtual bool bounding_box(double t0, double t1, aabb& output_box) const;

        glm::dvec3 center(double time) const;

    public:
        glm::dvec3 center0, center1;
        double time0, time1;
        double radius;
        shared_ptr<material> mat_ptr;
};


inline glm::dvec3 moving_sphere::center(double time) const{
    return center0 + ((time - time0) / (time1 - time0))*(center1 - center0);
}


inline bool moving_sphere::bounding_box(double t0, double t1, aabb& output_box) const {
    aabb box0(
        center(t0) - glm::dvec3(radius, radius, radius),
        center(t0) + glm::dvec3(radius, radius, radius));
    aabb box1(
        center(t1) - glm::dvec3(radius, radius, radius),
        center(t1) + glm::dvec3(radius, radius, radius));
    output_box = surrounding_box(box0, box1);
    return true;
}


// replace "center" with "center(r.time())"
inline bool moving_sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    glm::dvec3 oc = r.origin() - center(r.time());
    auto a = glm::length(r.direction())*glm::length(r.direction());
    auto half_b = glm::dot(oc, r.direction());
    auto c = glm::length(oc)*glm::length(oc) - radius*radius;

    auto discriminant = half_b*half_b - a*c;

    if (discriminant > 0) {
        auto root = sqrt(discriminant);

        auto temp = (-half_b - root)/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.at(rec.t);
            glm::dvec3 outward_normal = (rec.p - center(r.time())) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            return true;
        }

        temp = (-half_b + root)/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.at(rec.t);
            glm::dvec3 outward_normal = (rec.p - center(r.time())) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }

    return false;
}

#endif