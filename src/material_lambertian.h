#ifndef __MATERIAL_LAMBERTIAN_H_
#define __MATERIAL_LAMBERTIAN_H_

#include "material_abstract.h"
#include "hittable_abstract.h"

class lambertian : public material{
  public:
    lambertian(const color& a) : albedo(a) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override{
        auto scatter_direction = rec.normal + random_unit_vector();

        //Catch degenerate scatter directions
        if (scatter_direction.near_zero()){
            scatter_direction = rec.normal;
        }

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

  public:
    color albedo;
};


#endif // __MATERIAL_LAMBERTIAN_H_
