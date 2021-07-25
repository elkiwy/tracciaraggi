#ifndef __TEXTURE_IMAGE_H_
#define __TEXTURE_IMAGE_H_

//Base Library
#include <iostream>

//STB
#include "extern_stb_image.h"

//Project files
#include "utils.h"
#include "texture_abstract.h"




class texture_image : public texture{
  public:
    const static int bytes_per_pixel = 3;

    texture_image():data(nullptr),width(0),height(0),bytes_per_scanline(0) {}

    texture_image(const char* filename){
        auto components_per_pixel = bytes_per_pixel;
        data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);
        if(!this->data){std::cerr << "ERROR: Could not load texture image file '"<<filename<<"'.\n"; width = height = 0;}
        bytes_per_scanline = bytes_per_pixel * width;
        std::cout << "Loaded image with size: " << width << " x " << height << " ; " << bytes_per_scanline << std::endl;
    }

    ~texture_image(){
        delete data;
    }


    virtual color value(double u, double v, const vec3& p) const override{
        if (data == nullptr){return color(0,1,1);}

        //Clamp input coords into 0,1 x 1,0
        u = clamp(u, 0.0, 1.0);
        v = 1.0 - clamp(v, 0.0, 1.0); //Flip coords

        //Map UV to width and height of the image
        auto i = static_cast<int>(u * width);
        auto j = static_cast<int>(v * height);
        if (i >= width ) i = width - 1;
        if (j >= height) j = height - 1;

        //Get the pixel and return its color
        const auto color_scale = 1.0 / 255.0;
        auto pixel = data + j*bytes_per_scanline + i*bytes_per_pixel;
        return color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
    }


  private:
    unsigned char* data;
    int width, height;
    int bytes_per_scanline;
};



#endif // __TEXTURE_IMAGE_H_
