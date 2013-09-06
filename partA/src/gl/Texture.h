// DO NOT MODIFY THIS FILE

// (c) Kyros Kutulakos 10/1/99 
//
//    modified 10/15/99 to include a disable() method in the Texture
//    class. 
//
//    modified 05/10/04 to replace Vista libraty with VXL. F. Estrada
//
//    Adapted for use in CSC320

#ifndef _texture_h
#define _texture_h

// The libraries below are required to manipulate VXL images
#include "../vxl_includes.h"


class Texture {
  GLdouble max_x;
  GLdouble max_y;
  GLuint tex_object;
  GLubyte *tex_data;
 public:
  // Create a texture associated with the given VXL image
  Texture(vil_image_view<vil_rgb<vxl_byte> > im);

  // Update the texture contents with a new VXL image
  void refresh(vil_image_view<vil_rgb<vxl_byte> > im);

  // Enable texture mapping & use image currently stored in texture memory for 
  // all texturing operations
  void activate();
  // Disable texture mapping
  void deactivate();

  // Return the maximum x and y coordinates of the texture in texture
  // memory. This is useful when the texture memory is larger than the 
  // image used for texture-mapping
  void get_bounds(GLdouble& max_x, GLdouble& max_y);

  // specify the dimensions of the "real" texture in texture memory
  // this is useful if the image occupies a region smaller than
  // the entire texture memory (max_x = image width, max_y = image_height)
  void set_bounds(int max_x, int max_y);

  // Get the dimensions of texture memory
  void get_dimensions(int& w, int& h);
};

// Initialize the texturing parameters
// Returns 1 if parameters were initialized by the routine or 0 if they were not
int texture_set_globals(int max_texture_width, int max_texture_height,
			int max_texture_objects);

// Return the width & height of the allocated texture memory
int texture_width();
int texture_height();

#endif
