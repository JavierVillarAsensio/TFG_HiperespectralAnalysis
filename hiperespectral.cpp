/*
    La lectura es bil, lo que quiere decir que lee
    el primer pixel de cada banda, luego el segundo...
*/


#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstring>

#define WIDTH 100
#define HEIGHT 100
#define CHANNELS 198
#define DATA_SIZE 2

#define IMG_PATH "jasperRidge2_R198/jasperRidge2_R198.img"
#define HDR_PATH "jasperRidge2_R198/jasperRidge2_R198.img"

using namespace std;


static int n_pixels = WIDTH * HEIGHT * CHANNELS;

int read_img(float *img) {
    ifstream file(IMG_PATH, std::ios::binary);
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

    if (index != n_pixels-1)
        return EXIT_FAILURE;
    else
        return EXIT_SUCCESS;
}

int main(){
    
    
    
    float *image = (float*)malloc(n_pixels * sizeof(float));
    if (read_img(image) == EXIT_FAILURE) {
        cout << "Error reading file, the number of pixels read was not the expected. Aborting." << endl;
        return EXIT_FAILURE;
    }
    
    return(EXIT_SUCCESS);
}