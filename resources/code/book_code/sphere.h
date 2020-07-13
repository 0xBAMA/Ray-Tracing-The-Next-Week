#ifndef SPHERE_H
#define SPHERE_H
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


class sphere: public hittable  {
    public:
        sphere() {}
        sphere(glm::dvec3 cen, double r, shared_ptr<material> m)
            : center(cen), radius(r), mat_ptr(m) {};
        virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;
        virtual bool bounding_box(double t0, double t1, aabb& output_box) const;

    public:
        glm::dvec3 center;
        double radius;
        shared_ptr<material> mat_ptr;
};


inline bool sphere::bounding_box(double t0, double t1, aabb& output_box) const {
    output_box = aabb(
        center - glm::dvec3(radius, radius, radius),
        center + glm::dvec3(radius, radius, radius));
    return true;
}

inline bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    glm::dvec3 oc = r.origin() - center;
    auto a = glm::length(r.direction())*glm::length(r.direction());
    auto half_b = dot(oc, r.direction());
    auto c = glm::length(oc)*glm::length(oc) - radius*radius;

    auto discriminant = half_b*half_b - a*c;

    if (discriminant > 0) {
        auto root = sqrt(discriminant);

        auto temp = (-half_b - root)/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.at(rec.t);
            glm::dvec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            get_sphere_uv((rec.p-center)/radius, rec.u, rec.v);
            rec.mat_ptr = mat_ptr;
            return true;
        }

        temp = (-half_b + root)/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.at(rec.t);
            glm::dvec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            get_sphere_uv((rec.p-center)/radius, rec.u, rec.v);
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }

    return false;
}

#endif
