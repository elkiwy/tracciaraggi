#ifndef __HITTABLE_RECT_H_
#define __HITTABLE_RECT_H_


#include "hittable_abstract.h"
#include "utils.h"

class material;

class hittable_rect : public hittable{
  public:
    //Constructors
    hittable_rect() {}

    hittable_rect(const point3& _a, const point3& _b, shared_ptr<material> _m) : a(_a), b(_b), mat_ptr(_m) {
        //Asign the right axis by checking where it's aligned
        if      (_a.x() == _b.x()){ this->ax = yz;}
        else if (_a.y() == _b.y()){ this->ax = xz;}
        else if (_a.z() == _b.z()){ this->ax = xy;}
        else{std::cerr << "WARNING: Rect it's not aligned to any axis " << endl;}

        //Select the right axis
        this->ax_1 = (ax == xy) ? 0 : ((ax == xz) ? 0 : 1);
        this->ax_2 = (ax == xy) ? 1 : ((ax == xz) ? 2 : 2);
        this->ax_k = (ax == xy) ? 2 : ((ax == xz) ? 1 : 0);
    };

    //Hittable methods
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool bounding_box(aabb& output_box) const override;


  private:
    point3 a, b;
    shared_ptr<material> mat_ptr;
    axis ax;

    int ax_1, ax_2, ax_k;

};


bool hittable_rect::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    //Check if ray interesct rect between the aligned axis
    double k = a[ax_k];
    auto t = (k - r.origin()[ax_k]) / r.direction()[ax_k];
    if (t < t_min || t > t_max) return false;

    //Check if it it's the rect
    auto v1 = r.origin()[ax_1] + t*r.direction()[ax_1];
    auto v2 = r.origin()[ax_2] + t*r.direction()[ax_2];
    if (v1 < a[ax_1] || v1 > b[ax_1] || v2 < a[ax_2] || v2 > b[ax_2]) return false;

    //Write the hit data
    double u = (v1 - a[ax_1])/(b[ax_1]-a[ax_1]);
    double v = (v2 - a[ax_2])/(b[ax_2]-a[ax_2]);
    vec3 normal = vec3((int)(ax == yz), (int)(ax == xz), (int)(ax == xy));
    rec.write_data(r, t, r.at(t), normal, this->mat_ptr, u, v);
    return true;
}


///Bounding box for sphere
bool hittable_rect::bounding_box(aabb &output_box) const{
    const vec3 epsilon = vec3(0.0001, 0.0001, 0.0001);
    output_box = aabb(a-epsilon, b+epsilon);
    return true;
}











#endif // __HITTABLE_RECT_H_
