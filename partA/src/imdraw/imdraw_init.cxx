// WARNING: DO NOT MODIFY THIS FILE EXCEPT WHERE EXPLICITLY NOTED


#include <math.h>
#include "../gl/glutils.h"
#include "../vxl_includes.h"

#include "imdraw.h"


////// PUT CODE USED BY CONTRUCTOR (IF ANY) BETWEEN THESE LINES ///////
///////////////////////////////////////////////////////////////////////

// Constructor initalization routine
ImDraw::ImDraw(int x,int y,int w,int h,const char *l)
     : Fl_Gl_Window(x,y,w,h,l)
{
     // w and h are the width and height of the OpenGL canvas 
     width = w;
     height = h;
     im_width = width;
     im_height = height;
     aspect_ratio = 1;
     magnification = 1;
	 fit_magnification = 1;
	 shift_i = shift_j = 0;
	 view_i = view_j = 0;
     // Initially, the allocated texture memory contains no data
     imTexture = NULL;
	 valid_image = false;
	 canonical_view_ = false;

	 // Initially, the interface is put in browsing mode
	 set_browsing_mode();

	 // The view center is initialized to be the center of the opengl canvas
	 gl2vxl(width/2, height/2, image_center_i, image_center_j);
	 shift_j = shift_i = 0;

     ////// PUT YOUR INITIALIZATION CODE (IF ANY) BETWEEN THESE LINES ///////

     // user has not drawn anything yet
	 mode(FL_RGB | FL_ALPHA);
     expand = false;
	 pos1_i = pos1_j = pos2_i = pos2_j = 0;
	 brush_radius = 10;
	 shift_pressed = false;

     ////////////////////////////////////////////////////////////////////////
};


