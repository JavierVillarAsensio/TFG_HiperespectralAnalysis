/*
    The format is bil so it is read the firstpixel of every channel
*/


#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <map>
#include <string>
#include <sstream>

#define WIDTH 100
#define HEIGHT 100
#define CHANNELS 198
#define DATA_SIZE 2

#define IMG_PATH "jasperRidge2_R198/jasperRidge2_R198.img"
#define HDR_PATH "jasperRidge2_R198/jasperRidge2_R198.hdr"

#define ROAD_PATH "spectrums/manmade.concrete.pavingconcrete.solid.all.0092uuu_cnc.jhu.becknic.spectrum.txt"
#define SOIL1_PATH "spectrums/soil.mollisol.cryoboroll.none.all.85p4663.jhu.becknic.spectrum.txt"
#define SOIL2_PATH "spectrums/soil.utisol.hapludult.none.all.87p707.jhu.becknic.spectrum.txt"
#define TREE_PATH "spectrums/vegetation.tree.eucalyptus.maculata.vswir.jpl087.jpl.asd.spectrum.txt"
#define WATER_PATH "spectrums/water.tapwater.none.liquid.all.tapwater.jhu.becknic.spectrum.txt"

#define WAVELENGTH_FIELD "wavelength"
#define END_FIELD "}"

#define SPECTRUM_FIRST_VALUE_FIELD "First X Value"
#define SPECTRUM_LAST_VALUE_FIELD "Last X Value" 
#define FROM_LAST_VALUE_TO_VALUES 3

#define REFLECTANCES_FOLDER "reflectances/"

#define ASC 2   // 0 and 1 are EXIT_SUCCESS and EXIT_FAILURE
#define DESC 3

#define FLOAT_MAX 3.4028234663852886e+38F

#define WAVELENGTH_UNIT_REFACTOR 1000

using namespace std;

static int n_pixels = WIDTH * HEIGHT * CHANNELS;

int read_img(float *img) {
    ifstream file(IMG_PATH, ios::binary);
    if(!file.is_open()){
        cout << "Error opening the file, it could not be opened. Aborting." << endl;
        return(EXIT_FAILURE);
    }

    int index = 0;
    char buffer[DATA_SIZE];
    short int value;
    while(index < n_pixels){
        file.read(buffer, DATA_SIZE);

        memcpy(&value, buffer, DATA_SIZE);
        img[index] = static_cast<float>(value);
        index++;
    }
    file.close();

    if (index != n_pixels-1){
        cout << "Error reading file, the number of pixels read was not the expected. Aborting." << endl;
        return EXIT_FAILURE;
    }
    else
        return EXIT_SUCCESS;
}

int read_hdr(float *wavelengths){
    ifstream file(HDR_PATH);
    if(!file.is_open()){
        cout << "Error opening the file, it could not be opened. Aborting." << endl;
        return(EXIT_FAILURE);
    }

    map<string, string> headerData;
    string line;
    int index = 0;
    while (getline(file, line)) {
        istringstream lineStream(line);
        string key;
        if (getline(lineStream, key, '=')) {

            key.erase(key.find_last_not_of(" \n\r\t")+1); //erase spaces or any dirty char
            if(key == WAVELENGTH_FIELD){
                string value;
                while (1) {
                    getline(file, line, ',');
                    stringstream numbers(line);
                    numbers >> wavelengths[index];
                    index++;
                    if(line.find(END_FIELD) != string::npos)
                        break;
                }
                
                }
            }
        }

    for(int i = 0; i < CHANNELS; i++){
        wavelengths[i] /= WAVELENGTH_UNIT_REFACTOR;
    }
    file.close();
    return EXIT_SUCCESS;
}

void save_reflectances(ifstream& file, float previous_reflectance, float previous_diff, float *wavelengths, float *reflectances, int order){
    string line, segment;
    int reflectances_position = 0, wavelengths_position = CHANNELS - 1;
    if (order == ASC)
        wavelengths_position = 0;
    float final_wavelength = wavelengths[CHANNELS - 1], previous_wavelength, wavelength_read, diff, reflectance_read;

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
            else if(wavelengths_position <= CHANNELS  && order == ASC){
                wavelengths_position++;
                previous_diff = FLOAT_MAX;
            }
            else 
                break;                                                                  
        }
        reflectances[reflectances_position] = previous_reflectance;
    } 

    
    if (order == DESC) {
        int swap_index = CHANNELS - 1;
        float *aux = (float*)malloc(CHANNELS * sizeof(float));
        memcpy(aux, reflectances, CHANNELS * sizeof(float));
        for(int i = 0; i < CHANNELS; i++){
            reflectances[swap_index] = aux[i];
            swap_index--;
        }
        free(aux);
    }                                
}

int read_spectrum(float initial_wavelength, float final_wavelength, float *reflectances, float *wavelengths, string path){
    int number_of_reflectances, order;
    float reflectance_read, previous_reflectance, reflectance, previous_diff, diff, wavelength_read, previous_wavelength;
    streampos begin_pointer;

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

int main(){

    float *reflectances = (float*)malloc(CHANNELS * sizeof(float));
    float *channels = (float*)malloc(CHANNELS * sizeof(float));
    float *image = (float*)malloc(n_pixels * sizeof(float));

    if (read_hdr(channels) == EXIT_FAILURE)
        return EXIT_FAILURE;  

    int spectrum_wavelengths_order = read_spectrum(channels[0], channels[CHANNELS - 1], reflectances, channels, TREE_PATH);
    if (spectrum_wavelengths_order == EXIT_FAILURE)
        return EXIT_FAILURE;  
    
    
    for(int i = 0; i < CHANNELS; i++){
        cout << i << ": " << reflectances[i] << endl;
    }
    /*
    if (read_img(image) == EXIT_FAILURE)
        return EXIT_FAILURE;
    */
    
    cout << "Freeing reflectances..." << endl;
    free(reflectances);
    cout << "Freeing imgage..." << endl;
    free(image);
    cout << "Freeing channels..." << endl;
    free(channels);
    cout << "Freed" << endl;
    return(EXIT_SUCCESS);
}