#ifndef __MATERIAL_ISOTROPIC_H_
#define __MATERIAL_ISOTROPIC_H_


#include "hittable_abstract.h"
#include "material_abstract.h"
#include "texture_abstract.h"
#include "texture_solid.h"
#include "utils.h"

class material_isotropic : public material{
  public:
    material_isotropic(const color& a) : albedo(make_shared<solid_color>(a)) {}
    material_isotropic(shared_ptr<texture> a) : albedo(a) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override{
        //Scatter the ray in a random direction
        scattered = ray(rec.p, random_in_unit_sphere());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

  public:
    shared_ptr<texture> albedo;
};


#endif // __MATERIAL_ISOTROPIC_H_
