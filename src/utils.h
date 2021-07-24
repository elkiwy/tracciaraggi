#ifndef __UTILS_H_
#define __UTILS_H_


#include <math.h>
#include <limits>
#include <memory>

/*
** Usings
*/
using std::shared_ptr;
using std::make_shared;



/*
** Constants
*/
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;



/*
** Utility functions
*/

///Degrees to radians
inline double deg_to_rad(double deg){
    return deg * pi / 180.0;
}

///Clamp function
inline double clamp(double x, double min, double max){
    if (x<min) return min;
    if (x>max) return max;
    return x;
}



/*
** Common headers
*/
///Returns a random number in [0,1)
inline double random_double(){
    return rand()/(RAND_MAX + 1.0);
}

///Returns a random number between [min,max)
inline double random_double(double min, double max){
    return min+(max-min)*random_double();
}

///Returns a random integer in [min, max]
inline int random_int(int min, int max){
    return static_cast<int>(random_double(min, max+1));
}


/*
** Common headers
*/
#include "ray.h"
#include "utils_vec3.h"
#include "utils_aabb.h"


#endif // __UTILS_H_
