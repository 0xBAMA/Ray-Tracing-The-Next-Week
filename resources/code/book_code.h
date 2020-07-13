#include "book_code/rtweekend.h"
#include "book_code/box.h"
#include "book_code/bvh.h"
#include "book_code/camera.h"
#include "book_code/color.h"
#include "book_code/constant_medium.h"
#include "book_code/hittable_list.h"
#include "book_code/material.h"
#include "book_code/moving_sphere.h"
#include "book_code/sphere.h"
#include "book_code/texture.h"


inline hittable_list random_scene() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(
        make_shared<solid_color>(0.2, 0.3, 0.1),
        make_shared<solid_color>(0.9, 0.9, 0.9)
    );

    world.add(make_shared<sphere>(glm::dvec3(0,-1000,0), 1000, make_shared<lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            glm::dvec3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - glm::dvec3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = glm::dvec3(random_double()*random_double(), random_double()*random_double(), random_double()*random_double());
                    sphere_material = make_shared<lambertian>(make_shared<solid_color>(albedo));
                    auto center2 = center + glm::dvec3(0, random_double(0,.5), 0);
                    world.add(make_shared<moving_sphere>(
                        center, center2, 0.0, 1.0, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = glm::dvec3(random_double(0.5, 1), random_double(0.5, 1), random_double(0.5, 1));
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(glm::dvec3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(make_shared<solid_color>(glm::dvec3(0.4, 0.2, 0.1)));
    world.add(make_shared<sphere>(glm::dvec3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(glm::dvec3(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(glm::dvec3(4, 1, 0), 1.0, material3));

    return hittable_list(make_shared<bvh_node>(world, 0.0, 1.0));
}


inline hittable_list two_spheres() {
    hittable_list objects;

    auto checker = make_shared<checker_texture>(
        make_shared<solid_color>(0.2, 0.3, 0.1),
        make_shared<solid_color>(0.9, 0.9, 0.9)
    );

    objects.add(make_shared<sphere>(glm::dvec3(0,-10, 0), 10, make_shared<lambertian>(checker)));
    objects.add(make_shared<sphere>(glm::dvec3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    return objects;
}


inline hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(glm::dvec3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(glm::dvec3(0,2,0), 2, make_shared<lambertian>(pertext)));

    return objects;
}


inline hittable_list earth() {
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(glm::dvec3(0,0,0), 2, earth_surface);

    return hittable_list(globe);
}


inline hittable_list simple_light() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(glm::dvec3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(glm::dvec3(0,2,0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(make_shared<solid_color>(4,4,4));
    objects.add(make_shared<sphere>(glm::dvec3(0,7,0), 2, difflight));
    objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));

    return objects;
}


inline hittable_list cornell_box() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(make_shared<solid_color>(.65, .05, .05));
    auto white = make_shared<lambertian>(make_shared<solid_color>(.73, .73, .73));
    auto green = make_shared<lambertian>(make_shared<solid_color>(.12, .45, .15));
    auto light = make_shared<diffuse_light>(make_shared<solid_color>(15,15,15));

    objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));

    shared_ptr<hittable> box1 = make_shared<box>(glm::dvec3(0,0,0), glm::dvec3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1,  15);
    box1 = make_shared<translate>(box1, glm::dvec3(265,0,295));
    objects.add(box1);

    shared_ptr<hittable> box2 = make_shared<box>(glm::dvec3(0,0,0), glm::dvec3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, glm::dvec3(130,0,65));
    objects.add(box2);

    return objects;
}


inline hittable_list cornell_balls() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(make_shared<solid_color>(.65, .05, .05));
    auto white = make_shared<lambertian>(make_shared<solid_color>(.73, .73, .73));
    auto green = make_shared<lambertian>(make_shared<solid_color>(.12, .45, .15));
    auto light = make_shared<diffuse_light>(make_shared<solid_color>(5,5,5));

    objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));

    auto boundary = make_shared<sphere>(glm::dvec3(160,100,145), 100, make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(boundary, 0.1, make_shared<solid_color>(1,1,1)));

    shared_ptr<hittable> box1 = make_shared<box>(glm::dvec3(0,0,0), glm::dvec3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, glm::dvec3(265,0,295));
    objects.add(box1);

    return objects;
}


inline hittable_list cornell_smoke() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(make_shared<solid_color>(.65, .05, .05));
    auto white = make_shared<lambertian>(make_shared<solid_color>(.73, .73, .73));
    auto green = make_shared<lambertian>(make_shared<solid_color>(.12, .45, .15));
    auto light = make_shared<diffuse_light>(make_shared<solid_color>(7, 7, 7));

    objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));

    shared_ptr<hittable> box1 = make_shared<box>(glm::dvec3(0,0,0), glm::dvec3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1,  15);
    box1 = make_shared<translate>(box1, glm::dvec3(265,0,295));

    shared_ptr<hittable> box2 = make_shared<box>(glm::dvec3(0,0,0), glm::dvec3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, glm::dvec3(130,0,65));

    objects.add(make_shared<constant_medium>(box1, 0.01, make_shared<solid_color>(0,0,0)));
    objects.add(make_shared<constant_medium>(box2, 0.01, make_shared<solid_color>(1,1,1)));

    return objects;
}


inline hittable_list cornell_final() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(0.1);

    auto mat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));

    auto red   = make_shared<lambertian>(make_shared<solid_color>(.65, .05, .05));
    auto white = make_shared<lambertian>(make_shared<solid_color>(.73, .73, .73));
    auto green = make_shared<lambertian>(make_shared<solid_color>(.12, .45, .15));
    auto light = make_shared<diffuse_light>(make_shared<solid_color>(7, 7, 7));

    objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));

    shared_ptr<hittable> boundary2 =
        make_shared<box>(glm::dvec3(0,0,0), glm::dvec3(165,165,165), make_shared<dielectric>(1.5));
    boundary2 = make_shared<rotate_y>(boundary2, -18);
    boundary2 = make_shared<translate>(boundary2, glm::dvec3(130,0,65));

    auto tex = make_shared<solid_color>(0.9, 0.9, 0.9);

    objects.add(boundary2);
    objects.add(make_shared<constant_medium>(boundary2, 0.2, tex));

    return objects;
}


inline hittable_list final_scene() {
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(make_shared<solid_color>(0.48, 0.83, 0.53));

    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(make_shared<box>(glm::dvec3(x0,y0,z0), glm::dvec3(x1,y1,z1), ground));
        }
    }

    hittable_list objects;

    objects.add(make_shared<bvh_node>(boxes1, 0, 1));

    auto light = make_shared<diffuse_light>(make_shared<solid_color>(7, 7, 7));
    objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));

    auto center1 = glm::dvec3(400, 400, 200);
    auto center2 = center1 + glm::dvec3(30,0,0);
    auto moving_sphere_material =
        make_shared<lambertian>(make_shared<solid_color>(0.7, 0.3, 0.1));
    objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

    objects.add(make_shared<sphere>(glm::dvec3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    objects.add(make_shared<sphere>(
        glm::dvec3(0, 150, 145), 50, make_shared<metal>(glm::dvec3(0.8, 0.8, 0.9), 10.0)
    ));

    auto boundary = make_shared<sphere>(glm::dvec3(360,150,145), 70, make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(
        boundary, 0.2, make_shared<solid_color>(0.2, 0.4, 0.9)
    ));
    boundary = make_shared<sphere>(glm::dvec3(0,0,0), 5000, make_shared<dielectric>(1.5));
    objects.add(make_shared<constant_medium>(boundary, .0001, make_shared<solid_color>(1,1,1)));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
    objects.add(make_shared<sphere>(glm::dvec3(400,200,400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.1);
    objects.add(make_shared<sphere>(glm::dvec3(220,280,300), 80, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(make_shared<solid_color>(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(glm::dvec3(random_double(0,165), random_double(0,165), random_double(0,165)), 10, white));
    }

    objects.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2, 0.0, 1.0), 15),
            glm::dvec3(-100,270,395)
        )
    );

    return objects;
}
