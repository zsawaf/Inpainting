// 
//  THIS CONTAINS THE TOP-LEVEL ROUTINES FOR 
//  THE INPAINTING ALGORITHM
//
//  YOU NEED TO READ & UNDERSTAND THE ROUTINES HERE
//  VERY WELL, BUT DO NOT MODIFY THIS FILE!!!!
//

#include "inpainting.h"
#include "inpainting_eval.h"


//
//  The Exemplar-Based Inpainting algorithm
//
//

bool inpainting::compute()
{
	return compute(0);
}

bool inpainting::compute(int max_iterations)
{
	int ui, uj;
	int iterations_done;

	if ((inpainted_computed_ == true) && (outdated_ == false))
		// the results have already been computed, so
		// we have nothing to do
		return true;

	if (outdated_ == true) {
		// we need to recalculate everything
		inpainted_computed_ = false;
		inpainted_partial_ = false;
	}

	if (((bool) source_ == false) || 
		((bool) init_filled_ == false))
		// we do not have enough information yet to run the
		// algorithm, we have nothing to do
		return false;

	// Ok, we have enough information to proceed
	
	// if no pixels have been inpainted yet, 
	// we need to initialize the data structures
	// used by the algorithm
	if (inpainted_partial_ == false)
		initialize();

	// if max_iterations is zero, that means we need to run the 
	// algorithm until the entire image is inpainted
	if (max_iterations == 0)
		max_iterations = ni_ * nj_;

	//
	// Step 1a of the algorithm in Table 1: Identify the fill front
	//
	// To do this, we search for the first unfilled pixel in the image;
	// this pixel must be on the boundary of the filled region, i.e.,
	// the fill front delta_Omega
	iterations_done = 0;


	while ((find_first_unfilled_pixel(ui, uj) == true) &&
		   (iterations_done < max_iterations)) {

		// Then we call a vil 'boundary detection' routine that collects all 
		// the pixels on the boundary and returns their coordinates in 
		// two vectors
		// These pixels define the initial fill front delta_Omega

		// The vectors below will hold the i and j coordinates of the
		// boundary pixels: the coordinates of the n-th pixel on the
		// initial fill front delta_Omega will be (boundary_i[n], boundary_j[n])
		vcl_vector<int> boundary_i;
		vcl_vector<int> boundary_j;

		// Run the boundary detector routine. 
		vil_trace_8con_boundary(boundary_i, boundary_j, unfilled_, ui, uj);

		// run inpainting algorithm for a single, connected, unfilled region
		// of the image
		// the routine runs the algorithm for a maximum of max_iterations,
		// i.e., at most max_iterations patches will be filled
		inpaint_region(boundary_i, boundary_j, max_iterations, iterations_done);

		if (iterations_done < max_iterations)
			// we exited the inpaint_region routine because the
			// entire region has been inpainted so we now move to
			// a new region
			inpainted_new_region_ = true;
	}

	if (iterations_done < max_iterations) {
		inpainted_partial_ = false;
		inpainted_computed_ = true;
		// show the results
		// show the inpainted image on the left panel
		debug_display_left(Inpainted);
		// show the original image on the right panel
		debug_display_right(Source);
		// clear all other graphics
		debug_reset();
	}
	if (iterations_done > 0)
		inpainted_partial_ = true;

	outdated_ = false;

	return true;
}

//
//  Main loop of the inpainting algorithm
//
//

void inpainting::inpaint_region(
			const vcl_vector<int>& boundary_i, 
			const vcl_vector<int>& boundary_j,
			int max_iterations, 
			int& iterations_done)
{	

	//
	// Step 1a: mark all pixels that belong to the fill front
	//          and create a patch centered at each one of those pixels
	//

	if (inpainted_new_region_ == true) {
		// we execute this initialization step only at the very first
		// iteration of the region inpainting algorithm
		initialize_fill_front(boundary_i, boundary_j);
		inpainted_new_region_ = false;
	}


	// loop until all patches have been removed/filled or
	// we have reached the maximum number of iterations
	while ((delta_Omega_->empty() == false) && 
		   (iterations_done < max_iterations)) {

		vcl_cerr << "Iteration " << iterations_done << vcl_endl;

		// clear all debugging information displayed on the OpenGL
		// panels
		debug_reset();

		//
		// Step 1b: (re)compute the priority of each patch 
		//          in the priority list
		//

		recompute_patch_priorities();

		// if the priority list is empty, all pixels in the region have
		// been inpainted, so we are done!
		if (delta_Omega_->empty())
			break;

		//
		// Step 2a: find the maximum-priority patch and remove 
		//          it from the priority list
		//

		// get the patch at the top of the priority list 
		// (ie. the highest-priority patch)
		psi PSI_hat_p(delta_Omega_->top());

		// remove it from the list
		delta_Omega_->pop();

		// if debugging is on, draw it in yellow on the UI panels
		debug_draw_patch(PSI_hat_p, 1, 1, 0);

		// Step 2b: find the 'example' patch, psi_hat_q, in the 
		//          original image that is most similar to this 
		//          patch
		psi PSI_hat_q(find_best_match(PSI_hat_p));

		// if debugging is on, draw it in red on the UI panels
		debug_draw_patch(PSI_hat_q, 1, 0, 0);

		// Step 2c: copy into the unfilled pixels of PSI_hat_p the
		//          corresponding pixels from PSI_hat_q and return
		//          a binary mask that indicates which pixels were
		//          newly filled
		vnl_matrix<int> newly_filled;
		copy_unfilled_pixels(PSI_hat_p, PSI_hat_q, newly_filled);

		// Step 3: update the confidences of the newly-filled pixels
		update_confidence(PSI_hat_p, newly_filled);
		
		// Step 1a: Finally, update the fill front and the set of 
		//          unfilled pixels 
		update_fill_front(PSI_hat_p, newly_filled);

		iterations_done += 1;
	}

}

///////////////////////////////////////////////////////////////////////
//          Methods that implement individual steps                  //
///////////////////////////////////////////////////////////////////////


void inpainting::initialize_fill_front(
		const vcl_vector<int>& boundary_i, 
		const vcl_vector<int>& boundary_j)
{
	int n;

	// first we mark the pixels on the fill front
	for (n=0; n<boundary_i.size(); n++) {
		// mark that pixel as a pixel belonging to the fill front
		fill_front_(boundary_i[n], boundary_j[n]) = true;
	}

	// first we have to fill the priority queue with the
	// patches on the initial fill front, delta_Omega^0
	for (n=0; n<boundary_i.size(); n++) {

		// store the coordinates of the boundary pixel p in a 
		// 2D vector
		vnl_double_2 p(boundary_i[n], boundary_j[n]);

		// create a patch that is centered at pixel p
		psi PSI(p, w_, ni_, nj_);

		// store the patch in the priority list
		delta_Omega_->push(PSI);
	}

}

void inpainting::recompute_patch_priorities()
{
	vnl_double_2 patch_gradient, front_normal;
	double c, d;

	// to recompute the priorities, we use a second queue:
	// we remove all patches from delta_Omega_, update
	// their priorities and insert the removed patches 
	// into the second queue
	
	vcl_priority_queue<psi>* new_Q = new vcl_priority_queue<psi>;

	while (!delta_Omega_->empty()) {
		// retrieve the top patch from the queue
		psi PSI(delta_Omega_->top());

		// remove it from the priority queue
		delta_Omega_->pop();

		// compute the confidence term for the patch center
		c = compute_C(PSI, C_, unfilled_);

		// compute the data term for the patch
		d = compute_D(PSI, inpainted_grey_, unfilled_, fill_front_, alpha_, 
			          patch_gradient, front_normal);

		// update the patch priority
		PSI.set_P(c * d, c, d);

		// if the pixel at the patch center has not been filled already,
		// push the patch back onto the second priority queue
		if ( unfilled_((int)(PSI.p()(0)), (int)(PSI.p()(1))) )
			new_Q->push(PSI);
	}

	// swap the two queues
	delete delta_Omega_;
	delta_Omega_ = new_Q;
}

// return the patch in the source image that is most  
// similar to the supplied patch in the partially-inpainted
// image. note that some of the pixels inside the patch
// in the inpainted image may be unfilled (and therefore cannot
// be taken into account in the similarity comparison)
psi inpainting::find_best_match(const psi& patch)
{
	int source_i, source_j, plane;
	double d;

	// create a local copy of the patch data structure
	psi target_patch(patch);

    /////////////////////////////////////////////////////////
	//  Debugging code                                     //
	/////////////////////////////////////////////////////////
	vnl_double_2 gradient, front_normal;

	// for debugging and visualization purposes only, we
	// recompute the gradient and front normal of the best_patch
	// and store the information in a global variable
	d = compute_D(target_patch, inpainted_grey_, unfilled_, 
		          fill_front_, alpha_, gradient, front_normal);

	// draw the vectors corresponding to the image gradient
	// and the fill front normal at the center of best_patch
	// draw the gradient in red
	debug_draw_vec(target_patch, gradient, 1, 0, 0);

	//draw the front normal in green
	debug_draw_vec(target_patch, front_normal, 0, 1, 0);

	// uncomment these lines to print to stderr the intensities inside the
	// patch best_patch (very useful for debugging purposes)
	//debug_print_psi(best_patch, inpainted_grey_, "best patch:");

    /////////////////////////////////////////////////////////
	//  Main code of method begins here                    //
	/////////////////////////////////////////////////////////

	// first, we copy the pixel intensities of best_patch into 
	// three matrices (one each for the r, g, and b color channels)
	vnl_matrix<int> target_planes[3];
	vnl_matrix<int> target_valid, target_unfilled;

	// now do the pixel copy from the partially-inpainted image
	// into the three matrices
	for (plane=0; plane<3; plane++)
		target_patch.get_pixels(
			vil_plane(vil_view_as_planes(inpainted_), plane), 
			target_planes[plane], target_valid);

	// we also copy the corresponding patch of the unfilled_ image 
	// into a matrix; this matrix will tell us which intensities in
	// matrices target_planes[0], target_planes[1], target_planes[2]
	// correspond to already-inpainted pixels (or pixels from the
	// source photo)
	target_patch.get_pixels(unfilled_, target_unfilled, target_valid);

	// do a lookup in the "patch database"; this returns the image coordinates
	// (source_i, source_j) of the best-matching patch in the source photo
	pdb_->lookup(target_planes, nplanes_, target_unfilled, source_i, source_j);

	// store the coordinates in a patch data structure; this patch represents the 
	// pixels that must be copied to the unfilled pixels in best_patch 

	vnl_double_2 hat_q(source_i, source_j);
	psi best_source_patch(hat_q, w_, ni_, nj_);

	// uncomment these lines to print to stderr the red channel intensities 
	// inside target_patch and best_source_patch respectively
	// (useful for debugging purposes)
	debug_print_psi(target_patch, 
	                vil_plane(vil_view_as_planes(inpainted_), 0), 
	                "target patch");
	debug_print_psi(best_source_patch, 
	                vil_plane(vil_view_as_planes(inpainted_), 0), 
	                "best source patch:");

	return best_source_patch;
}

// copy the pixels in patch PSI_hat_q to the pixels in PSI_hat_p
// this copy should be done only for those pixels in PSI_hat_p that
// are not already filled
void inpainting::copy_unfilled_pixels(
		const psi& psi_hat_p, 
		const psi& psi_hat_q, 
		vnl_matrix<int>& newly_filled)
{
	int plane;

	// create local copies of the patch data structures
	psi PSI_hat_p(psi_hat_p);
	psi PSI_hat_q(psi_hat_q);

	// create a matrix that indicates which 
	// pixels are unfilled in patch PSI_hat_p
	vnl_matrix<int> unfilled_mat, temp;
	PSI_hat_p.get_pixels(unfilled_, unfilled_mat, temp);

	// copy the red channel intensities of 
	// PSI_hat_q into a matrix 
	vnl_matrix<int>* source_planes = new vnl_matrix<int>[nplanes_];
	PSI_hat_q.get_pixels(vil_plane(vil_view_as_planes(inpainted_), 0), 
		source_planes[0], newly_filled);

	// restrict the pixels (i,j) copied from hat_q to hat_p to only those 
	// that are (1) unfilled (ie. unfilled_mat(i,j)=1)
	// and (2) lie within the image border (ie. target_valid(i,j)=1)
	newly_filled = element_product(newly_filled, unfilled_mat);

	// copy the pixels into PSI_hat_p
	vil_image_view<vxl_byte> inp_plane = 
	  vil_plane(vil_view_as_planes(inpainted_), 0);
	PSI_hat_p.set_pixels(source_planes[0], newly_filled, 
		inp_plane);

	// repeat for the remaining color channels
	for (plane=1; plane<nplanes_; plane++) {
	        inp_plane = vil_plane(vil_view_as_planes(inpainted_), plane);
		PSI_hat_q.get_pixels(inp_plane, 
				     source_planes[plane], temp);
		PSI_hat_p.set_pixels(source_planes[plane], newly_filled, 
				     inp_plane);
	}

	// repeat the copy for the already-computed grayscale image (used for
	// gradient computations)
	vnl_matrix<int> source_grey;
	PSI_hat_q.get_pixels(inpainted_grey_, source_grey, temp);
	PSI_hat_p.set_pixels(source_grey, newly_filled, inpainted_grey_);
}

// update the confidence term C for all pixels inside the given
// patch that were inpainted in the current iteration of the 
// algorithm. newly_filled is a binary matrix that has 1 for all
// pixels that were inpainted in the current iteration. the confidence
// of these pixels should be made equal to the confidence of the 
// patch's center.
void inpainting::update_confidence(
		const psi& psi_hat_p, 
		const vnl_matrix<int>& newly_filled)
{
	psi PSI_hat_p(psi_hat_p);

	int center_i, center_j;
	double C_center;

	center_i = (int)(PSI_hat_p.p()(0));
	center_j = (int)(PSI_hat_p.p()(1));

	// store the confidence of the center pixel 
	C_center = C_(center_i, center_j);

	// create a new matrix that whose elements are all equal to the 
	// already-computed confidence of the patche's center pixel
	vnl_matrix<double> 
		new_C(newly_filled.rows(), 
		      newly_filled.columns(), 
			  PSI_hat_p.C());
	
	// set the confidence of all unfilled pixels in PSI_hat_p to be equal
	// to the confidence of the center pixel
	PSI_hat_p.set_pixels(new_C, newly_filled, C_);
}

// update the fill front at the end of the current iteration
// this method performs the following bookkeeping operations:
//    (1) sets fill_front_(i,j)=0 for all pixels that were
//        inpainted in the current iteration
//    (2) sets fill_front_(i,j)=1 for all unfilled pixels at the
//        border of patch psi_hat_p that were not already on the
//        fill front (ie. unfilled pixels that are 
//        adjacent to the current patch and for which 
//        fill_front_(i,j) is currently 0)
//    (3) creates a patch data structure for each of those
//        pixels and pushes it onto the priority queue
// psi_hat_p is the patch that was just inpainted
// newly_filled is the binary matrix indicating the pixels inside
// psi_hat_p that were just inpainted
void inpainting::update_fill_front(
		const psi& psi_hat_p, 
		const vnl_matrix<int>& newly_filled)
{
	// create a local copy of the patch data structure
	psi PSI_hat_p(psi_hat_p);

	// Mark as filled the newly_filled pixels in PSI_p_hat 
	vnl_matrix<int> zeromat(newly_filled.rows(), newly_filled.columns(), 0);
	PSI_hat_p.set_pixels(zeromat, newly_filled, unfilled_);

	// Remove from the fill front all pixels inside PSI_p_hat
	PSI_hat_p.set_pixels(zeromat, newly_filled, fill_front_);

	// If there is any change in the fill front, it will have to be
	// at the border of the current patch, ie. any pixels that 
	// are unfilled and surround PSI_hat_p will now belong to the 
	// fill front

	// To find these pixels, first we define a patch that 
	// has radius one larger
	// than the radius of PSI_hat_p, which is w_

	psi psi_outer(PSI_hat_p.p(), w_+1, ni_, nj_);

	// then we define a matrix that is 1 for every pixel on the border of
	// PSI_hat_p and is 0 everywhere else
	vnl_matrix<int> border(2*(w_+1)+1, 2*(w_+1)+1, 0);
	border.set_row(0, 1);
	border.set_row(border.rows()-1, 1);
	border.set_column(0, 1);
	border.set_column(border.columns()-1,1);

	// finally, we loop over those border pixels, adding them to the fill front
	// if they are not already there
	psi_outer.begin(border);
	do {
		int i, j;

		psi_outer.image_coord(i, j);

		// If the pixel is unfilled, it belongs to the fill front.
		// If it is not already on the fill front, we must therefore add it,
		// and create a new patch that goes onto the priority list
		if (unfilled_(i, j) && !fill_front_(i, j)) {
				vnl_double_2 new_p(i, j);
				fill_front_(i, j) = 1;
				delta_Omega_->push(psi(new_p, w_, ni_, nj_));
		}
	} while (psi_outer.next(border));

}



// search the image for the first unfilled pixel and 
// return its coordinates
bool inpainting::find_first_unfilled_pixel(int& unf_i, int& unf_j)
{
	int i, j;

	for (i=0; i<ni_; i++)
		for (j=0; j<nj_; j++)
			if (unfilled_(i,j) == true) {
				unf_i = i;
				unf_j = j;
				return true;
			} 

	return false;
}



void inpainting::initialize()
{
	int plane;

	//
	// initialize the sizes of the various auxiliary
	// images given the size of the input photo
	//

	C_.set_size(ni_, nj_);
	inpainted_.set_size(ni_, nj_);
	inpainted_grey_.set_size(ni_, nj_);
	unfilled_.set_size(ni_, nj_);
	fill_front_.set_size(ni_, nj_);

	// since this is the first time the algorithm is 
	// run on this photo, it will be applied onto a new image
	// region
	inpainted_new_region_ = true;

	//
	// Initialize the confidence value at each pixel
	//

	vil_convert_cast(init_filled_, C_);

	// 
	// Create an image that is 1 at every unfilled pixel
	// and is 0 otherwise. This should be equal to
	// (1-init_filled_)
	//

	// allocate some temporary storage
	vil_image_view<bool> tmp_im(ni_, nj_), true_im(ni_, nj_);
	// copy the initial mask to temporary image:
	vil_copy_deep(init_filled_, tmp_im);
	// create an image that has 'true' at every pixel
	true_im.fill(true);
	// and subtract the init_filled_ image
	vil_math_image_difference(true_im, tmp_im, unfilled_);

	// 
	// Initialize the inpainted image. It should retain
	// the pixels of the original photo when (init_filled=1)
	// and should be zero everywhere else. We can do this
	// computation by a pixelwise multiplication of 
	// init_filled (or C) and the input photo
	//

	// since this is an RGB image, we have to do the multiplication
	// for each of color component
	for (plane=0; plane<nplanes_; plane++) {
	        vil_image_view<vxl_byte> splane, iplane;
		splane = vil_plane(vil_view_as_planes(source_),plane);
		iplane = vil_plane(vil_view_as_planes(inpainted_),plane); 
		vil_math_image_product(splane, C_, iplane);
	}

	//
	// compute a gray-scale version of the inpainted image
	// to be used for gradient computations
	//

	vil_convert_rgb_to_grey(inpainted_, inpainted_grey_); 

	//
	// finally, initialize the image whose pixels are 1
	// if and only if the pixels are on the fill front
	// 

	// since no pixel is on the fill front initially,
	// this image is 0 everywhere
	fill_front_.fill(false);

	//
	// Initialize the remaining data structures
	//

	// initialize the data structures for searching the
	// pathces in a photo
	if (pdb_ != 0)
		delete pdb_;
	pdb_ = new patch_db(inpainted_, unfilled_, w_);

	// initialize the patch priority queue
	if (delta_Omega_ != 0)
		delete delta_Omega_;
	delta_Omega_ = new vcl_priority_queue<psi>;
}



/////////////////////////////////////////////////////////

