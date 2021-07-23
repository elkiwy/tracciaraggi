
#ifndef __MATERIAL_ABSTRACT_H_
#define __MATERIAL_ABSTRACT_H_

#include "utils.h"

struct hit_record;

class material{
  public:
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
};



#endif // __MATERIAL_ABSTRACT_H_
