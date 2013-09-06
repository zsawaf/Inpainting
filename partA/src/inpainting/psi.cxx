// DO NOT MODIFY THIS FILE!!!

// NOTE: It is not necessary to read or understand
// this file for the Inpainting assignment. 
// 
// If you want to see examples of how to loop over
// pixels, look at the bottom of the file, for the
// get_pixels and copy_pixels routines. 
//


#include "psi.h"

//
// Basic constructor routines
//

psi::psi(const vnl_double_2& center, int rad, int ni, int nj) 
{
	P_ = 0; 
	C_ = 0;
	D_ = 0;
	p_ = center; 
	w_ = rad;
	sz_ = 2*rad+1;
	ni_ = ni; 
	nj_ = nj;

	begin();
}

psi::psi(const psi& PSI) 
{ 
	P_ = PSI.P_;  
	C_ = PSI.C_;
	D_ = PSI.D_;
	p_ = PSI.p_; 
	w_ = PSI.w_; 
	ni_ = PSI.ni_; 
	nj_ = PSI.nj_; 
	sz_ = PSI.sz_;
	
	begin();
}

bool psi::operator<(const psi & rhs) const 
{ 
	return (P() < rhs.P()); 
} 

int psi::sz() const
{
	return 2*w_+1;
}

double psi::P() const 
{
	return P_;
}

double psi::C() const
{
	return C_;
}

double psi::D() const
{
	return D_;
}

vnl_double_2 psi::p() const 
{
	return p_;
}

int psi::w() const 
{
	return w_;
}

int psi::ni() const
{
	return ni_;
}

int psi::nj() const
{
	return nj_;
}

void psi::set_P(double priority, double confidence, double dataterm) 
{
	P_ = priority;
	C_ = confidence;
	D_ = dataterm;
}

int psi::begin_i() const
{
  return vcl_max<int>(-(int)(p_(0)), -w_);
}

int psi::begin_j() const
{
  return vcl_max<int>(-(int)(p_(1)), -w_);  
}

void psi::begin()
{
	di_ = begin_i();
	dj_ = begin_j();
}

void psi::begin(const vnl_matrix<int>& mask)
{
	begin();
	while (!mask(di_+w_, dj_+w_))
		if (!next())
			break;
}

bool psi::next(const vnl_matrix<int>& mask)
{	
	if (!next())
		return false;
	else 
		while (!mask(di_+w_, dj_+w_))
			if (!next())
				return false;
	return true;
}

bool psi::end_i() const
{
  return ( di_ == vcl_min<int>(w_, ni_-1-(int)(p_(0))) );
}

bool psi::end_j() const
{
  return ( dj_ == vcl_min<int>(w_, nj_-1-(int)(p_(1))) );
}

bool psi::end() const
{
	return (end_i() && end_j());
}

bool psi::next()
{
	if (!end_j()) {
		dj_ += 1;
		return true;
	} else if (!end_i()) {
		di_ += 1;
		dj_ = begin_j();
		return true;
	} else 
		return false;
}

void psi::image_coord(int& i, int& j) const
{
  i = (int)(p_(0)) + di_;
  j = (int)(p_(1)) + dj_;
}

void psi::image_vertices(int& j1, int& i1, int& j2, int& i2) const
{
  j1 = (int)(p_(1))-w_;
  j2 = (int)(p_(1))+w_;
  i1 = (int)(p_(0))-w_;
  i2 = (int)(p_(0))+w_;
}

void psi::psi_coord(int& i, int& j) const
{
	i = di_ + w_;
	j = dj_ + w_;
}

void psi::vector_coord(int& i) const
{
	int x, y;

	psi_coord(x, y);
	i = x*(2*w_ + 1) + y;
}

int psi::vector_length() const
{
	return sz()*sz();
}

psi psi::expand(int i)
{
	psi expanded(*this);

	expanded.w_ = w_+ i;
	expanded.sz_ = 2*expanded.w_ + 1;

	return expanded;
}

void psi::get_pixels(const vil_image_view<vxl_byte>& image, 
					vnl_matrix<int>& pixel_mat,
					vnl_matrix<int>& valid_mat)
{
	int pi, pj, i, j;

	if ((image.ni() != ni_) && (image.nj() != nj_)) {
		vcl_cerr << "Warning: psi::get_pixels: incompatible image dimensions\n";
		valid_mat = 0;
		return;
	}

	pixel_mat.set_size(sz_, sz_);
	valid_mat.set_size(sz_, sz_);

	valid_mat.fill(false);

	begin();
	do {
		psi_coord(pi, pj);
		image_coord(i, j);
		pixel_mat(pi, pj) = image(i, j);
		valid_mat(pi, pj) = true;
	} while (next());
}

void psi::get_pixels(const vil_image_view<bool>& image, 
					vnl_matrix<int>& pixel_mat,
					vnl_matrix<int>& valid_mat)
{
	int pi, pj, i, j;

	if ((image.ni() != ni_) && (image.nj() != nj_)) {
		vcl_cerr << "Warning: psi::get_pixels: incompatible image dimensions\n";
		valid_mat = 0;
		return;
	}

	pixel_mat.set_size(sz_, sz_);
	valid_mat.set_size(sz_, sz_);

	valid_mat.fill(false);

	begin();
	do {
		psi_coord(pi, pj);
		image_coord(i, j);
		pixel_mat(pi, pj) = (vxl_byte) image(i, j);
		valid_mat(pi, pj) = true;
	} while (next());
}

void psi::get_pixels(const vil_image_view<double>& image, 
					vnl_matrix<double>& pixel_mat,
					vnl_matrix<int>& valid_mat)
{
	int pi, pj, i, j;

	if ((image.ni() != ni_) && (image.nj() != nj_)) {
		vcl_cerr << "Warning: psi::get_pixels: incompatible image dimensions\n";
		valid_mat = 0;
		return;
	}

	pixel_mat.set_size(sz_, sz_);
	valid_mat.set_size(sz_, sz_);

	valid_mat.fill(false);

	begin();
	do {
		psi_coord(pi, pj);
		image_coord(i, j);
		pixel_mat(pi, pj) = image(i, j);
		valid_mat(pi, pj) = true;
	} while (next());
}

void psi::set_pixels(const vnl_matrix<int>& pixel_mat,
					 const vnl_matrix<int>& valid_mat,
					 vil_image_view<vxl_byte>& image)
{
	int pi, pj, i, j;

	if ((image.ni() != ni_) && (image.nj() != nj_)) {
		vcl_cerr << "Warning: psi::set_pixels: incompatible image dimensions\n";
		return;
	}

	begin();
	do {
		psi_coord(pi, pj);
		image_coord(i, j);
		if (valid_mat(pi, pj)) 
			image(i, j) = pixel_mat(pi, pj);
	} while (next());
}

void psi::set_pixels(const vnl_matrix<int>& pixel_mat,
					 const vnl_matrix<int>& valid_mat,
					 vil_image_view<bool>& image)
{
	int pi, pj, i, j;

	if ((image.ni() != ni_) && (image.nj() != nj_)) {
		vcl_cerr << "Warning: psi::set_pixels: incompatible image dimensions\n";
		return;
	}

	begin();
	do {
		psi_coord(pi, pj);
		image_coord(i, j);
		if (valid_mat(pi, pj)) 
			image(i, j) = (bool) pixel_mat(pi, pj);
	} while (next());
}


void psi::set_pixels(const vnl_matrix<double>& pixel_mat,
					 const vnl_matrix<int>& valid_mat,
					 vil_image_view<double>& image)
{
	int pi, pj, i, j;

	if ((image.ni() != ni_) && (image.nj() != nj_)) {
		vcl_cerr << "Warning: psi::set_pixels: incompatible image dimensions\n";
		return;
	}

	begin();
	do {
		psi_coord(pi, pj);
		image_coord(i, j);
		if (valid_mat(pi, pj)) 
			image(i, j) = pixel_mat(pi, pj);
	} while (next());
}


