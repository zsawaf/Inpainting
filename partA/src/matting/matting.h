// DO NOT MODIFY THIS FILE EXCEPT WHERE EXCPLICTLY NOTED!!!

#ifndef _matting_h
#define _matting_h

#include "../vxl_includes.h"


class matting {
public:
	// descriptors for all the images used in the algorithm
	enum im_type {Back1, Back2, Comp1, Comp2, Alpha, Object, NewBack, NewComp};
private:
	// private variables holding the images used for
	// the triangulation matting algorithm (ie. the
	// input and output dataset)	
	vil_image_view<vil_rgb<vxl_byte> > back_1_;
	vil_image_view<vil_rgb<vxl_byte> > back_2_;
	vil_image_view<vil_rgb<vxl_byte> > comp_1_;
	vil_image_view<vil_rgb<vxl_byte> > comp_2_;
	vil_image_view<vil_rgb<vxl_byte> > object_;
	vil_image_view<vxl_byte> alpha_;

	// descriptive strings for each of these images
	vcl_vector<vcl_string> im_labels;

	// variable holding the last image used as background in
	// a compositing operation (ie. the new_back parameter
	// of the compute_composite() method)
	vil_image_view<vil_rgb<vxl_byte> > new_back_;
	// variable holding the most recently-computed composite
	// (ie. the new_comp parameter of the compute composite method)
	vil_image_view<vil_rgb<vxl_byte> > new_comp_;


	// flag indicating that the results have already been
	// computed
	bool alpha_computed_;
	bool new_composite_computed_;

	// flag indicating that the results in the data structure
	// (if any) are out of date
	bool outdated_;

	// the dimensions of the current image set
	int ni_;
	int nj_;
	bool first_image_;

	// method that performs error checking of the user-supplied image input_im
	// and updates the variable im accordingly
	bool check_and_set_input(vil_image_view<vil_rgb<vxl_byte> > input_im,
							 vil_image_view<vil_rgb<vxl_byte> > &im);

	// method that performs error checking of the internal image im
	// and updates the user-requested variable out_im accordingly
	bool check_and_get_input(vil_image_view<vil_rgb<vxl_byte> > im,
							 vil_image_view<vil_rgb<vxl_byte> > &out_im);

	// PLACE ANY ADDITIONAL PRIVATE METHOD DECLARATIONS HERE 
	// AND PLACE YOUR CODE FOR THESE METHODS IN FILE 
	// matting_algorithm.cxx

	//////////////////////////////////////////////////
	// PLACE YOUR CODE BETWEEN THESE LINES          //
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////

public:
	// class constructor
	matting(void);
	// methods for setting the input to the matting algorithm
	// the methods return true if the image im can be added to the
	// input dataset (ie. it contains image data and has the same 
	// dimensions as the already-specified images)
	bool set(im_type imt, vil_image_view<vil_rgb<vxl_byte> > im);
	// accessor functions for the inputs & outputs of the algorithm
	// the functions return an empty image if the requested information 
	// is not yet available
	vil_image_view<vil_rgb<vxl_byte> > get(im_type imt);
	// this version is for 1-band images (ie. alpha matte)
	vil_image_view<vxl_byte> get1(im_type imt);	
	// get descriptive title of each image
	const vcl_string& get_title(im_type imt);

	// method that runs the matting algorithm
	// it should return true if the matting algorithm
	// was run successfully and false otherwise
	//
	// this will be the top-level method of your matting implementation
	bool compute(void);

	// method that creates a new composite image from a source
	// image and the computed foreground and alpha matte
	bool compute_composite(
		vil_image_view<vil_rgb<vxl_byte> > new_background,
		vil_image_view<vil_rgb<vxl_byte> > &new_composite);


	// PLACE ANY ADDITIONAL PUBLIC METHOD DECLARATIONS BELOW
	// AND PLACE YOUR CODE FOR THESE METHODS IN FILE 
	// matting_algorithm.cxx
	
    //////////////////////////////////////////////////
    // PLACE YOUR CODE BETWEEN THESE LINES          //
    //////////////////////////////////////////////////

    //////////////////////////////////////////////////
};

#endif

