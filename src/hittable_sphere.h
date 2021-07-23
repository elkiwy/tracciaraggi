#ifndef __HITTABLE_SPHERE_H_
#define __HITTABLE_SPHERE_H_


#include "hittable_abstract.h"
#include "utils.h"

class material;

class sphere : public hittable{
  public:
    sphere() {}
    sphere(point3 cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

  public:
    point3 center;
    double radius;
    shared_ptr<material> mat_ptr;
};


bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius*radius;
    auto discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    //Find nearest root that lies in the range
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root){
       root = (-half_b + sqrtd) / a;
       if (root < t_min || t_max < root){
           return false;
       }
    }

    //Write the hit record data
    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;
    return true;
}




#endif // __HITTABLE_SPHERE_H_
