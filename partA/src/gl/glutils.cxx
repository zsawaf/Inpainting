// DO NOT MODIFY THIS FILE

// (c) Kyros Kutulakos 10/1/99
//
// Jan 99: Adapted for use in CSC320 course
//
// May 04: Converted by F. Estrada to use the VXL library
// 
// Jan 06: Replaced RGBA conversion with RGB conversion
//

#include "glutils.h"
#include "../vxl_includes.h"


// converts a VXL image to an image suitable for GL-based texture mapping
GLubyte* glutils_VXL_to_RGB(vil_image_view<vil_rgb<vxl_byte> > im)
{
  GLubyte *data, *ptr;
  int i, j, ni, nj;

  ni = im.ni();
  nj = im.nj();

  // allocate space for the texture
  data = new GLubyte[ni*nj*3];
	  
//	  (GLubyte *) 
//    calloc(ni*nj*3, sizeof(GLubyte));

  if (!data) {
    vcl_cerr << "glutils_VXL_to_RGB(): calloc failed\n";
    return 0;
  }

  // copy the VXL image data to the allocated texture space
  ptr = data;
  for (j=0; j < nj; j++) 
      for (i=0; i < ni; i++) {
		  *(ptr++) = im(i,j).r;
		  *(ptr++) = im(i,j).g;
		  *(ptr++) = im(i,j).b;
	  }

  return data;
}


vil_image_view<vil_rgb<vxl_byte> > glutils_RGB_to_VXL(GLubyte* data, int width, int height)
{
  GLubyte *ptr;
  vil_image_view<vil_rgb<vxl_byte> > im(width,height);	// Createn a new rgb image
  int i, j, b;

  if (im.size_bytes()<(width*height*3*sizeof(vxl_byte))) {
    vcl_cerr<< "glutils_RGB_to_VXL: vil image creation failed";
    return im;
  }
  //im.clear();					// Clear the new image

  // copy the texture data to the VXL image
  ptr = data;
  for (j=0; j<height; j++) 
	  for (i=0; i<width; i++) {
		  im(i,j).r = *(ptr++);
		  im(i,j).g = *(ptr++);
		  im(i,j).b = *(ptr++);
	  }
  return im;
}

vil_image_view<vxl_byte> glutils_ALPHA_to_VXL(GLubyte* data, int width, int height)
{
  GLubyte *ptr;
  vil_image_view<vxl_byte> im(width,height);	// Createn a new rgb image
  int i, j, b;

  if (im.size_bytes()<(width*height*sizeof(vxl_byte))) {
    vcl_cerr<< "glutils_ALPHA_to_VXL: vil image creation failed";
    return im;
  }
  //im.clear();					// Clear the new image

  // copy the texture data to the VXL image
  ptr = data;
  for (j=0; j<height; j++) 
	  for (i=0; i<width; i++) {
		  im(i,j) = *(ptr++);
	  }
  return im;
}

// stores the contents of the current GL buffer to a VXL rgb image
vil_image_view<vil_rgb<vxl_byte> > glutils_read_VXLimage(int width, int height) 
{
  GLubyte *data;
  vil_image_view<vil_rgb<vxl_byte> > im,flipped;
 
  // allocate space to hold the GL buffer data
  data = (GLubyte *) calloc(width*height*3, sizeof(GLubyte));
  if (!data) {
    vcl_cerr << "glutils_read_VXLimage: calloc failed\n";
    exit(1);
  }
  // copy the pixels from the GL buffer to the allocated space
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
  flipped = glutils_RGB_to_VXL(data, width, height);

  // image needs to be flipped because VXL images have their (0,0) point 
  // at the top-left corner of the image, while OpenGL panels have their (0,0)
  // point at their bottom-left corner

  im=vil_flip_ud(flipped);
  free(data);
  return im;
}

// stores the contents of the current GL buffer to a VXL rgb image
vil_image_view<vxl_byte> glutils_read_alpha_VXLimage(int width, int height) 
{
  GLubyte *data;
  vil_image_view<vxl_byte> im,flipped;

  // allocate space to hold the GL buffer data
  data = (GLubyte *) calloc(width*height, sizeof(GLubyte));
  if (!data) {
    vcl_cerr << "glutils_read_alpha_VXLimage: calloc failed\n";
    exit(1);
  }
  // copy the pixels from the GL buffer to the allocated space
  glReadPixels(0, 0, width, height, GL_ALPHA, GL_UNSIGNED_BYTE, data);
  flipped = glutils_ALPHA_to_VXL(data, width, height);

  // image needs to be flipped because VXL images have their (0,0) point 
  // at the top-left corner of the image, while OpenGL panels have their (0,0)
  // point at their bottom-left corner

  im=vil_flip_ud(flipped);
  free(data);
  return im;
}
