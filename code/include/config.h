#ifndef CONFIG_H
#define CONFIG_H

#include <string>

/////////////////////////spectrum_replica.cpp/////////////////////////

//directories management
#define IMG_PATH "jasperRidge2_R198/jasperRidge2_R198.img"
#define HDR_PATH "jasperRidge2_R198/jasperRidge2_R198.hdr"

#define OUTPUT_DISTANCES_FOLDER "output/distances"
#define OUTPUT_DISTANCES_EXTENSION ".bin"
#define OUTPUT_LOG_FOLDER "output/logs"
#define OUTPUT_LOG_EXTENSION ".log"
#define SPECTRUM_FOLDER "spectrums"

//hdr needed fields
#define WAVELENGTH_FIELD "wavelength"
#define ROWS_FIELD "lines"
#define COLS_FIELD "samples"
#define CHANNELS_FIELD "bands"
#define HEADER_OFFSET_FIELD "header offset"
#define WAVELENGTH_UNIT_FIELD "wavelength units"
#define DATA_TYPE_FIELD "data type"
#define END_FIELD "}"

//spectrum needed fields
#define SPECTRUM_FIRST_VALUE_FIELD "First X Value"
#define SPECTRUM_LAST_VALUE_FIELD "Last X Value" 
#define SPEC_WAVELENGTH_UNIT_FIELD "X Units"

//find spec
#define INDEX_FILE_VAR "SERVICE_NAME"

//common numbers
constexpr int DATA_SIZE = sizeof(short);

constexpr int FROM_LAST_VALUE_TO_VALUES = 3; //offset between fields

constexpr int ASC = 2;   // 0 and 1 are EXIT_SUCCESS and EXIT_FAILURE
constexpr int DESC = 3;

constexpr float FLOAT_MAX = 3.4028234663852886e+38F;
constexpr float FLOAT_MIN = 1.175494e-38F;
constexpr int PERCENTAGE_REFACTOR = 100;

constexpr int SIZE_OF_KNOWN_METRICS = 4;
const std::string KNOWN_METRICS[SIZE_OF_KNOWN_METRICS] = {"micrometer", "nanometer", "angstrom", "picometer"};
constexpr int SCALE_FACTORS[SIZE_OF_KNOWN_METRICS] = {6, 9, 10, 12};

/////////////////////////master.cpp/////////////////////////

#define DISTANCES_FOLDER "output/distances"
#define HDR_PATH "jasperRidge2_R198/jasperRidge2_R198.hdr"

#define ROWS_FIELD "lines"
#define COLS_FIELD "samples"

#define RESULT_FILE "output/result.jpg"
#define LEGEND_FILE "output/legend.txt"
#define COMPARATION_FILE "output/comparation"
#define MASTER_LOG_FILE "output/logs/master.log"
#define CONFUSION_FILE "output/confusion_matrix.txt"

#define NUM_FILES_VAR "NUM_FILES"
#define N_FILES_FILE "n_files.txt"
#define MAT_FILE "jasperRidge2_R198/end4.mat"

#define PERFORMANCE "performance"
constexpr int PERFORMANCE_OPTION = 1;

#endif