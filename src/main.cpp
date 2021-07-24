
//Base library
#include <iostream>
#include <math.h>

//STB Library
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image_write.h"
#include "external/stb_image.h"

//Include OpemMP for multithreading
#include <omp.h>

//Project files
#include "utils.h"
#include "hittables.h"
#include "camera.h"

//Namespaces
using namespace std;






void write_color(unsigned char* pixelsData, const int index, color pixel_color, int samples_per_pixel){
    double r = pixel_color.x(); double g = pixel_color.y(); double b = pixel_color.z();

    double scale = 1.0 / samples_per_pixel;
    r = sqrt(scale*r); g = sqrt(scale*g); b = sqrt(scale*b);

    pixelsData[index+0] = (unsigned char)(256*clamp(r, 0.0, 0.999));
    pixelsData[index+1] = (unsigned char)(256*clamp(g, 0.0, 0.999));
    pixelsData[index+2] = (unsigned char)(256*clamp(b, 0.0, 0.999));
}







color ray_color(const ray& r, const hittable& world, int depth){
    //Limit max recursion
    if (depth<=0){return color(0,0,0);}

    //Check for world collision
    hit_record rec;
    if(world.hit(r, 0.001, infinity, rec)){
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth-1);
        return color(0,0,0);
    }

    //Background
    vec3 unit_direction = unit_vector(r.direction());
    double t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0)+t*color(0.5, 0.7, 1.0);
}

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}



int main(int argc, char *argv[]) {
    //Image data
    const int IMG_WIDTH = 512;
    const int IMG_HEIGHT = 256;
    const int CHANNELS = 3;
    const char* OUTPUT_PATH = "output.png";
    unsigned char pixels[IMG_WIDTH * IMG_HEIGHT * CHANNELS];
    const int SPP = 32;
    const int MAX_DEPTH = 8;


    //Scene
    hittable_list world = random_scene();
    cout << "Scene created." << endl;


    //Camera
    const vec3 lookfrom = vec3(13, 2, 3);
    const vec3 lookat   = vec3( 0, 0, 0);
    const vec3 vup      = vec3( 0, 1, 0);
    const double FOV = 20.0;
    const double ASPECT_RATIO = IMG_WIDTH / IMG_HEIGHT;
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;
    camera cam(lookfrom, lookat, vup, FOV, ASPECT_RATIO, aperture, dist_to_focus);
    cout << "Camera created." << endl;


    //Multithreading
    double time = 0.0;
    double begin = omp_get_wtime();
    const unsigned int THREADS = 16;
    const int STEP = floor(IMG_HEIGHT/THREADS);
    int chunksDone = 0;

    //Render all the chunks
    #pragma omp parallel for
    for(unsigned int k=0; k<THREADS; ++k){
        const double thread_begin = 0.0;

        //Cycle all the rows in this chunk
        for(int sj=STEP-1; sj>=0; --sj){
            int j = sj+(k*STEP);
            //Cycle each pixel in this row
            for(int i=0; i<IMG_WIDTH; ++i){
                //Accumulate samples for this pixel
                color pixel_color(0,0,0);
                for(int s=0; s<SPP; ++s){
                    const double u = (i + random_double()) / (IMG_WIDTH-1);
                    const double v = (j + random_double()) / (IMG_HEIGHT-1);
                    ray r = cam.get_ray(u, v);
                    pixel_color += ray_color(r, world, MAX_DEPTH);
                }

                //Output the color into the right pixel
                const int PIXEL_INDEX = (i+(j*IMG_WIDTH)) * CHANNELS;
                write_color(pixels, PIXEL_INDEX, pixel_color, SPP);
            }
        }

        //Output feedback of chunk completed
        int thread_id = omp_get_thread_num();
        double thread_end = omp_get_wtime();
        chunksDone++;
        printf("Thread %d (chunk: %d-%d) finished in %f, remains %d chunks.\n", thread_id, k*STEP, (k+1)*STEP, (double)(thread_end - thread_begin), THREADS-chunksDone);
    }

    //Flip the final image and save it
    stbi_flip_vertically_on_write(1);
    stbi_write_png(OUTPUT_PATH, IMG_WIDTH, IMG_HEIGHT, CHANNELS, pixels, IMG_WIDTH * CHANNELS);

    //Output total time elapsed
    double end = omp_get_wtime();
    time = (double)(end - begin);
    printf("Time elpased for rendering %f\n", time);


    return 0;
}
