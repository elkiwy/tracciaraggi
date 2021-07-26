#ifndef __HITTABLE_ABSTRACT_H_
#define __HITTABLE_ABSTRACT_H_

#include <memory>
#include <algorithm>
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
    double u,v;
    bool front_face;

    inline void set_face_normal(const ray& r, const vec3& n){
        this->front_face = dot(r.direction(), n) < 0;
        this->normal = this->front_face ? n : -n;
    }

    inline void write_data(const ray& r, double t, const point3& point, const vec3& outward_normal, shared_ptr<material> material, double u, double v){
        //Set point and time
        this->p = point;
        this->t = t;

        //Set face normal and face sign
        this->set_face_normal(r, outward_normal);

        //Set material
        this->mat_ptr = material;

        //Set uv
        this->u = u;
        this->v = v;
    }
};

inline std::ostream& operator<<(std::ostream &out, const hit_record& r) {
    return out << "{p:" << r.p << ",t:" <<  r.t << ",normal:" <<  r.normal << ",uv:[" <<  r.u << "," <<  r.v << "]}" ;
}




/*
** Hittable abstract class
 */

class hittable{
  public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
    virtual bool bounding_box(aabb& output_box) const = 0;
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
    virtual bool bounding_box(aabb& output_box) const override;

  public:
    vector<shared_ptr<hittable>> objects;
};


///Hit check for hittable lists
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


///Bounding box for hittable list
bool hittable_list::bounding_box(aabb &output_box) const{
    if (objects.empty()) return false;

    aabb temp_box;
    bool first_box = true;
    for(const auto& obj : objects){
        if (!obj->bounding_box(temp_box)) return false;
        output_box = first_box ? temp_box : box_including(output_box, temp_box);
        first_box = false;
    }

    return true;
}











/*
** Bounding volume hierarchy structure
 */

class bvh_node : public hittable{
  public:
    //Constructors
    bvh_node();
    bvh_node(const hittable_list& list) : bvh_node(list.objects, 0, list.objects.size()) {}
    bvh_node(const vector<shared_ptr<hittable>>& src_objects, size_t start, size_t end);

    //Hittable methods
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool bounding_box(aabb& output_box) const override;

  public:
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    aabb box;
};


///Hit check for BVH
bool bvh_node::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    //Check first if the ray hit the bounding box
    if (!box.hit(r, t_min, t_max)) return false;

    //Check wether the ray hits the left node or right node
    bool hit_left = left->hit(r, t_min, t_max, rec);
    bool hit_right = left->hit(r, t_min, hit_left ? rec.t : t_max, rec);
    return hit_left || hit_right;
}


///Bounding box for BVH
bool bvh_node::bounding_box(aabb &output_box) const{
    output_box = box;
    return true;
}

///Box comparators along different axis
inline bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis) {
    aabb box_a, box_b;
    if (!a->bounding_box(box_a) || !b->bounding_box(box_b)) std::cerr << "No bounding box in bvh_node constructor.\n";
    return box_a.min().e[axis] < box_b.min().e[axis];}
bool box_x_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {return box_compare(a, b, 0);}
bool box_y_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {return box_compare(a, b, 1);}
bool box_z_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {return box_compare(a, b, 2);}

///BVH Constructor
bvh_node::bvh_node(const vector<shared_ptr<hittable>>& src_objects, size_t start, size_t end){
    //Create a modifiable version of the src objects
    auto objects = src_objects;

    int axis = random_int(0,2);
    auto comparator = (axis == 0) ? box_x_compare : ((axis == 1) ? box_y_compare : box_z_compare);
    size_t object_span = end - start;
    if (object_span == 1){
        //Set both leaves to point to this object
        left = right = objects[start];
    }else if (object_span == 2){
        //Fit the two objects into the right leaf
        if (comparator(objects[start], objects[start+1])){left  = objects[start]; right = objects[start+1];
        }else{left  = objects[start+1]; right = objects[start];}
    }else{
        //Sort the objects and recursively create other BVH nodes
        std::sort(objects.begin()+start, objects.begin()+end, comparator);
        auto mid = start + object_span/2;
        left  = make_shared<bvh_node>(objects, start, mid);
        right = make_shared<bvh_node>(objects, mid, end);
    }

    //Check that both leaves have a valid bounding box
    aabb box_left, box_right;
    if (!left->bounding_box(box_left) || !right->bounding_box(box_right)){
        std::cerr << "No bounding box in bvh_node constructor";
    }

    //Create the final bounding box
    box = box_including(box_left, box_right);
}









#endif // __HITTABLE_ABSTRACT_H_
