#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <unistd.h>

//#include <opencv2/opencv.hpp>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;
//using namespace cv;

#define DISTANCES_FOLDER "output/distances"
#define HDR_PATH "jasperRidge2_R198/jasperRidge2_R198.hdr"

#define ROWS_FIELD "lines"
#define COLS_FIELD "samples"

#define FLOAT_MAX 3.4028234663852886e+38F

#define NUMBER_OF_FILES_EXPECTED 4

#define RESULT_FILE "output/result.jpg"
#define LEGEND_FILE "output/legend.txt"

int width, height;

int read_hdr(){
    float number;
    string value;

    ifstream file(HDR_PATH);
    if(!file.is_open()){
        cout << "Error opening the hdr file, it could not be opened. Aborting." << endl;
        return(EXIT_FAILURE);
    }

    string line;
    int index = 0;
    bool waves_read = false;
    while (getline(file, line)) {
        istringstream lineStream(line);
        string key;
        if (getline(lineStream, key, '=')) {
            key.erase(key.find_last_not_of(" \n\r\t")+1); //erase spaces or any dirty char

            //set size of img to read
            if(key == ROWS_FIELD){
                string value;
                getline(lineStream, value, '=');
                height = stoi(value);
            }
            else if(key == COLS_FIELD){
                string value;
                getline(lineStream, value, '=');
                width = stoi(value);
            }
        }
    }

    file.close();
    return EXIT_SUCCESS;
}

size_t count_files() {
    size_t count = 0;

    for (const auto& entry : filesystem::directory_iterator(DISTANCES_FOLDER)) {
        if (entry.is_regular_file()) 
            count++;
    }

    return count;
}

int read_all_distances(float *all_distances, size_t distances_size, string *materials) {
    float distances[distances_size];
    string path, mat_name;
    int file_position = 0;

    for (const auto& entry : filesystem::directory_iterator(DISTANCES_FOLDER)) {
        if (entry.is_regular_file()) {
            path = entry.path().string();
            mat_name = path.substr(0, path.find_last_of("."));
            mat_name = mat_name.substr(0, mat_name.find_last_of("."));
            mat_name = mat_name.substr(mat_name.find_last_of("/") + 1, mat_name.size());
            materials[file_position] = mat_name;

            ifstream in(path, ios::binary);
            if (!in) {
                cout << "Error reading " << path << " Aborting..." << endl;
                return EXIT_FAILURE;
            }

            in.read(reinterpret_cast<char*>(distances), distances_size * sizeof(float));
            in.close();

            copy(distances, distances + distances_size, all_distances + (file_position*distances_size));
            file_position++;
        }
    }

    return EXIT_SUCCESS;
}

void find_nearest_materials(int file_count, size_t distances_size, float *all_distances, int *nearest_materials_images){
    float min_dist, dist;
    int min_spectrum;

    for(int pixel = 0; pixel < distances_size; pixel++){
        min_dist = FLOAT_MAX;
        for(int spectrum = 0; spectrum < file_count; spectrum++){
            dist = all_distances[(spectrum * distances_size) + pixel];
            if (dist < min_dist){
                min_dist = dist;
                min_spectrum = spectrum;
            }
        }
        nearest_materials_images[pixel] = min_spectrum;
    }
}
/*
void write_img(int *nearest_materials_image, size_t distances_size) {
    Mat result_image(width, height, CV_8UC3);

    //  Mapa de color (B, G, R) en OpenCV
    const Vec<uchar, 3> colors[] = {
        Vec<uchar, 3>(0, 0, 255),   // Red
        Vec<uchar, 3>(0, 255, 0),   // Green
        Vec<uchar, 3>(255, 0, 0),   // Blue
        Vec<uchar, 3>(255, 255, 0), // Yellow
        Vec<uchar, 3>(255, 0, 255), // Magenta
        Vec<uchar, 3>(0, 255, 255), // Cyan
        Vec<uchar, 3>(255, 255, 255), // White
        Vec<uchar, 3>(0, 0, 0),     // Black
        Vec<uchar, 3>(128, 128, 128), // Gray
        Vec<uchar, 3>(64, 64, 64) // Gray
    };

    for(int i = 0; i < distances_size; i++){
        result_image.at<Vec3b>((int)(i/height), i%height) = colors[nearest_materials_image[i]];
    }

    Mat resultT;
    transpose(result_image,resultT); 
    imwrite(RESULT_FILE, resultT);
}
*/
int write_jpg(int *nearest_materials_image, size_t distances_size){
    
    const int channels = 3; //RGB
    int colors[10 * channels] = {
        255, 0, 0,      //red
        0, 255, 0,      //blue
        0, 0, 255,      //green
        255, 255, 0,    //cyan
        255, 0, 255,    //magenta
        0, 255, 255,    //yellow
        255, 255, 255,  //white
        0, 0, 0,        //black
        128, 128, 128,  //light gray
        64, 64, 64,     //gray
    };

    unsigned char* image = new unsigned char[width * height * channels];

    for (int i = 0; i < distances_size; i++){
        image[channels * i] = static_cast<unsigned char>(colors[nearest_materials_image[i] * channels]);
        image[(channels * i) + 1] = static_cast<unsigned char>(colors[(nearest_materials_image[i] * channels) + 1]);
        image[(channels * i) + 2] = static_cast<unsigned char>(colors[(nearest_materials_image[i] * channels) + 2]);
    }

    if (!stbi_write_jpg(RESULT_FILE, width, height, channels, image, 100)) {
        cout << "Error creating jpg. Aborting..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int write_legend(string *materials, size_t file_count){
    string colors_name[10] = {
        "Red",
        "Blue",
        "Green",
        "Cyan",
        "Magenta",
        "Yellow",
        "White",
        "Black",
        "Ligth Gray",
        "Gray",
    };

    ofstream out(LEGEND_FILE);
    if(!out){
        cout << "Error writing legend file. Aborting..." << endl;
        return EXIT_FAILURE;
    }

    for(int i = 0; i < file_count; i++){
        out << i+1 << ": " << materials[i] << "   =>   " << colors_name[i] << endl;
    }

    return EXIT_SUCCESS;
}

int main() {
    cout << "Starting..." << endl;
    if(read_hdr())
        return EXIT_FAILURE;

    size_t distances_size = width * height;

    if (!filesystem::exists(DISTANCES_FOLDER) || !filesystem::is_directory(DISTANCES_FOLDER)){
        cout << "The folder does not exist. Aborting..." << endl;
        return EXIT_FAILURE;
    }

    cout << "Counting files..." << endl;
    size_t file_count = count_files();
    while(file_count != NUMBER_OF_FILES_EXPECTED){
        sleep(5);
        file_count = count_files();
    }
    cout << "Files counted: " << file_count << endl;

    cout << "Allocating dinamic memory..." << endl;
    float *all_distances = (float*)malloc(distances_size * file_count * sizeof(float));
    if (all_distances == NULL){
        cout << "Error allocating dinamic memory. Aborting..." << endl;
        return EXIT_FAILURE;
    }
    cout << "Memory allocated" << endl;

    cout << "Reading all distances..." << endl;
    string materials[file_count];
    if(read_all_distances(all_distances, distances_size, materials))
        return EXIT_FAILURE;
    cout << "Distances read" << endl;

    int nearest_materials_image[distances_size];
    cout << "Choosing materials in pixels..." << endl;
    find_nearest_materials(file_count, distances_size, all_distances, nearest_materials_image);

    cout << "Writing image..." << endl;
    if(write_jpg(nearest_materials_image, distances_size))
        return EXIT_FAILURE;

    cout << "Writing legend..." << endl;
    if(write_legend(materials, file_count))
        return EXIT_FAILURE;

    cout << "Execution finished successfully" << endl;
    return EXIT_SUCCESS;
}