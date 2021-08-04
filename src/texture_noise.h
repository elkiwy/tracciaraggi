#ifndef __TEXTURE_NOISE_H_
#define __TEXTURE_NOISE_H_

#include "utils.h"
#include "texture_abstract.h"

class texture_noise : public texture{
  public:
    texture_noise() {}
    texture_noise(double sc) : scale(sc) {}


    virtual color value(double u, double v, const vec3& p) const override{
        ////Uniform turbolence
        return color(1,1,1) * noise.turb(scale * p);
        //Marble like
        //return color(1,1,1) * 0.5 * (1+sin(scale*p.x() + 10*noise.turb(p)));
    }

  private:
    perlin noise;
    double scale;
};


#endif // __TEXTURE_NOISE_H_
