// DO NOT MODIFY THIS FILE!!!!

#include "matting.h"

// define a descriptive string (ie. label) for each image
// these labels are what is returned by the get_title() method

static const vcl_string emptystr("");
static const vcl_string b1str("Background 1");
static const vcl_string b2str("Background 2");
static const vcl_string c1str("Composite 1");
static const vcl_string c2str("Composite 2");
static const vcl_string ostr("Object");
static const vcl_string astr("Alpha Matte");
static const vcl_string nbstr("New Background");
static const vcl_string ncstr("New Composite");

// Return a descriptive label for each type of image used in the
// matting algorithm

const vcl_string& matting::get_title(im_type imt)
{
	switch (imt) {
	case Back1:
		return b1str;
		break;
	case Back2:
		return b2str;
		break;
	case Comp1:
		return c1str;
		break;
	case Comp2:
		return c2str;
		break;
	case Object:
		return ostr;
		break;
	case Alpha:
		return astr;
		break;
	case NewBack:
		return nbstr;
		break;
	case NewComp:
	default:
		return ncstr;
	}
}

//
//  Constructor and accessor methods for setting & retrieving
//  the input/output of the triangulation matting algorithm
//

matting::matting(void) 
{

	// initially we do not have any images loaded
	outdated_ = true;
	alpha_computed_ = false;
	new_composite_computed_ = false;
	first_image_ = true;
}


bool matting::set(im_type imt, vil_image_view<vil_rgb<vxl_byte> > im)
{
	switch (imt) {
	case Back1:
		return check_and_set_input(im, back_1_);
		break;
	case Back2:
		return check_and_set_input(im, back_2_);
		break;
	case Comp1:
		return check_and_set_input(im, comp_1_);
		break;
	case Comp2:
		return check_and_set_input(im, comp_2_);
		break;
	default:
		return false;
	}
}

// methods that return an image that is the input or output
// of the matting algorithm
// it returns an empty image if the algorithm has not been run yet on the
// input images (or if some of the images required by the algorithm
// have not been specified yet)

// this get() method is designed for returning all images except
// the alpha component (which is single-band and treated in another
// get() method
vil_image_view<vil_rgb<vxl_byte> > matting::get(im_type imt)
{
	vil_image_view<vil_rgb<vxl_byte> > empty;

	switch (imt) {
	case Object:
		// we can only reutrn an image if the algorithm has been run
		// on the most-recently specified input images
		if ((alpha_computed_ == true) && (outdated_ == false)) 
			return object_;
		else
			return empty;
		break;
	case NewComp:
		// we can only reutrn an image if the algorithm has been run
		// on the most-recently specified input images
		if ((new_composite_computed_ == true) && (outdated_ == false) && (alpha_computed_ == true))
			return new_comp_;
		else
			return empty;
		break;
	case NewBack:
		// we can only reutrn an image if the algorithm has been run
		// on the most-recently specified input images
		if ((new_composite_computed_ == true) && (outdated_ == false))
			return new_back_;
		else
			return empty;
		break;
	case Back1:
		if ((bool) back_1_ == true)
			return back_1_;
		else
			return empty;
		break;
	case Comp1:
		if ((bool) comp_1_ == true)
			return comp_1_;
		else
			return empty;
		break;
	case Back2:
		if ((bool) back_2_ == true)
			return back_2_;
		else
			return empty;
		break;
	case Comp2:
		if ((bool) comp_2_ == true)
			return comp_2_;
		else
			vcl_cerr << "matting::get(): Warning: cannot be used to retrieve an alpha image. Use get1() instead." << vcl_endl;
			return empty;
		break;
	default:
		return empty;
	}
}

// get1() function, designed to return the Alpha component only
vil_image_view<vxl_byte>  matting::get1(im_type imt)
{
    vil_image_view<vxl_byte> empty;

	switch (imt) {
	case Alpha:
		// we can only reutrn an image if the algorithm has been run
		// on the most-recently specified input images
		if ((alpha_computed_ == true) && (outdated_ == false))
			return alpha_;
		else
			vcl_cerr << "matting::get1(): Warning: cannot be used to retrieve a non-alpha image. Use get() instead." << vcl_endl;
			return empty;
		break;
	default:
		return empty;
	}
}

// routine that updates an input image of the class after first
// checking that the supplied image is valid
bool matting::check_and_set_input(vil_image_view<vil_rgb<vxl_byte> > input_im,
												vil_image_view<vil_rgb<vxl_byte> > &im)
{
	// does the input image contain valid image data?
	if ((bool) input_im == false)
		// if not, we return false
		return false;
		
	if (first_image_ == true) {
		// this is the first image specified, so we must set the dimensions
		// that all input images should have
		ni_ = input_im.ni();
		nj_ = input_im.nj();
		first_image_ = false;

		// add the image to the input dataset
		im = input_im;
		outdated_ = true;

		return true;
	} else
		if ((input_im.ni() == ni_) && (input_im.nj() == nj_)) {
			// ok, the image matches the dimensions of the other images
			// in the dataset, so we add it to the dataset
			im  = input_im;
			outdated_ = true;
			return true;
		} else
			// the dimensions of this image do not match the dimensions of 
			// the already-specified images so we cannot include it in the 
			// input dataset
			return false;
}

// if the requested internal variable im contains valid image data, this method will
// return true and will set the variable out_im to point to the 
// image; otherwise it returns false
bool matting::check_and_get_input(vil_image_view<vil_rgb<vxl_byte> > im,
												vil_image_view<vil_rgb<vxl_byte> > &out_im)
{
	if ((bool) im == true) {
		out_im = im;
		return true;
	} else
		return false;
}

