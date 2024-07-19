/*
    The format is bil so it is read the line of every channel

    so the pointer goes:
    100 first pixels of band #1, 100 first pixels of band #2...
    when bands are finished reads next line of every channel
*/
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
//#include <opencv2/opencv.hpp>

#define DATA_SIZE sizeof(short)
#define CHANNELS 198

#define IMG_PATH "jasperRidge2_R198/jasperRidge2_R198.img"
#define HDR_PATH "jasperRidge2_R198/jasperRidge2_R198.hdr"

#define OUTPUT_DISTANCES_FOLDER "output/distances"
#define OUTPUT_DISTANCES_EXTENSION ".bin"
#define OUTPUT_LOG_FOLDER "output/logs"
#define OUTPUT_LOG_EXTENSION ".log"
#define SPECTRUM_FOLDER "spectrums"

#define WAVELENGTH_FIELD "wavelength"
#define ROWS_FIELD "lines"
#define COLS_FIELD "samples"
#define CHANNELS_FIELD "bands"
#define END_FIELD "}"

#define SPECTRUM_FIRST_VALUE_FIELD "First X Value"
#define SPECTRUM_LAST_VALUE_FIELD "Last X Value" 
#define FROM_LAST_VALUE_TO_VALUES 3

#define ASC 2   // 0 and 1 are EXIT_SUCCESS and EXIT_FAILURE
#define DESC 3

#define FLOAT_MAX 3.4028234663852886e+38F

#define WAVELENGTH_UNIT_REFACTOR 1000   //from nanometers to micrometers
#define PERCENTAGE_REFACTOR 100

#define INDEX_FILE_VAR "SERVICE_NAME"

using namespace std;

int width, height, n_channels = CHANNELS;

int read_img(float *img) {
    int n_pixels = width * height * n_channels;

    ifstream file(IMG_PATH, ios::binary);
    if(!file.is_open()){
        cout << "Error opening the img file, it could not be opened. Aborting." << endl;
        return(EXIT_FAILURE);
    }

    int index = 0;
    char buffer[DATA_SIZE];
    short int value;
    float refl;
    while(index < n_pixels){
        file.read(buffer, DATA_SIZE);

        memcpy(&value, buffer, DATA_SIZE);
        refl = static_cast<float>(value)/PERCENTAGE_REFACTOR;
        if(refl < 0)
            refl = 0;

        img[index] = refl;

        index++;
    }
    file.close();

    if (index != n_pixels){
        cout << "Error reading img file, the number of pixels read was not the expected. Aborting." << endl;
        return EXIT_FAILURE;
    }
    else
        return EXIT_SUCCESS;
}

int read_hdr(float *wavelengths){
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
            if(key == WAVELENGTH_FIELD){
                
                string value;
                while (!waves_read) {
                    getline(file, line, ',');
                    stringstream numbers(line);
                    while(getline(numbers, value) && !waves_read){
                        if(value.size() > 0){
                            number = stof(value);
                            wavelengths[index] = number;
                            index++;
                            
                        }
                        if(value.find(END_FIELD) != string::npos){
                            waves_read = true;
                        }
                    }
                }
            }
            //set size of img to read
            else if(key == CHANNELS_FIELD){
                string value;
                getline(lineStream, value, '=');
                n_channels = stoi(value);
            }
            else if(key == ROWS_FIELD){
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

    for(int i = 0; i < n_channels; i++){
        wavelengths[i] = wavelengths[i] / WAVELENGTH_UNIT_REFACTOR;
    }

    file.close();
    return EXIT_SUCCESS;
}

void save_reflectances(ifstream& file, float *wavelengths, float *reflectances, int order){
    string line, segment;
    int reflectances_position = 0, wavelengths_position = n_channels - 1;
    float previous_diff, previous_reflectance;

    if (order == ASC)
        wavelengths_position = 0;
    float final_wavelength = wavelengths[n_channels - 1], previous_wavelength, wavelength_read, diff, prev_diff, reflectance;

    while (getline(file, line)){ 
        istringstream line_stream(line);

        line_stream >> segment;

        previous_wavelength = stof(segment);
        if(previous_wavelength <= final_wavelength){
            previous_diff = abs(previous_wavelength - final_wavelength);
            line_stream >> segment;
            previous_reflectance = stof(segment);
            break;
        }
    }    

    float reflectance_read;
    int jump = 1, lines_read = 1;
    getline(file, line);
    while(true){
        if(reflectances_position == n_channels)
            break;
        
        istringstream line_stream(line);

        line_stream >> segment;
        wavelength_read = stof(segment);

        previous_diff = abs(wavelengths[wavelengths_position] - previous_wavelength);
        diff = fabs(wavelengths[wavelengths_position] - wavelength_read);
        if (diff < previous_diff){
            previous_diff = diff;
            line_stream >> segment;
            
            reflectance_read = stof(segment);
            previous_reflectance = reflectance_read;
            previous_wavelength = wavelength_read;
            getline(file, line);
        }
        else {
            reflectances[reflectances_position] = previous_reflectance;
            reflectances_position++;
            
            if(wavelengths_position >= 0 && order == DESC){
                wavelengths_position--;
                previous_diff = FLOAT_MAX;
            }
            else if(wavelengths_position <= n_channels && order == ASC){
                wavelengths_position++;
                previous_diff = FLOAT_MAX;
            }                                                             
        }
        
    }

    if (reflectances[reflectances_position] == 0)
        reflectances[reflectances_position] = previous_reflectance;
    
    if (order == DESC) {
        int swap_index = n_channels - 1;
        float *aux = (float*)malloc(n_channels * sizeof(float));
        memcpy(aux, reflectances, n_channels * sizeof(float));
        for(int i = 0; i < n_channels; i++){
            reflectances[swap_index] = aux[i];
            swap_index--;
        }
        free(aux);
    } 
    for(int i = 0; i < n_channels; i++){
        cout << i << ": " << wavelengths[i] << " - " << reflectances[i] << endl;
    }                           
}

int read_spectrum(float initial_wavelength, float final_wavelength, float *reflectances, float *wavelengths, string path){
    int order;

    ifstream file(path);
    if(!file.is_open()){
        cout << "Error opening the spectrum file, it could not be opened. Aborting." << endl;
        return(EXIT_FAILURE);
    }

    string line, segment;
    int iter = 0;
    float first_value, last_value;

    while (getline(file, line)) {
        istringstream line_stream(line);
        getline(line_stream, segment, ':');
        if (segment == SPECTRUM_FIRST_VALUE_FIELD){
            getline(line_stream, segment, ':');
            first_value = stof(segment);
        }
        if (segment == SPECTRUM_LAST_VALUE_FIELD){
            getline(line_stream, segment, ':');
            last_value = stof(segment);
            for(int i = 0; i < FROM_LAST_VALUE_TO_VALUES; i++){
                getline(file, line);
            }
            break;
        }
    }

    if (first_value > last_value)
        order = DESC;
    else
        order = ASC;

    save_reflectances(file, wavelengths, reflectances, order);

    file.close();
    return EXIT_SUCCESS;
}

void calculate_distance_of_every_pixel_to_spectrum(float *image, float *reflectances, float *distances) {
    float sum, image_reflectance;

    for(int height_offset = 0; height_offset < height; height_offset++){
        for(int width_offset = 0; width_offset < width; width_offset++){
            sum = 0;
            for(int band_offset = 0; band_offset < n_channels; band_offset++){
                image_reflectance = image[(height_offset * (width * n_channels)) + (band_offset * width) + width_offset];
                
                if (image_reflectance == 0)
                    image_reflectance = reflectances[band_offset];

                //cout << "pixel " << height_offset << "x" << width_offset << " band " << band_offset << ": image " << image_reflectance << " - reflec " << reflectances[band_offset] << " = " << pow((image_reflectance - reflectances[band_offset]), 2.0) << endl;
                sum += pow((image_reflectance - reflectances[band_offset]), 2.0);
            }
            distances[(height_offset * width) + width_offset] = sqrt(sum);
        }
    }
}

/*
void print_img(float *distances) {
    float value;
    uint8_t img[height * width];
    
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            value = distances[(i * height) + j];
            if (value > 255)
                value = 255;
            img[(i * height) + j] = (int)value;
        }
    }

    cv::Mat gray(height, width, CV_8UC1, img);
    cv::namedWindow("test", cv::WINDOW_AUTOSIZE);
    cv::imshow("test", gray);
    cv::waitKey(0);
}
*/

int write_distances_file(float *distances, const string output_file){
    size_t distances_size = (height * width)*sizeof(float);

    ofstream out(output_file, ios::binary);
    if(!out){
        cout << "Error writing distances file. Aborting..." << endl;
        return EXIT_FAILURE;
    }

    out.write(reinterpret_cast<char*>(distances), distances_size);
    out.close();

    return EXIT_SUCCESS;
}

int get_index() {
    string value = getenv(INDEX_FILE_VAR), numberStr;
    size_t pos = value.find('-');

    if (pos != string::npos)
        numberStr = value.substr(pos + 1);

    return stoi(numberStr);
}

string get_spectrum_file_name(int *file_index) {
    int hostname_index = get_index(), index = 0;
    file_index[0] = hostname_index;
    
    string path;
    for (const auto& entry : filesystem::directory_iterator(SPECTRUM_FOLDER)) {
        if (entry.is_regular_file()) {
            if(index == hostname_index-1){
                path = entry.path().string();
                break;
            }
            index++;
        }
    }
    return path;
}

string get_output_file_name(string file_path, string folder, string extension){ 
    string out_name = file_path;
    
    size_t slash = out_name.find_first_of("/");
    out_name.replace(0, slash, folder);
    size_t last_dot = out_name.find_last_of(".");
    out_name.replace(last_dot, out_name.size(), extension);

    return out_name;
}

int main(){
    cout << "Starting program..." << endl;
    int file_index[0];

    string file_path = get_spectrum_file_name(file_index);
    
    string distances_file = get_output_file_name(file_path, OUTPUT_DISTANCES_FOLDER, OUTPUT_DISTANCES_EXTENSION);
    string log_file = get_output_file_name(file_path, OUTPUT_LOG_FOLDER, OUTPUT_LOG_EXTENSION);

    ofstream log(log_file);
    streambuf *std_out = cout.rdbuf();
    cout.rdbuf(log.rdbuf());

    float *reflectances = (float*)malloc(n_channels * sizeof(float));
    float *channels = (float*)malloc(n_channels * sizeof(float));

    cout << "File index: " << file_index[0] << endl;
    cout << "Read spectrum file: " << file_path << endl;

    
    if (reflectances == NULL || channels == NULL) {
        cout << "Error allocating memory. Aborting..." << endl;
        return EXIT_FAILURE;
    }
    cout << "Memory allocated" << endl;

    if (read_hdr(channels))
        return EXIT_FAILURE;
    cout << "File .hdr read" << endl;

    int n_pixels = width * height * n_channels; 
    float *image = (float*)malloc(n_pixels * sizeof(float));
    float *distances = (float*)malloc(width * height * sizeof(float));

    if (read_spectrum(channels[0], channels[n_channels - 1], reflectances, channels, file_path))
        return EXIT_FAILURE; 

    cout << "Spectrum read" << endl;
    cout << "Freeing channels..." << endl;
    free(channels);

    if (read_img(image))
        return EXIT_FAILURE;
    cout << "Image read" << endl;

    cout << "Calculating distances..." << endl;
    calculate_distance_of_every_pixel_to_spectrum(image, reflectances, distances);

    //print_img(distances);

    cout << "Writing distances file..." << endl;
    if (write_distances_file(distances, distances_file))
        return EXIT_FAILURE;

    cout << "Freeing reflectances..." << endl;
    free(reflectances);
    cout << "Freeing image..." << endl;
    free(image);
    
    cout << "Freed" << endl;
    cout << "Execution finished successfully" << endl;

    cout.rdbuf(std_out);

    return(EXIT_SUCCESS);
}