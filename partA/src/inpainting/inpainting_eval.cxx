// 
//  YOU NEED TO MODIFY THIS FILE FOR YOUR INPAINTING
//  IMPLEMENTATION
//
//  DO NOT MODIFY THIS FILE ANYWHERE EXCEPT WHERE 
//  EXPLICITLY NOTED!!!!
//

// See the file inpainting_eval.h for a detailed explanation of
// the input and output parameters of the routines you must 
// implement

#include "inpainting_eval.h"

// When beginning to write your code, I suggest you work in 
// three steps:
// 
//   - First, implement the routine for computing the
//     confidence term. To help with debugging, you should
//     let compute_D() always return 1, so that patches
//     are selected purely based on their confidence term.
//     (correctness of the confidence computation routine
//     should be relatively easy to check since if it 
//     incorrect patches with fewer filled pixels will end up
//     having higher priorities and will be chosen first)
//
//  - Second, implement the routine that does the patch lookup.
//    The correctness of the lookup routine should also be fairly
//    easy to debug, since if it is incorrect the routine will
//    be choosing patches that look nothing like the patch on
//    the fill front.
//    Together, these two steps will allow you to get somewhat
//    reasonable inpaintings done.
//
//  - Third, implement the data term computation routine. You
//    should also try to do this in steps to help with 
//    debugging: (1) make your compute_C() function return 1
//    always, so that patch priorities are computed entirely
//    according to the data term. (2) make your compute_D() 
//    function return just the magnitude of the 
//    gradient computed by the compute_gradient() routine
//    ---this will help you debug gradient computatoins, since an 
//    incorrect computatoin will cause patches that contain 
//    very low intensity variation to be selected before 
//    patches with lots of intensity variations. (3) once
//    you are satisfied that gradient computatoins are 
//    correct, move on to the normal computation routine, etc.
// 
//  - Only when you are satisfied that the above routines
//    are correct individually should you try to compute the
//    priorities as the product C()*D(). Otherwise, if the
//    patch selections 'don't look right' you won't know
//    what is causing this behavior
//
//  

double compute_D(psi& PSI, 
				 const vil_image_view<vxl_byte> im,
				 const vil_image_view<bool> unfilled, 
				 const vil_image_view<bool> fill_front, 
				 double alpha,
				 vnl_double_2& gradient, 
				 vnl_double_2& front_normal)
{

	// holds the perpendicular to the gradient
	vnl_double_2 grad_normal;    
	// compute the gradient at a filled pixel in the 
	// direction of the front normal
	if (compute_gradient(PSI, im, unfilled, gradient)) {

		grad_normal(0) = -gradient(1);
		grad_normal(1) = gradient(0);

		// now compute the normal of the fill front
		if (compute_normal(PSI, fill_front, front_normal)) {
			double dotp;

			dotp = fabs(dot_product(grad_normal, front_normal))/alpha;

			return dotp;
		}
		if (alpha > 0) 
			return 1/alpha;
		else {
			return 0;
		}
	} else {
		// if we cannot compute a normal, the fill boundary consists
		// of exactly one pixel; the data term in this case is meaningless
		// so we just return a default value
		return 0;
	}
}

///////////////////////////////////////////////////////////
//     DO NOT CHANGE ANYTHING ABOVE THIS LINE            //
///////////////////////////////////////////////////////////


//
// YOU NEED TO IMPLEMENT THE ROUTINES BELOW
//



double compute_C(psi& PSI, const vil_image_view<double>& C, 
				 const vil_image_view<bool>& unfilled)
{
	///////////////////////////////////////////////////////////
	//     DO NOT CHANGE ANYTHING ABOVE THIS LINE            //
	///////////////////////////////////////////////////////////
	int x, y, patch_size, patch_area;
	double c_value, final_C, C_sum;
	vnl_double_2 center = PSI.p();
	patch_size = PSI.sz();
	patch_area = patch_size*patch_size;
	C_sum = 0;
	// Idea is to find all the C values in the patch and then 
	// divide them by the area of the patch. 
	PSI.begin();
	do {
		PSI.image_coord(x, y);
		c_value = C(x, y);
		if  (unfilled(x, y) != 1) {
			C_sum += c_value;
		}
	} while (PSI.next());

	final_C = C_sum / patch_area;

	///////////////////////////////////////////////////////////
	//              PLACE YOUR CODE HERE                     //
	///////////////////////////////////////////////////////////
	return final_C;

	///////////////////////////////////////////////////////////
	//     DO NOT CHANGE ANYTHING BELOW THIS LINE            //
	///////////////////////////////////////////////////////////
}


bool compute_normal(psi& PSI,
					vil_image_view<bool> fill_front, 
					vnl_double_2& normal)
{
	///////////////////////////////////////////////////////////
	//     DO NOT CHANGE ANYTHING ABOVE THIS LINE            //
	///////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////
	//              PLACE YOUR CODE HERE                     //
	///////////////////////////////////////////////////////////

	// first we need to identify I, I would be the 1's on the fill front. 
	// so what we need to do is have to matrices I_x and I_y, and from there
	// iterate through the patch and fill out the matrices 
	bool full = true;
	vnl_matrix<double> I_x((2*PSI.w() + 1),1);
	vnl_matrix<double> I_y((2*PSI.w() + 1),1);
	vnl_matrix<double> W(PSI.sz(), PSI.sz());
	vnl_matrix<double> X(PSI.sz(), 3);
	vnl_matrix<double> D_x(3, 1);
	vnl_matrix<double> D_y(3, 1);
	int x,y,fill_count=0,w;
	double new_w;
	//find the center coordinates of the patch. 
	vnl_double_2 center_patch = PSI.p();

	// Now let's fill both pixels with -1's
	I_x.fill(-1);
	I_y.fill(-1);

	// we know that the center pixels lie on the fill front. Then
	// we have to put them in the I_x and I_y on the wth element. 

	I_x.put(PSI.w(), 0, center_patch[0]);
	I_y.put(PSI.w(), 0, center_patch[1]);
	fill_count=0;
	//now we can start iterating through the patch.
	PSI.begin();
	do {
		PSI.image_coord(x, y);
		if(fill_front(x,y) == 1) {
			if(fill_count < 9){
				I_x.put(fill_count, 0, x);
				I_y.put(fill_count, 0, y);
				fill_count++;
			}
			else {
				break;
			}
		}
	} while (PSI.next());

	// we do not have enough information.
	if(fill_count<9){
		return false;
	}

	//now that we have our I_x and I_y we need to fill out our W matrix. 
	W.fill(0);

	//what the W matrix is basically -w to w on the diagonals. 
	x=0;
	y=0;
	w=PSI.w()*-1;
	while(x<PSI.sz()){
		W.put(x,y,w);
		w++;
		x++;
		y++;
	}

	//now that we have our w, we need to populate our X. X is filled with 1's
	//in the first column, -w to w in the second column and w^2/2 in the last column. 

	//reset counters
	x=0;
	new_w=PSI.w()*-1;
	while(x < X.rows()) {
		//first column is always 1.
		X.put(x,0,1);
		//second column is an alternating w.
		X.put(x,1,new_w);
		//third column is w^2/2
		X.put(x,2,(new_w*new_w)/2.0);
		x++;
		new_w++;
	}

	// now we solve the awesome formula. 
	D_x = vnl_matrix_inverse<double>(W * X).solve(W * I_x);
	D_y = vnl_matrix_inverse<double>(W * X).solve(W * I_y);
	int res_x = -1 * (int)D_y[1];
	int res_y = (int)D_x[1];

    // NOT SO DUMMY ANYMORE! Lol I'm still drunk from last night.
    normal(0) = res_x;
    normal(1) = res_y;
    return true;

	///////////////////////////////////////////////////////////
	//     DO NOT CHANGE ANYTHING BELOW THIS LINE            //
	///////////////////////////////////////////////////////////
}

// return the gradient with the strongest magnitude inside the
// patch of radius w or return false if no gradients can be computed
bool compute_gradient(psi& PSI,
					  const vil_image_view<vxl_byte>& inpainted_grayscale, 
					  const vil_image_view<bool>& unfilled, 
					  vnl_double_2& grad)
{
	///////////////////////////////////////////////////////////
	//     DO NOT CHANGE ANYTHING ABOVE THIS LINE            //
	///////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////
	//              PLACE YOUR CODE HERE                     //
	///////////////////////////////////////////////////////////
	int x,y, ceil_x, ceil_y, count_x, count_y;
	vnl_matrix<double> sobel_x, sobel_y, kern, res_x, res_y;
	double _grad =-1, max_grad=-1, max_x = -1, max_y = -1, grad_x=0, grad_y=0;
	bool compute;

	// This algorithm uses sobel masks to solve the gradient problem.
	sobel_x = vnl_matrix<double>(3,3,0);
	sobel_y = vnl_matrix<double>(3,3,0);
	kern = vnl_matrix<double>(3,3,0);
	// At each point in the image, the result of the Sobel operator is either the 
	// corresponding gradient vector or the norm of this vector. The Sobel operator is based on 
	// convolving the image with a small, separable, and integer valued filter in horizontal and 
	// vertical direction and is therefore relatively inexpensive in terms of computations. 
	// On the other hand, the gradient approximation that it produces is relatively crude, 
	// in particular for high frequency variations in the image.
	// According to Wikipedia, this algo is freaking awesome. 
	sobel_x.put(0, 0, -1);
	sobel_x.put(1, 0, -2);
	sobel_x.put(2, 0, -1);
	sobel_x.put(2, 0, 1);
	sobel_x.put(2, 1, 2);
	sobel_x.put(2, 2, 1);

	sobel_y.put(0, 0, -1);
	sobel_y.put(0, 1, -2);
	sobel_y.put(0, 2, -1);
	sobel_y.put(2, 0, 1);
	sobel_y.put(2, 1, 2);
	sobel_y.put(2, 2, 1);

	PSI.begin();

	do {
		PSI.image_coord(x, y);
		if (x < inpainted_grayscale.ni() - PSI.sz() && 
			x > PSI.sz() && y < inpainted_grayscale.nj() - PSI.sz() &&
			 y > PSI.sz()) {

			compute = true;

			// iterate over all the pixels in the kernel
			kern.put(0, 0, inpainted_grayscale(x-1, y-1));
			kern.put(0, 1, inpainted_grayscale(x-1, y));
			kern.put(0, 2, inpainted_grayscale(x-1, y+1));
			kern.put(1, 0, inpainted_grayscale(x, y-1));
			kern.put(1, 1, inpainted_grayscale(x, y));
			kern.put(1, 2, inpainted_grayscale(x, y+1));
			kern.put(2, 1, inpainted_grayscale(x+1, y));
			kern.put(2, 2, inpainted_grayscale(x+1, y+1));
			kern.put(2, 0, inpainted_grayscale(x+1, y-1));

			if (unfilled(x-1,y-1) == 1) {
				compute = false;
			}
			if (unfilled(x-1,y) == 1) {
				compute = false;
			}
			if (unfilled(x-1,y+1) == 1) {
				compute = false;
			}
			if (unfilled(x,y-1) == 1) {
				compute = false;
			}
			if (unfilled(x,y) == 1) {
				compute = false;
			}
			if (unfilled(x,y+1) == 1) {
				compute = false;
			}
			if (unfilled(x+1,y) == 1) {
				compute = false;
			}
			if (unfilled(x+1,y+1) == 1) {
				compute = false;
			}
			if (unfilled(x+1,y-1) == 1) {
				compute = false;
			}

			// proceed only if all the pixels have been filled. 
			if (compute) {
				// find the result for x pixels and y pixels of the 
				res_x = sobel_x * kern;
				res_y = sobel_y * kern;

				grad_x = res_x.get(0,0) / 8;
				grad_y = res_y.get(0,0) / 8;

				_grad = sqrt(pow(grad_x, 2) + pow(grad_y ,2));

				// keep track of max gradient
				if (_grad > max_grad) {
					max_x = grad_x;
					max_y = grad_y;
					max_grad = _grad;
				}
			}

		}
	} while (PSI.next());

    // dummy routine
    grad(0) = max_x;
    grad(1) = max_y;
    if (max_grad < 0) {
		return false;
	} 
	return true;

	///////////////////////////////////////////////////////////
	//     DO NOT CHANGE ANYTHING BELOW THIS LINE            //
	///////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////
//     PLACE ANY ADDITIONAL CODE (IF ANY) HERE           //
///////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////

