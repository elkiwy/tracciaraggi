#ifndef __MATERIAL_LIGHT_H_
#define __MATERIAL_LIGHT_H_


#include "hittable_abstract.h"
#include "material_abstract.h"
#include "texture_abstract.h"
#include "texture_solid.h"

class material_light : public material{
  public:
    material_light(shared_ptr<texture> a) : emit(a) {}
    material_light(color c) : emit(make_shared<solid_color>(c)) {}


    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override{
        return false;
    }

    virtual color emitted(double u, double v, const point3& p) const override{
        return emit->value(u, v, p);
    }


  public:
    shared_ptr<texture> emit;
};


#endif // __MATERIAL_LIGHT_H_
