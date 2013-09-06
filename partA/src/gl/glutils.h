// DO NOT MODIFY THIS FILE

// (c) Kyros Kutulakos 10/15/99
// modified on 05/10/04 to replace Vista library with VXL. F. Estrada

#ifndef _glutils_h
#define _glutils_h

#include "../fltk_includes.h"

// we must undefine the max() and min() macros, as they interfere
// with VXL class declarations
#undef max
#undef min


// VXL libraries
#include<core/vil/vil_image_view.h> 
#include<core/vil/vil_rgb.h>
#include<core/vil/vil_convert.h>
#include<core/vil/vil_image_resource.h>
#include<core/vil/vil_flip.h>

#define glutils_FONT_DISPLAY_LIST_BASE 100

// Convert a VXL  image into an RGBA format appropriate for
// glCopyPixels, glDrawPixels, glTexImage2D, and general texture-mapping 
// operations and return the pointer 
GLubyte* glutils_VXL_to_RGB(vil_image_view<vil_rgb<vxl_byte> > im);

// Draw a VXL image using OpenGL 
void glutils_draw_VXLimage(vil_image_view<vil_rgb<vxl_byte> > im);

// Create a VXL image with the contents of the current GL buffer
// The routine requires the dimensions of the GL buffer
vil_image_view<vil_rgb<vxl_byte> > glutils_read_VXLimage(int width, int height);

// Create a VXL image with the contents of the current alpha plane in the GL buffer
// The routine requires the dimensions of the GL buffer
vil_image_view<vxl_byte> glutils_read_alpha_VXLimage(int width, int height);

// Convert the current RGBA OpenGL buffer into a 3-component color VXL image
vil_image_view<vil_rgb<vxl_byte> > glutils_RGB_to_VXL(GLubyte* data, int width, int height);

#endif
