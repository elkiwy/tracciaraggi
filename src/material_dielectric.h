#ifndef __MATERIAL_DIELECTRIC_H_
#define __MATERIAL_DIELECTRIC_H_

#include "material_abstract.h"
#include "hittable_abstract.h"

class dielectric : public material{
  public:
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override{
        //Color is always mantained
        attenuation = color(1.0, 1.0, 1.0);

        //Check if this ray can be refracted by the surface
        double refraction_ratio = rec.front_face ? (1.0/ir) : ir;
        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
        bool cannot_refract = refraction_ratio * sin_theta > 1.0;

        //Create the scattered ray either by reflecting or refracting it
        vec3 direction;
        bool should_reflect = cannot_refract || (reflectance(cos_theta, refraction_ratio) > random_double());
        if (should_reflect){direction = reflect(unit_direction, rec.normal);
        }else{direction = refract(unit_direction, rec.normal, refraction_ratio);}
        scattered = ray(rec.p, direction);

        //Always scatter ray
        return true;
    }


  private:
    static double reflectance(double cosine, double ref_idx){
        auto r0 = (1-ref_idx) / (1+ref_idx);
        r0 = r0*r0;
        return r0 + (1-r0)*pow((1 - cosine), 5.0);
    }


  public:
    double ir;
};


#endif // __MATERIAL_DIELECTRIC_H_
