#ifndef __HITTABLE_ABSTRACT_H_
#define __HITTABLE_ABSTRACT_H_

#include <memory>
#include <vector>

#include "utils.h"

using namespace std;

class material;

/*
** Hit record data structure
 */

struct hit_record{
    point3 p;
    vec3 normal;
    shared_ptr<material> mat_ptr;
    double t;
    bool front_face;

    inline void set_face_normal(const ray& r, const vec3& outward_normal){
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};



/*
** Hittable abstract class
 */

class hittable{
  public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};



/*
** Hittable List class
 */

class hittable_list : public hittable{
  public:
    //Constructors
    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    //Functionality
    void clear() { objects.clear(); }
    void add(shared_ptr<hittable> object) { objects.push_back(object); }

    //Hittable methods
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

  public:
    vector<shared_ptr<hittable>> objects;
};

bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    hit_record temp_rec;
    bool hit_anything = false;
    double closest_so_far = t_max;

    for(const auto& object : objects){
        if(object->hit(r, t_min, closest_so_far, temp_rec)){
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}










#endif // __HITTABLE_ABSTRACT_H_