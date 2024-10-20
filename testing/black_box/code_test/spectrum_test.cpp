#include "spectrum_common_functions.h"
#include "master_common_functions.h"
#include "config_test.h"
#include "config.h"
#include <filesystem>
#include <math.h>
#include <algorithm>
#include <iostream>

using namespace std;

int width, height, n_channels, header_offset, n_files;
string wavelength_unit_hdr;

int write_test_img_bil(){
    short int bil_image[TESTING_IMG_N_ELEMENTS];
    FILE *file = fopen(TEST_IMG_FILE_PATH, "wb");
    if (file == NULL){
        cout << "Error opening testing file to write. Aborting..." << endl;
        return EXIT_FAILURE;
    }

    //change to bilS
    int line_length = TEST_BANDS * TEST_SAMPLES, calculated_index;
    for(int index = 0; index < TESTING_IMG_N_ELEMENTS; index++){
        //                              column offset                               band offset                                  line offset
        calculated_index = (int)((index%TEST_SAMPLES)*TEST_BANDS)   +   (int)((index/TEST_SAMPLES)%TEST_BANDS)   +   (int)((index/line_length)*line_length);
        bil_image[index] = TESTING_IMG[calculated_index];
    }

    fwrite(bil_image, sizeof(short int), TESTING_IMG_N_ELEMENTS, file);

    fclose(file);

    return EXIT_SUCCESS;
}

void get_test_spectrums(string *specs_filenames){
    int index = 0;
    filesystem::path path = filesystem::current_path().parent_path();
    cout << "path:" << path << endl;
    for (const auto& entry : filesystem::directory_iterator(TEST_SPEC_FILE_PATH)) {
        if(index == N_TEST_SPECTRUM_FILES)
            break;

        if (entry.is_regular_file()) 
            specs_filenames[index] = entry.path().string();

        index++;
    }
}

int check_hdr_values(){
    bool check = true;

    if(width != TEST_SAMPLES){
        check = false;
        cout << "\t\tError reading width: " << width << ", expected value: " << TEST_SAMPLES << endl;
    }
    if(height != TEST_LINES){
        check = false;
        cout << "\t\tError reading height: " << height << ", expected value: " << TEST_LINES << endl;
    }
    if(n_channels != TEST_BANDS){
        check = false;
        cout << "\t\tError reading number of bands: " << n_channels << ", expected value: " << TEST_BANDS << endl;
    }
    if(header_offset != TEST_HEADER_OFFSET){
        check = false;
        cout << "\t\tError reading header offset: " << header_offset << ", expected value: " << TEST_HEADER_OFFSET << endl;
    }
    if(wavelength_unit_hdr != TEST_WAVELENGTH_UNITS){
        check = false;
        cout << "\t\tError reading wavelengths unit: " << wavelength_unit_hdr << ", expected value: " << TEST_WAVELENGTH_UNITS << endl;
    }

    if(check)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}

bool compare_floats_eq(float a, float b, float comparison_tolerance = 0.00001){
    return (fabs(a - b) < comparison_tolerance);
}

int check_read_reflectances(float *read_reflectanes, int spec_index){
    float spec_reflectance = TEST_SPECTRUMS_CORRECT_REFLECTANCES[spec_index];

    for(int index = 0; index < TEST_BANDS; index++)
        if(!compare_floats_eq(read_reflectanes[index], spec_reflectance))
            return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int check_read_image_bil(float *read_img){
    int line_length = TEST_BANDS * TEST_SAMPLES, calculated_index;
    for(int index = 0; index < (TEST_BANDS * TEST_LINES * TEST_SAMPLES); index++){
        //                              column offset                               band offset                                  line offset
        calculated_index = (int)((index%TEST_SAMPLES)*TEST_BANDS)   +   (int)((index/TEST_SAMPLES)%TEST_BANDS)   +   (int)((index/line_length)*line_length);
        if(!compare_floats_eq(read_img[index], (float)TESTING_IMG[calculated_index]/TEST_SCALE_FACTOR))
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int check_calculation_distances(float *calculated_distances, int spec_index){
    float sum, spec_reflectance = TEST_SPECTRUMS_CORRECT_REFLECTANCES[spec_index];

    for(int index = 0; index < (height * width); index += 2){
        sum = 0;
        sum = pow(((TESTING_IMG[index]/TEST_SCALE_FACTOR) - spec_reflectance), 2.0) +
              pow(((TESTING_IMG[index + 1]/TEST_SCALE_FACTOR) - spec_reflectance), 2.0);

        if(!compare_floats_eq(calculated_distances[(index/TEST_BANDS)+(index%TEST_BANDS)], sqrt(sum)))
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int check_nearest_materials(int *nearest_materials_image){
    for(int index = 0; index < (TEST_LINES * TEST_SAMPLES); index++)
        if(!(nearest_materials_image[index] == nearest_materials_image[index % 2]))
            return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int testing_static(){
    if(write_test_img_bil()){
        cout << "Error writing binary test image. Aborting..." << endl;
        return EXIT_FAILURE;
    }

    string test_spec_filenames[N_TEST_SPECTRUM_FILES];
    get_test_spectrums(test_spec_filenames);

    int tests_done, tests_passed, test_result, spec_index = 0;

    for(string filename : test_spec_filenames){
        cout << "\nChecking tests with test spectrum: " << filename << endl;
        tests_done = 0, tests_passed = 0;

        ///STEP 1: EXTRACT METADATA///
        float *channels;
        if (read_hdr(&channels, TEST_HDR_FILE_PATH))
            return EXIT_FAILURE;

        tests_done++;
        test_result = check_hdr_values();
        if(!test_result)
            tests_passed++;
        cout << "\tTEST 1: Extract metadata from .hdr file test: " << TEST_RESULTS[test_result] << endl;

        ///STEP 2: EXTRACT REFLECTANCES OF REFERENCE///
        float *reflectances = (float *)malloc(n_channels * sizeof(float));
        if (read_spectrum(channels[0], channels[n_channels - 1], reflectances, channels, filename))
            return EXIT_FAILURE; 
        free(channels);

        tests_done++;
        test_result = check_read_reflectances(reflectances, spec_index);
        if(!test_result)
            tests_passed++;
        cout << "\tTEST 2: Extract reflectances of reference test: " << TEST_RESULTS[test_result] << endl;
    
        ///STEP 3: READ HIPERESPECTRAL IMAGE///
        size_t n_pixels = width * height * n_channels;
        float *image = (float*)malloc(n_pixels * sizeof(float));
        if(read_img_bil(image, TEST_IMG_FILE_PATH))
            return EXIT_FAILURE;

        tests_done++;        
        test_result = check_read_image_bil(image);
        if(!test_result)
            tests_passed++;
        cout << "\tTEST 3: Read hiperespectral image test: " << TEST_RESULTS[test_result] << endl;
    
        ///STEP 4: DISTANCE BETWEEN REFERENCE AND IMAGE///
        float *distances = (float*)malloc(width * height * sizeof(float));
        calculate_distance_of_every_pixel_to_spectrum(image, reflectances, distances);

        tests_done++;
        test_result = check_calculation_distances(distances, spec_index);
        if(!test_result)
            tests_passed++;
        cout << "\tTEST 4: Calculate distance between reference and image: " << TEST_RESULTS[test_result] << endl;
        

        if(write_distances_file(distances, get_output_file_name(filename, OUTPUT_DISTANCES_FOLDER, OUTPUT_DISTANCES_EXTENSION)))
            return EXIT_FAILURE;
        
        spec_index++;
    }

    ///STEP 5: CLASSIFICATION///
    size_t distances_size = TEST_SAMPLES * TEST_LINES;
    float *all_distances = (float*)malloc(distances_size * N_TEST_SPECTRUM_FILES * sizeof(float));
    string materials[N_TEST_SPECTRUM_FILES];
    if(read_all_distances(all_distances, distances_size, materials, DISTANCES_FOLDER))
        return EXIT_FAILURE;

    int nearest_materials_image[distances_size];
    find_nearest_materials(N_TEST_SPECTRUM_FILES, distances_size, all_distances, nearest_materials_image);

    tests_done++;
    test_result = check_nearest_materials(nearest_materials_image);
    if(!test_result)
        tests_passed++;
    cout << "\nTEST 5: Material classification: " << TEST_RESULTS[test_result] << endl;

    ///TEST 6: COLOURED MAP///
    write_jpg(nearest_materials_image, distances_size);
    tests_done++;
    if(filesystem::exists(TEST_RESULT_JPG_PATH)){
        tests_passed++;
        test_result = 0;
    }
    else {
        cout << "The result.jpg file was not created" << endl;
        test_result = 1;
    }
    cout << "\nTEST 6: Coloured map creation: " << TEST_RESULTS[test_result] << endl;

    if(tests_passed == tests_done)
        test_result = EXIT_SUCCESS;
    else
        test_result = EXIT_FAILURE;
    cout << "\nStatic tests passed: " << tests_passed << "/" << tests_done << ": " << TEST_RESULTS[test_result] << endl << endl;

    return EXIT_SUCCESS;
}

int main(){
    if(testing_static()){
        cout << "An unexpected error in the static version of testing ocurred, the error was not caused by the checking process" << endl;
        return EXIT_FAILURE;
    }
}