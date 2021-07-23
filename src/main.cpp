
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




int main(int argc, char *argv[]) {
    //Image data
    const int IMG_WIDTH = 512;
    const int IMG_HEIGHT = 256;
    const int CHANNELS = 3;
    const char* OUTPUT_PATH = "output.png";
    unsigned char pixels[IMG_WIDTH * IMG_HEIGHT * CHANNELS];
    const int SPP = 32;
    const int MAX_DEPTH = 4;


    //Scene
    hittable_list world;
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
    auto material_left   = make_shared<dielectric>(1.5);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    //world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    //world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));

    world.add(make_shared<sphere>(point3(0.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<sphere>(point3(0.0,    0.0, -1.0),  -0.4, material_left));

    //world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
    cout << "Scene created." << endl;


    //Camera
    camera cam(IMG_WIDTH / IMG_HEIGHT);
    cout << "Camera created." << endl;


    //Multithreading
    double time = 0.0;
    double begin = omp_get_wtime();
    const unsigned int THREADS = 16;
    const int STEP = floor(IMG_HEIGHT/THREADS);
    int chunksDone = 0;

    //Render all the chunks
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
        const int thread_id = 0;
        const double thread_end = 0.0;
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
