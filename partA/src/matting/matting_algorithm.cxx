// 
//  USE THIS FILE FOR YOUR MATTING-SPECIFIC CODE
//
//  DO NOT MODIFY THIS FILE ANYWHERE EXCEPT WHERE 
//  EXPLICITLY NOTED!!!!
//




#include "matting.h"


//
//  The Triangulation Matting algorithm
//
//

bool matting::compute(void)
{
	if ((alpha_computed_ == true) && (outdated_ == false))
		// the results have already been computed, so
		// we have nothing to do
		return true;

	if (((bool) comp_1_ == false) || 
		((bool) comp_2_ == false) || 
		((bool) back_1_ == false) || 
		((bool) back_2_ == false))
		// we do not have enough information yet to run the
		// algorithm, we have nothing to do
		return false;

	// Ok, we have enough information to proceed
	
	// first let us allocate space for the result images
	alpha_.set_size(ni_, nj_);
	object_.set_size(ni_, nj_);


	//////////////////////////////////////////////////
	// PLACE YOUR CODE BETWEEN THESE LINES          //
	//////////////////////////////////////////////////

	/////////////////////////////////////////////////


	alpha_computed_ = true;
	outdated_ = false;

	return true;
}

bool matting::compute_composite(vil_image_view<vil_rgb<vxl_byte> > input_im,
				vil_image_view<vil_rgb<vxl_byte> > &output_im)
{
	//////////////////////////////////////////////////
	// PLACE YOUR CODE BETWEEN THESE LINES          //
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////

	return true;
}

//////////////////////////////////////////////////
// PLACE ANY ADDITIONAL CODE BETWEEN THESE LINES//
//////////////////////////////////////////////////

//////////////////////////////////////////////////
