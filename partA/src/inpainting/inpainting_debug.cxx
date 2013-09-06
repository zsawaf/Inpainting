// 
//  USE THIS FILE FOR YOUR INPAINTING-SPECIFIC CODE
//
//  DO NOT MODIFY THIS FILE !!!
//

#include "inpainting.h"

//
// Display and printing routines helpful for debugging
// your inpainting code
//

// print to stderr the contents of patch in_patch
// in image. title is just a string to print along
// with the contents of the patch

// note that this routine only works on grayscale images
// so if you want to print the intensities in a single
// channel of an RGB image, you need to use the vil_plane()
// routine
void inpainting::debug_print_psi(const psi& in_patch, 
								 const vil_image_view<vxl_byte>& image,
								 const vcl_string& title)
{
	if (debug_print_) {
		// create a local copy of the patch
		psi patch(in_patch);

		// create a VXL matrix that will hold the patch intensities
		vnl_matrix<int> intensities, valid_pixels;

		// copy the pixels from the image into the matrix
		patch.get_pixels(image, intensities, valid_pixels);

		// print the matrix to stderr. 
		// note that we have to print a transposed version of the 
		// matrix because rows in a VXL matrix correspond to columns
		// in a VXL image
		vcl_cerr 
			<< "------------------------------------------" << vcl_endl
			<< title << " center (" << patch.p()(0) << "," << patch.p()(1) << ") "
			<< "width=" << patch.w() << vcl_endl 
			<< "Intensities:" << vcl_endl
			<< intensities.transpose() << vcl_endl
			<< "Valid pixels:" << vcl_endl
			<< valid_pixels.transpose() << vcl_endl
			<< "------------------------------------------" << vcl_endl;
	}
}

//
// You can use the following routines if you want to show specific
// patches or vectors on the opegl panels, for debugging purposes
//

// draw a vector of color (red, green, blue)
// that begins at the center of patch
// and is along the direction indicated by vec
//
void inpainting::debug_draw_vec(const psi& patch,
								const vnl_double_2& vec,
								double red, double green,
								double blue)
{
	// we cannot proceed if drawing is not enabled
	if (draw_enabled_ == false)
		return;

	double gi, gj;
	vnl_double_2 gradn;
	int i, j;

	if (debug_show_vecs_) {
		// gradient at the center of PSI_hat_p
		gradn = vec;
		// make sure the vector is of unit length
		gradn.normalize();
		gi = gradn(0);
		gj = gradn(1);
		// coordinates of the center of patch PSI_hat_p
		i = (int)(patch.p()(0));
		j = (int)(patch.p()(1));

		// draw a vector that begins at (i,j), is in the
		// direction (gi, gj), its scaled by a factor of 10
		// and its color is green
		left_panel_->draw_object(new imdraw_vec(gj, gi, j, i, 10, 
									red, green, blue));
		right_panel_->draw_object(new imdraw_vec(gj, gi, j, i, 10, 
									red, green, blue));
	}
}

// draw the outline of a patch using color
// (red, green, blue) where the color ranges
// from 0 to 1
void inpainting::debug_draw_patch(const psi& patch, 
								  double red, double green, 
								  double blue)
{
	// we cannot proceed if drawing is not enabled
	if (draw_enabled_ == false)
		return;

	int i1, i2, j1, j2;

	if (debug_show_PSI_) {
		// get the top-left and bottom-right vertex of the patch
		patch.image_vertices(j1, i1, j2, i2);		

		// draw the rectangle in both panels using color (R,G,B)
		// (ie. yellow)
		left_panel_->draw_object(new imdraw_rect(j1, i1, j2, i2, 
			red, green, blue));
		right_panel_->draw_object(new imdraw_rect(j1, i1, j2, i2, 
			red, green, blue));
	}
}


//
// Methods for controlling the display of debugging informatoin
//

//
// for each iteration, show the highest-priority patch and 
// its most-similar patch in the source image
//
void inpainting::debug_show_PSI(bool yes)
{
	debug_show_PSI_ = yes;
}

//
// for each iteration, show (1) the gradient used to calculate
// the data term of the highest-priority patch and (2) the
// normal to the fill front curve at the center of that patch
// (also used for data term calculation)
//
void inpainting::debug_show_vecs(bool yes)
{
	debug_show_vecs_ = yes;
}

// enable printing to stderr (useful for printing out
// the intensities in an image patch)
void inpainting::debug_print(bool yes)
{
	debug_print_ = yes;
}


//
// this routine causes all debugging graphics from the left 
// and right panel to be deleted
//
void inpainting::debug_reset()
{
	// we cannot proceed if drawing is not enabled
	if (draw_enabled_ == false)
		return;

	if (debug_show_PSI_ || debug_show_vecs_) {
		left_panel_->clear_objects();
		right_panel_->clear_objects();
	}

	// choose which image to show on the left panel
	switch (debug_display_left_) {
	case Inpainted:
	case Source:
		left_panel_->set(get(debug_display_left_), 
			             get_title(debug_display_left_));
		break;
	case Fill_Front:
	case Init_Filled:
	case Unfilled:
	case Confidence:
	case Greyscale:
		left_panel_->set(get1(debug_display_left_), 
			             get_title(debug_display_left_));
		break;
	}
	// choose which image to show on the right panel
	switch (debug_display_right_) {
	case Inpainted:
	case Source:
		right_panel_->set(get(debug_display_right_), 
			             get_title(debug_display_right_));
		break;
	case Fill_Front:
	case Init_Filled:
	case Unfilled:
	case Confidence:
	case Greyscale:
		right_panel_->set(get1(debug_display_right_), 
			             get_title(debug_display_right_));
		break;
	}
}

//
// controls which image will be shown in the two panels
//

void inpainting::debug_display_left(im_type imt)
{
	debug_display_left_ = imt;
}

void inpainting::debug_display_right(im_type imt)
{
	debug_display_right_ = imt;
}



/////////////////////////////////////////////////////////

