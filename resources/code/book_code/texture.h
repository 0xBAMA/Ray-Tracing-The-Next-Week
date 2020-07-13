#ifndef TEXTURE_H
#define TEXTURE_H
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

#include "perlin.h"
#include "../includes.h"

#include <iostream>


class texture  {
    public:
        virtual glm::dvec3 value(double u, double v, const glm::dvec3& p) const = 0;
};


class solid_color : public texture {
    public:
        solid_color() {}
        solid_color(glm::dvec3 c) : color_value(c) {}

        solid_color(double red, double green, double blue)
          : solid_color(glm::dvec3(red,green,blue)) {}

        virtual glm::dvec3 value(double u, double v, const glm::dvec3& p) const {
            return color_value;
        }

    private:
        glm::dvec3 color_value;
};


class checker_texture : public texture {
    public:
        checker_texture() {}
        checker_texture(shared_ptr<texture> t0, shared_ptr<texture> t1): even(t0), odd(t1) {}

        virtual glm::dvec3 value(double u, double v, const glm::dvec3& p) const {
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

        virtual glm::dvec3 value(double u, double v, const glm::dvec3& p) const {
            // return color(1,1,1)*0.5*(1 + noise.turb(scale * p));
            // return color(1,1,1)*noise.turb(scale * p);
            return glm::dvec3(1,1,1)*0.5*(1 + sin(scale*p.z + 10*noise.turb(p)));
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
            data.clear();
        }

        virtual glm::dvec3 value(double u, double v, const glm::dvec3& p) const {
            // If we have no texture data, then return solid cyan as a debugging aid.
            if (data.empty())
                return glm::dvec3(0,1,1);

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

            return glm::dvec3(color_scale*pixelr, color_scale*pixelg, color_scale*pixelb);
        }

    private:
        std::vector<unsigned char> data;
        unsigned int width, height;
        int bytes_per_scanline;
};


#endif
