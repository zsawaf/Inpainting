// DO NOT MODIFY THIS FILE

#ifndef load_image_h
#define load_image_h

#include "../vxl_includes.h"

// a function that takes a filename as input and returns a 3-component
// rgb VXL image if successful, otherwise returns and image with 
// zero rows and columns
// the function can be used to read many image formats, including
// jpeg and pnm images
vil_image_view<vil_rgb<vxl_byte> > load_image(vcl_string fname);

// a function that takes a filename as input and returns a 
// VXL image if successful, otherwise returns and image with 
// zero rows and columns
// the function can be used to read many image formats, including
// jpeg and pnm images
vil_image_view<vxl_byte> load_image1(vcl_string fname);


#endif

