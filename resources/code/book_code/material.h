#ifndef MATERIAL_H
#define MATERIAL_H
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
#include "texture.h"


inline double schlick(double cosine, double ref_idx) {
    double r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1 - cosine),5);
}


class material  {
    public:
        virtual glm::dvec3 emitted(double u, double v, const glm::dvec3& p) const {
            return glm::dvec3(0,0,0);
        }

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, glm::dvec3& attenuation, ray& scattered
        ) const = 0;
};


class dielectric : public material {
    public:
        dielectric(double ri) : ref_idx(ri) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, glm::dvec3& attenuation, ray& scattered
        ) const {
            attenuation = glm::dvec3(1.0, 1.0, 1.0);
            double etai_over_etat = (rec.front_face) ? (1.0 / ref_idx) : (ref_idx);

            glm::dvec3 unit_direction = glm::normalize(r_in.direction());
            double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
            if (etai_over_etat * sin_theta > 1.0 ) {
                glm::dvec3 reflected = glm::reflect(unit_direction, rec.normal);
                scattered = ray(rec.p, reflected, r_in.time());
                return true;
            }

            double reflect_prob = schlick(cos_theta, etai_over_etat);
            if (random_double() < reflect_prob)
            {
                glm::dvec3 reflected = glm::reflect(unit_direction, rec.normal);
                scattered = ray(rec.p, reflected, r_in.time());
                return true;
            }

            glm::dvec3 refracted = glm::refract(unit_direction, rec.normal, etai_over_etat);
            scattered = ray(rec.p, refracted, r_in.time());
            return true;
        }

    public:
        double ref_idx;
};


class diffuse_light : public material {
    public:
        diffuse_light(shared_ptr<texture> a) : emit(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, glm::dvec3& attenuation, ray& scattered
        ) const {
            return false;
        }

        virtual glm::dvec3 emitted(double u, double v, const glm::dvec3& p) const {
            return emit->value(u, v, p);
        }

    public:
        shared_ptr<texture> emit;
};


class isotropic : public material {
    public:
        isotropic(shared_ptr<texture> a) : albedo(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, glm::dvec3& attenuation, ray& scattered
        ) const  {
            scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }

    public:
        shared_ptr<texture> albedo;
};


class lambertian : public material {
    public:
        lambertian(shared_ptr<texture> a) : albedo(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, glm::dvec3& attenuation, ray& scattered
        ) const {
            glm::dvec3 scatter_direction = rec.normal + random_unit_vector();
            scattered = ray(rec.p, scatter_direction, r_in.time());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }

    public:
        shared_ptr<texture> albedo;
};


class metal : public material {
    public:
        metal(const glm::dvec3& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, glm::dvec3& attenuation, ray& scattered
        ) const {
            glm::dvec3 reflected = glm::reflect(glm::normalize(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere(), r_in.time());
            attenuation = albedo;
            return (glm::dot(scattered.direction(), rec.normal) > 0);
        }

    public:
        glm::dvec3 albedo;
        double fuzz;
};


#endif