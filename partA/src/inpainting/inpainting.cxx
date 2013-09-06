// DO NOT MODIFY THIS FILE!!!!

#include "inpainting.h"

// define a descriptive string (ie. label) for each image
// these labels are what is returned by the get_title() method

static const vcl_string emptystr("");
static const vcl_string srcstr("Source");
static const vcl_string inpstr("Inpainted");
static const vcl_string grystr("Greyscale Source");
static const vcl_string constr("Confidence Term");
static const vcl_string datstr("Data Term");
static const vcl_string bdystr("Fill Front");
static const vcl_string uflstr("Unfilled Pixels");
static const vcl_string iflstr("Initial Mask");

// Return a descriptive label for each type of image used in the
// matting algorithm

const vcl_string& inpainting::get_title(im_type imt)
{
	switch (imt) {
	case Source:
		return srcstr;
		break;
	case Inpainted:
		return inpstr;
		break;
	case Greyscale:
		return grystr;
		break;
	case Confidence:
		return constr;
		break;
	case Fill_Front:
		return bdystr;
		break;
	case Unfilled:
		return uflstr;
		break;
	default:
	case Init_Filled:
		return iflstr;
		break;
	}
}

//
//  Constructor and accessor methods for setting & retrieving
//  the input/output of the inpainting algorithm
//

void inpainting::default_params()
{
	// initially we do not have any images loaded
	outdated_ = true;
	inpainted_computed_ = false;
	inpainted_partial_ = false;
	inpainted_new_region_ = false;
	first_image_ = true;
	ni_ = nj_ = 0;

	// don't show any debugging information
	debug_show_PSI_ = false;
	debug_show_vecs_ = false;
	debug_print_ = false;
	// drawing is not enabled by default
	draw_enabled_ = false;


	// by default use 9x9 patches (so radius is 4)
	w_ = 4;

	// by default set alpha = 255 (as recommended in 
	// the paper)
	alpha_ = 255;

	pdb_ = 0;
	delta_Omega_ = 0;
}

inpainting::inpainting() 
{
	default_params();
}

void inpainting::add_panels(ImDraw* lpanel, ImDraw* rpanel)
{
	left_panel_ = lpanel;
	right_panel_ = rpanel;

	// since we have pointers to the display panels
	// we can enable drawing
	draw_enabled_ = true;
	draw_enabled_ = true;
}

inpainting::inpainting(ImDraw* lpanel, ImDraw* rpanel)
{
	default_params();

	add_panels(lpanel, rpanel);
}

void inpainting::set_alpha(int alpha)
{
	alpha_  = alpha;
}

void inpainting::set_patch_radius(int prad)
{
	// we can only change the patch radius if we have not
	// already started the inpainting algorithm

	if (inpainted_partial_ == false)
		w_ = prad;
}

//
// Routines for setting / accessing images (mostly for communicating
// with the UI)
//

bool inpainting::set(im_type imt, vil_image_view<vil_rgb<vxl_byte> > im)
{
	switch (imt) {
	case Source:
		return check_and_set_input(im, source_);
		break;
	default:
		return false;
	}
}

bool inpainting::set(im_type imt, vil_image_view<bool> im)
{
	switch (imt) {
	case Init_Filled:
		return check_and_set_input(im, init_filled_);
		break;
	default:
		return false;
	}
}

bool inpainting::set(im_type imt, vil_image_view<vxl_byte> im)
{
	switch (imt) {
	case Init_Filled:
		return check_and_set_input(im, init_filled_);
		break;
	default:
		return false;
	}
}


// methods that return an image that is the input or output
// of the inpainting algorithm
// it returns an empty image if the algorithm has not been run yet on the
// input images (or if some of the images required by the algorithm
// have not been specified yet)

// this get() method is designed for returning  3-component images 
// (1-component images are handled in the get1() method)
vil_image_view<vil_rgb<vxl_byte> > inpainting::get(im_type imt)
{
	vil_image_view<vil_rgb<vxl_byte> > empty;

	switch (imt) {
	case Inpainted:
		// we can only reutrn an image if the algorithm has been run
		// on the most-recently specified input images
		if (((inpainted_computed_ == true) || (inpainted_partial_ == true)) && 
			(outdated_ == false)) 
			return inpainted_;
		else
			return empty;
		break;
	case Source:
		if ((bool) source_ == true)
			return source_;
		else
			return empty;
		break;
	default:
		return empty;
	}
}

// get1() method, designed to return 1-component images only
vil_image_view<vxl_byte>  inpainting::get1(im_type imt)
{
    vil_image_view<vxl_byte> empty;
	vil_image_view<vxl_byte> byteim;

	switch (imt) {
	case Unfilled:
		if (((inpainted_computed_ == true) || (inpainted_partial_ == true)) &&
			(outdated_ == false)) {
			// map {false,true} to {0,255}
			vil_convert_stretch_range(unfilled_, byteim);
			return byteim;
		} else
			return empty;
		break;
	case Fill_Front:
		if (((bool) fill_front_ == true) && (outdated_ == false)) {
			// map {false,true} to {0,255}
			vil_convert_stretch_range(fill_front_, byteim);
			return byteim;
		} else
			return empty;
		break;
	case Init_Filled:
		if ((bool) init_filled_ == true) {
			// map {false,true} to {0,255}
			vil_convert_stretch_range(init_filled_, byteim);
			return byteim;
		} else
			return empty;
		break;
	case Confidence:
		if (((inpainted_computed_ == true) || (inpainted_partial_ == true)) && 
			(outdated_ == false)) {
			// map {false,true} to {0,255}
			vil_convert_stretch_range(C_, byteim);
			return byteim;
		} else
			return empty;
		break;
	case Greyscale:
		if (((inpainted_computed_ == true) || (inpainted_partial_ == true)) && 
			(outdated_ == false)) {
			// map {false,true} to {0,255}
			vil_convert_stretch_range(inpainted_grey_, byteim);
			return byteim;
		} else
			return empty;
		break;
	default:
		return empty;
	}
}

// routine that updates an input image of the class after first
// checking that the supplied image is valid
bool inpainting::check_and_set_input(vil_image_view<vil_rgb<vxl_byte> > input_im,
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
		// currently we can only deal with 3-band images
		nplanes_ = 3;
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

// routine that updates an input image of the class after first
// checking that the supplied image is valid
// identical with method above, but this version works for bool-type images
bool inpainting::check_and_set_input(vil_image_view<bool> input_im,
									 vil_image_view<bool> &im)
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

// routine that updates an input image of the class after first
// checking that the supplied image is valid
// identical with method above, but this version works for vxl_byte-type images
bool inpainting::check_and_set_input(vil_image_view<vxl_byte> input_im,
									 vil_image_view<bool> &im)
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
		vil_convert_cast(input_im, im);
		outdated_ = true;

		return true;
	} else
		if ((input_im.ni() == ni_) && (input_im.nj() == nj_)) {
			// ok, the image matches the dimensions of the other images
			// in the dataset, so we add it to the dataset
			// if it is a grayscale image, map all intensities below
			// 254 to 0 
			//vil_clamp_below(input_im, (vxl_byte)254, (vxl_byte)0);
			vil_convert_cast(input_im, im);
			outdated_ = true;
			return true;
		} else
			// the dimensions of this image do not match the dimensions of 
			// the already-specified images so we cannot include it in the 
			// input dataset
			return false;
}


