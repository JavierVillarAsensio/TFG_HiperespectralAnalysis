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

int main(){
    cout << "Starting program..." << endl;
    string file_path = get_spectrum_file_name();
    string log_file = get_output_file_name(file_path, OUTPUT_LOG_FOLDER, OUTPUT_LOG_EXTENSION);
    ofstream log(log_file);
    cout.rdbuf(log.rdbuf()); //This redirects standard output to a file, to save the execution steps in a log file



    cout << "Reading .hdr file..." << endl;
    float *channels;
    if (read_hdr(channels))
        return EXIT_FAILURE;
    cout << "File .hdr read" << endl;



    cout << "Preparing memory needed..." << endl;
    float *reflectances = (float *)malloc(n_channels * sizeof(float));
    int n_pixels = width * height * n_channels; 
    float *image = (float*)malloc(n_pixels * sizeof(float));
    float *distances = (float*)malloc(width * height * sizeof(float));
    if (reflectances == NULL || channels == NULL || image == NULL || distances == NULL) {
        cout << "Error allocating memory. Aborting..." << endl;
        return EXIT_FAILURE;
    }
    cout << "Memory allocated" << endl;



    cout << "Reading spectrum file: " << file_path << endl;
    if (read_spectrum(channels[0], channels[n_channels - 1], reflectances, channels, file_path))
        return EXIT_FAILURE; 
    free(channels);
    cout << "Spectrum read" << endl;



    cout << "Reading hiperespectral image..." << endl;
    if (read_img_bil(image, IMG_PATH))
        return EXIT_FAILURE;
    cout << "Image read" << endl;



    cout << "Calculating distances..." << endl;
    calculate_distance_of_every_pixel_to_spectrum(image, reflectances, distances);
    cout << "Distances calculated" << endl;
    //print_img(distances);



    cout << "Writing distances file..." << endl;
    if (write_distances_file(distances, get_output_file_name(file_path, OUTPUT_DISTANCES_FOLDER, OUTPUT_DISTANCES_EXTENSION)))
        return EXIT_FAILURE;
    cout << "Distances file written" << endl;



    cout << "Freeing reflectances..." << endl;
    free(reflectances);
    cout << "Freeing image..." << endl;
    free(image);
    cout << "Freed" << endl;



    cout << "Execution finished successfully" << endl;
    return(EXIT_SUCCESS);
}