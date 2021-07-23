#ifndef __MATERIAL_METAL_H_
#define __MATERIAL_METAL_H_

#include "material_abstract.h"
#include "hittable_abstract.h"

class metal : public material{
  public:
    metal(const color& a, double f) : albedo(a), fuzz(f) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override{
        //Color derive from albedo
        attenuation = albedo;

        //Create the scattered ray
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());

        //Scatter only if the scattered ray is in the same direction of the normal
        return (dot(scattered.direction(), rec.normal) > 0);
    }


  public:
    color albedo;
    double fuzz;
};


#endif // __MATERIAL_METAL_H_
