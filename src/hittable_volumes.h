#ifndef __HITTABLE_VOLUMES_H_
#define __HITTABLE_VOLUMES_H_



#include "hittable_abstract.h"
#include "texture_abstract.h"
#include "material_abstract.h"
#include "material_isotropic.h"
#include "utils.h"

class material;

class hittable_constant_medium : public hittable{
  public:
    //Constructors
    hittable_constant_medium() {}
    hittable_constant_medium(shared_ptr<hittable> b, double d, shared_ptr<texture> a) : boundary(b), neg_inv_density(-1.0/d), phase_function(make_shared<material_isotropic>(a)) {};
    hittable_constant_medium(shared_ptr<hittable> b, double d, color c) : boundary(b), neg_inv_density(-1.0/d), phase_function(make_shared<material_isotropic>(c)) {};

    //Hittable methods
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool bounding_box(aabb& output_box) const override;

  private:
    shared_ptr<hittable> boundary;
    double neg_inv_density;
    shared_ptr<material> phase_function;
};


//ASSUMES convex shapes, TODO improve for every shape
bool hittable_constant_medium::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    hit_record rec1, rec2;
    if (!boundary->hit(r, -infinity, infinity, rec1)) return false;
    if (!boundary->hit(r, rec1.t+0.0001, infinity, rec2)) return false;


    if (rec1.t < t_min) rec1.t = t_min;
    if (rec2.t > t_max) rec2.t = t_max;
    if (rec1.t >= rec2.t) return false;
    if (rec1.t < 0) rec1.t = 0;

    const double ray_length = r.direction().length();
    const double distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
    const double hit_distance = neg_inv_density * log(random_double());
    if (hit_distance > distance_inside_boundary) return false;

    //Write hit data
    //double t = rec1.t + hit_distance / ray_length;
    //rec.write_data(r, t, r.at(t), vec3(1,0,0), phase_function, rec1.u, rec1.v);
    //rec.normal = vec3(1,0,0);
    //rec.front_face = true;

    rec.t = rec1.t + hit_distance / ray_length;
    rec.p = r.at(rec.t);


    rec.normal = vec3(1,0,0);  // arbitrary
    rec.front_face = true;     // also arbitrary
    rec.mat_ptr = phase_function;


    return true;
}


///Bounding box for hittable_constant_medium
bool hittable_constant_medium::bounding_box(aabb &output_box) const{
    return boundary->bounding_box(output_box);
}











#endif // __HITTABLE_VOLUMES_H_
