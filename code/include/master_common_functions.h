#ifndef MASTER_H
#define MASTER_H

#include <fstream>
#include <string>

using namespace std;

extern int width, height;
extern size_t file_count;

int count_files(const string directory);
int read_hdr();
int count_files();
int read_all_distances(float *all_distances, size_t distances_size, string *materials, string distances_folder);
void find_nearest_materials(int file_count, size_t distances_size, float *all_distances, int *nearest_materials_images);
int write_jpg(int *nearest_materials_image, size_t distances_size);
int write_legend(string *materials, size_t file_count);
void print_separation_line(ofstream &stream);
int write_comparison(int *nearest_materials_image, int *to_compare, size_t distances_size);
int compare_result(int *nearest_materials_image, size_t distances_size, string *materials);
void monitor_files(int expected_n_files);
void wait_files(int expected_n_files);

#endif