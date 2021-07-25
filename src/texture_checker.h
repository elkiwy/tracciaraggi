#ifndef __TEXTURE_CHECKER_H_
#define __TEXTURE_CHECKER_H_


#include "utils.h"
#include "texture_abstract.h"
#include "texture_solid.h"

class checker_texture : public texture{
  public:
    checker_texture() {}
    checker_texture(shared_ptr<texture> _even, shared_ptr<texture> _odd) : even(_even), odd(_odd) {}
    checker_texture(color c1, color c2) : even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)) {}

    virtual color value(double u, double v, const point3& p) const override{
        auto sines = sin(10*p.x())*sin(10*p.y())*sin(10*p.z());
        return (sines < 0 ? even : odd)->value(u, v, p);
    }

  private:
    shared_ptr<texture> even;
    shared_ptr<texture> odd;
};





#endif // __TEXTURE_CHECKER_H_
