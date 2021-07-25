#ifndef __TEXTURE_SOLID_H_
#define __TEXTURE_SOLID_H_

#include "utils.h"
#include "texture_abstract.h"

class solid_color : public texture{
  public:
    solid_color() {}
    solid_color(color c) : color_value(c) {}
    solid_color(double r, double g, double b): solid_color(color(r,g,b)) {}

    virtual color value(double u, double v, const vec3& p) const override{
        return color_value;
    }

  private:
    color color_value;
};



#endif // __TEXTURE_SOLID_H_
