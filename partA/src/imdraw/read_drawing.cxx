// WARNING: DO NOT MODIFY THIS FILE EXECPT WHERE EXPLICITLY NOTED

#include "../vxl_includes.h"


#include "../VisCompUI.h"

// DO NOT MODIFY THIS FILE FOR A1
// IT IS A PLACEHOLDER FOR FUTURE ASSIGNMENTS

// Routine for reading and executing drawing commands from a file
void process_drawing_commands(char* progname, char* filename, VisCompUI& window)
{
	FILE *cmdfile;
	char cmd;

	if ((cmdfile = fopen(filename, "r")) == NULL)
		 vcl_cerr << progname << ": Error opening file " 
			  << filename << "\n";
	else
		 while ((cmd = fgetc(cmdfile)) != EOF) {
			 switch (cmd) {
			 case 'r': {
			      // Command:
			      //    r <j1> <i1> <j2> <i2>
			      // Draw a rectangular region with corners
			      // (j1,i1) and (j2,i2) expressed in VXL
			      // coordinates
			      int j1, i1, j2, i2;
			      fscanf(cmdfile, "%d %d %d %d", 
				     &j1, &i1, &j2, &i2);
			      // This drawing function is defined in fluid
			      // as a method of the VisCompUI interface.
			      // That method passes the command
			      // to the ImDraw class
			      break;
			 }
			      // add your code for the other drawing commands
			      // here
			 default:
			      break;
			 }
		    }
}


