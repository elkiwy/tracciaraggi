#ifndef __HITTABLE_TRANSFORMS_H_
#define __HITTABLE_TRANSFORMS_H_


#include "utils.h"
#include "hittable_abstract.h"


/*
** Hittable translated
 */

class hittable_translated : public hittable{
  public:
    //Constructors
    hittable_translated() {}
    hittable_translated(shared_ptr<hittable> object, const vec3& o) : ptr(object), offset(o) { }

    //Hittable methods
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool bounding_box(aabb& output_box) const override;

  public:
    shared_ptr<hittable> ptr;
    vec3 offset;
};

///Hit check for hittable translateds
bool hittable_translated::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    ray moved_r(r.origin() - offset, r.direction());
    if (!ptr->hit(moved_r, t_min, t_max, rec)) return false;

    //Rec data is already written from the ptr->hit test, so we need only to update it
    rec.p += offset;
    rec.set_face_normal(moved_r, rec.normal);
    return true;
}

///Bounding box for hittable translated
bool hittable_translated::bounding_box(aabb &output_box) const{
    if (!ptr->bounding_box(output_box)) return false;
    output_box = aabb(output_box.min() + offset, output_box.max() + offset);
    return true;
}





/*
** Hittable rotated
 */

class hittable_rotated : public hittable{
  public:
    //Constructors
    hittable_rotated() {}
    hittable_rotated(shared_ptr<hittable> object, axis a, double ang);

    //Hittable methods
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool bounding_box(aabb& output_box) const override;

  public:
    shared_ptr<hittable> ptr;
    double sin_theta;
    double cos_theta;
    bool hasbox;
    aabb bbox;
    axis ax;
};


hittable_rotated::hittable_rotated(shared_ptr<hittable> object, axis a, double ang) : ptr(object), ax(a){
    //Cache the sin and cos of the angle
    double rads = deg_to_rad(ang);
    this->sin_theta = sin(rads);
    this->cos_theta = cos(rads);

    //Get the object bbox
    this->hasbox = ptr->bounding_box(bbox);

    //Recalculate the bounding box of the object after its rotation
    point3 max( infinity, infinity, infinity);
    point3 min(-infinity,-infinity,-infinity);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                auto x = i*bbox.max().x() + (1-i)*bbox.min().x();
                auto y = j*bbox.max().y() + (1-j)*bbox.min().y();
                auto z = k*bbox.max().z() + (1-k)*bbox.min().z();

                if (ax == axis_x){
                    auto newy =  cos_theta*y + sin_theta*z;
                    auto newz = -sin_theta*y + cos_theta*z;
                    vec3 tester(x, newy, newz);
                    for (int c = 0; c < 3; c++) {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }else if(ax == axis_y){
                    auto newx =  cos_theta*x + sin_theta*z;
                    auto newz = -sin_theta*x + cos_theta*z;
                    vec3 tester(newx, y, newz);
                    for (int c = 0; c < 3; c++) {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }else if(ax == axis_z){
                    auto newx =  cos_theta*x + sin_theta*y;
                    auto newy = -sin_theta*x + cos_theta*y;
                    vec3 tester(newx, newy, z);
                    for (int c = 0; c < 3; c++) {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }
            }
        }
    }
    this->bbox = aabb(min, max);
}


///Hit check for hittable rotateds
bool hittable_rotated::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    int AX_1 = (this->ax == axis_x) ? 1 : ((this->ax == axis_y) ? 0 : 0);
    int AX_2 = (this->ax == axis_x) ? 2 : ((this->ax == axis_y) ? 2 : 1);

    //Update origin and direction vectors rotating them
    auto origin = r.origin();
    auto direction = r.direction();
    origin[AX_1]    = cos_theta*r.origin()[AX_1]    - sin_theta*r.origin()[AX_2];
    origin[AX_2]    = sin_theta*r.origin()[AX_1]    + cos_theta*r.origin()[AX_2];
    direction[AX_1] = cos_theta*r.direction()[AX_1] - sin_theta*r.direction()[AX_2];
    direction[AX_2] = sin_theta*r.direction()[AX_1] + cos_theta*r.direction()[AX_2];

    //Recreate the rotated ray and re-test
    ray rotated_r(origin, direction);
    if (!ptr->hit(rotated_r, t_min, t_max, rec)) return false;

    //Update the hit record data with the correct one
    auto p = rec.p;
    auto normal = rec.normal;
    p[AX_1] =  cos_theta*rec.p[AX_1] + sin_theta*rec.p[AX_2];
    p[AX_2] = -sin_theta*rec.p[AX_1] + cos_theta*rec.p[AX_2];
    normal[AX_1] =  cos_theta*rec.normal[AX_1] + sin_theta*rec.normal[AX_2];
    normal[AX_2] = -sin_theta*rec.normal[AX_1] + cos_theta*rec.normal[AX_2];
    rec.p = p;
    rec.set_face_normal(rotated_r, normal);
    return true;
}

///Bounding box for hittable rotated
bool hittable_rotated::bounding_box(aabb &output_box) const{
    output_box = bbox;
    return hasbox;
}



#endif // __HITTABLE_TRANSFORMS_H_
