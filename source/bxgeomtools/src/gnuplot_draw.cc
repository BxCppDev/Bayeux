// -*- mode: c++; -*- 
/* gnuplot_draw.cc
 */

#include <geomtools/gnuplot_draw.h>

namespace geomtools {

  void 
  gnuplot_draw::basic_draw_point (std::ostream & out_, 
				  double x_, double y_, double z_,
				  bool endl_)
  {
    out_ << x_ << ' ' 
	 << y_ << ' ' 
	 << z_;
    if (endl_) out_ << std::endl;
  }

  void 
  gnuplot_draw::basic_draw_point (std::ostream & out_, 
				  const vector_3d & point_,
				  bool endl_)
  {
    gnuplot_draw::basic_draw_point (out_, 
				    point_.x (), 
				    point_.y (),
				    point_.z (), 
				    endl_);
  }

  void 
  gnuplot_draw::basic_draw_polyline (std::ostream & out_, 
				     const polyline_t & pl_)
  {
    for (polyline_t::const_iterator i = pl_.begin ();
	 i != pl_.end ();
	 ++i) 
      {
	basic_draw_point (out_, *i);
      }
    out_ << std::endl;
  }

  void 
  gnuplot_draw::draw_line (std::ostream & out_, 
			   const vector_3d & start_ , 
			   const vector_3d & stop_)
  {
    basic_draw_point (out_, start_);
    basic_draw_point (out_, stop_);
    out_ << std::endl;
  }

  void 
  gnuplot_draw::draw_polyline (std::ostream & out_, 
			       const vector_3d & position_, 
			       const rotation_3d & rotation_,
			       const polyline_t & pl_, 
			       bool  more_)
  {
    rotation_3d inverseRotation (rotation_);
    inverseRotation.invert ();
    polyline_t polyline;
    vector_3d first;
    bool start = true;
    for (polyline_t::const_iterator i = pl_.begin ();
	 i != pl_.end ();
	 i++) 
      {
	vector_3d P (*i);
	P.transform (inverseRotation);
	P += position_;
	if (start) 
	  {
	    first = P;
	    start = false;
	  }
	polyline.push_back (P);	
      } 
    if (more_) polyline.push_back (first);
    basic_draw_polyline (out_, polyline);
  }

  void 
  gnuplot_draw::draw_segment (std::ostream & out_, 
			      const vector_3d & position_, 
			      const rotation_3d & rotation_,  
			      const vector_3d & start_, 
			      const vector_3d & stop_)
  {
    rotation_3d inverseRotation (rotation_);
    inverseRotation.invert ();
    vector_3d A (start_);
    A.transform (inverseRotation);
    A += position_;
    vector_3d B (stop_);
    B.transform (inverseRotation);
    B += position_;
    polyline_t polyline;
    polyline.push_back (A);
    polyline.push_back (B);
    basic_draw_polyline (out_, polyline);
  }

  void 
  gnuplot_draw::draw_rectangle (std::ostream & out_, 
				const vector_3d & position_, 
				const rotation_3d & rotation_, 
				double length_, 
				double width_, 
				bool more_)
  {
    vector_3d A ( 0.5 * length_,  0.5 * width_, 0.);
    vector_3d B ( 0.5 * length_, -0.5 * width_, 0.);
    vector_3d C (-0.5 * length_, -0.5 * width_, 0.);
    vector_3d D (-0.5 * length_,  0.5 * width_, 0.);

    rotation_3d inverseRotation (rotation_);
    inverseRotation.invert ();

    vector_3d A2 (A);
    A2.transform (inverseRotation);
    A2 += position_;

    vector_3d B2 (B);
    B2.transform (inverseRotation);
    B2 += position_;

    vector_3d C2 (C);
    C2.transform (inverseRotation);
    C2 += position_;

    vector_3d D2 (D);
    D2.transform (inverseRotation);
    D2 += position_;

    polyline_t polyline;
    polyline.push_back (A2);
    polyline.push_back (B2);
    polyline.push_back (C2);
    polyline.push_back (D2);
    polyline.push_back (A2);
    if (more_) polyline.push_back (B2); 
    basic_draw_polyline (out_,polyline);
  }

  void 
  gnuplot_draw::draw_box (std::ostream & out_,
			  const vector_3d & position_, 
			  const rotation_3d & rotation_,
			  double length_, 
			  double width_, 
			  double height_)
  {
    vector_3d A ( 0.5 * length_,  0.5 * width_,  0.5 * height_);
    vector_3d B ( 0.5 * length_, -0.5 * width_,  0.5 * height_);
    vector_3d C (-0.5 * length_, -0.5 * width_,  0.5 * height_);
    vector_3d D (-0.5 * length_,  0.5 * width_,  0.5 * height_);
    vector_3d P ( 0.5 * length_,  0.5 * width_, -0.5 * height_);
    vector_3d Q ( 0.5 * length_, -0.5 * width_, -0.5 * height_);
    vector_3d R (-0.5 * length_, -0.5 * width_, -0.5 * height_);
    vector_3d S (-0.5 * length_,  0.5 * width_, -0.5 * height_);

    rotation_3d inverseRotation (rotation_);
    inverseRotation.invert ();

    vector_3d A2 (A);
    A2.transform (inverseRotation);
    A2 += position_;

    vector_3d B2 (B);
    B2.transform (inverseRotation);
    B2 += position_;

    vector_3d C2 (C);
    C2.transform (inverseRotation);
    C2 += position_;

    vector_3d D2 (D);
    D2.transform (inverseRotation);
    D2 += position_;

    vector_3d P2 (P);
    P2.transform (inverseRotation);
    P2 += position_;

    vector_3d Q2 (Q);
    Q2.transform (inverseRotation);
    Q2 += position_;

    vector_3d R2 (R);
    R2.transform (inverseRotation);
    R2 += position_;

    vector_3d S2 (S);
    S2.transform (inverseRotation);
    S2 += position_;

    polyline_t polyline;
    polyline.push_back (A2);
    polyline.push_back (B2);
    polyline.push_back (C2);
    polyline.push_back (D2);
    polyline.push_back (A2);
    basic_draw_polyline (out_, polyline);

    polyline.clear ();
    polyline.push_back (P2);
    polyline.push_back (Q2);
    polyline.push_back (R2);
    polyline.push_back (S2);
    polyline.push_back (P2);
    basic_draw_polyline (out_, polyline);

    polyline.clear ();
    polyline.push_back (A2);
    polyline.push_back (P2);
    basic_draw_polyline (out_, polyline);

    polyline.clear ();
    polyline.push_back (B2);
    polyline.push_back (Q2);
    basic_draw_polyline (out_, polyline);

    polyline.clear ();
    polyline.push_back (C2);
    polyline.push_back (R2);
    basic_draw_polyline (out_, polyline);

    polyline.clear ();
    polyline.push_back (D2);
    polyline.push_back (S2);
    basic_draw_polyline (out_, polyline);

  }

  void 
  gnuplot_draw::draw_cylinder (std::ostream & out_,
			       const vector_3d & position_, 
			       const rotation_3d & rotation_,
			       double radius_, 
			       double height_,
			       size_t arc_sampling_)
  {
    rotation_3d inverseRotation (rotation_);
    inverseRotation.invert ();

    size_t sample =  arc_sampling_;
    double dangle =  2 * M_PI * CLHEP::radian / sample;
    polyline_t polyline_top;
    polyline_t polyline_bottom;
    polyline_t polyline_segment;
    for (size_t i = 0; i <= sample ; ++i) 
      {
	vector_3d P,Q;
	double angle = i * dangle;
	P.set (radius_ * std::cos (angle), 
	       radius_ * std::sin (angle),  
	       0.5 * height_);
	Q.set (radius_ * std::cos (angle), 
	       radius_ * std::sin (angle), 
	       -0.5 * height_);
	vector_3d P2 (P);
	P2.transform (inverseRotation);
	P2 += position_;
	polyline_top.push_back (P2);
	vector_3d Q2 (Q);
	Q2.transform (inverseRotation);
	Q2 += position_;
	polyline_bottom.push_back (Q2);
	polyline_segment.clear ();
	polyline_segment.push_back (P2);
	polyline_segment.push_back (Q2);
	basic_draw_polyline (out_, polyline_segment);
      }
    basic_draw_polyline (out_, polyline_top);
    basic_draw_polyline (out_, polyline_bottom);
    
  }

  void 
  gnuplot_draw::draw_tube (std::ostream & out_,
			   const vector_3d & position_, 
			   const rotation_3d & rotation_,
			   double inner_radius_, 
			   double outer_radius_, 
			   double height_,
			   size_t arc_sampling_)
  {
    rotation_3d inverseRotation (rotation_);
    inverseRotation.invert ();

    size_t sample = arc_sampling_;
    double dangle = 2 * M_PI * CLHEP::radian / sample;
    polyline_t polyline_top_i;
    polyline_t polyline_bottom_i;
    polyline_t polyline_segment_i;
    polyline_t polyline_top_o;
    polyline_t polyline_bottom_o;
    polyline_t polyline_segment_o;
    polyline_t polyline_endcap_top;
    polyline_t polyline_endcap_bottom;
    for (size_t i = 0; i <= sample ; ++i) 
      {
	vector_3d P_i, Q_i;
	vector_3d P_o, Q_o;
	double angle = i * dangle;
	P_i.set (inner_radius_ * std::cos (angle),
		 inner_radius_ * std::sin (angle),
		 0.5 * height_);
	Q_i.set (inner_radius_ * std::cos (angle),
		 inner_radius_ * std::sin (angle),
		 -0.5 * height_);
	P_o.set (outer_radius_ * std::cos (angle),
		 outer_radius_ * std::sin (angle),
		 0.5 * height_);
	Q_o.set (outer_radius_ * std::cos (angle),
		 outer_radius_ * std::sin (angle),
		 -0.5 * height_);
	vector_3d P2_i (P_i);
	P2_i.transform (inverseRotation);
	P2_i += position_;
	polyline_top_i.push_back (P2_i);
	vector_3d Q2_i (Q_i);
	Q2_i.transform (inverseRotation);
	Q2_i += position_;

	vector_3d P2_o (P_o);
	P2_o.transform (inverseRotation);
	P2_o += position_;
	polyline_top_o.push_back (P2_o);
	vector_3d Q2_o (Q_o);
	Q2_o.transform (inverseRotation);
	Q2_o += position_;

	polyline_bottom_i.push_back (Q2_i);
	polyline_segment_i.clear ();
	polyline_segment_i.push_back (P2_i);
	polyline_segment_i.push_back (Q2_i);
	basic_draw_polyline (out_, polyline_segment_i);

	polyline_bottom_o.push_back (Q2_o);
	polyline_segment_o.clear ();
	polyline_segment_o.push_back (P2_o);
	polyline_segment_o.push_back (Q2_o);
	basic_draw_polyline (out_, polyline_segment_o);

	polyline_endcap_top.clear ();
	polyline_endcap_top.push_back (P2_i);
	polyline_endcap_top.push_back (P2_o);
	basic_draw_polyline (out_, polyline_endcap_top);

	polyline_endcap_bottom.clear ();
	polyline_endcap_bottom.push_back (Q2_i);
	polyline_endcap_bottom.push_back (Q2_o);
	basic_draw_polyline (out_, polyline_endcap_bottom);

      }
    basic_draw_polyline (out_, polyline_top_i);
    basic_draw_polyline (out_, polyline_bottom_i);
    basic_draw_polyline (out_, polyline_top_o);
    basic_draw_polyline (out_, polyline_bottom_o);
    
  }

  void 
  gnuplot_draw::draw_circle (std::ostream & out_, 
			     const vector_3d & position_, 
			     const rotation_3d & rotation_, 
			     double radius_,
			     size_t arc_sampling_)
  {
    rotation_3d inverseRotation (rotation_);
    inverseRotation.invert ();

    size_t sample =  arc_sampling_;
    double dangle = 2 * M_PI * CLHEP::radian / sample;
    polyline_t polyline;
    for (size_t i = 0; i <= sample; ++i) 
      {
	vector_3d P;
	double angle = i * dangle;
	P.set (radius_ * std::cos (angle),
	       radius_ * std::sin (angle),
	       0.0);
	vector_3d P2 (P);
	P2.transform (inverseRotation);
	P2 += position_;
	polyline.push_back (P2);
      }
    basic_draw_polyline (out_, polyline);
  }

  void 
  gnuplot_draw::draw_sphere (std::ostream & out_,
			     const vector_3d & position_, 
			     const rotation_3d & rotation_,
			     double radius_, 
			     size_t arc_sampling_,
			     size_t z_sampling_)
  {
    rotation_3d inverseRotation (rotation_);
    inverseRotation.invert ();

    size_t phy_sample = arc_sampling_;
    size_t z_sample = z_sampling_;
    double dphi =  2 * M_PI * CLHEP::radian / phy_sample;
    double dz   =  2 * radius_ / z_sample;
    polyline_t polyline_meridian;
    for (size_t i = 0; i <= phy_sample ; ++i) 
      {
	polyline_meridian.clear ();
	vector_3d P,Q;
	double phi = i * dphi;
	for (int j = 0; j <= (int) z_sample ; j++)
	  {
	    double z = -radius_ + j * dz;
	    double theta = std::acos (z / radius_);
	    P.set (radius_ * std::cos (phi) * std::sin (theta), 
		   radius_ * std::sin (phi) * std::sin (theta),  
		   z);
	    vector_3d P2 (P);
	    P2.transform (inverseRotation);
	    P2 += position_;
	    polyline_meridian.push_back (P2);
	  }
	basic_draw_polyline (out_, polyline_meridian);
      }
    //polyline_t polyline_parallel;
    //basic_draw_polyline (out_, polyline_parallel);
 
    //throw std::runtime_error ("gnuplot_draw::draw_sphere: Not implemented yet!");
  }

} // end of namespace geomtools

// end of gnuplot_draw.cc
