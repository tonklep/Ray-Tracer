//Anthony Klepacki
//Ray Tracing

//Main File

#include <iostream>
#include "sphere.h"
#include "moving_sphere.h"
#include "triangle.h"
#include "hitable_list.h"
#include <float.h>
#include "vec3.h"
#include "camera.h"
#include "ray.h"
#include "random.h"
#include "material.h"
#include "aabb.h"
#include "bvh.h"
#include "perlin.h"
#include "surface_texture.h"
#include "aarect.h"
#include "box.h"
#include "constant_medium.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"




vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
        ray scattered;
        vec3 attenuation;
        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
        if (depth < 10 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
             return emitted + attenuation*color(scattered, world, depth+1);
        else
            return emitted;
    }
    else
        return vec3(0,0,0);
}


    hitable *two_perlin_spheres() {
        texture *pertext = new noise_texture(4);
        hitable **list = new hitable*[2];
        list[0] = new sphere(vec3(0,-1000, 0), 1000, new lambertian(pertext));
        list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
        return new hitable_list(list, 2);
    }
    
    hitable *earth() {
        int nx1, ny1, nn1;
        //unsigned char *tex_data = stbi_load("tiled.jpg", &nx, &ny, &nn, 0);
        unsigned char *tex_data = stbi_load("earthmap.jpg", &nx1, &ny1, &nn1, 0);
        material *mat =  new lambertian(new image_texture(tex_data, nx1, ny1));
        return new sphere(vec3(0,0, 0), 2, mat);
    }
    
    hitable *simple_light() {
        texture *pertext = new noise_texture(4);
        hitable **list = new hitable*[3];
        list[0] = new sphere(vec3(0,-1000, 0), 1000, new lambertian(pertext));
        list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
        //list[2] = new sphere(vec3(0, 7, 0), 2,new diffuse_light(new constant_texture(vec3(4,4,4))));
        list[2] = new xy_rect(3, 5, 1, 3, -2,new diffuse_light(new constant_texture(vec3(4,4,4))));
        return new hitable_list(list,3);
    }
    
    hitable *cornell_box() {
        hitable **list = new hitable*[8];
        int i = 0;
        material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
        material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
        material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
        material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
    
        list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
        list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
        list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
        list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
        list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
        list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
        
        list[i++] = new translate(
                new rotate_y(new box(vec3(0,0,0), vec3(165,165,165), white), -18),
                vec3(130,0,65)
            );
        list[i++] = new translate(
                new rotate_y(new box(vec3(0,0,0), vec3(165,330,165), white), 15),
                vec3(265,0,295)
            );
    
        return new hitable_list(list,i);
    }

    hitable *cornell_smoke() {
        hitable **list = new hitable*[8];
        int i = 0;
        material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
        material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
        material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
        material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));
    
        list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
        list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
        list[i++] = new xz_rect(113, 443, 127, 432, 554, light);
        list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
        list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
        list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    
        hitable *b1 = new translate(
            new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18),
            vec3(130,0,65));
        hitable *b2 = new translate(
            new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white),  15),
            vec3(265,0,295));
    
        list[i++] = new constant_medium(
            b1, 0.01, new constant_texture(vec3(1.0, 1.0, 1.0)));
        list[i++] = new constant_medium(
            b2, 0.01, new constant_texture(vec3(0.0, 0.0, 0.0)));
    
        return new hitable_list(list,i);
    }
    
    
    hitable *final() {
        int nb = 20;
        hitable **list = new hitable*[30];
        hitable **boxlist = new hitable*[10000];
        hitable **boxlist2 = new hitable*[10000];
        material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)));
        material *ground = new lambertian( new constant_texture(vec3(0.48, 0.83, 0.53)));
        int b = 0;
        for (int i = 0; i < nb; i++) {
            for (int j = 0; j < nb; j++) {
                float w = 100;
                float x0 = -1000 + i*w;
                float z0 = -1000 + j*w;
                float y0 = 0;
                float x1 = x0 + w;
                float y1 = 100*(random_double()+0.01);
                float z1 = z0 + w;
                boxlist[b++] = new box(vec3(x0,y0,z0), vec3(x1,y1,z1), ground);
            }
        }
        int l = 0;
        list[l++] = new bvh_node(boxlist, b, 0, 1);
        material *light = new diffuse_light( new constant_texture(vec3(7, 7, 7)));
        list[l++] = new xz_rect(123, 423, 147, 412, 554, light);
        vec3 center(400, 400, 200);
        list[l++] = new moving_sphere(center, center+vec3(30, 0, 0),
            0, 1, 50, new lambertian(new constant_texture(vec3(0.7, 0.3, 0.1))));
        list[l++] = new sphere(vec3(260, 150, 45), 50, new dielectric(1.5));
        list[l++] = new sphere(vec3(0, 150, 145), 50,
            new metal(vec3(0.8, 0.8, 0.9), 10.0));
        hitable *boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
        list[l++] = boundary;
        list[l++] = new constant_medium(boundary, 0.2,
            new constant_texture(vec3(0.2, 0.4, 0.9)));
        boundary = new sphere(vec3(0, 0, 0), 5000, new dielectric(1.5));
        list[l++] = new constant_medium(boundary, 0.0001,
            new constant_texture(vec3(1.0, 1.0, 1.0)));
        int nx, ny, nn;
        unsigned char *tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);
        material *emat =  new lambertian(new image_texture(tex_data, nx, ny));
        list[l++] = new sphere(vec3(400, 200, 400), 100, emat);
        texture *pertext = new noise_texture(0.1);
        list[l++] =  new sphere(vec3(220, 280, 300), 80, new lambertian( pertext ));
        int ns = 1000;
        for (int j = 0; j < ns; j++) {
            boxlist2[j] = new sphere(
                vec3(165*random_double(), 165*random_double(), 165*random_double()),
                10, white);
        }
        list[l++] = new translate(new rotate_y(
            new bvh_node(boxlist2, ns, 0.0, 1.0), 15), vec3(-100,270,395));
        return new hitable_list(list,l);
    }


int main(){
    int x, y, n;

    //Pixel initializations
    //int scaler = 4;
    //int nx = scaler*200;
    //int ny = scaler*100;
    //int ns = 10;
    
    
    int nx = 400;
    int ny = 400;
    int ns = 100;
    
    int pixels = nx*ny;
    
    
    unsigned char data[nx * ny * 3];
    int index = 0;
    
    int counter = 0;
    int counterDashes = 50;

    /*
    hitable *list[5];
    list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.8, 0.3, 0.3)));
    list[1] = new sphere(vec3(0,-100.5,-1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8, 0.6, 0.2),0.5));
    list[3] = new sphere(vec3(-1,0,-1), 0.5, new dielectric(1.5));
    list[4] = new triangle(vec3(1,-0.5,-1),vec3(-1,-0.5,-1),vec3(0,-0.5,-0.5), new metal(vec3(0.5, 0.8, 0.25),0.0));
    hitable *world = new hitable_list(list,5);
    */
    
    hitable *world = final();
    

    
    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278,278,0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    float vfov = 40.0;
    
    camera cam(lookfrom, lookat, vec3(0,1,0), vfov, float(nx)/float(ny),aperture, dist_to_focus, 0.0, 1.0);

    
    // for loop for outputting image information to file
    for (int j = ny-1; j>= 0; j--){
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for (int s=0; s < ns; s++) {
                float u = float(i+random_double())/ float(nx);
                float v = float(j+random_double())/ float(ny);
                ray r = cam.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0);
                col += color(r, world,0);
            }
            col /= float(ns);
            col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
            
            data[index++] = int(255.99*col[0]);
            data[index++] = int(255.99*col[1]);
            data[index++] = int(255.99*col[2]);
            counter++;
        }
        

        //std::cout << "Progress:[";
        printf("Progress:[");
        double progress = (double)counter/pixels;
        for (int d = 0; d < counterDashes; d++){
            
            if (progress <= (double)d/counterDashes){
                //std::cout << "*";
                printf("#");
            }else{
                printf("-");
            }
            //std::cout << progress << std::endl;
        }
        printf("] \n");
        //std::cout << "]  " << progress*100 << "%"<< std::endl;
    }
    // write data to jpg image using stb library
    stbi_write_jpg("foo_out.jpg",nx, ny,3,data,100);
}