/*
    La lectura es bil, lo que quiere decir que lee
    el primer pixel de cada banda, luego el segundo...
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

#define WAVELENGTH_FIELD "wavelength"
#define END_FIELD "}"
#define CHANNELS_PER_LINE 6

using namespace std;

static int n_pixels = WIDTH * HEIGHT * CHANNELS;

int read_img(float *img) {
    ifstream file(IMG_PATH, ios::binary);
    if(!file.is_open()){
        cout << "Error opening the file, it could not be opened. Aborting." << endl;
        return(EXIT_FAILURE);
    }

    int index = 0;
    char buffer[2];
    short int value;
    while(index < n_pixels){
        file.read(buffer, 2);

        memcpy(&value, buffer, 2);
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

int main(){

    float *channels = (float*)malloc(CHANNELS * sizeof(float));
    if (read_hdr(channels) == EXIT_FAILURE)
        return EXIT_FAILURE;    

    float *image = (float*)malloc(n_pixels * sizeof(float));
    if (read_img(image) == EXIT_FAILURE)
        return EXIT_FAILURE;

    return(EXIT_SUCCESS);
}