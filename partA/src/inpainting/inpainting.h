// DO NOT MODIFY THIS FILE EXCEPT WHERE EXCPLICTLY NOTED!!!

#ifndef _inpainting_h
#define _inpainting_h

#include "../vxl_includes.h"
#include "psi.h"

#include "../gl/glutils.h"
#include "../imdraw/imdraw.h"

class inpainting {
public:
	// descriptors for all the images used in the algorithm
	enum im_type {Source, Init_Filled, Unfilled, Fill_Front, Inpainted, Confidence, Greyscale};

private:
	// the dimensions of the current image set
	int ni_;
	int nj_;
	int nplanes_;
	bool first_image_;

	//
	// the images used by the inpainting algorithm 
	//

	// the source image (stored locally)
	vil_image_view<vil_rgb<vxl_byte> > source_;
	// a greyscale version of the source image (used for gradient computations)
	vil_image_view<vxl_byte> inpainted_grey_;
	// a binary image indicating which pixels are occupied
	// when the algorithm starts
	vil_image_view<bool> init_filled_;
	// a binary image indicating which pixels are currently
	// "unfilled" (ie. empty)
	vil_image_view<bool> unfilled_;
	// a binary image indicating the pixels on the fill front
	vil_image_view<bool> fill_front_;
	// an image that holds the (partial or complete) result
	// of the inpainting algorithm
	vil_image_view<vil_rgb<vxl_byte> > inpainted_;
	// an image that stores the confidence value of every pixel
	vil_image_view<double> C_;
	
	//
	// other major data structures used by the algorithm
	// 

	// priority list that contains the patches on the fill front
	vcl_priority_queue<psi>* delta_Omega_;
	// the patch radius (ie. a patch has dimensions (2*w_+1) x (2*w_+1)
	int w_;
	// the algorithm's alpha parameter (controls the data term in 
	// patch priority computations)
	double alpha_;
	// data structure for finding the most similar patch in an image
	patch_db* pdb_;

	//
	//  Private methods
	//

	// routine that initializes the basic data structures of the
	// inpainting algorithm; should be run only once for each 
	// input photo
	void initialize();

	// routing that sets the algorithm's default parameters
	void default_params();

	// the main loop of the inpainting algorithm: it inpaints a 
	// single connected region of unfilled pixels in the image
	//   * bi and bj:  are the vectors representing the region's boundary:
	//        the coordinates (bi[k], bj[k]) are the image coordinates
	//        of the k-th pixel on the boundary
	//   * max_iterations:  the maximum number of iterations that
	//        the algorithm should be run, where one iteration corresponds
	//        to one iteration of the algorithm's main loop (Steps 1-3 in
	//        the paper)
	//   * iterations:  the number of iterations the algorithm was actually
	//        run (iterations < max_iterations) if the region was completely
	//        inpainted at fewer than max_iterations iterations

	void inpaint_region(
		const vcl_vector<int>& bi, 
		const vcl_vector<int>& bj,
		int max_iterations, 
		int& iterations);

	// search the image for an unfilled pixel and return the coordinates
	// of the first unfilled pixel that was found
	bool find_first_unfilled_pixel(int& ui, int& uj);

	// 
	// Routines implementing the specific steps of the algorithm
	// See file inpainting_algorithm.cxx for details
	// 

	// Step 1a (used only in the first iteration of the algorithm)
	void initialize_fill_front(
		const vcl_vector<int>& bi, 
		const vcl_vector<int>& bj);
	// Step 1b
	void recompute_patch_priorities();
	// Step 2b
	psi find_best_match(const psi& psi_hat_p);
	// Step 2c
	void copy_unfilled_pixels(
		const psi& PSI_hat_p, 
		const psi& PSI_hat_q, 
		vnl_matrix<int>& newly_filled);
	// Step 3
	void update_confidence(
		const psi& PSI_hat_p, 
		const vnl_matrix<int>& newly_filled);
	// Step 1a (used in all iterations of the algorithm except the first one)
	void update_fill_front(
		const psi& PSI_hat_p, 
		const vnl_matrix<int>& newly_filled);

	//
	// Routines & variables useful for visualizing the progress of the
	// algorithm and for algorithm debugging purposes
	//

	// is drawing enabled on the display panels?
	bool draw_enabled_;
	// debugging flags
	bool debug_show_PSI_;
	bool debug_show_vecs_;
	bool debug_print_;
	im_type debug_display_left_;
	im_type debug_display_right_;
	// information printing/displaying routines
	void debug_draw_patch(const psi& PSI, 
		 double col_r, double col_g, double col_b);
	void debug_draw_vec(const psi& PSI, const vnl_double_2& vec, 
		 double col_r, double col_g, double col_b);
	// routine for printing to stderr the contents of the image patch
	// defined by PSI in image im
	void debug_print_psi(
		const psi& PSI, 
		const vil_image_view<vxl_byte>& im,
		const vcl_string& title);
	// routine for printing to stderr the contents of the image
	// patch whose radius is one larger than that of PSI and whose
	// center is the same as PSI
	void debug_print_psi_plus1(
		const psi& PSI, 
		const vil_image_view<vxl_byte>& im,
		const vcl_string& title);

	// PLACE ANY ADDITIONAL PRIVATE METHOD DECLARATIONS HERE 
	// AND PLACE YOUR CODE FOR THESE METHODS IN FILE 
	// .cxx

	//////////////////////////////////////////////////
	// PLACE YOUR CODE BETWEEN THESE LINES          //
	//////////////////////////////////////////////////


	//////////////////////////////////////////////////

public:
	//
    // class constructors
	// 

	// default constructor
	inpainting(void);

	// constructor used when we want to show 
	// debugging information on the opengl canvas
	inpainting(ImDraw* lpanel, ImDraw* rpanel);

	// 
	// methods for executing the inpainting 
	// algorithm. they return false if the
	// algorithm cannot be run
	//

	// run the algorithm for up to the given number iterations
	// this will be the top-level method of your inpaitig 
	// implementation
	// if iterations=0, the algorithm will run until all unfilled
	// pixels in the image have been inpainted
	bool compute(int iterations);
	// run the algorithm until all unfilled regions in the image
	// have been inpainted
	bool compute();  // same as compute(0);

	//
	// controlling the method's parameters
	//
	void set_patch_radius(int w);
	void set_alpha(int alpha);

	//
	// controlling the display of debugging
	// information 
	//
	void debug_reset();
	void debug_show_PSI(bool show);
	void debug_show_vecs(bool show);
	void debug_print(bool show);
	// which image to show on the left panel when running in
	// debug mode
	void debug_display_left(im_type imt);
	// which image to show on the right panel when running in
	// debug mode
	void debug_display_right(im_type imt);
	// pass pointers to the opengl display panels
	void add_panels(ImDraw* left, ImDraw* right);

	// 
	// public functions for communicating to/from the UI
	// (the associated private variables and methods are below)
	// 

	// methods for setting the input to the inpainting algorithm
	// the methods return true if the image im can be added to the
	// input dataset (ie. it contains image data and has the same 
	// dimensions as the already-specified images)
	bool set(im_type imt, vil_image_view<vil_rgb<vxl_byte> > im);
	bool set(im_type imt, vil_image_view<bool> im);
	bool set(im_type imt, vil_image_view<vxl_byte> im);

	// accessor functions for the inputs & outputs of the algorithm
	// the functions return an empty image if the requested information 
	// is not yet available
	vil_image_view<vil_rgb<vxl_byte> > get(im_type imt);
	// this version is for getting 1-band images 
	vil_image_view<vxl_byte> get1(im_type imt);	
	// get descriptive title of each image
	const vcl_string& get_title(im_type imt);

	// method that runs the inpainting algorithm
	// it should return true if the inpainting algorithm
	// was run successfully and false otherwise
	//

private:
	// 
	// private variables used for passing information to/from the UI
	//

	// descriptive strings for each of these images
	vcl_vector<vcl_string> im_labels;

	// flag indicating that the results have already been
	// computed
	bool inpainted_computed_;
	// flag indicating that some pixels have been inpainted,
	// but not all of them
	bool inpainted_partial_;
	// flag indicating that we are beginning to inpaint a new
	// region inside the image
	bool inpainted_new_region_;
	// flag indicating that the results in the data structure
	// (if any) are out of date
	bool outdated_;

	// method that performs error checking of the user-supplied image input_im
	// and updates the variable im accordingly
	bool check_and_set_input(vil_image_view<vil_rgb<vxl_byte> > input_im,
							 vil_image_view<vil_rgb<vxl_byte> > &im);
	bool check_and_set_input(vil_image_view<bool> input_im,
							 vil_image_view<bool> &im);
	bool check_and_set_input(vil_image_view<vxl_byte> input_im,
							 vil_image_view<bool> &im);

	// we use these pointers to issue drawing commands to the
	// left and right opengl panels
	ImDraw* left_panel_;
	ImDraw* right_panel_;


	// PLACE ANY ADDITIONAL PUBLIC METHOD DECLARATIONS BELOW
	// AND PLACE YOUR CODE FOR THESE METHODS IN FILE 
	// .cxx
	
    //////////////////////////////////////////////////
    // PLACE YOUR CODE BETWEEN THESE LINES          //
    //////////////////////////////////////////////////

    //////////////////////////////////////////////////
};


#endif


