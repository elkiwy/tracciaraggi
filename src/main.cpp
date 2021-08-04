
//Base library
#include <iostream>
#include <math.h>
#include <chrono>

//
#include "extern_stb_image.h"

//Include OpemMP for multithreading
#include <omp.h>



//Project files
#include "utils.h"
#include "objects.h"
#include "camera.h"


//SDL Display
#include "SDL2/SDL.h"
#include "display_controller.h"

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


void write_color_acc(double* pixelsData, const int index, color pixel_color){
    pixelsData[index+0] += pixel_color.x();
    pixelsData[index+1] += pixel_color.y();
    pixelsData[index+2] += pixel_color.z();
}





struct scene{
    hittable_list objects;
    color background;
};


color ray_color(const ray& r, const scene& world, int depth){
    //Limit max recursion
    if (depth<=0){return color(0,0,0);}

    //Check for world collision
    hit_record rec;
    if(!world.objects.hit(r, 0.001, infinity, rec)){return world.background;}

    //Check the scattered ray
    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered)){return emitted;}

    //Recur
    return emitted + attenuation * ray_color(scattered, world, depth-1);
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

    auto marble = make_shared<lambertian>(make_shared<texture_noise>(4));
    world.add(make_shared<sphere>(point3(4, 0.8, 2), 0.8, marble));

    //auto material5 = make_shared<lambertian>(make_shared<texture_image>("src/earthmap.jpg"));
    //world.add(make_shared<sphere>(point3(4, 0.8,-2), 0.8, material5));

    auto material6 = make_shared<material_light>(color(4,4,4));
    //world.add(make_shared<sphere>(point3(4, 4, 0), 2.0, material6));
    //world.add(make_shared<hittable_rect>(point3(-2, 1, -2), point3(2, 4, -2), material6)); //Z aligned
    //world.add(make_shared<hittable_rect>(point3(-2, 1, -2), point3(-2, 4, 2), material6)); // X aligned
    world.add(make_shared<hittable_rect>(point3(-4, 4, -4), point3( 4, 4, 4), material6)); // Y aligned

    return world;
}




void cornell_box(scene* outputScene){
    outputScene->background = color(0.035, 0.025, 0.05);

    //Materials
    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));

    //auto red   = make_shared<metal>(color(.65, .05, .05), 0.5);
    //auto white = make_shared<metal>(color(.73, .73, .73), 0.5);
    //auto green = make_shared<metal>(color(.12, .45, .15), 0.5);

    //Box
    int s = 2;
    outputScene->objects.add(make_shared<hittable_rect>(point3(-s,   0, -s), point3( s,   0, s), white));
    outputScene->objects.add(make_shared<hittable_rect>(point3(-s,   0, -s), point3( s, s*2,-s), white));
    outputScene->objects.add(make_shared<hittable_rect>(point3(-s,   0, -s), point3(-s, s*2, s), red));
    outputScene->objects.add(make_shared<hittable_rect>(point3( s,   0, -s), point3( s, s*2, s), green));
    outputScene->objects.add(make_shared<hittable_rect>(point3(-s, s*2, -s), point3( s, s*2, s), white));

    //Light
    //outputScene->objects.add(make_shared<hittable_rect>(point3(-s*0.5, s*2-0.1, -s*0.5), point3( s*0.5, s*2-0.1, s*0.5), light));

    //Things
    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    outputScene->objects.add(make_shared<hittable_constant_medium>(make_shared<sphere>(point3( 0, 1.0, 0), 9.0, white), 0.10, color(0.1,0.1,0.1)));

    //Metal ball
    auto met = make_shared<metal>(color(0.7, 0.6, 0.5), 0.01);
    outputScene->objects.add(make_shared<sphere>(point3(-1, 2, -1), 0.9, met));

    //Marble ball
    //auto marble = make_shared<lambertian>(make_shared<texture_noise>(16));
    auto marble = make_shared<metal>(make_shared<texture_noise>(8), 0.75);
    outputScene->objects.add(make_shared<sphere>(point3(1, 1, 0), 0.7, marble));

    //Marble ball
    auto light1 = make_shared<material_light>(color(5,15,15));
    auto light2 = make_shared<material_light>(color(15,15,5));
    outputScene->objects.add(make_shared<sphere>(point3( 1.8, 3.6, -1.8), 0.6, light1));
    outputScene->objects.add(make_shared<sphere>(point3(-1.8, 3.6, -1.8), 0.6, light2));

    //Rect
    auto light3 = make_shared<material_light>(color(10,10,10));
    outputScene->objects.add(make_shared<hittable_rect>(point3(-1.75, 0.01, 1.25), point3( 1.75, 0.01, 1.75), light3));
    //rect = make_shared<hittable_rotated>(rect, axis_z, 30);
    //rect = make_shared<hittable_rotated>(rect, axis_x, 60);
    //rect = make_shared<hittable_rotated>(rect, axis_y, 30);
    //rect = make_shared<hittable_translated>(rect, vec3(-1, 0, -1));
    //outputScene->objects.add(rect);
}





void renderScene(double* pixels, const scene& world, const camera& cam, int IMG_WIDTH, int IMG_HEIGHT, int SPP, int MAX_DEPTH, bool accumulative = false){
    const int CHANNELS = 3;

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
                const int PIXEL_INDEX = (i+(j*IMG_WIDTH)) * 3;//3 channels
                write_color_acc(pixels, PIXEL_INDEX, pixel_color);

            }
        }

        //Output feedback of chunk completed
        int thread_id = omp_get_thread_num();
        double thread_end = omp_get_wtime();
        chunksDone++;
        //if(!accumulative){printf("Thread %d (chunk: %d-%d) finished in %f, remains %d chunks.\n", thread_id, k*STEP, (k+1)*STEP, (double)(thread_end - thread_begin), THREADS-chunksDone);}
    }

    //Output total time elapsed
    double end = omp_get_wtime();
    time = (double)(end - begin);
    printf("Time elpased for rendering %f\n", time);
}


/*
int main_renderToFile(int argc, char* argv[]){
    //Image data
    const double RES_MUL = 1;
    const int IMG_WIDTH = 512 * RES_MUL;
    const int IMG_HEIGHT = 512 * RES_MUL;
    const char* OUTPUT_PATH = "output.png";
    const int SPP = 128 * RES_MUL;
    const int MAX_DEPTH = 8;
    unsigned char pixels[IMG_WIDTH * IMG_HEIGHT * 3];

    //Render scene
    scene world;
    cornell_box(&world);

    //Camera
    const vec3 lookfrom = vec3( 0, 2,  10);
    const vec3 lookat   = vec3( 0, 2, 0);
    const vec3 vup      = vec3( 0, 1, 0);
    const double FOV = 30.0;
    const double ASPECT_RATIO = IMG_WIDTH / IMG_HEIGHT;
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;
    camera cam(lookfrom, lookat, vup, FOV, ASPECT_RATIO, aperture, dist_to_focus);
    cout << "Camera created." << endl;

    //Render
    renderScene(pixels, world, cam, IMG_WIDTH, IMG_HEIGHT, SPP, MAX_DEPTH);

    //Flip the final image and save it
    stbi_flip_vertically_on_write(1);
    stbi_write_png(OUTPUT_PATH, IMG_WIDTH, IMG_HEIGHT, 3, pixels, IMG_WIDTH * 3);

    return 0;
}
*/





void saveFrame(uint32_t* screenPixelData, int width, int height, long epoch, int suffix){
    unsigned char pixelData[width * height * 3];
    for (int j=0;j<height;++j){
        for (int i=0;i<width;++i){
            const int ind = (i+j*width);
            pixelData[ind*3+0] = (unsigned char)(screenPixelData[ind]);
            pixelData[ind*3+1] = (unsigned char)(screenPixelData[ind] >> 8);
            pixelData[ind*3+2] = (unsigned char)(screenPixelData[ind] >> 16);
        }
    }

    stbi_flip_vertically_on_write(1);
    string name = "frames/"+ std::to_string(epoch) + "_" + std::to_string(suffix) + ".png";
    stbi_write_png(name.c_str(), width, height, 3, pixelData, width * 3);
    cout<<"Frame '"<<name<<"' saved."<<endl;
}



int main_renderToDisplay(int argc, char *argv[]) {
    //Image data
    const double RES_MUL = 2;
    const int IMG_WIDTH = 512 * RES_MUL;
    const int IMG_HEIGHT = 512 * RES_MUL;
    const int SPP = 2;
    const int MAX_DEPTH = 8;
    double pixelsAcc[IMG_WIDTH * IMG_HEIGHT * 3];
    int samples = 0;

    //Init controller (SDL, Window, etc...)
    DisplayController& controller = DisplayController::getInstance();
    controller.init(IMG_WIDTH, IMG_HEIGHT);
    uint32_t* screenPixelData = controller.getPixelDataPtr();

    //Init scene
    scene world;
    cornell_box(&world);
    cout << "Scene created." << endl;

    //Camera
    const vec3 lookfrom = vec3( 0, 2,  10);
    const vec3 lookat   = vec3( 0, 2, 0);
    const vec3 vup      = vec3( 0, 1, 0);
    const double FOV = 29.0;
    const double ASPECT_RATIO = IMG_WIDTH / IMG_HEIGHT;
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;
    camera cam(lookfrom, lookat, vup, FOV, ASPECT_RATIO, aperture, dist_to_focus);
    cout << "Camera created." << endl;

    //Get epoch
    auto p1 = std::chrono::system_clock::now();
    auto epoch = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();

    //Display loop
    bool quit = false;
    SDL_Event e;
    while(!quit){
        while(SDL_PollEvent(&e) != 0){
            //Quit event
            if(e.type == SDL_QUIT){quit = true;}
        }

        //Keydown event
        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
        controller.onKeyDown(currentKeyStates);

        //Update
        controller.update();

        //Draw
        renderScene(pixelsAcc, world, cam, IMG_WIDTH, IMG_HEIGHT, SPP, MAX_DEPTH, true);
        samples += SPP;
        double gammaScale = 1.0 / samples;
        for (int j=0;j<IMG_HEIGHT;++j){
            for (int i=0;i<IMG_WIDTH;++i){
                int ind = i + (j * IMG_WIDTH);

                //Extract with gamma correction
                double r = sqrt(gammaScale * pixelsAcc[ind*3+0]);
                double g = sqrt(gammaScale * pixelsAcc[ind*3+1]);
                double b = sqrt(gammaScale * pixelsAcc[ind*3+2]);

                screenPixelData[ind] = 0;
                screenPixelData[ind] |= (unsigned char)(256*clamp(r, 0.0, 0.999));
                screenPixelData[ind] |= (unsigned char)(256*clamp(g, 0.0, 0.999)) << 8;
                screenPixelData[ind] |= (unsigned char)(256*clamp(b, 0.0, 0.999)) << 16;
            }
        }
        cout << "Samples: " << samples << endl;

        saveFrame(screenPixelData, IMG_WIDTH, IMG_HEIGHT, epoch, samples / SPP);

        controller.draw();
        controller.drawImGui();
    }

    //Close event
    controller.close();
    return 0;
}








int main(int argc, char *argv[]) {
    //return main_renderToFile(argc, argv);
    return main_renderToDisplay(argc, argv);
}
