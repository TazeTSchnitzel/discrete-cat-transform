#ifndef NETPBM_H
#define NETPBM_H

#include <stdio.h>
#include <stdbool.h>

#include "image.h"

image *ppm_read(FILE *fp);
bool ppm_write(FILE *fp, image *img);

#endif
