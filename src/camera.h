#ifndef __CAMERA_H_
#define __CAMERA_H_


#include "utils.h"

class camera{
  public:
    camera(double aspect_ratio){
        const double viewport_h = 2.0;
        const double viewport_w = aspect_ratio * viewport_h;
        const double focal_length = 1.0;

        origin = point3(0, 0, 0);
        horizontal = vec3(viewport_w, 0, 0);
        vertical = vec3(0, viewport_h, 0);
        lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focal_length);
    }

    ray get_ray(double u, double v) const{
        return ray(origin, lower_left_corner + u*horizontal + v*vertical - origin);
    }

  public:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
};

#endif // __CAMERA_H_
