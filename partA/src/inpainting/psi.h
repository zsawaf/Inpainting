// DO NOT MODIFY THIS FILE EXCEPT WHERE EXCPLICTLY NOTED!!!

#ifndef _psi_h
#define _psi_h

#include "../vxl_includes.h"

//
//	This file contains the specs for two classes used in
//	the inpainting assignment:
//	   - psi:       this is a class that represents a patch,
//	                along with a variety of routines that manipulate
//			        the pixels in a patch
//     - patch_db:  this is a very simple class that implements
//	                a patch lookup operation, i.e., given an image
//					(with some of its pixels possibly unfilled) and
//					a patch, find a patch that (1) is most similar
//					to the given patch and (2) has all its pixels 
//					filled
//

// 
//  Patches represented by the psi class as not associated with 
//  an image explicitly. They only contain information about the
//  location and size of the patch, its confidence and data terms,
//  and its priority (as defined by the inpainting algorithm).
//  In additionl, the psi class provides a collection of routines 
//  that be used to extract, copy, and loop through pixels
//  at the patche's location in a user-supplied image.
// 
//  Even though a psi patch is not associated with a single image,
//  it only applies to images of a given dimension


class psi {
private:
	int w_;          // radius of the patch
	double P_;       // priority of the patch
	double C_;       // confidence term
	double D_;       // data term
	vnl_double_2 p_; // coordinates (i,j) of center pixel
	int ni_, nj_;    // image dimensions 
	int di_, dj_;    // internal variables for looping over
	                 //   pixels in a patch
	int sz_;         // patch size (= 2*w_ + 1)
private:
	// private routines for implementing loops over pixels
	// in a patch
	int begin_i() const; 
	int begin_j() const;
	bool end_i() const;
	bool end_j() const;
protected:
	// accessor functions for the image dimensions
	int ni() const;
	int nj() const;
public:
	//
	// constructors
	// 
	// create a patch that 
	//   - is centered at vector center ie. its VXL coordinates 
	//     are (i,j)=(center(0), center(1)
    //   - has radius rad
	//   - can be used to view/extract/manipulate pixels in 
	//     a VXL image of dimensions ni x nj
	psi(const vnl_double_2& center, int rad, int ni, int nj);
	// copy constructor
	psi(const psi& PSI);
	// create a new patch that is cented at the same pixel 
	// but whose radius is increased by i pixels (ie.
	// PSI.expand(1) is a patch whose center is PSI.p() and
	// whose radius is PSI.w()+1
	psi expand(int i); 

	//
	// comparison function: psi1 < psi2 if and only if
	// psi1 has lower priority than psi2
	// 
	// this function is used for implementing the 
	// fill front as a standard C++ priority list whose
	// elements are objects of class psi
	bool operator<(const psi & rhs) const;

	// accessor functions
	double P() const; // returns the priority of the patch
	double C() const; // returns the patche's confidence term
	double D() const; // returns the patche's data term
	vnl_double_2 p() const; // returns the patche's center as a 
	                        // 2D vector of class vnl_double_2
	int w() const;    // returns the patche's radius
	int sz() const;   // returns the patche's size
	// sets the patche's priority, confidence and data terms
	// (i.e., the C(p), D(p) and P(p) terms in the paper)
	void set_P(double priority, double confidence, double dataterm);

	// 
	// pixel copy methods
	// 
	// this is a set of very useful methods that allow you to copy
	// pixels from an image patch into a VXL matrix data structure
	// and vice versa
	// 

	// Copy pixels from the patche's location in a supplied image 
	//   the routines implement the same copy operation for different
	//   image datatypes
	// 
	// Parameters: 
	//      image: (input) the image from which the pixels will be taken
	//  pixel_mat: (output) a 2w_+1 x 2w_+1 matrix that holds the copied
	//                      pixels
	//  valid_mat: (output) a binary matrix of the same size as pixel_mat
	//                      that indicates which pixels in pixel_mat are
	//                      valid (eg. if the patch extends beyond the image
	//                      border, valid_mat(i,j)=0 for all those pixels)
	void get_pixels(const vil_image_view<vxl_byte>& image, 
					vnl_matrix<int>& pixel_mat,
					vnl_matrix<int>& valid_mat);
	void get_pixels(const vil_image_view<bool>& image, 
					vnl_matrix<int>& pixel_mat,
					vnl_matrix<int>& valid_mat);
	void get_pixels(const vil_image_view<double>& image, 
					vnl_matrix<double>& pixel_mat,
					vnl_matrix<int>& valid_mat);

	// Copy pixels from a supplied matrix into an image at the 
	// patche's location
	//   the routines implement the same copy operation for different
	//   image datatypes
	// 
	// Parameters: 
	//  pixel_mat: (input)  a 2w_+1 x 2w_+1 matrix that holds the pixels
	//                      to be copied into the image
	//  valid_mat: (input)  a binary matrix of the same size as pixel_mat
	//                      that indicates which pixels in pixel_mat should
	//                      be copied (if valid_mat(i,j)=0, the corresponding
	//                      pixel in the image is left *untouched*)
	//     image:  (input/output) the image in which the pixels will be copied;
	//                      the routine assumes that the image has already been
	//                      allocated (ie. it is of dimension ni_ x nj_)
	void set_pixels(const vnl_matrix<int>& pixel_mat,
					const vnl_matrix<int>& valid_mat,
					vil_image_view<vxl_byte>& image);
	void set_pixels(const vnl_matrix<int>& pixel_mat,
					const vnl_matrix<int>& valid_mat,
					vil_image_view<bool>& image);
	void set_pixels(const vnl_matrix<double>& pixel_mat,
					const vnl_matrix<int>& valid_mat,
					vil_image_view<double>& image);

	//
	// methods for creating loops over pixels (see the implementation of
	// the get_pixels() method in file psi.cxx for examples on how to 
	// create loops using these methods)
	//
	// the method implicitly implements bounds checking. for example,
	// if a patch is centered at a pixel at the edge of an image, the
	// loop will cycle only over those patch pixels that lie *inside*
	// the image
	
	void begin();     // reset the pixel counters
	bool end() const; // returns true if we have iterated over all pixels
	                  // in the patch
	bool next();      // get the next pixel in the patch
	// VXL coordinates of the current pixel in the loop. These coordinates
	// will be in the range (0..ni-1, 0..nj-1)
	void image_coord(int& i, int& j) const; 
	// patch coordinates of the current pixel. These coordinates will be
	// in the range (0..2w_, 0..2_w)
	void psi_coord(int& i, int& j) const; 
	// coordinates of the current pixel when psi is expressed as a 1D vector
	// (not used for assignment 2)
	void vector_coord(int& i) const;

	// 
	// The begin() and next() methods above loops over all patch pixels 
	// that are within the range (0..ni-1,0..,nj-1)
    // 
	// The two methods below generalize these loops so that we only loop
	// through a *selected* set of pixels inside the patch, controlled by
	// the binary 2w_+1 x 2w_+1 matrix mask.
	// 
	// These methods allow us to loop over all pixels that satisfy 
	// two conditions simultaneously:
	//   (1) they are within the range (0..ni-1,0..,nj-1)
	//   (2) mask(i,j)=1, where i,j range between 0 and 2w_+1 and
	//       correspond to the pixel's position inside the patch
	//       (i.e, the values returned by psi_coord())
	void begin(const vnl_matrix<int>& mask);
	bool next(const vnl_matrix<int>& mask);


	int vector_length() const;
	// the VXL coordinates of the top-left and bottom-right corner of
	// the patch (used for drawing it on the opengl panel)
	void image_vertices(int& j1, int& i1, int& j2, int& i2) const;


};


// 
// A simple class for searching a color image for a patch similar to a given patch
// The class assumes that a portion of the image may be "unfilled"
// 
// These classes are implemented in file patch_db.cxx

class patch_db {
public:
	//
	// Constructor: Initialize the data structures used for lookup
	//   Parameters:
	//        source_image:   the image that will be searched for 
	//                        similar patches to a given patch
	//            unfilled:   binary image indicating which pixels in the
	//                        source images are unfilled
	//        patch_radius:   the radius w_ of the patch used in the 
	//                        inpainting algorithm
	// NOTE: The constructor creates a local copy of the source image for
	//       implementing the lookup operations.
	//
	//       This means that subsequent changes to the image (eg. inpainting)
	//       will NOT affect the behavior of this routine
	// 
	//       It also means that a new patch_db object must be created for
	//       each new source image (regardless of its dimensions)
    //
	// NOTE2: The routine is specifically tailored to patches of a given radius.
	//        The result of lookup operations with patches of radius other than
	//        w_ is undefined and may cause a crash.
	// 
	patch_db(const vil_image_view<vil_rgb<vxl_byte> >& source_image, 
		     vil_image_view<bool> unfilled, int patch_radius);


	// 
	// the main lookup routine:
	//      
	//    
	// Parameters:
	//     target_planes:   (input) A vector of 3 2w_+1 x 2w_+1 matrices, 
	//                      holding the red, green and blue
	//                      intensities inside a patch of radius w_. 
	//                      Variable target_planes should be declared as 
	//                                vnl_matrix<int>* target_planes[3];
	//                      In this definition, target_planes[0], target_planes[1] 
	//                      and target_planes[2] are the three matrices, each of 
	//                      which must have 2w_+1 rows and 2w_+1 columns
	//     nplanes:         (input) The number of color components. 
	//                      You should always pass the value 3 
	//     target_valid:    (input) A 2w_+1 x 2w_+1 binary matrix that indicates
	//                      which pixels inside the target_planes[0] matrices
	//                      are valid (ie. NOT unfilled)
	//     source_i
	//     source_j         (output) The routine should return the 
	//                      coordinates (source_i, source_j) of the patch in the
	//                      source image that is most similar to the RGB intensities 
	//                      supplied in the target_planes[] matrices.
	//                      Similarity should be assessed by measuring the total sum-of-squared
	//                      difference between the R,G,B components of the valid pixels
	//                      in the supplied target_planes matrices and their corresponding 
	//                      pixels in the source image
	//
	bool lookup(const vnl_matrix<int>* target_planes, 
		        int nplanes, 
				const vnl_matrix<int>& target_valid, 
				int& source_i, int& source_j);
private:
	// The total number of patches in the source image that are 
	// "completely full" ie. they have  all of their pixels valid 
	// (ie. NOT unfilled)
	int top_;
	// This is a matrix of top_ rows and 2 columns, where the
	// t-th row contains the (i,j) coordinates of the center 
	// of the t-th completely full patch in the source image. This
	// matrix is filled by the compute_patch_centers() method called
	// by the constructor routine.
	vnl_matrix<int> patch_center_coords_;
	int rows_;
	// the size of patches to be used in lookup operations
	int w_;
	// the number of color components (should be 3)
	int nplanes_;
	int plen_;
	// a local copy of the source image (this is the image 
	// that should be searched for similar patches duringthe lookup operation)
	vil_image_view<vil_rgb<vxl_byte> > im_;
	// convert a 2D patch, represented by a (2w_+1) x (2w_+1) matrix  
	// into a 1D vector of length (2w_+1)x(2w_+1)
	void vectorize(const vnl_matrix<int>& mat, vnl_vector<int>& vec);
	// method for finding the centers of all completely full patches in a source
	// image and placing them in a matrix
	void compute_patch_centers(const vil_image_view<vil_rgb<vxl_byte> >& image, vil_image_view<bool> unfilled);
};


#endif

