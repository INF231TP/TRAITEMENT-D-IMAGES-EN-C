#ifndef IMAGE_OPS_H
#define IMAGE_OPS_H
#include "ppm.h"

char dominante_of_pixels(int R, int G, int B);

void op_dom (Image *img, char C, int val);

void op_gris (Image *img);

void op_size (const Image *img);

int op_cut (const Image *img, int l1, int l2, int c1, int c2, const char *out_filename);

int op_neg (const Image *img, const char *out_filename);

int op_fil (const Image *img, const char *out_filename);

#endfil