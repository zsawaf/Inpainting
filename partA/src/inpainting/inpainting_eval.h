// DO NOT MODIFY THIS FILE EXCEPT WHERE EXCPLICTLY NOTED!!!

#ifndef inpainting_evel_h
#define inpainting_eval

#include "inpainting.h"

//
// YOU WILL NEED TO IMPLEMENT THE FUNCTIONS DESCRIBED BELOW
// BY COMPLETING/EXTENDING THE CODE IN FILE 
// inpainting/inpainting_eval.cxx
//

// Function that computes and returns the data term D(p) in
// Eq.(1) of the paper
//
//   Parameters:
//          PSI:  (input) the patch for which the data term is computed
//           im:  (input) the source image (and/or the image being inpainted)
//     unfilled:  (input) a binary image where unfilled(i,j)=1 if and only if
//                pixel (i,j) is unfilled (ie. is yet to be inpainted)
//   fill_front:  (input) a binary image where fill_front(i,j)=1 if and only if
//                pixel (i,j) is on the fill front
//        alpha:  (input) the alpha parameter in the equation defining D(p)
//     gradient:  (output) the gradient used for computing the data term
// front_normal:  (output) the normal of the fill front at the center of patch PSI
//                (also used for computing the D(p) )
//
// Return value:  the value of the data term 
// 
double compute_D(psi& PSI, const vil_image_view<vxl_byte> im,
				 const vil_image_view<bool> unfilled, 
				 const vil_image_view<bool> fill_front, double alpha,
				 vnl_double_2& gradient, vnl_double_2& front_normal);

// Function that computes and returns the confidence term C(p) in
// Eq.(1) of the paper
// 
// 
//   Parameters:
//          PSI:  (input) the patch for which the confidence term is computed
//            C:  (input) an image representing the confidence function C(p) 
//                (defined on p.3 in the paper) 
//     unfilled:  (input) a binary image where unfilled(i,j)=1 if and only if
//                pixel (i,j) is unfilled (ie. is yet to be inpainted)
// 
// Return value:  the value of the condifence term
//
double compute_C(psi& PSI, const vil_image_view<double>& C, 
				 const vil_image_view<bool>& unfilled);

// Function that computes the normal of the fill front at the 
// center of the given patch. The function can assume that the patch
// center *is* on the front normal, ie. fill_front(i,j)=1 at the 
// patch center
//
// Parameters:
//          PSI:  (input) the patch for which the data term is computed
//   fill_front:  (input) a binary image where fill_front(i,j)=1 if and only if
//                pixel (i,j) is on the fill front
//       normal:  (output) the computed unit normal vector
// 
// Return value:  true if the normal can be computed, false if normal 
//                computation was not possible (in this case, the value of
//                the normal output parameter is undefined)
//
bool compute_normal(psi& PSI,
					vil_image_view<bool> fill_front, 
					vnl_double_2& normal);

// Function that computes the gradient in a given patch of a grayscale 
// image. Note that since the patch center is on the fill front, it is 
// an *unfilled* pixel, and therefore its intensity is not known. 
// This means that it is not possible to compute the gradient at the 
// patch center. Instead, this function should do the following:
//    (1) compute the gradient for *every* pixel whose gradient can
//        be computed inside the patch
//    (2) of the computed gradients inside the patch, return the gradient 
//        with the highest magnitude
// 
// Parameters: 
//                 PSI:  (input) the patch for which the data term is computed
// inpainted_grayscale:  (input) a grayscale version of the image being
//                       inpainted. This is the image whose gradient the 
//                       function computes
//            unfilled:  (input) a binary image where unfilled(i,j)=1 if and only if
//                       pixel (i,j) is unfilled (ie. is yet to be inpainted)
//            gradient:  (output) the gradient used for computing the data term
//
//  Return value:   true if the gradient can be computed, false 
//                  if no pixel in the patch has a computable gradient
bool compute_gradient(psi& PSI,
					  const vil_image_view<vxl_byte>& inpainted_grayscale, 
					  const vil_image_view<bool>& unfilled, 
					  vnl_double_2& grad);


#endif


