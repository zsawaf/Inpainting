// DO NOT MODIFY THIS FILE

// (c) Kyros Kutulakos 10/15/99 
//    modified on 10/15/99 to include a disable() method in the
//    Texture class Adapted for use in CSC290C course
//
//  Modified by F. Estrada on 05/10/04 to replace Vista library
//   with VXL lib.
//
// Note to CSC320 students: You are not required to read and/or
// understand the code provided in this file. You may want to browse
// through the code at a later date, when we begin covering the topic
// of Texture Mapping in the course lectures

#include "glutils.h"
#include "Texture.h"

#include "../vxl_includes.h"

// Default sizes for the texture memory
// Usually the texture memory size is specified by calling the
// texture_set_globals() function.
// See the file imdraw_utils.cxx for an example

static int MaxTextureWidth = 512;
static int MaxTextureHeight = 512;
static int MaxTextureObjects = 3;

static int initialized=0;
static GLuint *tex_names;
static int top;

int texture_set_globals(int max_texture_width, int max_texture_height,
			int max_texture_objects)
{
  max_texture_objects += 1;
  if (!initialized) {
    MaxTextureWidth = max_texture_width;
    MaxTextureHeight = max_texture_height;
    MaxTextureObjects = max_texture_objects;
    return 1;
  } else if ((MaxTextureWidth != max_texture_width) || 
	     (MaxTextureHeight != max_texture_height) || 
	     (MaxTextureObjects != max_texture_objects))
    return 0;
  return 1;
}

int texture_width()
{
  return MaxTextureWidth;
}

int texture_height()
{
  return MaxTextureHeight;
}

static int get_new_texobject()
{
  if (initialized) {
    if (top < MaxTextureObjects)
      return tex_names[top++];
    else
      return -1;
  }
  else {
    initialized = 1;
    if ((tex_names = (GLuint *)
	 calloc(MaxTextureObjects, sizeof(GLuint))) == NULL) {
      vcl_cerr << "Error at get_new_texobject(), out of memory for tex_names";
      exit(1);
    }
    glGenTextures(MaxTextureObjects, tex_names);
    top = 0;
    return tex_names[top++];
  }
}

Texture::Texture(vil_image_view<vil_rgb<vxl_byte> > im)
{
  int to;
  vil_image_view<vil_rgb<vxl_byte> > flipped;
  double ni, nj;

  // Get a texture object name 
  if ((to = get_new_texobject()) < 0) {
    vcl_cerr << "Texture::Texture: No more texture objects to allocate\n";
    exit(1);
  } else {
	  // check that the image fits in texture memory
	  if ((im.ni() > MaxTextureWidth) || (im.nj() > MaxTextureWidth)) {
		  vcl_cerr << "Texture::Texture: Texture memory too small\n";
		  exit(1);
	  }
  }

  tex_object = to;
  ni = im.ni();
  nj = im.nj();

  max_x = ni/MaxTextureWidth;
  max_y = nj/MaxTextureHeight;

  // Convert VXL image to a packed GL RGB format
  flipped = vil_flip_ud(im);			// Vertically flip image
  tex_data = glutils_VXL_to_RGB(flipped);	// create texture data

  // Bind texture name to the 2D texture target
  glBindTexture(GL_TEXTURE_2D, tex_object);
  // Create the texture image
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
	       MaxTextureWidth, MaxTextureHeight,
	       0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  // Copy image data to texture memory
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
		  ni, nj,
		  GL_RGB, GL_UNSIGNED_BYTE, tex_data);

  // Set texturing parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  // we need to use GL_NEAREST if we want to ensure that 
  // enlarged pixels map to a constant-intensity square in the
  // opengl canvas
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glEnable(GL_TEXTURE_2D);

  // free allocated memory
  free(tex_data);
  flipped.clear();
}

void Texture::refresh(vil_image_view<vil_rgb<vxl_byte> > im)
{
  vil_image_view<vil_rgb<vxl_byte> > flipped;

  activate();

  if ((im.ni() > MaxTextureWidth) || (im.nj() > MaxTextureWidth)) {
	  vcl_cerr << "Texture::Texture: Texture memory too small\n";
	  exit(1);
  }

  // Convert VXL image to a packed GL RGB format
  flipped = vil_flip_ud(im);			// Flip image vertically

  tex_data = glutils_VXL_to_RGB(flipped);

  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
		  im.ni(), im.nj(),
		  GL_RGB, GL_UNSIGNED_BYTE, tex_data);

  max_x = im.ni()*1.0/MaxTextureWidth;
  max_y = im.nj()*1.0/MaxTextureHeight;

  // free allocated memory
  free(tex_data);
  flipped.clear();
}

void Texture::activate()
{
  glBindTexture(GL_TEXTURE_2D, tex_object);
}

void Texture::deactivate()
{
  //glDisable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
}


void Texture::get_bounds(GLdouble& mx, GLdouble& my)
{
  mx = max_x;
  my = max_y;
}

void Texture::get_dimensions(int& w, int& h)
{
  w = MaxTextureWidth;
  h = MaxTextureHeight;
}

void Texture::set_bounds(int width, int height)
{
  max_x = width*1.0/MaxTextureWidth;
  max_y = height*1.0/MaxTextureHeight;
}
  
