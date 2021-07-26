#ifndef __HITTABLES_H_
#define __HITTABLES_H_


/*
** Common includes
*/

//Objects
#include "hittable_abstract.h"
#include "hittable_transforms.h"
#include "hittable_volumes.h"
#include "hittable_sphere.h"
#include "hittable_rect.h"

//Materials
#include "material_abstract.h"
#include "material_lambertian.h"
#include "material_metal.h"
#include "material_dielectric.h"
#include "material_light.h"
#include "material_isotropic.h"

//Textures
#include "texture_abstract.h"
#include "texture_solid.h"
#include "texture_checker.h"
#include "texture_noise.h"
#include "texture_image.h"

#endif // __HITTABLES_H_
