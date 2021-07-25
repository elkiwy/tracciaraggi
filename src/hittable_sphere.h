#ifndef __HITTABLE_SPHERE_H_
#define __HITTABLE_SPHERE_H_


#include "hittable_abstract.h"
#include "utils.h"

class material;

class sphere : public hittable{
  public:
    //Constructors
    sphere() {}
    sphere(point3 cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};

    //Hittable methods
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool bounding_box(aabb& output_box) const override;

    //Utilites
    static uv get_sphere_uv(const point3& p){
        double theta = acos(-p.y());
        double phi = atan2(-p.z(), p.x()) + pi;
        return {phi / (2*pi), theta / pi};
    }

  private:
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
    point3 p = r.at(root);
    point3 normal = (p-center)/radius;
    uv coords = get_sphere_uv(normal);
    rec.write_data(r, root, p, normal, mat_ptr, coords.u, coords.v);

    return true;
}


///Bounding box for sphere
bool sphere::bounding_box(aabb &output_box) const{
    vec3 r = vec3(radius, radius, radius);
    output_box = aabb(center - r, center + r);

    return true;
}










#endif // __HITTABLE_SPHERE_H_
