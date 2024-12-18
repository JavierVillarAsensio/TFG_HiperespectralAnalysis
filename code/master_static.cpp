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

int main(int argc, char *argv[]) {
    int input = 0;
    if(argc > 1){
        char *program_input = argv[1];
        if(strcmp(program_input, PERFORMANCE) == 0)
            input = PERFORMANCE_OPTION;
    }
    
    ofstream log(MASTER_LOG_FILE);
    streambuf *original = cout.rdbuf();
    cout.rdbuf(log.rdbuf());
    


    cout << "Starting master..." << endl;
    if(read_hdr())
        return EXIT_FAILURE;
    if (!filesystem::exists(DISTANCES_FOLDER) || !filesystem::is_directory(DISTANCES_FOLDER)){
        cout << "The folder does not exist. Aborting..." << endl;
        return EXIT_FAILURE;
    }

    

    file_count = count_files();
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



    if(!input == PERFORMANCE_OPTION){ //the comparison is prepared only for given solution
        cout << "Comparing with groundtruth..." << endl;
        if(compare_result(nearest_materials_image, distances_size, materials))
            return EXIT_FAILURE;
        cout << "Comparison finished" << endl;
    }
    


    free(all_distances);
    
    cout << "Execution finished successfully" << endl;
    cout.rdbuf(original);
    log.close();
    return EXIT_SUCCESS;
}