#ifndef BOOK_CODE
#define BOOK_CODE

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include "includes.h"


// Usings

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

#define color glm::dvec3

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

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
    long unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();

    std::default_random_engine engine{seed};
    std::uniform_real_distribution<double> distribution{0, 1};

    return distribution(engine);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double(min, max+1));
}


inline glm::dvec3 random_in_unit_disk() {
    while (true) {
        auto p = glm::dvec3(random_double(-1,1), random_double(-1,1), 0);
        if (p.length()*p.length() >= 1) continue;
        return p;
    }
}

inline glm::dvec3 random_unit_vector() {
    auto a = random_double(0, 2*pi);
    auto z = random_double(-1, 1);
    auto r = sqrt(1 - z*z);
    return glm::dvec3(r*cos(a), r*sin(a), z);
}

inline glm::dvec3 random_in_unit_sphere() {
    while (true) {
        auto p = glm::dvec3(random_double(), random_double(), random_double());
        if (p.length()*p.length() >= 1) continue;
        return p;
    }
}

inline glm::dvec3 random_in_hemisphere(const glm::dvec3& normal) {
    glm::dvec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

inline glm::dvec3 reflect(const glm::dvec3& v, const glm::dvec3& n) {
    return v - 2*dot(v,n)*n;
}

inline glm::dvec3 refract(const glm::dvec3& uv, const glm::dvec3& n, double etai_over_etat) {
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    glm::dvec3 r_out_parallel =  etai_over_etat * (uv + cos_theta*n);
    glm::dvec3 r_out_perp = -sqrt(1.0 - r_out_parallel.length()*r_out_parallel.length()) * n;
    return r_out_parallel + r_out_perp;
}

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

class aabb {
    public:
        aabb() {}
        aabb(const glm::dvec3& a, const glm::dvec3& b) { _min = a; _max = b; }

        glm::dvec3 min() const {return _min; }
        glm::dvec3 max() const {return _max; }

        bool hit(const ray& r, double tmin, double tmax) const {
            for (int a = 0; a < 3; a++) {
                auto t0 = fmin((_min[a] - r.origin()[a]) / r.direction()[a],
                               (_max[a] - r.origin()[a]) / r.direction()[a]);
                auto t1 = fmax((_min[a] - r.origin()[a]) / r.direction()[a],
                               (_max[a] - r.origin()[a]) / r.direction()[a]);
                tmin = fmax(t0, tmin);
                tmax = fmin(t1, tmax);
                if (tmax <= tmin)
                    return false;
            }
            return true;
        }

        double area() const {
            auto a = _max.x - _min.x;
            auto b = _max.y - _min.y;
            auto c = _max.z - _min.z;
            return 2*(a*b + b*c + c*a);
        }

        int longest_axis() const {
            auto a = _max.x - _min.x;
            auto b = _max.y - _min.y;
            auto c = _max.z - _min.z;
            if (a > b && a > c)
                return 0;
            else if (b > c)
                return 1;
            else
                return 2;
        }

    public:
        glm::dvec3 _min;
        glm::dvec3 _max;
};

aabb surrounding_box(aabb box0, aabb box1);

class material;

void get_sphere_uv(const glm::dvec3& p, double& u, double& v);

struct hit_record {
    glm::dvec3 p;
    glm::dvec3 normal;
    shared_ptr<material> mat_ptr;
    double t;
    double u;
    double v;
    bool front_face;

    inline void set_face_normal(const ray& r, const glm::dvec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
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

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const
		{
		    ray moved_r(r.origin() - offset, r.direction(), r.time());
		    if (!ptr->hit(moved_r, t_min, t_max, rec))
		        return false;

		    rec.p += offset;
		    rec.set_face_normal(moved_r, rec.normal);

		    return true;
		}

        virtual bool bounding_box(double t0, double t1, aabb& output_box) const
		{
		    if (!ptr->bounding_box(t0, t1, output_box))
		        return false;

		    output_box = aabb(
		        output_box.min() + offset,
		        output_box.max() + offset);

		    return true;
		}

    public:
        shared_ptr<hittable> ptr;
        glm::dvec3 offset;
};


class rotate_y : public hittable {
    public:
        rotate_y(shared_ptr<hittable> p, double angle) : ptr(p) {
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
		                    min[c] = fmin(min[c], tester[c]);
		                    max[c] = fmax(max[c], tester[c]);
		                }
		            }
		        }
		    }

		    bbox = aabb(min, max);
		}

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const{
		    glm::dvec3 origin = r.origin();
		    glm::dvec3 direction = r.direction();

		    origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
		    origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];

		    direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
		    direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];

		    ray rotated_r(origin, direction, r.time());

		    if (!ptr->hit(rotated_r, t_min, t_max, rec))
		        return false;

		    glm::dvec3 p = rec.p;
		    glm::dvec3 normal = rec.normal;

		    p[0] =  cos_theta*rec.p[0] + sin_theta*rec.p[2];
		    p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];

		    normal[0] =  cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
		    normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];

		    rec.p = p;
		    rec.set_face_normal(rotated_r, normal);

		    return true;
		}


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


class xy_rect: public hittable {
    public:
        xy_rect() {}

        xy_rect(
            double _x0, double _x1, double _y0, double _y1, double _k, shared_ptr<material> mat
        ) : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {};

        virtual bool hit(const ray& r, double t0, double t1, hit_record& rec) const{
		    auto t = (k-r.origin().z) / r.direction().z;
		    if (t < t0 || t > t1)
		        return false;

		    auto x = r.origin().x + t*r.direction().x;
		    auto y = r.origin().y + t*r.direction().y;
		    if (x < x0 || x > x1 || y < y0 || y > y1)
		        return false;

		    rec.u = (x-x0)/(x1-x0);
		    rec.v = (y-y0)/(y1-y0);
		    rec.t = t;
		    auto outward_normal = glm::dvec3(0, 0, 1);
		    rec.set_face_normal(r, outward_normal);
		    rec.mat_ptr = mp;
		    rec.p = r.at(t);

		    return true;
		}

        virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
            // The bounding box must have non-zero width in each dimension, so pad the Z
            // dimension a small amount.
            output_box = aabb(glm::dvec3(x0,y0, k-0.0001), glm::dvec3(x1, y1, k+0.0001));
            return true;
        }

    public:
        double x0, x1, y0, y1, k;
        shared_ptr<material> mp;
};

class xz_rect: public hittable {
    public:
        xz_rect() {}

        xz_rect(
            double _x0, double _x1, double _z0, double _z1, double _k, shared_ptr<material> mat
        ) : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

        virtual bool hit(const ray& r, double t0, double t1, hit_record& rec) const{
		    auto t = (k-r.origin().y) / r.direction().y;
		    if (t < t0 || t > t1)
		        return false;

		    auto x = r.origin().x + t*r.direction().x;
		    auto z = r.origin().z + t*r.direction().z;
		    if (x < x0 || x > x1 || z < z0 || z > z1)
		        return false;

		    rec.u = (x-x0)/(x1-x0);
		    rec.v = (z-z0)/(z1-z0);
		    rec.t = t;
		    auto outward_normal = glm::dvec3(0, 1, 0);
		    rec.set_face_normal(r, outward_normal);
		    rec.mat_ptr = mp;
		    rec.p = r.at(t);

		    return true;
		}

        virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
            // The bounding box must have non-zero width in each dimension, so pad the Y
            // dimension a small amount.
            output_box = aabb(glm::dvec3(x0,k-0.0001,z0), glm::dvec3(x1, k+0.0001, z1));
            return true;
        }

    public:
        double x0, x1, z0, z1, k;
        shared_ptr<material> mp;
};

class yz_rect: public hittable {
    public:
        yz_rect() {}

        yz_rect(
            double _y0, double _y1, double _z0, double _z1, double _k, shared_ptr<material> mat
        ) : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

        virtual bool hit(const ray& r, double t0, double t1, hit_record& rec) const{
		    auto t = (k-r.origin().x) / r.direction().x;
		    if (t < t0 || t > t1)
		        return false;

		    auto y = r.origin().y + t*r.direction().y;
		    auto z = r.origin().z + t*r.direction().z;
		    if (y < y0 || y > y1 || z < z0 || z > z1)
		        return false;

		    rec.u = (y-y0)/(y1-y0);
		    rec.v = (z-z0)/(z1-z0);
		    rec.t = t;
		    auto outward_normal = glm::dvec3(1, 0, 0);
		    rec.set_face_normal(r, outward_normal);
		    rec.mat_ptr = mp;
		    rec.p = r.at(t);

		    return true;
		}


        virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
            // The bounding box must have non-zero width in each dimension, so pad the X
            // dimension a small amount.
            output_box = aabb(glm::dvec3(k-0.0001, y0, z0), glm::dvec3(k+0.0001, y1, z1));
            return true;
        }

    public:
        double y0, y1, z0, z1, k;
        shared_ptr<material> mp;
};



class hittable_list: public hittable  {
    public:
        hittable_list() {}
        hittable_list(shared_ptr<hittable> object) { add(object); }

        void clear() { objects.clear(); }
        void add(shared_ptr<hittable> object) { objects.push_back(object); }

        virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const{
		    hit_record temp_rec;
		    auto hit_anything = false;
		    auto closest_so_far = tmax;

		    for (const auto& object : objects) {
		        if (object->hit(r, tmin, closest_so_far, temp_rec)) {
		            hit_anything = true;
		            closest_so_far = temp_rec.t;
		            rec = temp_rec;
		        }
		    }

		    return hit_anything;
		}

        virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
		    if (objects.empty()) return false;

		    aabb temp_box;
		    bool first_box = true;

		    for (const auto& object : objects) {
		        if (!object->bounding_box(t0, t1, temp_box)) return false;
		        output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
		        first_box = false;
		    }

		    return true;
		}

    public:
        std::vector<shared_ptr<hittable>> objects;
};


class box: public hittable  {
    public:
        box() {}
        box(const glm::dvec3& p0, const glm::dvec3& p1, shared_ptr<material> ptr){
		    box_min = p0;
		    box_max = p1;

		    sides.add(make_shared<xy_rect>(p0.x, p1.x, p0.y, p1.y, p1.z, ptr));
		    sides.add(make_shared<flip_face>(
		        make_shared<xy_rect>(p0.x, p1.x, p0.y, p1.y, p0.z, ptr)));

		    sides.add(make_shared<xz_rect>(p0.x, p1.x, p0.z, p1.z, p1.y, ptr));
		    sides.add(make_shared<flip_face>(
		        make_shared<xz_rect>(p0.x, p1.x, p0.z, p1.z, p0.y, ptr)));

		    sides.add(make_shared<yz_rect>(p0.y, p1.y, p0.z, p1.z, p1.x, ptr));
		    sides.add(make_shared<flip_face>(
		        make_shared<yz_rect>(p0.y, p1.y, p0.z, p1.z, p0.x, ptr)));
		}


        virtual bool hit(const ray& r, double t0, double t1, hit_record& rec) const
		{
		    return sides.hit(r, t0, t1, rec);
		}

        virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
            output_box = aabb(box_min, box_max);
            return true;
        }

    public:
        glm::dvec3 box_min;
        glm::dvec3 box_max;
        hittable_list sides;
};



class bvh_node : public hittable  {
    public:
        bvh_node();

        bvh_node(hittable_list& list, double time0, double time1)
            : bvh_node(list.objects, 0, list.objects.size(), time0, time1)
        {}

        bvh_node(
            std::vector<shared_ptr<hittable>>& objects,
            size_t start, size_t end, double time0, double time1);

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const
		{
		    if (!box.hit(r, t_min, t_max))
		        return false;

		    bool hit_left = left->hit(r, t_min, t_max, rec);
		    bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

		    return hit_left || hit_right;
		}

        virtual bool bounding_box(double t0, double t1, aabb& output_box) const
		{
		    output_box = box;
		    return true;
		}


    public:
        shared_ptr<hittable> left;
        shared_ptr<hittable> right;
        aabb box;
};



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




class perlin {
    public:
        perlin() {
            ranvec = new glm::dvec3[point_count];
            for (int i = 0; i < point_count; ++i) {
                ranvec[i] = glm::normalize(glm::dvec3(random_double(), random_double(), random_double()));
            }

            perm_x = perlin_generate_perm();
            perm_y = perlin_generate_perm();
            perm_z = perlin_generate_perm();
        }

        ~perlin() {
            delete[] ranvec;
            delete[] perm_x;
            delete[] perm_y;
            delete[] perm_z;
        }

        double noise(const glm::dvec3& p) const {
            auto u = p.x - floor(p.x);
            auto v = p.y - floor(p.y);
            auto w = p.z - floor(p.z);
            auto i = static_cast<int>(floor(p.x));
            auto j = static_cast<int>(floor(p.y));
            auto k = static_cast<int>(floor(p.z));
            glm::dvec3 c[2][2][2];

            for (int di=0; di < 2; di++)
                for (int dj=0; dj < 2; dj++)
                    for (int dk=0; dk < 2; dk++)
                        c[di][dj][dk] = ranvec[
                            perm_x[(i+di) & 255] ^
                            perm_y[(j+dj) & 255] ^
                            perm_z[(k+dk) & 255]
                        ];

            return perlin_interp(c, u, v, w);
        }

        double turb(const glm::dvec3& p, int depth=7) const {
            auto accum = 0.0;
            auto temp_p = p;
            auto weight = 1.0;

            for (int i = 0; i < depth; i++) {
                accum += weight * noise(temp_p);
                weight *= 0.5;
                temp_p *= 2;
            }

            return fabs(accum);
        }

    private:
        static const int point_count = 256;
        glm::dvec3* ranvec;
        int* perm_x;
        int* perm_y;
        int* perm_z;

        static int* perlin_generate_perm() {
            auto p = new int[point_count];

            for (int i = 0; i < point_count; i++)
                p[i] = i;

            permute(p, point_count);

            return p;
        }

        static void permute(int* p, int n) {
            for (int i = n-1; i > 0; i--) {
                int target = random_int(0,i);
                int tmp = p[i];
                p[i] = p[target];
                p[target] = tmp;
            }
        }

        inline static double perlin_interp(glm::dvec3 c[2][2][2], double u, double v, double w) {
            auto uu = u*u*(3-2*u);
            auto vv = v*v*(3-2*v);
            auto ww = w*w*(3-2*w);
            auto accum = 0.0;

            for (int i=0; i < 2; i++)
                for (int j=0; j < 2; j++)
                    for (int k=0; k < 2; k++) {
                        glm::dvec3 weight_v(u-i, v-j, w-k);
                        accum += (i*uu + (1-i)*(1-uu))*
                            (j*vv + (1-j)*(1-vv))*
                            (k*ww + (1-k)*(1-ww))*dot(c[i][j][k], weight_v);
                    }

            return accum;
        }
};


class texture  {
    public:
        virtual color value(double u, double v, const glm::dvec3& p) const = 0;
};


class solid_color : public texture {
    public:
        solid_color() {}
        solid_color(color c) : color_value(c) {}

        solid_color(double red, double green, double blue)
          : solid_color(color(red,green,blue)) {}

        virtual color value(double u, double v, const glm::dvec3& p) const {
            return color_value;
        }

    private:
        color color_value;
};


class checker_texture : public texture {
    public:
        checker_texture() {}
        checker_texture(shared_ptr<texture> t0, shared_ptr<texture> t1): even(t0), odd(t1) {}

        virtual color value(double u, double v, const glm::dvec3& p) const {
            auto sines = sin(10*p.x)*sin(10*p.y)*sin(10*p.z);
            if (sines < 0)
                return odd->value(u, v, p);
            else
                return even->value(u, v, p);
        }

    public:
        shared_ptr<texture> even;
        shared_ptr<texture> odd;
};


class noise_texture : public texture {
    public:
        noise_texture() {}
        noise_texture(double sc) : scale(sc) {}

        virtual color value(double u, double v, const glm::dvec3& p) const {
            // return color(1,1,1)*0.5*(1 + noise.turb(scale * p));
            // return color(1,1,1)*noise.turb(scale * p);
            return color(1,1,1)*0.5*(1 + sin(scale*p.z + 10*noise.turb(p)));
        }

    public:
        perlin noise;
        double scale;
};


class image_texture : public texture {
    public:
        const static int bytes_per_pixel = 3;

        image_texture()
          : data(), width(0), height(0), bytes_per_scanline(0) {}

        image_texture(const char* filename) {
            unsigned error = lodepng::decode(data, width, height, filename);

            if(error) std::cout << "decode error during load(\" "+ std::string(filename) +" \") " << error << ": " << lodepng_error_text(error) << std::endl;

            bytes_per_scanline = bytes_per_pixel * width;
        }

        ~image_texture() {
            // delete data;
            data.clear();
        }

        virtual color value(double u, double v, const glm::dvec3& p) const {
            // If we have no texture data, then return solid cyan as a debugging aid.
            if (data.empty())
                return color(0,1,1);

            // Clamp input texture coordinates to [0,1] x [1,0]
            u = clamp(u, 0.0, 1.0);
            v = 1.0 - clamp(v, 0.0, 1.0);  // Flip V to image coordinates

            auto i = static_cast<unsigned int>(u * width);
            auto j = static_cast<unsigned int>(v * height);

            // Clamp integer mapping, since actual coordinates should be less than 1.0
            if (i >= width)  i = width-1;
            if (j >= height) j = height-1;

            const auto color_scale = 1.0 / 255.0;
            auto pixelr = data[0 + j*bytes_per_scanline + i*bytes_per_pixel];
            auto pixelg = data[1 + j*bytes_per_scanline + i*bytes_per_pixel];
            auto pixelb = data[2 + j*bytes_per_scanline + i*bytes_per_pixel];

            return color(color_scale*pixelr, color_scale*pixelg, color_scale*pixelb);
        }

    private:
        std::vector<unsigned char> data;
        unsigned int width, height;
        int bytes_per_scanline;
};


inline double schlick(double cosine, double ref_idx) {
    double r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1 - cosine),5);
}


class material  {
    public:
        virtual color emitted(double u, double v, const glm::dvec3& p) const {
            return color(0,0,0);
        }

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const = 0;
};


class dielectric : public material {
    public:
        dielectric(double ri) : ref_idx(ri) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const {
            attenuation = color(1.0, 1.0, 1.0);
            double etai_over_etat = (rec.front_face) ? (1.0 / ref_idx) : (ref_idx);

            glm::dvec3 unit_direction = glm::normalize(r_in.direction());
            double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
            if (etai_over_etat * sin_theta > 1.0 ) {
                glm::dvec3 reflected = reflect(unit_direction, rec.normal);
                scattered = ray(rec.p, reflected, r_in.time());
                return true;
            }

            double reflect_prob = schlick(cos_theta, etai_over_etat);
            if (random_double() < reflect_prob)
            {
                glm::dvec3 reflected = reflect(unit_direction, rec.normal);
                scattered = ray(rec.p, reflected, r_in.time());
                return true;
            }

            glm::dvec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
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
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const {
            return false;
        }

        virtual color emitted(double u, double v, const glm::dvec3& p) const {
            return emit->value(u, v, p);
        }

    public:
        shared_ptr<texture> emit;
};


class isotropic : public material {
    public:
        isotropic(shared_ptr<texture> a) : albedo(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
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
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
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
        metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const {
            glm::dvec3 reflected = reflect(glm::normalize(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere(), r_in.time());
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }

    public:
        color albedo;
        double fuzz;
};


class constant_medium : public hittable  {
    public:
        constant_medium(shared_ptr<hittable> b, double d, shared_ptr<texture> a)
            : boundary(b), neg_inv_density(-1/d)
        {
            phase_function = make_shared<isotropic>(a);
        }

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const{
		    // Print occasional samples when debugging. To enable, set enableDebug true.
		    const bool enableDebug = false;
		    const bool debugging = enableDebug && random_double() < 0.00001;

		    hit_record rec1, rec2;

		    if (!boundary->hit(r, -infinity, infinity, rec1))
		        return false;

		    if (!boundary->hit(r, rec1.t+0.0001, infinity, rec2))
		        return false;

		    if (debugging) std::cerr << "\nt0=" << rec1.t << ", t1=" << rec2.t << '\n';

		    if (rec1.t < t_min) rec1.t = t_min;
		    if (rec2.t > t_max) rec2.t = t_max;

		    if (rec1.t >= rec2.t)
		        return false;

		    if (rec1.t < 0)
		        rec1.t = 0;

		    const auto ray_length = r.direction().length();
		    const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
		    const auto hit_distance = neg_inv_density * log(random_double());

		    if (hit_distance > distance_inside_boundary)
		        return false;

		    rec.t = rec1.t + hit_distance / ray_length;
		    rec.p = r.at(rec.t);

		    if (debugging) {
		        std::cerr << "hit_distance = " <<  hit_distance << '\n'
		                  << "rec.t = " <<  rec.t << '\n'
		                  << "rec.p = " <<  rec.p.x << " " << rec.p.y << " " << rec.p.z << '\n';
		    }

		    rec.normal = glm::dvec3(1,0,0);  // arbitrary
		    rec.front_face = true;     // also arbitrary
		    rec.mat_ptr = phase_function;

		    return true;
		}


        virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
            return boundary->bounding_box(t0, t1, output_box);
        }

    public:
        shared_ptr<hittable> boundary;
        shared_ptr<material> phase_function;
        double neg_inv_density;
};


class moving_sphere : public hittable {
    public:
        moving_sphere() {}
        moving_sphere(
            glm::dvec3 cen0, glm::dvec3 cen1, double t0, double t1, double r, shared_ptr<material> m)
            : center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(m)
        {};

        virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const
		{
		   glm::dvec3 oc = r.origin() - center(r.time());
		   auto a = r.direction().length() * r.direction().length();
		   auto half_b = dot(oc, r.direction());
		   auto c = oc.length()*oc.length() - radius*radius;

		   auto discriminant = half_b*half_b - a*c;

		   if (discriminant > 0) {
			   auto root = sqrt(discriminant);

			   auto temp = (-half_b - root)/a;
			   if (temp < tmax && temp > tmin) {
				   rec.t = temp;
				   rec.p = r.at(rec.t);
				   glm::dvec3 outward_normal = (rec.p - center(r.time())) / radius;
				   rec.set_face_normal(r, outward_normal);
				   rec.mat_ptr = mat_ptr;
				   return true;
			   }

			   temp = (-half_b + root)/a;
			   if (temp < tmax && temp > tmin) {
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


        virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
		    aabb box0(
		        center(t0) - glm::dvec3(radius, radius, radius),
		        center(t0) + glm::dvec3(radius, radius, radius));
		    aabb box1(
		        center(t1) - glm::dvec3(radius, radius, radius),
		        center(t1) + glm::dvec3(radius, radius, radius));
		    output_box = surrounding_box(box0, box1);
		    return true;
		}

        glm::dvec3 center(double time) const{
		    return center0 + ((time - time0) / (time1 - time0))*(center1 - center0);
		}

    public:
        glm::dvec3 center0, center1;
        double time0, time1;
        double radius;
        shared_ptr<material> mat_ptr;
};


class sphere: public hittable  {
    public:
        sphere() {}
        sphere(glm::dvec3 cen, double r, shared_ptr<material> m)
            : center(cen), radius(r), mat_ptr(m) {};

        virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const
		{
		    glm::dvec3 oc = r.origin() - center;
		    auto a = r.direction().length() * r.direction().length();
		    auto half_b = dot(oc, r.direction());
		    auto c = oc.length()*oc.length() - radius*radius;

		    auto discriminant = half_b*half_b - a*c;

		    if (discriminant > 0) {
		        auto root = sqrt(discriminant);

		        auto temp = (-half_b - root)/a;
		        if (temp < tmax && temp > tmin) {
		            rec.t = temp;
		            rec.p = r.at(rec.t);
		            glm::dvec3 outward_normal = (rec.p - center) / radius;
		            rec.set_face_normal(r, outward_normal);
		            get_sphere_uv((rec.p-center)/radius, rec.u, rec.v);
		            rec.mat_ptr = mat_ptr;
		            return true;
		        }

		        temp = (-half_b + root)/a;
		        if (temp < tmax && temp > tmin) {
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

        virtual bool bounding_box(double t0, double t1, aabb& output_box) const
		{
		    output_box = aabb(
		        center - glm::dvec3(radius, radius, radius),
		        center + glm::dvec3(radius, radius, radius));
		    return true;
		}

    public:
        glm::dvec3 center;
        double radius;
        shared_ptr<material> mat_ptr;
};

#endif