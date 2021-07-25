
#ifndef __MATERIAL_LAMBERTIAN_H_
#define __MATERIAL_LAMBERTIAN_H_

#include "hittable_abstract.h"
#include "material_abstract.h"
#include "texture_abstract.h"
#include "texture_solid.h"

class lambertian : public material{
  public:
    lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
    lambertian(shared_ptr<texture> a) : albedo(a) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override{
        //Create the scattered ray
        auto scatter_direction = rec.normal + random_unit_vector();
        if (scatter_direction.near_zero()){scatter_direction = rec.normal;}
        scattered = ray(rec.p, scatter_direction);

        //Get the attenuation color from the texture at the UV point
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

  public:
    shared_ptr<texture> albedo;
};


#endif // __MATERIAL_LAMBERTIAN_H_
