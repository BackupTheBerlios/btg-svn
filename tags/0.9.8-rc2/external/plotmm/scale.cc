/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * PlotMM Widget Library
 * Copyright (C) 2004   Andy Thaller
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 *****************************************************************************/

#include <pangomm/layout.h>

#include <plotmm/scale.h> 

#include <iostream>
#include <sstream>

using namespace PlotMM;



/*! Constructor
 */
ScaleLabels::ScaleLabels(Gtk::PositionType p) :
    position_(p),
    enabled_(true)
{
    layout_= create_pango_layout("");
}

/*! Destructor */
ScaleLabels::~ScaleLabels()
{
}

/*! Set the labels to be drawn
 *
 *  The given map holds double values associated with int positions on
 *  a scale.  The given offs can be taken as a hint to calculate the
 *  right positions for the labels.  Normally, the leftmost or upmost
 *  border of the scale's Gdk::Window is taken for this offset.
 *  
 *  \sa newsize_ (called automatically)
 */
void ScaleLabels::set_labels(int offs, const std::map<int,double> &labels)
{
    offset_= offs;
    labels_= labels;
    newsize_();
    update_();
}

/*! Enable drawing of scalelabels
 *  
 *  \sa newsize_ (called automatically)
 */
void ScaleLabels::set_enabled(bool b)
{
    if (b==enabled_) return;
    enabled_= b;
    newsize_();
}

/*! Format a string from a double value
 *  
 *  @todo find a smart way to user-define scale label formatting
 */
Glib::ustring ScaleLabels::format(double d) const
{
    char tmp[100];
    sprintf(tmp,"%g",d);
    return tmp;
}

/*! Query the pixel width of the given text when rendered with the
 *  current font() settings
 */
int ScaleLabels::text_width(const Glib::ustring &str) const
{
    int layw,layh;
    layout_->set_font_description(font_);
    layout_->set_text(str);
    layout_->context_changed();
    layout_->get_pixel_size(layw,layh);
    return layw;
}

/*! Query the pixel height of the given text when rendered with the
 *  current font() settings
 */
int ScaleLabels::text_height(const Glib::ustring &str) const
{
    int layw,layh;
    layout_->set_font_description(font_);
    layout_->set_text(str);
    layout_->context_changed();
    layout_->get_pixel_size(layw,layh);
    return layh;
}

/* ****************************************************************** */

/*! Constructor
 */
VScaleLabels::VScaleLabels(Gtk::PositionType p) :
    ScaleLabels(p)
{
}

/*! Destructor */
VScaleLabels::~VScaleLabels()
{
}

bool VScaleLabels::on_expose_event(GdkEventExpose* event)
{
    update_();
}

/* ****************************************************************** */

/*! Constructor
 */
HScaleLabels::HScaleLabels(Gtk::PositionType p) :
    ScaleLabels(p)
{
}

/*! Destructor */
HScaleLabels::~HScaleLabels()
{
}

bool HScaleLabels::on_expose_event(GdkEventExpose* event)
{
    update_();
}

/* ****************************************************************** */

void ScaleLabels::newsize_()
{
    if (!labels_.size()) return;
    if (!enabled()) {
	set_size_request(0,0);
	return;
    } else {
	layh_= text_height("8,8"); 
	layw_= 0;
	int min, max;
	min= max= labels_.begin()->first;
	
	std::map<int,double>::iterator daPos;
	for (daPos= labels_.begin(); daPos!=labels_.end(); ++daPos) {
	    layw_= MAX(layw_,text_width(format(daPos->second)));
	    min= MIN(min,daPos->first);
	    max= MAX(max,daPos->first);
	}
	range_= max-min;
    }
    requestsize_();
}

void VScaleLabels::requestsize_()
{
    Glib::RefPtr<Gdk::Window> window= get_window();
    if (!window) return;

    int winx,winy,winw,winh,wind;
    window->get_geometry(winx, winy, winw, winh, wind);
    set_size_request(layw_,-1);
}

void HScaleLabels::requestsize_()
{
    Glib::RefPtr<Gdk::Window> window= get_window();
    if (!window) return;

    int winx,winy,winw,winh,wind;
    window->get_geometry(winx, winy, winw, winh, wind);
    set_size_request(-1,layh_);
}

/* ****************************************************************** */

void VScaleLabels::update_()
{
    if (!enabled()) return;
    if (!labels_.size()) return;
    Glib::RefPtr<Gdk::Window> window= get_window();
    if (!window) return;
    Glib::RefPtr<Gdk::GC> gc= Gdk::GC::create(window);

    int layw,layh;
    int winx,winy,winw,winh,wind;
    window->get_geometry(winx, winy, winw, winh, wind);
    window->clear();

    std::map<int,double>::iterator daPos;
    layout_->set_font_description(font_);
    for (daPos= labels_.begin(); daPos!=labels_.end(); ++daPos) {
	layout_->set_text(format(daPos->second));
	layout_->context_changed();
	layout_->get_pixel_size(layw,layh);
	int lx=0,ly=offset_-winy-layh/2+daPos->first;
	if (position()==Gtk::POS_LEFT) {
	    lx= winw-layw;
	}
	if (ly<0) ly=0; 
	else if (ly+layh>winh) ly=winh-layh;

	window->draw_layout(gc,lx,ly,layout_);
    }
}

void HScaleLabels::update_()
{
    if (!enabled()) return;
    if (!labels_.size()) return;
    Glib::RefPtr<Gdk::Window> window= get_window();
    if (!window) return;
    Glib::RefPtr<Gdk::GC> gc= Gdk::GC::create(window);

    int layw,layh;
    int winx,winy,winw,winh,wind;
    window->get_geometry(winx, winy, winw, winh, wind);
    window->clear();

    std::map<int,double>::iterator daPos;
    layout_->set_font_description(font_);
    for (daPos= labels_.begin(); daPos!=labels_.end(); ++daPos) {
	layout_->set_text(format(daPos->second));
	layout_->context_changed();
	layout_->get_pixel_size(layw,layh);
	int lx=offset_-winx-layw/2+daPos->first,ly=0;
	if (position()==Gtk::POS_TOP) {
	    ly= winh-layh;
	}
	if (lx<0) lx=0; 
	else if (lx+layw>winw) lx=winw-layw;

	window->draw_layout(gc,lx,ly,layout_);
    }
}


/* ****************************************************************** */


/*! Constructor
 *
 *  Upon construction, the position for the Scale must be declared.
 *  Optionally, a reference to a ScaleLabels instance can be provided
 *  to allow for automatic label generation.  If the labels are actually
 *  shown can be set with ScaleLabels::set_enabled()
 */
Scale::Scale(Gtk::PositionType p, ScaleLabels *l) :
    labels_(l),
    position_(p),
    enabled_(false),
    majorTL_(10),
    minorTL_(5)
{
    set_range(-1,1,false);
}

/*! Destructor */
Scale::~Scale()
{
}

/*! Change the position of the Scale (may not work) 
 *
 * @todo check this out
 */
void Scale::set_position(Gtk::PositionType p)
{
    position_= p;
}

/*! Enable drawing of scales
 *
 *  If scale drawing is disabled, no space will be occupied and
 *  nothing will be displayed.  If a ScaleLabels instance was
 *  specified, the ScaleLabels will also be disabled.  
 *
 *  To re-enable Scale and ScaleLabels drawing, call this function
 *  with true.
 */
void Scale::set_enabled(bool b)
{
    if (labels_) labels_->set_enabled(b);
    if (b==enabled_) return;
    enabled_= b;
    on_tick_change();
    signal_enabled(enabled_);
}

/*! Set the length of major and minor ticks */
void Scale::set_ticklengths(int major, int minor)
{
    majorTL_= major;
    minorTL_= minor;
    on_tick_change();
}

/*! Do the necessary initializations when window is realized.
 */
void Scale::on_realize()
{
    Gtk::DrawingArea::on_realize();
    window_= get_window();
#if 0
    if (labels_) {
	labwin_= labels_->get_window();
    }
#endif
    gc_ = Gdk::GC::create(window_);
    on_tick_change();
}	

/*! Set the range of double values the scale represents.
 *
 *  To make the scale logarithmic, true can be given as the third
 *  argument.  After setting the new range, the scale is redrawn.
 */ 
void Scale::set_range(double l, double r)
{
    set_range(l,r,scaleMap_.logarithmic());
}

/*! Set the range of double values the scale represents.
 *
 *  To make the scale logarithmic, true can be given as the third
 *  argument.  After setting the new range, the scale is redrawn.
 */ 
void Scale::set_range(double l, double r, bool lg)
{
    scaleMap_.set_dbl_range(l,r,lg);
    if (scaleMap_.logarithmic())
	scaleDiv_.rebuild(l, r, 10, 10, true, 0.0);
    else 
	scaleDiv_.rebuild(l, r, 10, 10, false, 0.0);
    
    if (window_) redraw(); // to avoid probs when called from constructor
}

/*! Turn on autoscaling 
 */
void Scale::set_autoscale(bool b)
{
    autoscale_= b;
}

/*! Reset autoscale information
 *
 *  Before updating autoscale information with autoscale(min,max),
 *  begin_autoscale must be called to reset the old autoscale
 *  information.  Call end_autoscale() when finished.
 */
void Scale::begin_autoscale()
{
    if (!autoscale_) return;
    asMin_= 1; asMax_= -1;
}

/*! Add autoscale information
 *
 *  The given minimum and maximum value extend the current autoscale
 *  information.  Call end_autoscale() when finished.
 */
void Scale::autoscale(double min, double max)
{
    if (!autoscale_) return;
    if (asMin_ > asMax_) {
	asMin_= MIN(min,max);
	asMax_= MAX(min,max);
    } else {
	asMin_= MIN(asMin_,MIN(min,max));
	asMax_= MAX(asMax_,MAX(min,max));
    }
}

/*! Finish update of autoscale information
 *
 *  After updating autoscale information with autoscale(min,max)
 *  this function sets the new range of values.
 *
 *  \sa set_range
 */
void Scale::end_autoscale()
{
    if (!autoscale_) return;
    if (asMin_ != asMax_) {
	set_range(asMin_,asMax_,scaleMap_.logarithmic());
    }
}

/*! Handles an expose event */
bool Scale::on_expose_event(GdkEventExpose* event)
{
    redraw();
}

/*! Constructor
 * 
 *  \see Scale::Scale()
 */
VScale::VScale(Gtk::PositionType p, ScaleLabels *l) : Scale(p,l)
{
}

/*! Destructor */
VScale::~VScale()
{
}

/*! Requests a size for the drawingarea that can hold the major tick */
void VScale::on_tick_change()
{
    if (enabled())
	set_size_request(major_ticklength(),-1);
    else
	set_size_request(1,-1);
}

/*! Draws the scale */
void VScale::redraw()
{
    if (!window_) return;
    int winx, winy, winw, winh, wind;
    window_->get_geometry(winx, winy, winw, winh, wind);
    window_->clear();
    scaleMap_.set_int_range(winh-1,0);
    int i, iy;
    double y;
    int l0= 0;
    int l1= minor_ticklength();
    int l2= major_ticklength();
    if (position()==Gtk::POS_LEFT) {
	l0= winw-1;
	l1= l0-l1;
	l2= l0-l2;
    }
    std::map<int,double> lblpos;
    for (i=0;i<scaleDiv_.maj_count();i++) {
	y= scaleDiv_.maj_mark(i);
	iy= scaleMap_.transform(y);
	if (labels_) lblpos[iy]= y;
	window_->draw_line(gc_,l0,iy,l2,iy);
    }
    for (i=0;i<scaleDiv_.min_count();i++) {
	y= scaleDiv_.min_mark(i);
	iy= scaleMap_.transform(y);
	window_->draw_line(gc_,l0,iy,l1,iy);
    }
    window_->draw_line(gc_,l0,0,l0,winh-1);
    if (labels_) labels_->set_labels(winy,lblpos);
}

/*! Constructor
 * 
 *  \see Scale::Scale()
 */
HScale::HScale(Gtk::PositionType p, ScaleLabels *l) : Scale(p,l)
{
}

/*! Destructor */
HScale::~HScale()
{
}

/*! Requests a size for the drawingarea that can hold the major tick */
void HScale::on_tick_change()
{
    if (enabled())
	set_size_request(-1,major_ticklength());
    else
	set_size_request(-1,1);    
}

/*! Draws the scale */
void HScale::redraw()
{
    if (!window_) return;
    int winx, winy, winw, winh, wind;
    window_->get_geometry(winx, winy, winw, winh, wind);
    window_->clear();
    scaleMap_.set_int_range(0,winw-1);
    int i, ix;
    double x;
    int l0= 0;
    int l1= minor_ticklength();
    int l2= major_ticklength();
    if (position()==Gtk::POS_TOP) {
	l0= winh-1;
	l1= l0-l1;
	l2= l0-l2;
    }

    std::map<int,double> lblpos;
    for (i=0;i<scaleDiv_.maj_count();i++) {
	x= scaleDiv_.maj_mark(i);
	ix= scaleMap_.transform(x);
	if (labels_) lblpos[ix]= x;
	window_->draw_line(gc_,ix,l0,ix,l2);
    }
    for (i=0;i<scaleDiv_.min_count();i++) {
	x= scaleDiv_.min_mark(i);
	ix= scaleMap_.transform(x);
	window_->draw_line(gc_,ix,l0,ix,l1);
    }
    window_->draw_line(gc_,0,l0,winw-1,l0);
    if (labels_) labels_->set_labels(winx,lblpos);
}

