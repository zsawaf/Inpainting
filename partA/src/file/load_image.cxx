// DO NOT MODIFY THIS FILE

#include "../vxl_includes.h"

// function takes a filename as input and returns a 3-component
// rgb VXL image, upon failure, the returned image has 0 rows and
// columns
vil_image_view<vil_rgb<vxl_byte> > load_image(vcl_string fname)
{
	vil_image_view<vil_rgb<vxl_byte> > empty_im;

	// if the filename string was empty
	if (fname.empty() == true)
		// return an empty image
		return empty_im;

  // The call below does the following: It opens and reads the file
  // specified in fname (should work on .jpg, .tif, and .pnm files),
  // ensures that the resulting image has a data type of vxl_byte,
  // ensure that it is a 3 plane image (basically converting any
  // grayscale image to RGB with 3 identical planes).
  // And return the resulting image view.

	return vil_convert_to_component_order( // convert image to RGB format
		          vil_convert_to_n_planes( // convert image to a 3-band image
				           3, vil_convert_cast(vxl_byte(),  // force image to
															// have byte-sized pixels
						                       vil_load(fname.c_str()))));
}

// function takes a filename as input and returns a 
// VXL image, upon failure, the returned image has 0 rows and
// columns
vil_image_view<vxl_byte> load_image1(vcl_string fname)
{
	vil_image_view<vxl_byte> empty_im;

	// if the filename string was empty
	if (fname.empty() == true)
		// return an empty image
		return empty_im;

  // The call below does the following: It opens and reads the file
  // specified in fname (should work on .jpg, .tif, and .pnm files),
  // ensures that the resulting image has a data type of vxl_byte,
  // and returns the resulting image view.

	return vil_convert_cast(vxl_byte(), vil_load(fname.c_str()));
}


