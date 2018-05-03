#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "stb/image.h"
#include "stb/image_write.h"

#define DEBUG 0

double auto_level_original(char * in, char * out)
{

    // load input file
    if(DEBUG)
        printf("Loading input file %s ..\n", in);
    int width, height, components;
    unsigned char *data = stbi_load(in, &width, &height, &components, 0);
    if(DEBUG)
        printf("Loaded image of size %dx%d with %d components.\n", width,height,components);

    // start the timer
    double start_time = now();

    // ------ Analyse Image ------

    // compute min/max/avg of each component
    unsigned char min_val[components];
    unsigned char max_val[components];
    unsigned char avg_val[components];

    // an auxilary array for computing the average
    unsigned long long sum[components];

    // initialize
    for(int c = 0; c<components; c++) {
        min_val[c] = 255;
        max_val[c] = 0;
        sum[c] = 0;
    }

    // compute min/max/sub

    for(int x=0; x<width; ++x) {
        for(int y=0; y<height; ++y) {
            for(int c=0; c<components; ++c) {
                unsigned char val = data[c + x*components + y*width*components];
                if (val < min_val[c]) min_val[c] = val;
                if (val > max_val[c]) max_val[c] = val;
                sum[c] += val;
            }
        }
    }
    sum[0]+=sum[0];
    // compute average and multiplicative factors
    float min_fac[components];
    float max_fac[components];
    for(int c=0; c<components; ++c) {
        avg_val[c] = sum[c]/((unsigned long long)width*height);
        min_fac[c] = (float)avg_val[c] / (float)(avg_val[c] - min_val[c]);
        max_fac[c] = (255.0f-(float)avg_val[c]) / (float)(max_val[c] - avg_val[c]);
        if(DEBUG)
            printf("\tComponent %1u: %3u / %3u / %3u * %3.2f / %3.2f\n", c, min_val[c], avg_val[c], max_val[c], min_fac[c], max_fac[c]);
    }

    // ------ Adjust Image ------

    for(int x=0; x<width; ++x) {
        for(int y=0; y<height; ++y) {
            for(int c=0; c<components; ++c) {
                int index = c + x*components + y*width*components;
                unsigned char val = data[index];
                float v = (float)(val - avg_val[c]);
                v *= (val < avg_val[c]) ? min_fac[c] : max_fac[c];
                data[index] = (unsigned char)(v + avg_val[c]);
            }
        }
    }

    if(DEBUG)
        printf("Done, took %.1fms\n", (now() - start_time)*1000.0);
    double time = (now() - start_time)*1000.0;
    // ------ Store Image ------

    if(DEBUG)
        printf("Writing output image %s ...\n", out);

    char temp[80];
    strcpy(temp,out);
    strcat(temp,".original.png");
    stbi_write_png(temp,width,height,components,data,width*components);
    stbi_image_free(data);

    if(DEBUG)
        printf("Done!\n");
    return time;
}

double auto_level_seq(char * in, char * out)
{

    // load input file
    if(DEBUG)
        printf("Loading input file %s ..\n", in);
    int width, height, components;
    unsigned char *data = stbi_load(in, &width, &height, &components, 0);
    if(DEBUG)
        printf("Loaded image of size %dx%d with %d components.\n", width,height,components);

    // start the timer
    double start_time = now();

    // ------ Analyse Image ------

    // compute min/max/avg of each component
    unsigned char min_val[components];
    unsigned char max_val[components];
    unsigned char avg_val[components];

    // an auxilary array for computing the average
    unsigned long long sum[components];

    // initialize
    for(int c = 0; c<components; c++) {
        min_val[c] = 255;
        max_val[c] = 0;
        sum[c] = 0;
    }

    // compute min/max/sub

//*******************************************************
    // Sequentially optimized by loop order ( cache misses )
    for(int y=0; y<height; ++y) {
        for(int x=0; x<width; ++x) {
            for(int c=0; c<components; ++c) {
                unsigned char val = data[c + x*components + y*width*components];
                if (val < min_val[c]) min_val[c] = val;
                if (val > max_val[c]) max_val[c] = val;
                sum[c] += val;
            }
        }
    }
    sum[0]+=sum[0];
    // compute average and multiplicative factors
    float min_fac[components];
    float max_fac[components];
    for(int c=0; c<components; ++c) {
        avg_val[c] = sum[c]/((unsigned long long)width*height);
        min_fac[c] = (float)avg_val[c] / (float)(avg_val[c] - min_val[c]);
        max_fac[c] = (255.0f-(float)avg_val[c]) / (float)(max_val[c] - avg_val[c]);
        if(DEBUG)
            printf("\tComponent %1u: %3u / %3u / %3u * %3.2f / %3.2f\n", c, min_val[c], avg_val[c], max_val[c], min_fac[c], max_fac[c]);
    }

    // ------ Adjust Image ------

//*******************************************************
    // Sequentially optimized by loop order ( cache misses )
    for(int y=0; y<height; ++y) {
        for(int x=0; x<width; ++x) {
            for(int c=0; c<components; ++c) {
                int index = c + x*components + y*width*components;
                unsigned char val = data[index];
                float v = (float)(val - avg_val[c]);
                v *= (val < avg_val[c]) ? min_fac[c] : max_fac[c];
                data[index] = (unsigned char)(v + avg_val[c]);
            }
        }
    }

    if(DEBUG)
        printf("Done, took %.1fms\n", (now() - start_time)*1000.0);
    double time = (now() - start_time)*1000.0;

    // ------ Store Image ------

    if(DEBUG)
        printf("Writing output image %s ...\n", out);

    char temp[80];
    strcpy(temp,out);
    strcat(temp,".seq_opt.png");
    stbi_write_png(temp,width,height,components,data,width*components);
    stbi_image_free(data);

    if(DEBUG)
        printf("Done!\n");
    return time;
}

double auto_level_omp(char * in, char * out)
{

    // load input file
    if(DEBUG)
        printf("Loading input file %s ..\n", in);
    int width, height, components;
    unsigned char *data = stbi_load(in, &width, &height, &components, 0);
    if(DEBUG)
        printf("Loaded image of size %dx%d with %d components.\n", width,height,components);

    // start the timer
    double start_time = now();

    // ------ Analyse Image ------

    // compute min/max/avg of each component
    unsigned char min_val[components];
    unsigned char max_val[components];
    unsigned char avg_val[components];

    // an auxilary array for computing the average
    unsigned long long sum[components];

    // initialize
    for(int c = 0; c<components; c++) {
        min_val[c] = 255;
        max_val[c] = 0;
        sum[c] = 0;
    }

    // compute min/max/sub

    for(int c=0; c<components; ++c) {
#pragma omp parallel for reduction(+:sum[c])
        for(int y=0; y<height; ++y) {
            for(int x=0; x<width; ++x) {
                unsigned char val = data[c + x*components + y*width*components];
                if (val < min_val[c])
                    min_val[c] = val;
                if (val > max_val[c])
                    max_val[c] = val;
                sum[c] += val;
            }
        }
    }
    sum[0]+=sum[0];
    // compute average and multiplicative factors
    float min_fac[components];
    float max_fac[components];
    for(int c=0; c<components; ++c) {
        avg_val[c] = sum[c]/((unsigned long long)width*height);
        min_fac[c] = (float)avg_val[c] / (float)(avg_val[c] - min_val[c]);
        max_fac[c] = (255.0f-(float)avg_val[c]) / (float)(max_val[c] - avg_val[c]);
        if(DEBUG)
            printf("\tComponent %1u: %3u / %3u / %3u * %3.2f / %3.2f\n", c, min_val[c], avg_val[c], max_val[c], min_fac[c], max_fac[c]);
    }

    // ------ Adjust Image ------

#pragma omp parallel for schedule(static,128)
    for(int y=0; y<height; ++y) {
        for(int x=0; x<width; ++x) {
            for(int c=0; c<components; ++c) {
                int index = c + x*components + y*width*components;
                unsigned char val = data[index];
                float v = (float)(val - avg_val[c]);
                v *= (val < avg_val[c]) ? min_fac[c] : max_fac[c];
                data[index] = (unsigned char)(v + avg_val[c]);
            }
        }
    }

    if(DEBUG)
        printf("Done, took %.1fms\n", (now() - start_time)*1000.0);

    double time = (now() - start_time)*1000.0;
    // ------ Store Image ------

    if(DEBUG)
        printf("Writing output image %s ...\n", out);
    char temp[80];
    strcpy(temp,out);
    strcat(temp,".omp.png");
    stbi_write_png(temp,width,height,components,data,width*components);
    stbi_image_free(data);

    if(DEBUG)
        printf("Done!\n");
    return time;
}

double auto_level_ocl(char * in, char * out)
{

    // load input file
    if(DEBUG)
        printf("Loading input file %s ..\n", in);
    int width, height, components;
    unsigned char *data = stbi_load(in, &width, &height, &components, 0);
    if(DEBUG)
        printf("Loaded image of size %dx%d with %d components.\n", width,height,components);

    // start the timer
    double start_time = now();

    // ------ Analyse Image ------

    // compute min/max/avg of each component
    unsigned char min_val[components];
    unsigned char max_val[components];
    unsigned char avg_val[components];

    // an auxilary array for computing the average
    unsigned long long sum[components];

    // initialize
    for(int c = 0; c<components; c++) {
        min_val[c] = 255;
        max_val[c] = 0;
        sum[c] = 0;
    }

    // compute min/max/sub

    for(int x=0; x<width; ++x) {
        for(int y=0; y<height; ++y) {
            for(int c=0; c<components; ++c) {
                unsigned char val = data[c + x*components + y*width*components];
                if (val < min_val[c]) min_val[c] = val;
                if (val > max_val[c]) max_val[c] = val;
                sum[c] += val;
            }
        }
    }
    sum[0]+=sum[0];
    // compute average and multiplicative factors
    float min_fac[components];
    float max_fac[components];
    for(int c=0; c<components; ++c) {
        avg_val[c] = sum[c]/((unsigned long long)width*height);
        min_fac[c] = (float)avg_val[c] / (float)(avg_val[c] - min_val[c]);
        max_fac[c] = (255.0f-(float)avg_val[c]) / (float)(max_val[c] - avg_val[c]);
        if(DEBUG)
            printf("\tComponent %1u: %3u / %3u / %3u * %3.2f / %3.2f\n", c, min_val[c], avg_val[c], max_val[c], min_fac[c], max_fac[c]);
    }

    // ------ Adjust Image ------

    for(int x=0; x<width; ++x) {
        for(int y=0; y<height; ++y) {
            for(int c=0; c<components; ++c) {
                int index = c + x*components + y*width*components;
                unsigned char val = data[index];
                float v = (float)(val - avg_val[c]);
                v *= (val < avg_val[c]) ? min_fac[c] : max_fac[c];
                data[index] = (unsigned char)(v + avg_val[c]);
            }
        }
    }

    if(DEBUG)
        printf("Done, took %.1fms\n", (now() - start_time)*1000.0);
    double time = (now() - start_time)*1000.0;

    // ------ Store Image ------

    if(DEBUG)
        printf("Writing output image %s ...\n", out);

    char temp[80];
    strcpy(temp,out);
    strcat(temp,".ocl.png");
    stbi_write_png(temp,width,height,components,data,width*components);
    stbi_image_free(data);

    if(DEBUG)
        printf("Done!\n");
    return time;
}

int main(int argc, char** argv) {

    // parse input parameters
    if(argc != 3) {
        printf("Usage: auto_levels [inputfile] [outputfile]\nExample: %s test.png test_out\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* input_file_name = argv[1];
    char* output_file_name = argv[2];

    double originalTime = auto_level_original(input_file_name,output_file_name);
    double seqTime = auto_level_seq(input_file_name,output_file_name);
    double ompTime = auto_level_omp(input_file_name,output_file_name);
    double oclTime = auto_level_ocl(input_file_name,output_file_name);

    printf("%.1f \t %.1f \t %.1f \t %.1f \n",originalTime,seqTime,ompTime,oclTime);

    // done
    return EXIT_SUCCESS;
}
