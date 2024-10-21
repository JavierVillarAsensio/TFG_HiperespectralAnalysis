#include "spectrum_common_functions.h"
#include "config.h"
//#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <string>
#include <sstream>
#include <math.h>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <string>

using namespace std;

int width, height, n_channels, header_offset, n_files;
string wavelength_unit_hdr;

int main(){

    cout << "Starting program..." << endl;
    if(create_needed_directories(OUTPUT_LOG_FOLDER) || create_needed_directories(OUTPUT_DISTANCES_FOLDER)){
        cout << "Error creating needed directories" << endl;
        return EXIT_FAILURE;
    }



    cout << "Reading .hdr file..." << endl;
    float *channels;
    if (read_hdr(&channels, HDR_PATH))
        return EXIT_FAILURE;
    cout << "File .hdr read" << endl;  



    int n_pixels = width * height * n_channels;
    float *image = (float*)malloc(n_pixels * sizeof(float));
    cout << "Reading hiperespectral image..." << endl;
    if (read_img_bil(image, IMG_PATH))
        return EXIT_FAILURE;
    cout << "Image read" << endl;


    for (const auto& entry : filesystem::directory_iterator(SPECTRUM_FOLDER)) {
        string file_path = entry.path().string();
        string log_file = get_output_file_name(file_path, OUTPUT_LOG_FOLDER, OUTPUT_LOG_EXTENSION);
        ofstream log(log_file);
        cout.rdbuf(log.rdbuf()); //This redirects standard output to a file, to save the execution steps in a log file



        cout << "Preparing memory needed..." << endl;
        float *reflectances = (float *)malloc(n_channels * sizeof(float));
        float *distances = (float*)malloc(width * height * sizeof(float));
        float *channels_copy = (float *)malloc(n_channels * sizeof(float));
        memcpy(channels_copy, channels, n_channels * sizeof(float)); //we copy the original because the order may change
        if (reflectances == NULL || channels_copy == NULL || image == NULL || distances == NULL) {
            cout << "Error allocating memory. Aborting..." << endl;
            return EXIT_FAILURE;
        }
        cout << "Memory allocated" << endl;



        cout << "Reading spectrum file: " << file_path << endl;
        if (read_spectrum(channels_copy[0], channels_copy[n_channels - 1], reflectances, channels_copy, file_path))
            return EXIT_FAILURE; 
        cout << "Spectrum read" << endl;



        cout << "Calculating distances..." << endl;
        calculate_distance_of_every_pixel_to_spectrum(image, reflectances, distances);
        cout << "Distances calculated" << endl;
        //print_img(distances);



        cout << "Writing distances file..." << endl;
        if (write_distances_file(distances, get_output_file_name(file_path, OUTPUT_DISTANCES_FOLDER, OUTPUT_DISTANCES_EXTENSION)))
            return EXIT_FAILURE;
        cout << "Distances file written" << endl;



        cout << "Freeing resources..." << endl;
        free(reflectances);
        free(distances);
        free(channels_copy);
        cout << "Freed" << endl;
    }

    free(channels);
    free(image);
    cout << "Execution finished successfully" << endl;
    return(EXIT_SUCCESS);
}