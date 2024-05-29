/*
    The format is bil so it is read the firstpixel of every channel
*/


#include <iostream>
#include <fstream>
#include <unistd.h>
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

#define ASC 2   // 0 and 1 are EXIT_SUCCESS and EXIT_FAILURE
#define DESC 3

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

int read_hdr(float *channels){
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
                    numbers >> channels[index];
                    index++;
                    if(line.find(END_FIELD) != string::npos)
                        break;
                }
                
                }
            }
        }

    file.close();
    return EXIT_SUCCESS;
}

int read_spectrum(float initial_wavelength, float final_wavelength, float *reflectances){
    ifstream file(WATER_PATH);
    if(!file.is_open()){
        cout << "Error opening the file, it could not be opened. Aborting." << endl;
        return(EXIT_FAILURE);
    }

    string line, segment;
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
            break;
        }
    }


    file.close();
    if (first_value > last_value)
        return DESC;
    else
        return ASC;
}

int main(){

    float *reflectances = (float*)malloc(CHANNELS * sizeof(float));
    int spectrum_wavelengths_order = read_spectrum(0, 0, reflectances);
    if (spectrum_wavelengths_order == EXIT_FAILURE)
        return EXIT_FAILURE;

    /*
    float *channels = (float*)malloc(CHANNELS * sizeof(float));
    if (read_hdr(channels) == EXIT_FAILURE)
        return EXIT_FAILURE;    

    float *image = (float*)malloc(n_pixels * sizeof(float));
    if (read_img(image) == EXIT_FAILURE)
        return EXIT_FAILURE;
    */
    return(EXIT_SUCCESS);
}