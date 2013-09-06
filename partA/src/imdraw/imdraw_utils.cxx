// DO NOT MODIFY THIS FILE!!!!!!
//
// (c) Kyros Kutulakos 06/01/04
//
// Modified by F. Estrada on May 10, 2004 to replace Vista with the VXL 
//  library.
//
// This file gives an example of how an image can be displaed in an
// OpenGL panel which is part of an fltk window
//
// The idea behind the image display code below is to display an image
// by drawing a rectangle on the screen whose dimensions are equal to
// the initial size of the OpenGL panel. Rather than giving the
// rectangle a single color, however, we texture-map the VXL image
// to be displayed in that rectangle.
//
// Doing the image display in this way will be useful for a number of
// assignments later on, because it allows us to shrink, distort,
// enlarge, or warp an image by simply changing the size and position
// of the rectancle's 4 vertices.
//
// To better understand the code in this file, you may want to refer
// to the Texture Mapping section of the OpenGL programming guide


#include <math.h>

#include "../fltk_includes.h"
#include "../gl/glutils.h"
#include "imdraw.h"



// maximum dimensions of the texture map
// this has to be a power of 2
#define MAX_TEXTURE_SIZE 1024

// total number of texture maps to be used in the 
// program. it should be at least equal to the number
// of opengl panels in your fluid-designed interface
#define MAX_TEXTURES 2

// define minimum and maximum image magnification 
#define MIN_MAGNIFICATION (1.0/256)
#define MAX_MAGNIFICATION 256


// Sets the image being displayed in the window
bool ImDraw::set(vil_image_view<vil_rgb<vxl_byte> > new_im, vcl_string title)
{
	 // make sure the image points to valid data
     if ((bool) new_im == false) {
			return false;
     }

	 // I have discovered that the glTexSubImage2D behaves erratically
	 // when the number of image columns is not divisible by 4, so I
	 // have now disallowed the display of such images
	 if (new_im.ni() % 4 != 0) {
		 vcl_cerr << "ImDraw::set(): Number of image columns must be divisible by 4!" << vcl_endl;
		 return false;
	 }
     
     // Direct all further OpenGL commands to this OpenGL panel
     // This is NECESSARY if there are more than one OpenGL panels on 
     // the screen!
     make_current();
     if (imTexture == NULL) {
	  // These commands are executed only once, when the variable
	  // containing the image to be displayed points to a valid
	  // VXL image.  The command initializes the OpenGL
	  // texturing operations
	  
	  // Allocate space in texture memory for a single texture
	  // that has a maximum size of MAX_TEXTURE_SIZExMAX_TEXTURE_SIZE
	  
	  // Note that texture dimensions have to be powers of 2 and
	  // that the dimensions of any VXL image to be used as
	  // texture must have dimensions smaller than the texture
	  // dimensions.
	  
	  // You will need to call this routine with different
	  // parameters if you are to display VXL images larger than
	  // 1024x1024 (this is probably large enough and you might
	  // want to use smaller images to conserve memory & disk
	  // space
	  texture_set_globals(MAX_TEXTURE_SIZE, MAX_TEXTURE_SIZE, MAX_TEXTURES);
      imTexture = new Texture(new_im);
	} else
	  imTexture->refresh(new_im);
	 
	 // Set the flag that indicates that the interface is displaying a
	 // valid image
	 valid_image = true;
	 // Store the pointer to the image being displayed (useful for
	 // displaying a pixel's value during image browsing)
	 current_image = new_im;
     
     aspect_ratio = new_im.ni()*1.0/new_im.nj();
     im_height = new_im.nj();
     im_width = new_im.ni();

     // compute the initial magnification factor of the VXL image

     // define the width and height of the canvas area that will be
     // occupied by the displayed image, in order to preserve the
     // image's aspect ratio (which may be different from the aspect
     // ratio of the canvas window)
     double vh, vw;

     vh = (height < width/aspect_ratio)?height:(width/aspect_ratio);
     vw = vh*aspect_ratio;
     
     // Return the magnification factor of the image being
     // displayed. This is important if we want to get the coordinates
     // of the drawn rectangle to match pixel coordinates of the VXL
     // image being drawn
     //magnification = ;

	 // Store the magnification that 'fits' the image to the OpenGL
	 // canvas
	 fit_magnification = vw/im_width;

	 // Compute the VXL coordinates of the pixel at the center of the
	 // OpenGL canvas
	 image_center_j = vnl_rational(im_height/2).round();
	 image_center_i = vnl_rational(im_width/2).round();
	 view_j = image_center_j;
	 view_i = image_center_i;

	 // image is not displayed in canonical view
	 canonical_view_ = false;

	 update_view_data_in_ui(title);
	 // Center the image on the OpenGL canvas
	 //shift_j = shift_i = 0;
     
     // Redraw the entire window to show the new image
     Fl_Window::redraw();

	 return true;

}

// Sets the image being displayed in the window
// This version of the routine handles the case where the image has
// a single band/plane and had to be converted to an rgb image first
bool ImDraw::set(vil_image_view<vxl_byte> new_im, vcl_string title)
{
	vil_image_view<vil_rgb<vxl_byte> > im_rgb;

	if ((bool) new_im == false) 
		return false;

	// convert the first plane/band of the image into an RGB image
	// with identical color components
	im_rgb.set_size(new_im.ni(), new_im.nj()); 
	int i, j; 
	for (i=0; i<new_im.ni(); i++) 
		for (j=0; j<new_im.nj(); j++) 
			im_rgb(i,j).r = im_rgb(i,j).g = im_rgb(i,j).b = new_im(i,j); 
	return set(im_rgb,title); 
}

// Return the magnification factor of the image being displayed
double ImDraw::mag(void)
{
	return magnification;
}

// Set the magnification factor of the image being displayed
bool ImDraw::mag(double m)
{
	// check that the magnification value is within a valid range
	if ((m > MIN_MAGNIFICATION) && (m < MAX_MAGNIFICATION)) {
		// change magnification factor
		magnification = m;
		// ensure that the image is centered at 
		//the currently-selected image location
		recenter(view_i, view_j);
		// update the view information in the user interface
		update_view_data_in_ui();

		return true;
	} else {
	// if the magnification is invalid, revert to the magnification
	// that fits the entire image in the window
		magnification = fit_magnification;
		// ensure that the image is centered at the currently-selected
		// image location
		recenter(view_i, view_j);
		update_view_data_in_ui();

		return false;
	}
}
	
// Converts GL canvas coordinates to VXL coordinates. VXL
// coordinates are integers so that they can be used as indices into a
// VXL image array.  The function returns 1 if the GL coordinates
// corresponds to a point within the image boundary
int ImDraw::gl2vxl(double glx, double gly, int& i, int& j)
{
	// if image is empty, do nothing
	if (!valid_image)
		return 0;

	 j = vnl_rational(gly/magnification - 0.5).round() + shift_j;
	 i = vnl_rational(glx/magnification - 0.5).round() + shift_i;

     if ((j >= 0) && (j < im_height) && (i >= 0) && (i < im_width))
	  return 1;
     else 
	  return 0;
}

// Converts VXL coordinates to GL canvas coordinates
int ImDraw::vxl2gl(int i, int j, double& glx, double& gly)
{
	// if image is empty, do nothing
	if (!valid_image)
		return 0;

	gly = (j-shift_j)*magnification + 0.5;
     glx = (i-shift_i)*magnification + 0.5;

     if ((gly >= 0) && (gly < height) || (glx >= 0) && (glx < width))
	  return 1;
     else 
	  return 0;
}

// Converts VXL coordinates to GL canvas coordinates
int ImDraw::vxl2gl(double i, double j, double& glx, double& gly)
{
 	// if image is empty, do nothing
	if (!valid_image)
		return 0;

    gly = (j-shift_j)*magnification + 0.5;
     glx = (i-shift_i)*magnification + 0.5;

     if ((gly >= 0) && (gly < height) || (glx >= 0) && (glx < width))
	  return 1;
     else 
	  return 0;
}


// Displays a VXL image in a GL panel.  The image is displayed by
// texture-mapping it onto a polygon whose size is equal to the size
// of the GL panel
void ImDraw::draw_image()
{

	 double tex_width, tex_height;
     double w, h;
	 double relative_mag;
     
     // texture-map polygons with the current texture
     if (imTexture != NULL) {
	  // Record the texture coordinates of the top-right corner of
	  // the image
	  imTexture->get_bounds(tex_width, tex_height);
	  // Enable GL texture mapping
	  imTexture->activate();
     }
  
     // define the width and height of the canvas area that will be
     // occupied by the displayed image, in order to preserve the
     // image's aspect ratio (which may be different from the aspect
     // ratio of the canvas window)
     h = (height < width/aspect_ratio)?height:(width/aspect_ratio);
     w = h*aspect_ratio;

	 
	 relative_mag = magnification/fit_magnification;

     // The following statements are what actually display the image
     // in the panel.  The statements draw a rectangle defined by
     // (0,0) and (w, h); this rectangle is texture-mapped with a
     // rectangular image whose texture coordinates are defined by
     // (0,0) and (tex_width, tex_height)
 
     glColor3d(0.0, 0.0, 0.0);
     glBegin(GL_POLYGON);
     {
	  glTexCoord2d(0,0);
	  glVertex2d(-shift_i*magnification,h*relative_mag-shift_j*magnification);
	  
	  glTexCoord2d(0,tex_height);
	  glVertex2d(-shift_i*magnification,-shift_j*magnification);
	  
	  glTexCoord2d(tex_width, tex_height);
	  glVertex2d(w*relative_mag-shift_i*magnification, -shift_j*magnification);
	  
	  glTexCoord2d(tex_width,0);
	  glVertex2d(w*relative_mag-shift_i*magnification, h*relative_mag-shift_j*magnification);
     }
     glEnd();
     glFlush();
     
     // Disable texture mapping for the OpenGL commands that follow
     if (imTexture != NULL) {
	  imTexture->deactivate();
     }
}

// shift the image so that the pixel (r, c), in VXL coordinates,
// is at the center of the OpenGL canvas
void ImDraw::recenter_j(int j)
{
	if ((j >=0) && (j < im_height)) {
		// calculate the shift, in VXL coordinates, needed to 
		// bring pixel under the mouse to the center of the OpenGL
		// canvas
		shift_j = j - (int)(image_center_j*fit_magnification/magnification);
		view_j = j;
	}
	update_view_data_in_ui();

	canonical_view_ = false;
}

// shift the image so that the pixel (r, c), in VXL coordinates,
// is at the center of the OpenGL canvas
void ImDraw::recenter_i(int i)
{
	if ((i >=0) && (i < im_width)) {
		// calculate the shift, in VXL coordinates, needed to 
		// bring pixel under the mouse to the center of the OpenGL
		// canvas
		shift_i = i - (int)(image_center_i*fit_magnification/magnification);
		view_i = i;
	}
	update_view_data_in_ui();

	canonical_view_ = false;
}

// shift the image so that the pixel (i, j), in VXL coordinates,
// is at the center of the OpenGL canvas
void ImDraw::recenter(int i, int j)
{
	recenter_i(i);
	recenter_j(j);
}


// Routine that intercepts window resize operations performed by the user
void ImDraw::resize(int x, int y, int w, int h)
{
     // the window has been resized so we need to update the width & height 
     // information 
     width = w;
     height = h;

     // Need to call this method ensure that the GL panel "knows" that
     // it has been resized as well...
     Fl_Gl_Window::resize(x, y, w, h);

     // determine the new magnification of the VXL image

     // define the width and height of the canvas area that will be
     // occupied by the displayed image, in order to preserve the
     // image's aspect ratio (which may be different from the aspect
     // ratio of the canvas window)
     double vh, vw;
     vh = (height < width/aspect_ratio)?height:(width/aspect_ratio);
     vw = vh*aspect_ratio;
     
     // Return the magnification factor of the image being
     // displayed. This is important if we want to get the coordinates
     // of the drawn rectangle to match pixel coordinates of the VXL
     // image being drawn
     magnification = vw/im_width;

	 canonical_view_ = false;
}

void ImDraw::update_pixel_data_in_ui(int i, int j, int band1, int band2, int band3)
{
	char buf_pos[80], buf_data[80];

	// create the two strings to be displayed in the interface
	sprintf(buf_pos, "(%d,%d)", i, j);
	sprintf(buf_data, "(%d,%d,%d)", band1, band2, band3);

	// access the Fl_Output widget that displays the
	// cursor's VXL coordinates
	// This widget has index 1 because it is the second
	// widget of the group created through fluid

	((Fl_Output *)(parent()->child(1)))->value(buf_pos);

	// access the Fl_Output widget that displays the
	// image's RGB values
	// This widget has index 2 because it is the third
	// widget of the group created through fluid
	((Fl_Output *)(parent()->child(2)))->value(buf_data);

	// we need to redraw the window
	redraw();
}

void ImDraw::update_view_data_in_ui()
{
	char bufz[80], bufr[80], bufc[80];

	sprintf(bufz, "%g", magnification);
	sprintf(bufr, "%d", view_j);
	sprintf(bufc, "%d", view_i);

	// access the Fl_Output widget that displays the
	// cursor's VXL coordinates
	// This widget has index 3 because it is the 3rd
	// widget of the group created through fluid
	((Fl_Input *)(parent()->child(3)))->value(bufz);
    // update the widgets displaying the centered pixel coordinates
	((Fl_Input *)(parent()->child(4)))->value(bufc);
	((Fl_Input *)(parent()->child(5)))->value(bufr);

	// we need to redraw the window
	redraw();
}

void ImDraw::update_view_data_in_ui(vcl_string title)
{
	((Fl_Output *)(parent()->child(6)))->value(title.c_str());
	update_view_data_in_ui();
}


void ImDraw::update_view_data_from_ui()
{
	double m;
	int j; 
	int i;

	sscanf(((Fl_Input *)(parent()->child(3)))->value(), "%lg", &m);
	sscanf(((Fl_Input *)(parent()->child(4)))->value(), "%d", &i);
	sscanf(((Fl_Input *)(parent()->child(5)))->value(), "%d", &j);

	//  update the view settings
	mag(m);
	recenter(i, j);

	// we need to redraw the window
	redraw();
}

void ImDraw::set_canonical_view()
{
	// make sure the image is fully contained in the opengl panel
	// occupies its entire width or length
	mag(-1);
	// recenter the image so that its upper-left hand corder
	// is at the upper-left hand corner of the window
	recenter(image_center_i, image_center_j);

	canonical_view_ = true;
}

bool ImDraw::canonical_view()
{
	return canonical_view_;
}

vil_image_view<bool> ImDraw::get_alpha_image()
{
	vil_image_view<vxl_byte> im, cropped;
	vil_image_view<bool> resized(im_width, im_height);

	// copy the contents of the opengl panel to an image
	// that is the same size as the panel itself
	im = glutils_read_alpha_VXLimage(width, height);

	// now we need to extract the portion corresponding to the
	// image being displayed in the panel and then resize it
	// to the size of the image being currently displayed
	cropped = vil_crop(im, 0, im_width*fit_magnification, 0, im_height*fit_magnification);

	// resize the bitmap
	int i, j;
	for (i=0; i<im_width; i++)
		for (j=0; j<im_height; j++)
			resized(i, j) = cropped(i*fit_magnification, j*fit_magnification);

	return resized;
}


