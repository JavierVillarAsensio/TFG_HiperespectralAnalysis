#ifndef CONFIG_TEST_H
#define CONFIG_TEST_H

#include <string>
#include <iostream>

////////////GENERAL DATA////////////
#define TEST_IMG_FILE_PATH "../jasperRidge2_R198.img"
#define TEST_SPEC_FILE_PATH "../spectrums_test/"
#define PROJECT_ROOT "../../../"

extern const std::string ERROR = "\033[31mERROR\033[0m";   // Red
extern const std::string PASS  = "\033[32mPASS\033[0m";    // Green
extern const std::string TEST_RESULTS[2] = {PASS, ERROR};

constexpr int N_TEST_SPECTRUM_FILES  = 2;

////////////IMAGE DATA////////////
constexpr short int TESTING_IMG[] = {
    9, 11,   19, 21,   8, 12,
    18, 22,   10, 10,   20, 20,
    8, 12,   19, 21,   9, 11
};
size_t TESTING_IMG_N_ELEMENTS = sizeof(TESTING_IMG) / sizeof(TESTING_IMG[0]);

////////////HDR DATA////////////
constexpr int TEST_SAMPLES = 3;
constexpr int TEST_LINES   = 3;
constexpr int TEST_BANDS   = 2;
constexpr int TEST_HEADER_OFFSET = 0;
constexpr int TEST_DATA_TYPE = 2;
const std::string TEST_WAVELENGTH_UNITS = "Nanometers";
constexpr int TEST_SCALE_FACTOR = 10000.000000;
constexpr float WAVELENGTHS[TEST_BANDS] = {2.0, 4.0};

////////////SPECTRUMS DATA////////////
constexpr float TEST_SPECTRUMS_CORRECT_REFLECTANCES[N_TEST_SPECTRUM_FILES] = {10.0, 20.0};

#endif