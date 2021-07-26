
//Base library
#include <iostream>
#include <math.h>

//
#include "extern_stb_image.h"

//Include OpemMP for multithreading
#include <omp.h>

//Project files
#include "utils.h"
#include "objects.h"
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







color ray_color(const ray& r, const color& background, const hittable& world, int depth){
    //Limit max recursion
    if (depth<=0){return color(0,0,0);}

    //Check for world collision
    hit_record rec;
    if(!world.hit(r, 0.001, infinity, rec)){return background;}

    //Check the scattered ray
    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered)){return emitted;}

    //Recur
    return emitted + attenuation * ray_color(scattered, background, world, depth-1);
}



hittable_list random_scene() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(checker)));

    for (int a = -11; a < 11; a+=4) {
        for (int b = -11; b < 11; b+=4) {
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

    //auto material4 = make_shared<lambertian>(make_shared<texture_noise>(4));
    //world.add(make_shared<sphere>(point3(4, 0.8, 2), 0.8, material4));

    //auto material5 = make_shared<lambertian>(make_shared<texture_image>("src/earthmap.jpg"));
    //world.add(make_shared<sphere>(point3(4, 0.8,-2), 0.8, material5));

    auto material6 = make_shared<material_light>(color(4,4,4));
    //world.add(make_shared<sphere>(point3(4, 4, 0), 2.0, material6));
    //world.add(make_shared<hittable_rect>(point3(-2, 1, -2), point3(2, 4, -2), material6)); //Z aligned
    //world.add(make_shared<hittable_rect>(point3(-2, 1, -2), point3(-2, 4, 2), material6)); // X aligned
    world.add(make_shared<hittable_rect>(point3(-4, 4, -4), point3( 4, 4, 4), material6)); // Y aligned

    return world;
}


hittable_list cornell_box(){
    //Materials
    hittable_list world;
    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<material_light>(color(15,15,15));

    //Box
    int s = 2;
    world.add(make_shared<hittable_rect>(point3(-s,   0, -s), point3( s,   0, s), white));
    world.add(make_shared<hittable_rect>(point3(-s,   0, -s), point3( s, s*2,-s), white));
    world.add(make_shared<hittable_rect>(point3(-s,   0, -s), point3(-s, s*2, s), red));
    world.add(make_shared<hittable_rect>(point3( s,   0, -s), point3( s, s*2, s), green));
    world.add(make_shared<hittable_rect>(point3(-s, s*2, -s), point3( s, s*2, s), white));

    //Light
    world.add(make_shared<hittable_rect>(point3(-s*0.5, s*2-0.1, -s*0.5), point3( s*0.5, s*2-0.1, s*0.5), light));



    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);


    //world.add(make_shared<hittable_constant_medium>(make_shared<sphere>(point3(-2, 1.0, 0), 1.0, white), 0.99, color(0,0,0)));
    world.add(make_shared<hittable_constant_medium>(make_shared<sphere>(point3( 0, 1.0, 0), 9.0, white), 0.10, color(0.1,0.1,0.1)));
    //world.add(make_shared<hittable_constant_medium>(make_shared<sphere>(point3( 2, 1.0, 0), 1.0, white), 2.99, color(0,0,0)));



    shared_ptr<hittable> rect = make_shared<hittable_rect>(point3(-1, 0, 0), point3(1, 2, 0), material3);
    //rect = make_shared<hittable_translated>(rect, vec3(0.5, 0, 0));
    rect = make_shared<hittable_rotated>(rect, axis_z, 30);
    rect = make_shared<hittable_rotated>(rect, axis_x, 60);
    rect = make_shared<hittable_rotated>(rect, axis_y, 30);
    world.add(rect);

    //auto material6 = make_shared<material_light>(color(4,4,4));
    //world.add(make_shared<hittable_rect>(point3(-4, 4, -4), point3( 4, 4, 4), material6)); // Y aligned

    return world;
}


int main(int argc, char *argv[]) {
    //Image data
    const double RES_MUL = 1;
    const int IMG_WIDTH = 512 * RES_MUL;
    const int IMG_HEIGHT = 512 * RES_MUL;
    const int CHANNELS = 3;
    const char* OUTPUT_PATH = "output.png";
    unsigned char pixels[IMG_WIDTH * IMG_HEIGHT * CHANNELS];
    const int SPP = 128 * RES_MUL;
    const int MAX_DEPTH = 8;


    //Scene
    //hittable_list world = random_scene();
    hittable_list world = cornell_box();
    color background = color(0.01, 0.01, 0.01);
    cout << "Scene created." << endl;


    //Camera
    //const vec3 lookfrom = vec3(13, 2, 3);
    const vec3 lookfrom = vec3( 0, 2,  10);
    const vec3 lookat   = vec3( 0, 2, 0);
    const vec3 vup      = vec3( 0, 1, 0);
    const double FOV = 30.0;
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
                    pixel_color += ray_color(r, background, world, MAX_DEPTH);
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
