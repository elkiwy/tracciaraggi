#ifndef __TEXTURE_ABSTRACT_H_
#define __TEXTURE_ABSTRACT_H_


#include "utils.h"

class texture{
  public:
    virtual color value(double u, double v, const point3& p) const = 0;
};



#endif // __TEXTURE_ABSTRACT_H_
