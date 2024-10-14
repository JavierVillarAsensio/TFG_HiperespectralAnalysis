#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <string>
#include <fstream>

extern int width, height, n_channels, header_offset;
extern std::string wavelength_unit_hdr;


int read_img_bil(float *img, const char* filename);
int get_factor_scale(std::string unit);
int read_hdr(float *wavelengths);
void save_reflectances(std::ifstream& file, float *wavelengths, float *reflectances, int order);
int read_spectrum(float initial_wavelength, float final_wavelength, float *reflectances, float *wavelengths, std::string path);
void calculate_distance_of_every_pixel_to_spectrum(float *image, float *reflectances, float *distances);
int write_distances_file(float *distances, const std::string output_file);
int get_index();
std::string get_spectrum_file_name();
std::string get_output_file_name(std::string file_path, std::string folder, std::string extension);

#endif