#include "master_common_functions.h"
#include "config.h"
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
#include <thread>

using namespace std;
//using namespace cv;

int width, height;
size_t file_count;

int main() {
    ofstream log(MASTER_LOG_FILE);
    cout.rdbuf(log.rdbuf());
    


    cout << "Starting master..." << endl;
    if(read_hdr()){
        cout << "Error reading .hdr file. Aborting..." << endl;
        return EXIT_FAILURE;
    }

    

    file_count = stoi(getenv(NUM_FILES_VAR));
    cout << "Waiting for " << file_count << " distances files to be written..." << endl;
    thread monitor_thread(monitor_files, file_count);
    wait_files(file_count);
    monitor_thread.join();
    size_t file_count = count_files();
    cout << "Distances files written" << endl;



    cout << "Allocating dinamic memory..." << endl;
    size_t distances_size = width * height;
    float *all_distances = (float*)malloc(distances_size * file_count * sizeof(float));
    if (all_distances == NULL){
        cout << "Error allocating dinamic memory. Aborting..." << endl;
        return EXIT_FAILURE;
    }
    cout << "Memory allocated" << endl;



    cout << "Reading all distances..." << endl;
    string materials[file_count];
    if(read_all_distances(all_distances, distances_size, materials, DISTANCES_FOLDER))
        return EXIT_FAILURE;
    cout << "Distances read" << endl;



    cout << "Choosing materials in pixels..." << endl;
    int nearest_materials_image[distances_size];
    find_nearest_materials(file_count, distances_size, all_distances, nearest_materials_image);
    cout << "Materials chosen" << endl;



    cout << "Writing image..." << endl;
    if(write_jpg(nearest_materials_image, distances_size))
        return EXIT_FAILURE;
    cout << "Image written" << endl;



    cout << "Writing legend..." << endl;
    if(write_legend(materials, file_count))
        return EXIT_FAILURE;
    cout << "Legend written" << endl;


    if(file_count < 5){
        cout << "Comparing with groundtruth..." << endl;
        if(compare_result(nearest_materials_image, distances_size, materials))
            return EXIT_FAILURE;
        cout << "Comparison finished" << endl;
    }
    

    free(all_distances);
    cout << "Execution finished successfully" << endl;
    return EXIT_SUCCESS;
}