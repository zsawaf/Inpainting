#include "../gl/glutils.h"
#include "imdraw.h"

imdraw_object::object_type imdraw_object::id() 
{
	// return the type of object
	return id_;
}

void imdraw_object::clear()
{
	clear_ = true;
	fill_ = false;
	col_[3] = 0;
}

//
//  Classes for creating & drawing a vector object
//


void imdraw_circle::init(int j, int i, double rad)
{
	// do the default initialization first
	// by default, make the object displayable
	clear();

	// define the object id
	id_ = imdraw_object::Circ_ID;

	i_ = i;
	j_ = j;
	rad_ = rad;
	col_[0] = col_[1] = col_[2] = 1;
}

imdraw_circle::imdraw_circle(int j, int i, double rad)
{
	init(j, i, rad);
}

imdraw_circle::imdraw_circle(int j, int i, double rad,
							 double red, double green, double blue)
{
	init(j, i, rad);
	col_[0] = red;
	col_[1] = green;
	col_[2] = blue;
}


imdraw_circle::imdraw_circle(int j, int i, double rad,
							 double red, double green, double blue,
							 bool clear)
{
	init(j, i, rad);
	col_[0] = red;
	col_[1] = green;
	col_[2] = blue;
	clear_ = clear;
}

imdraw_circle::imdraw_circle(int j, int i, double rad,
							 bool fill,
							 double red, double green, double blue,
							 bool clear)
{
	init(j, i, rad);
	col_[0] = red;
	col_[1] = green;
	col_[2] = blue;
	fill_ = fill;
	clear_ = clear;
}

bool imdraw_circle::draw(ImDraw* panel)
{
	double glx1, gly1, glr1, glrz1, glr2, glrz2;

	// Draw the border of the rectangular region in red 
	glColor4dv(col_);

	// convert VXL coordinates to GL coordinates
	panel->vxl2gl(i_+0.5, j_+0.5, glx1, gly1);
	// to determine the radius of the circle in gl coordinates
	// we compute the distance, in gl cordinates of vxl points
	// (0,0) and (rad_,0)
	panel->vxl2gl(0.0, 0.0, glr1, glrz1);
	panel->vxl2gl(rad_, 0.0, glr2, glrz2);

	if (fill_)
		// if we need a filled circle, we draw a polygon
		glBegin(GL_POLYGON);
	else
		// otherwise, we draw a list of lines
		glBegin(GL_LINE_LOOP);
	{
		for (int i=0; i<360; i++) {
			// we just use the angle parameterization for
			// the circle to draw its vertices
			double rads = i*vnl_math::pi/180;
			glVertex2d(cos(rads)*(glr2-glr1)+glx1, 
				sin(rads)*(glr2-glr1)+gly1);
		}
	}
	glEnd();

	return clear_;
}


void imdraw_vec::init(double dj, double di, int j, int i)
{
	// do the default initialization first
	// by default, make the object displayable
	clear();

	// define the object id
	id_ = imdraw_object::Vec_ID;

	// compute the angle that the vector forms with the
	// j-axis
	angle_ = atan2(di, dj);

	// create a rotation matrix that will be used to 
	// rotate a (0,1) vector by angle
	vnl_double_2x2 rot;
	vnl_double_2 v1, v2;

	rot(0,0) = cos(angle_);
	rot(0,1) = sin(angle_);
	rot(1,0) = -sin(angle_);
	rot(1,1) = cos(angle_);
	v1(0) = 3*cos(vnl_math::pi/4);
	v1(1) = -3*sin(vnl_math::pi/4);
	v2(0) = -3*cos(vnl_math::pi/4);
	v2(1) = -3*sin(vnl_math::pi/4);

	vv1_ = rot * v1;
	vv2_ = rot * v2;

	i_ = i;
	j_ = j;
	di_ = di;
	dj_ = dj;
	scale_ = 1;
}

imdraw_vec::imdraw_vec(double dj, double di, int j, int i)
{
	init(dj, di, j, i);
}

imdraw_vec::imdraw_vec(double dj, double di, int j, int i, double scale,
					   double r, double g, double b)
{
	init(dj, di, j, i);

	scale_ = scale;

	// set the color
	col_[0] = r;
	col_[1] = g;
	col_[2] = b;

}

imdraw_vec::imdraw_vec(double dj, double di, int j, int i, double scale,
					   double r, double g, double b, bool clear)
{
	init(dj, di, j, i);

	scale_ = scale;

	// set the color
	col_[0] = r;
	col_[1] = g;
	col_[2] = b;
	// set the persistence flag
	clear_ = clear;
}


bool imdraw_vec::draw(ImDraw* panel)
{
	double glx1, gly1, glx2, gly2;
	double glx3, gly3, glx4, gly4;

	// Draw the border of the rectangular region in red 
	glColor4dv(col_);
	
	// convert VXL coordinates to GL coordinates
	panel->vxl2gl(i_+0.5, j_+0.5, glx1, gly1);
	panel->vxl2gl(i_+di_*scale_+0.5, j_+dj_*scale_+0.5, glx2, gly2);
	panel->vxl2gl(i_+di_*scale_+vv1_(0)+0.5, j_+dj_*scale_+vv1_(1)+0.5, glx3, gly3);
	panel->vxl2gl(i_+di_*scale_+vv2_(0)+0.5, j_+dj_*scale_+vv2_(1)+0.5, glx4, gly4);

	glBegin(GL_LINES);
    {
	    glVertex2d(glx1, gly1);
	    glVertex2d(glx2, gly2);

		glVertex2d(glx2, gly2);
		glVertex2d(glx3, gly3);

		glVertex2d(glx2, gly2);
		glVertex2d(glx4, gly4);
	}
	glEnd();

	return clear_;
}

//
//  Classes for creating & drawing a rectangle object
//

void imdraw_rect::init(int j1, int i1, int j2, int i2)
{
	// do the default initialization first
	// by default, make the object displayable

	clear();
	// set color to white by default
	col_[0] = col_[1] = col_[2] = 1;

	// define the object id
	id_ = imdraw_object::Rect_ID;

	i1_ = i1;
	i2_ = i2;
	j1_ = j1;
	j2_ = j2;

	fill_ = false;
}

imdraw_rect::imdraw_rect(int j1, int i1, int j2, int i2)
{
	init(j1, i1, j2, i2);
}

imdraw_rect::imdraw_rect(int j1, int i1, int j2, int i2, 
						 double r, double g, double b)
{
	// do the default initialization first
	init(j1, i1, j2, i2);

	// set the color
	col_[0] = r;
	col_[1] = g;
	col_[2] = b;
}

imdraw_rect::imdraw_rect(int j1, int i1, int j2, int i2, 
						 bool fill,
						 double r, double g, double b)
{
	// do the default initialization first
	init(j1, i1, j2, i2);

	// specify whether or not the rectangle should be
	// filled
	fill_ = fill;

	// set the color
	col_[0] = r;
	col_[1] = g;
	col_[2] = b;
}

imdraw_rect::imdraw_rect(int j1, int i1, int j2, int i2, 
						 bool fill,
						 double r, double g, double b,
						 bool clear)
{
	// do the default initialization first
	init(j1, i1, j2, i2);

	// specify whether or not the rectangle should be
	// filled
	fill_ = fill;

	// set the color
	col_[0] = r;
	col_[1] = g;
	col_[2] = b;

	// set the persistence flag
	clear_ = clear;
}

bool imdraw_rect::draw(ImDraw* panel)
{
	double glx1, gly1, glx2, gly2;	

	// Draw the border of the rectangular region in red 
	glColor4dv(col_);
	
	// convert VXL coordinates to GL coordinates
	panel->vxl2gl(i1_, j1_, glx1, gly1);
	panel->vxl2gl(i2_+1, j2_+1, glx2, gly2);

	if (!fill_) {
		// if the rectangle is not to be filled we
		// just draw the polygon corresponding to its
		// outline
		glBegin(GL_LINE_LOOP);
	    {
		    glVertex2d(glx1, gly1);
		    glVertex2d(glx2, gly1);
		    glVertex2d(glx2, gly2);
		    glVertex2d(glx1, gly2);
	    }
	    glEnd();
	} else {
		// otherwise we draw a solid quad
		glBegin(GL_QUADS);
	    {
		    glVertex2d(glx1, gly1);
		    glVertex2d(glx2, gly1);
		    glVertex2d(glx2, gly2);
		    glVertex2d(glx1, gly2);
	    }
	    glEnd();
	}
	return clear_;
}

void imdraw_display_list::push(imdraw_object* obj)
{
	Q.push(obj);
}

void imdraw_display_list::draw(ImDraw* panel)
{	
	int i, sz;
	bool to_clear;
	imdraw_object* obj;

	if (!Q.empty()) {
		// go through all the elements in the queue,
		// displaying them one by one and deleting all
		// those that are marked as 'cleared'
		sz = Q.size();
		for (i=0; i<sz; i++) {
			// get the next object to draw
			obj = Q.front();
			// remove from the queue
			Q.pop();
			// if the object's persistence flag is true
			// we draw and put it back on the list
			if (obj->draw(panel) == true)
				// if the object is NOT marked as cleared
				// we add it back to the end of the queue
				Q.push(obj);
			else 
				// otherwise, the object is deleted after drawing
				delete obj;
		}
	}
}

void imdraw_display_list::undo()
{
	// delete the most-recently inserted element of the 
	//display list
	int i;
	imdraw_object* obj;

	if (!Q.empty()) {
		// remove and re-insert all elements except the
		// most recently inserted on
		for (i=0; i<Q.size()-1; i++) {
			obj = Q.front();
			Q.pop();
			Q.push(obj);
		}
		// remove the most recently-inserted element
		obj = Q.front();
		Q.pop();
		delete obj;
	}
}

void imdraw_display_list::clear()
{	
	// delete all entries in the display list
	while (!Q.empty())
		Q.pop();
}


