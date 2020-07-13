#ifndef HITTABLE_H
#define HITTABLE_H
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

#include "aabb.h"


class material;

inline void get_sphere_uv(const glm::dvec3& p, double& u, double& v) {
    auto phi = atan2(p.z, p.x);
    auto theta = asin(p.y);
    u = 1-(phi + pi) / (2*pi);
    v = (theta + pi/2) / pi;
}


struct hit_record {
    glm::dvec3 p;
    glm::dvec3 normal;
    shared_ptr<material> mat_ptr;
    double t;
    double u;
    double v;
    bool front_face;

    inline void set_face_normal(const ray& r, const glm::dvec3& outward_normal) {
        front_face = glm::dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal :-outward_normal;
    }
};


class hittable {
    public:
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
        virtual bool bounding_box(double t0, double t1, aabb& output_box) const = 0;
};


class flip_face : public hittable {
    public:
        flip_face(shared_ptr<hittable> p) : ptr(p) {}

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
            if (!ptr->hit(r, t_min, t_max, rec))
                return false;

            rec.front_face = !rec.front_face;
            return true;
        }

        virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
            return ptr->bounding_box(t0, t1, output_box);
        }

    public:
        shared_ptr<hittable> ptr;
};


class translate : public hittable {
    public:
        translate(shared_ptr<hittable> p, const glm::dvec3& displacement)
            : ptr(p), offset(displacement) {}

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;
        virtual bool bounding_box(double t0, double t1, aabb& output_box) const;

    public:
        shared_ptr<hittable> ptr;
        glm::dvec3 offset;
};


inline bool translate::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    ray moved_r(r.origin() - offset, r.direction(), r.time());
    if (!ptr->hit(moved_r, t_min, t_max, rec))
        return false;

    rec.p += offset;
    rec.set_face_normal(moved_r, rec.normal);

    return true;
}


inline bool translate::bounding_box(double t0, double t1, aabb& output_box) const {
    if (!ptr->bounding_box(t0, t1, output_box))
        return false;

    output_box = aabb(
        output_box.min() + offset,
        output_box.max() + offset);

    return true;
}


class rotate_y : public hittable {
    public:
        rotate_y(shared_ptr<hittable> p, double angle);

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;
        virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
            output_box = bbox;
            return hasbox;
        }

    public:
        shared_ptr<hittable> ptr;
        double sin_theta;
        double cos_theta;
        bool hasbox;
        aabb bbox;
};


inline rotate_y::rotate_y(shared_ptr<hittable> p, double angle) : ptr(p) {
    auto radians = degrees_to_radians(angle);
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    hasbox = ptr->bounding_box(0, 1, bbox);

    glm::dvec3 min( infinity,  infinity,  infinity);
    glm::dvec3 max(-infinity, -infinity, -infinity);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                auto x = i*bbox.max().x + (1-i)*bbox.min().x;
                auto y = j*bbox.max().y + (1-j)*bbox.min().y;
                auto z = k*bbox.max().z + (1-k)*bbox.min().z;

                auto newx =  cos_theta*x + sin_theta*z;
                auto newz = -sin_theta*x + cos_theta*z;

                glm::dvec3 tester(newx, y, newz);

                for (int c = 0; c < 3; c++) {
					switch (c) {
						case 0:
							min.x = fmin(min.x, tester.x);
							max.x = fmax(max.x, tester.x);
							break;
						case 1:
							min.y = fmin(min.y, tester.y);
							max.y = fmax(max.y, tester.y);
							break;
						case 2:
							min.z = fmin(min.z, tester.z);
							max.z = fmax(max.z, tester.z);
							break;
					}
                }
            }
        }
    }

    bbox = aabb(min, max);
}


inline bool rotate_y::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    glm::dvec3 origin = r.origin();
    glm::dvec3 direction = r.direction();

    origin.x = cos_theta*r.origin().x - sin_theta*r.origin().z;
    origin.z = sin_theta*r.origin().x + cos_theta*r.origin().z;

    direction.x = cos_theta*r.direction().x - sin_theta*r.direction().z;
    direction.z = sin_theta*r.direction().x + cos_theta*r.direction().z;

    ray rotated_r(origin, direction, r.time());

    if (!ptr->hit(rotated_r, t_min, t_max, rec))
        return false;

    glm::dvec3 p = rec.p;
    glm::dvec3 normal = rec.normal;

    p.x =  cos_theta*rec.p.x + sin_theta*rec.p.z;
    p.z = -sin_theta*rec.p.x + cos_theta*rec.p.z;

    normal.x =  cos_theta*rec.normal.x + sin_theta*rec.normal.z;
    normal.z = -sin_theta*rec.normal.x + cos_theta*rec.normal.z;

    rec.p = p;
    rec.set_face_normal(rotated_r, normal);

    return true;
}


#endif
