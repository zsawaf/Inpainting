// DO NOT MODIFY THIS FILE FOR ASSIGNMENT 1 or 2 !!!


// WARNING: DO NOT MODIFY THIS FILE EXCEPT WHERE EXPLICITLY NOTED
//

#ifndef _imdraw_object_h 
#define _imdraw_object_h

// This file contains a set of classes for drawing simple geometric
// objects on an ImDraw panel. 
// 
// There are two main classes: 
//   (1) imdraw_object: this is a virtual class that encapsulates 
//       the basic functionalities that a 'drawable' geometric object 
//       possesses. The various geometric objects (circles, rectangles, 
//       vectors) are considered instances of this basic class and 
//       provide their own drawing routines.
//   (2) imdraw_display_list: this is a queue that serves as
//       a display list---each time the imdraw_draw() routine is 
//       called, all the objects in the display list are poped from
//       the queue, drawn, and then pushed back onto the queue. The
//       display list provides additional functionalities such as
//       an undo function, a clear function, etc.
// 
// Important: All coordinates stored in drawable objects are VXL 
// coordinates

class ImDraw;

// the idmraw_object class provides two kinds of drawable objects:
//   * persistent objects (clear_ = true), are drawn onto the display 
//     until there is an explicit call to the undo() or clear() method
//     of imdraw_display_list that explicitly removes them from the list.
//     the objects persit after refresh/redraw operations of the opengl
//     panel
//   * temporary objects (clear_ = false), are drawn onto the display
//     just once and then automatically removed from the display list;
//     these objects disappear after the next screen refresh.
// 
// each drawable object has the following descriptors and methods
//   * a color (r, g, b, alpha)
//   * whether or not the object should be filled (eg. whether it is
//     the outline of a circle, or a full disk)
//   * draw(): the method that draws the object onto the opengl panel
//   * clear(): return whether or not the object is persistent
//   * id(): the type of drawable
class imdraw_object {
protected:
	// descriptors for the types of object
	// that can be drawn
	enum object_type {Rect_ID, Vec_ID, Circ_ID};
	object_type id_;
	double col_[4];
	bool clear_;
	bool fill_;
public:
	object_type id();
	virtual bool draw(ImDraw* panel) = 0;
	void clear();
};


// 
// A class implementing a simple display list
// 
class imdraw_display_list {
	// a queue that implements the display list
	vcl_queue<imdraw_object*> Q;
public:
	// push another imdraw_object at the tail of the queue
	void push(imdraw_object* obj);
	// draw all objects in the queue; after drawing, all
	// objects with clear_=false are automatically deleted from
	// the queue
	void draw(ImDraw* panel);
	// delete all objects from the queue 
	void clear();
	// delete the object at the tail of the queue
	void undo();
	// IMPORTANT: the class assumes that all objects
	// pushed on the queue are NOT re-used elsewhere.
	// THE ROUTINE FREES THE MEMORY OF OBJECTS THAT ARE POPPED 
	// FROM THE QUEUE (by explicitly calling their destructor 
	// routine)
};

//
//  The rectangle subclass
// 

class imdraw_rect : public imdraw_object {
	// coordinates of the top left (i1,j1)
	// and bottom right (i2,j2) corner of the
	// rectangle
	int j1_;
	int i1_; 
	int j2_;
	int i2_;
	// initialization routine 
	void init(int j1, int i1, int j2, int i2);
public:
	// 
	// constructors
	// 
	// 1. create a persistent, filled, rectangle 
	imdraw_rect(int j1, int i1, int j2, int i2);
	// 2. create a persistent, filled, rectangle
	//    of a specified color (note that the 
	//    alpha component is always 1 and is NOT
	//    passed as a parameter)
	imdraw_rect(int j1, int i1, int j2, int i2, 
		        double r, double g, double b);
	// 3. create a persistent rectangle of a given
	//    color and specify whether or not it should
	//    be filled
	imdraw_rect(int j1, int i1, int j2, int i2, 
				bool fill,
		        double r, double g, double b);
	// 4. create a rectangle and specify whether or
	//    not it should be persistent, filled, and 
	//    what its color should be
	imdraw_rect(int j1, int i1, int j2, int i2, 
				bool fill,
		        double r, double g, double b,
				bool clear);
	// Drawing method
	bool draw(ImDraw* panel);
};

//
// The circle subclass
//

// the definitions parallel those for the rectangle class
class imdraw_circle: public imdraw_object {
	int i_;
	int j_;
	double rad_;
	void init(int j, int i, double rad);
public:
	imdraw_circle(int j, int i, double rad);
	imdraw_circle(int j, int i, double rad,
				  double r, double g, double b);
	imdraw_circle(int j, int i, double rad,
				  double r, double g, double b,
				  bool clear);
	imdraw_circle(int j, int i, double rad,
				  bool fill,
				  double r, double g, double b,
				  bool clear);
	bool draw(ImDraw* panel);
};

//
// The vector subclass
//

// this class is for drawing vectors on the screen
// the definitions parallel those of the rectangle class
class imdraw_vec : public imdraw_object {
	// (di_, dj_) is a unit vector that defines the vector's direction
	double di_;
	double dj_;
	// (i_, j_) define the vector's origin
	int i_;
	int j_;
	// a scaling that controls the vector's length in the
	// direction (di_, dj_)
	double scale_;
	//
	// internal parameters
	//
	// the angle of the vector (in radians)
	double angle_;
	// internal parameters 
	vnl_double_2 vv1_, vv2_;
	void init(double dj, double di, int j, int i);
public:
	imdraw_vec(double dj, double di, int j, int i);
	imdraw_vec(double dj, double di, int j, int i, double scale,
		        double r, double g, double b);
	imdraw_vec(double dj, double di, int j, int i, double scale,
		        double r, double g, double b,
				bool clear);
	bool draw(ImDraw* panel);
};


#endif


