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
#include <opencv2/opencv.hpp>

#define DATA_SIZE sizeof(short)
#define CHANNELS 198

#define IMG_PATH "jasperRidge2_R198/jasperRidge2_R198.img"
#define HDR_PATH "jasperRidge2_R198/jasperRidge2_R198.hdr"

#define ROAD_PATH "spectrums/manmade.concrete.pavingconcrete.solid.all.0092uuu_cnc.jhu.becknic.spectrum.txt"
#define SOIL1_PATH "spectrums/soil.mollisol.cryoboroll.none.all.85p4663.jhu.becknic.spectrum.txt"
#define SOIL2_PATH "spectrums/soil.utisol.hapludult.none.all.87p707.jhu.becknic.spectrum.txt"
#define TREE_PATH "spectrums/vegetation.tree.eucalyptus.maculata.vswir.jpl087.jpl.asd.spectrum.txt"
#define WATER_PATH "spectrums/water.tapwater.none.liquid.all.tapwater.jhu.becknic.spectrum.txt"

#define WAVELENGTH_FIELD "wavelength"
#define ROWS_FIELD "lines"
#define COLS_FIELD "samples"
#define CHANNELS_FIELD "bands"
#define END_FIELD "}"

#define SPECTRUM_FIRST_VALUE_FIELD "First X Value"
#define SPECTRUM_LAST_VALUE_FIELD "Last X Value" 
#define FROM_LAST_VALUE_TO_VALUES 3

#define REFLECTANCES_FOLDER "reflectances/"

#define ASC 2   // 0 and 1 are EXIT_SUCCESS and EXIT_FAILURE
#define DESC 3

#define FLOAT_MAX 3.4028234663852886e+38F

#define WAVELENGTH_UNIT_REFACTOR 1000   //from nanometers to micrometers
#define PERCENTAGE_REFACTOR 100

using namespace std;

int width, height, n_channels = CHANNELS;

int read_img(float *img) {
    int n_pixels = width * height * n_channels;

    ifstream file(IMG_PATH, ios::binary);
    if(!file.is_open()){
        cout << "Error opening the file, it could not be opened. Aborting." << endl;
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
        cout << "Error reading file, the number of pixels read was not the expected. Aborting." << endl;
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
        cout << "Error opening the file, it could not be opened. Aborting." << endl;
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

void save_reflectances(ifstream& file, float previous_reflectance, float previous_diff, float *wavelengths, float *reflectances, int order){
    string line, segment;
    int reflectances_position = 0, wavelengths_position = n_channels - 1;
    if (order == ASC)
        wavelengths_position = 0;
    float final_wavelength = wavelengths[n_channels - 1], previous_wavelength, wavelength_read, diff, reflectance_read;

    while (getline(file, line)){ 
        istringstream line_stream(line);

        getline(line_stream, segment, ' ');
        if(segment.length() == 0)
            getline(line_stream, segment, ' ');

        previous_wavelength = stof(segment);
        if(previous_wavelength <= final_wavelength){
            previous_diff = abs(previous_wavelength - final_wavelength);
            getline(line_stream, segment, ' ');
            previous_reflectance = stof(segment);
            break;
        }
    }

    while(getline(file, line)){
        if(reflectances_position == n_channels)
            break;
        
        istringstream line_stream(line);

        getline(line_stream, segment, ' ');
        if(segment.length() == 0)
            getline(line_stream, segment, ' ');
        wavelength_read = stof(segment);

        diff = abs(wavelengths[wavelengths_position] - wavelength_read);
        if (diff < previous_diff){
            previous_diff = diff;
            getline(line_stream, segment, ' ');
            reflectance_read = stof(segment);
            previous_reflectance = reflectance_read;
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
    int number_of_reflectances, order;
    float reflectance_read, previous_reflectance, reflectance, previous_diff, diff, wavelength_read, previous_wavelength;

    ifstream file(path);
    if(!file.is_open()){
        cout << "Error opening the file, it could not be opened. Aborting." << endl;
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

    save_reflectances(file, previous_reflectance, previous_diff, wavelengths, reflectances, order);

    file.close();
    return EXIT_SUCCESS;
}

void calculate_distance_of_every_pixel_to_spectrum(float *image, float *reflectances, float *distances) {
    float sum;

    for(int height_offset = 0; height_offset < height; height_offset++){
        for(int width_offset = 0; width_offset < width; width_offset++){
            sum = 0;
            for(int band_offset = 0; band_offset < n_channels; band_offset++){
                sum += pow((image[(height_offset * (width * n_channels)) + (band_offset * width) + width_offset] - reflectances[band_offset]), 2.0);
            }
            distances[(height_offset * width) + width_offset] = sqrt(sum);
        }
    }
}

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

int main(){
    cout << "Starting program..." << endl;
    
    float *reflectances = (float*)malloc(n_channels * sizeof(float));
    float *channels = (float*)malloc(n_channels * sizeof(float));
    

    cout << "Memory allocated" << endl;

    if (read_hdr(channels) == EXIT_FAILURE)
        return EXIT_FAILURE;
    cout << "File .hdr read" << endl;

    int n_pixels = width * height * n_channels; 
    float *image = (float*)malloc(n_pixels * sizeof(float));
    float *distances = (float*)malloc(width * height * sizeof(float));

    if (read_spectrum(channels[0], channels[n_channels - 1], reflectances, channels, TREE_PATH) == EXIT_FAILURE)
        return EXIT_FAILURE;  
    cout << "Spectrum read" << endl;
    cout << "Freeing channels..." << endl;
    free(channels);

    if (read_img(image) == EXIT_FAILURE)
        return EXIT_FAILURE;
    cout << "Image read." << endl;

    cout << "Calculating distances..." << endl;
    calculate_distance_of_every_pixel_to_spectrum(image, reflectances, distances);

    //print_img(distances);

    cout << "Freeing reflectances..." << endl;
    free(reflectances);
    cout << "Freeing image..." << endl;
    free(image);
    
    cout << "Freed" << endl;
    return(EXIT_SUCCESS);
}