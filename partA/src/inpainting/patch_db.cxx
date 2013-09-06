// 
//  YOU NEED TO MODIFY THIS FILE FOR YOUR INPAINTING
//  IMPLEMENTATION
//
//  DO NOT MODIFY THIS FILE ANYWHERE EXCEPT WHERE 
//  EXPLICITLY NOTED!!!!
//


#include "psi.h"

patch_db::patch_db(
			const vil_image_view<vil_rgb<vxl_byte> >& im, 
			vil_image_view<bool> unfilled, int patch_radius
			)

	// we initialize the size of the patch_center_coords_
	// matrix to the larged possible number of completely
	// full patches in the image
	: patch_center_coords_(im.ni() * im.nj(), 2)
{
	rows_ = 0;
	w_ = patch_radius;
	plen_ = (2*w_ + 1) * (2*w_ + 1);
	nplanes_ = 3;

	// create a local copy of the source image
	vil_copy_deep(im, im_);

	// compute the matrix of centers of the patches 
	// that are completely full (ie. contain no unfilled
	// pixels)
	compute_patch_centers(im, unfilled);
}


void patch_db::compute_patch_centers(
			const vil_image_view<vil_rgb<vxl_byte> >& im,
			vil_image_view<bool> uf
			)
{
	int plane;
	int i, j;
	bool full;
	int pi, pj;
	int vi;

	// loop over all possible patch centers
	top_ = 0;
	for (i=w_; i<im.ni()-w_; i++)
		for (j=w_; j<im.nj()-w_; j++) {
			full = true;
			patch_center_coords_(top_, 0) = i;
			patch_center_coords_(top_, 1) = j;
			// loop over all pixels in a patch
			for (pi=-w_, vi=0; (pi<=w_) && full; pi++)
				for (pj=-w_; (pj<=w_) && full; pj++, vi++) 
					// if pixel is unfilled, the patch is
					// not completely full so this patch
					// cannot be used for lookup operations
					if (uf(i+pi, j+pj)) 
						full = false;
			if (full)
				top_ += 1;
		}
	
}

///////////////////////////////////////////////////////////
//     DO NOT CHANGE ANYTHING ABOVE THIS LINE            //
///////////////////////////////////////////////////////////


//
// YOU NEED TO IMPLEMENT THE ROUTINE BELOW
//

bool patch_db::lookup(
			const vnl_matrix<int>* target_planes, 
			int nplanes,
			const vnl_matrix<int>& target_unfilled, 
			int& source_i, 
			int& source_j
			)
{
	int i, match;

	// if the data structures were not correctly initialized, 
	// quit the lookup operation
	if (top_ == 0)
		return false;

	// if the size of the supplied matrices is NOT equal to the 
	// patch size used for contstructing the patch_db object,
	// quit the lookup operation
	for (i=0; i<nplanes; i++)
		if ((target_planes[i].rows() != 2*w_+1) ||
			(target_planes[i].columns() != 2*w_+1) )
			return false;
		
	///////////////////////////////////////////////////////////
	//     DO NOT CHANGE ANYTHING ABOVE THIS LINE            //
	///////////////////////////////////////////////////////////

	// basic idea of the algorithm is to go through some patches in top_ and compute the
	// sos of that patch. Then we need to store the patch with the minimum sos difference
	// from the target image and return those coords. 

	
	int min_sos =0,current_sos=0,pixel_count, size, patch_count;
	int red_dif, green_dif, blue_dif;
	vnl_vector<int> tar_red_vector;
	vnl_vector<int> tar_green_vector;
	vnl_vector<int> tar_blue_vector;
	vnl_vector<int> tar_valid_vector;
	vnl_vector<int> valid_vector;
	vnl_vector<int> red_vector;
	vnl_vector<int> green_vector;
	vnl_vector<int> blue_vector;
	vnl_matrix<int> red_matrix, green_matrix, blue_matrix, valid_matrix;
	bool first_entry=true;
	

	// Vectorize target vectors so that we can manipulate them easier. 

	vectorize(target_planes[0], tar_red_vector);
	vectorize(target_planes[1], tar_green_vector);
	vectorize(target_planes[2], tar_blue_vector);
	vectorize(target_unfilled, tar_valid_vector);
	
	size = tar_red_vector.size();

	while (patch_count < top_) {

		// make a patch from the center coords. 
		vnl_double_2 cur_center(patch_center_coords_(patch_count, 0), patch_center_coords_(patch_count, 1));
		psi cur_patch = psi(cur_center, w_, im_.ni(), im_.nj());

		// retrieve the pixels and validity matrices
		cur_patch.get_pixels(vil_plane(vil_view_as_planes(im_), 0), red_matrix, valid_matrix);
		cur_patch.get_pixels(vil_plane(vil_view_as_planes(im_), 1), green_matrix, valid_matrix);
		cur_patch.get_pixels(vil_plane(vil_view_as_planes(im_), 2), blue_matrix, valid_matrix);

		vectorize(red_matrix, red_vector);
		vectorize(green_matrix, green_vector);
		vectorize(blue_matrix, blue_vector);
		vectorize(valid_matrix, valid_vector);

		// restart pixel count and sos count.
		pixel_count=0;
		current_sos = 0;
		// now we actually calculate the sos. 
		while (pixel_count < size) {
			if (tar_valid_vector[pixel_count] == 0) {
				red_dif = (tar_red_vector[pixel_count] - red_vector[pixel_count]);
				green_dif = (tar_green_vector[pixel_count] - green_vector[pixel_count]);
				blue_dif = (tar_blue_vector[pixel_count] - blue_vector[pixel_count]);
				// get the sum of squares of the patch. 
				current_sos += (red_dif*red_dif) + (green_dif*green_dif) + (blue_dif*blue_dif);
				//printf("red_dif %d, green_dif %d, blue_dif %d, sos %d\n", red_dif, blue_dif, green_dif, current_sos);
			}
			pixel_count++;
		}
		// keep track of the minimum, first entry is a bool for the first time we enter the condition.
		if ((current_sos < min_sos) || first_entry) {
			min_sos = current_sos;
			match = patch_count;
			first_entry = false;
			//printf("min_sos is %d\n", min_sos);
		}

		patch_count+=50;
	}

	///////////////////////////////////////////////////////////
	//              PLACE YOUR CODE HERE                     //
	///////////////////////////////////////////////////////////

	// dummy implementation: always returns the center of the 
	// patch indexed by top_/2 as the result of the lookup operation


	///////////////////////////////////////////////////////////
	//     DO NOT CHANGE ANYTHING BELOW THIS LINE            //
	///////////////////////////////////////////////////////////


	// get row and column coordinates of patch center
	source_i = patch_center_coords_(match,0);
	source_j = patch_center_coords_(match,1);

	return true;
}

///////////////////////////////////////////////////////////
//     DO NOT CHANGE ANYTHING BELOW THIS LINE            //
///////////////////////////////////////////////////////////

void patch_db::vectorize(
			const vnl_matrix<int>& mat, 
			vnl_vector<int>& vec)
{
	int i, j, k; 

	vec.set_size(mat.rows() * mat.columns());
	for (i=0, k=0; i<mat.rows(); i++)
		for (j=0; j<mat.columns(); j++, k++)
			vec(k) = mat(i, j);

}

/////////////////////////////////////////////////////////

