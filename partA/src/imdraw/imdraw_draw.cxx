// WARNING: DO NOT MODIFY THIS FILE EXCEPT WHERE EXPLICITLY NOTED

#include <math.h>
#include "../gl/glutils.h"
#include "imdraw.h"


/// DO NOT MODIFY THIS FILE FOR A1
/// THIS IS A PLACEHOLDER FOR FUTURE ASSIGNMENTS

////// PUT ANY ADDITIONAL DRAWING-RELATED CODE BETWEEN THESE LINES ///////
void ImDraw::draw_object(imdraw_object* obj)
{
	display_list.push(obj);
	redraw();
}

void ImDraw::clear_objects()
{
	display_list.clear();
	redraw();
}

void ImDraw::undo()
{
	display_list.undo();
	redraw();
}

void ImDraw::set_brush_radius(double rad)
{
	if (rad > 0)
		brush_radius = rad;
}

//////////////////////////////////////////////////////////////////////////


// This is the main GL drawing routine. 
void ImDraw::draw() {
     if (!valid()) {
	  // These commands just tell GL what the size of the canvas is
	  glLoadIdentity(); 
	  glViewport(0,0,width,height);
	  // This command ensures that both GL and VXL coordinates 
	  // draw the (0,0) point at the top-left of the window
	  glOrtho(0, width, height, 0, -1.0, 1.0);
     }
     
     // Use black to clear the GL canvas
     glClearColor(0.0, 0.0, 0.0, 0.0);
     // Command to clear the GL canvas 
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

     // the routine below displays a VXL image in a GL canvas. For
     // now, you can skip the details of how this is done (ie. you
     // don't need to read that code or understand it)
     draw_image();

	 // draw all objects in the display list
	 display_list.draw(this);

     ////// PUT YOUR DRAWING CODE BETWEEN THESE LINES ///////
     
     ////////////////////////////////////////////////////////
     
}


