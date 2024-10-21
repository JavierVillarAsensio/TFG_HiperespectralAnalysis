#include "spectrum_common_functions.h"
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
#include "config.h"

using namespace std;

string wavelength_unit_spec;
double wavelength_unit_refactor;

////////////////////////COMMON FUNCTIONS////////////////////////

int read_img_bil(float *img, const char* filename) {
    int n_pixels = width * height * n_channels;

    ifstream file(filename, ios::binary);
    if(!file.is_open()){
        cout << "Error opening the img file, it could not be opened. Aborting." << endl;
        return(EXIT_FAILURE);
    }

    int index = 0;
    char buffer[DATA_SIZE];
    short int value;
    float refl;

    //skip header bytes
    streampos offset = streampos(header_offset);
    file.seekg(offset, ios::beg);
    while(index < n_pixels){
        file.read(reinterpret_cast<char*>(&value), DATA_SIZE);

        refl = static_cast<float>(value)/PERCENTAGE_REFACTOR;
        if(refl < 0)
            refl = 0;

        img[index++] = refl;
    }

    file.close();

    if (index != n_pixels){
        cout << "Error reading img file, the number of pixels read was not the expected. Aborting." << endl;
        return EXIT_FAILURE;
    }
    else
        return EXIT_SUCCESS;
}

int get_factor_scale(string unit) {

    if (!unit.empty() && unit.back() == 's') 
        unit.pop_back(); //Erase plural

    string lower_case_wavelength_unit = unit;
    transform(unit.begin(), unit.end(), lower_case_wavelength_unit.begin(),[](unsigned char c){ return tolower(c); });
    lower_case_wavelength_unit.erase(remove(lower_case_wavelength_unit.begin(), lower_case_wavelength_unit.end(), ' '), lower_case_wavelength_unit.end());

    for(int i = 0; i < SIZE_OF_KNOWN_METRICS; i++)
        if (lower_case_wavelength_unit == KNOWN_METRICS[i])
            return SCALE_FACTORS[i];

    return -1; //to check later if any of the wavelengths is known
}

int read_hdr(float **wavelengths, const string filename){
    float number;

    ifstream file(filename);
    if(!file.is_open()){
        cout << "Error opening the hdr file, it could not be opened. Aborting." << endl;
        return(EXIT_FAILURE);
    }

    string line, key, value;
    int index = 0, a;
    bool waves_read = false;
    while (getline(file, line)) {
        istringstream lineStream(line);
        if (getline(lineStream, key, '=')) {
            key.erase(key.find_last_not_of(" \n\r\t")+1); //erase spaces or any dirty char

            if(key == WAVELENGTH_FIELD){
                while (!waves_read) {
                    getline(file, line, ',');
                    stringstream numbers(line);
                    while(getline(numbers, value) && !waves_read){
                        if(value.size() > 0){
                            number = stof(value);
                            (*wavelengths)[index] = number;
                            index++;
                            
                        }
                        if(value.find(END_FIELD) != string::npos)
                            waves_read = true;
                    }
                }
            }
            //set size of img to read
            else if(key == CHANNELS_FIELD){
                getline(lineStream, value, '=');
                n_channels = stoi(value);
                *wavelengths = (float *)malloc(n_channels * sizeof(float));
            }
            else if(key == ROWS_FIELD){
                getline(lineStream, value, '=');
                height = stoi(value);
            }
            else if(key == COLS_FIELD){
                getline(lineStream, value, '=');
                width = stoi(value);
            }
            else if(key == HEADER_OFFSET_FIELD){
                getline(lineStream, value, '=');
                header_offset = stoi(value);
            }
            else if(key == WAVELENGTH_UNIT_FIELD){
                getline(lineStream, wavelength_unit_hdr, '=');
                wavelength_unit_hdr.erase(remove(wavelength_unit_hdr.begin(), wavelength_unit_hdr.end(), ' '), wavelength_unit_hdr.end());
            }
        }
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
    float final_wavelength = wavelengths[n_channels - 1], previous_wavelength, wavelength_read, diff, reflectance;

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
            
            if(wavelengths_position >= 0 && order == DESC)
                wavelengths_position--;

            else if(wavelengths_position <= n_channels && order == ASC)
                wavelengths_position++;
            
            previous_diff = FLOAT_MAX;                                                             
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

        if (segment == SPEC_WAVELENGTH_UNIT_FIELD){
            getline(line_stream, segment, ':');
            size_t open = segment.find('('), close = segment.find(')');
            if (open != string::npos && close != string::npos && close > open)
                wavelength_unit_spec = segment.substr(open + 1, close - open - 1);
        }

        else if (segment == SPECTRUM_FIRST_VALUE_FIELD){
            getline(line_stream, segment, ':');
            first_value = stof(segment);
        }

        else if (segment == SPECTRUM_LAST_VALUE_FIELD){
            getline(line_stream, segment, ':');
            last_value = stof(segment);
            for(int i = 0; i < FROM_LAST_VALUE_TO_VALUES; i++){
                getline(file, line);
            }
            break;
        }
    }

    wavelength_unit_refactor = pow(10, get_factor_scale(wavelength_unit_hdr) - get_factor_scale(wavelength_unit_spec));
    if (wavelength_unit_refactor == -1){
        cout << "The wavelength unit read from .hdr or from spectrum was not found or not known" << endl;
        return EXIT_FAILURE;
    }

    for(int i = 0; i < n_channels; i++)
        wavelengths[i] = wavelengths[i] / wavelength_unit_refactor;

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
                 image_reflectance = image[(height_offset*(width*n_channels))+(band_offset*width)+width_offset];
                
                if (image_reflectance == 0)
                    image_reflectance = reflectances[band_offset];

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

    for(int i = 0; i < 10000; i++)
        //cout << distances[i] << endl;

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

string get_spectrum_file_name() {
    int hostname_index = get_index(), index = 0;
    
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
    
    size_t slash = out_name.find_last_of("/");
    out_name.replace(0, slash, folder);
    size_t last_dot = out_name.find_last_of(".");
    out_name.replace(last_dot, out_name.size(), extension);

    return out_name;
}

int create_needed_directories(const string directory){
    error_code ec;
    if(!filesystem::exists(directory))
        if(!filesystem::create_directories(directory, ec)){
            cout << "Error creating " << directory << " directory: " << ec.value() << " " << ec.message() << endl;
            return EXIT_FAILURE;
        }
    return EXIT_SUCCESS;
}