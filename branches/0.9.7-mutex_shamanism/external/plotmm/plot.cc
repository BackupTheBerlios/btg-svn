/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * PlotMM Widget Library
 * Copyright (C) 2004   Andy Thaller
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 *****************************************************************************/

#include <gdkmm/rectangle.h>

#include <pangomm/layout.h>

#include <plotmm/curve.h>
#include <plotmm/paint.h>
#include <plotmm/plot.h>

#include <iostream>

using namespace PlotMM;

PlotLabel::PlotLabel(Gtk::Widget *w,Gtk::Orientation o, 
		     const Glib::ustring &text) :
    widget_(w),
    orientation_(o), 
    tainted_(true),
    enabled_(false)
{
    layout_= create_pango_layout("");
    font_= w->get_style()->get_font();
    set_text(text);
}

void PlotLabel::on_realize()
{
  // We need to call the base on_realize()
  Gtk::DrawingArea::on_realize();

  // Now we can allocate any additional resources we need
  window_= get_window();
  gc_ = Gdk::GC::create(window_);
  buffer_= Gdk::Pixmap::create(window_,1,1,-1);
}

/*! Set the text for the label 
 *
 *  If the given text is an empty string the displayed label will
 *  show nothing but will still require some space.
 */
void PlotLabel::set_text(const Glib::ustring &str)
{
    if (str=="")
	layout_->set_text(" ");
    else
	layout_->set_text(str);
    layout_->set_font_description(font_);
    layout_->context_changed();
    update_();
}

/*! Query the label's text
 */
Glib::ustring PlotLabel::get_text()
{
    Glib::ustring str= layout_->get_text();
    return str==" " ? "" : str;
}

/*! Enable or disable displaying of the label's text
 *
 *  When disabled, the label will require no space
 */
void PlotLabel::set_enabled(bool b)
{
    if (b==enabled_) return;
    enabled_= b;
    update_();
}

bool PlotLabel::on_expose_event(GdkEventExpose* event)
{
    if (tainted_) update_();
    if (!enabled()) return true;
    int winx, winy, winw, winh, wind;
    window_->get_geometry(winx, winy, winw, winh, wind);

    int layw,layh;
    if (orientation_==Gtk::ORIENTATION_HORIZONTAL) {
	layout_->get_pixel_size(layw,layh);
    } else {
	layout_->get_pixel_size(layh,layw);
    }
    window_->draw_drawable(gc_, buffer_, 0, 0, (winw-layw)/2, (winh-layh)/2);
    // window_->draw_layout(gc_,(winw-layw)/2, 0,layout_);
    return true;
}

void PlotLabel::update_()
{
    tainted_= true;
    if (!window_) return;
    if (!buffer_) return;
    int layw,layh;
    layout_->get_pixel_size(layw,layh);

    int winx, winy, winw, winh, wind;
    window_->get_geometry(winx, winy, winw, winh, wind);

    Gtk::StateType state = Gtk::STATE_NORMAL;
    Gdk::Rectangle area(0, 0, layw, layh);
    Gdk::Color bg= widget_->get_style()->get_bg(state);
    Gdk::Color fg= widget_->get_style()->get_fg(state);
    
    buffer_= Gdk::Pixmap::create(window_,layw,layh,-1);
    buffer_->draw_layout(gc_,0, 0,layout_,fg,bg);
    
    if (orientation_==Gtk::ORIENTATION_HORIZONTAL) {
	set_size_request(-1,enabled()?layh:0);
    } else {
	set_size_request(enabled()?layh:0,-1);
	buffer_= rotate_left_(buffer_);
    }
    tainted_= false;
}

/*! Rotate the text label by 90 degree to the right
 */
Glib::RefPtr<Gdk::Pixmap> PlotLabel::rotate_right_(Glib::RefPtr<Gdk::Pixmap> p)
{
    int w,h;
    p->get_size(w,h);
    Glib::RefPtr<Gdk::Pixmap> o= Gdk::Pixmap::create(window_,h,w,-1);
    Glib::RefPtr<Gdk::Image> pi= p->get_image(0,0,w,h);
    Glib::RefPtr<Gdk::Image> oi= 
	Gdk::Image::create(Gdk::IMAGE_NORMAL,get_visual(),h,w);
    for (int x= 0; x<w; ++x)
	for (int y= 0; y<h; ++y)
	    oi->put_pixel(h-y-1,x,pi->get_pixel(x,y));
    o->draw_image(gc_, oi, 0, 0, 0, 0,-1,-1);
    return o;
}

/*! Rotate the text label by 90 degree to the left
 */
Glib::RefPtr<Gdk::Pixmap> PlotLabel::rotate_left_(Glib::RefPtr<Gdk::Pixmap> p)
{
    int w,h;
    p->get_size(w,h);
    Glib::RefPtr<Gdk::Pixmap> o= Gdk::Pixmap::create(window_,h,w,-1);
    Glib::RefPtr<Gdk::Image> pi= p->get_image(0,0,w,h);
    Glib::RefPtr<Gdk::Image> oi= 
	Gdk::Image::create(Gdk::IMAGE_NORMAL,get_visual(),h,w);
    for (int x= 0; x<w; ++x)
	for (int y= 0; y<h; ++y)
	    oi->put_pixel(y,w-x-1,pi->get_pixel(x,y));
    o->draw_image(gc_, oi, 0, 0, 0, 0,-1,-1);
    return o;
}



PlotCanvas::PlotCanvas(Gtk::Widget *w) : 
    window_(0),
    widget_(w),
    replotting_(false)
{
  Glib::RefPtr<Gdk::Colormap> colormap = get_default_colormap();
  black_ = Gdk::Color("black");
  white_ = Gdk::Color("white");
  grey_ = Gdk::Color("red");
  colormap->alloc_color(black_);
  colormap->alloc_color(white_);
  colormap->alloc_color(grey_);
  add_events(Gdk::EXPOSURE_MASK|
	     Gdk::BUTTON_PRESS_MASK|
	     Gdk::BUTTON_RELEASE_MASK|
	     Gdk::POINTER_MOTION_MASK|
	     Gdk::POINTER_MOTION_HINT_MASK|
	     Gdk::ENTER_NOTIFY_MASK|
	     Gdk::LEAVE_NOTIFY_MASK);
}


PlotCanvas::~PlotCanvas()
{
}

bool PlotCanvas::on_button_press_event(GdkEventButton* event)
{
    int x,y;
    get_pointer(x,y);
    signal_plot_mouse_press(x,y,event);
    return true;
}

bool PlotCanvas::on_button_release_event(GdkEventButton* event)
{
    int x,y;
    get_pointer(x,y);
    signal_plot_mouse_release(x,y,event);
    return true;
}

bool PlotCanvas::on_motion_notify_event (GdkEventMotion* event)
{
    int x,y;
    get_pointer(x,y);
    signal_plot_mouse_move(x,y,event);
    return true;
}

void PlotCanvas::on_realize()
{
  // We need to call the base on_realize()
  Gtk::DrawingArea::on_realize();

  // Now we can allocate any additional resources we need
  window_= get_window();

  gc_ = Gdk::GC::create(window_);
  
  //window_->set_background(grey_);
  window_->clear();
}

//! Clear the plot canvas (where the curves are drawn
void PlotCanvas::clear()
{
    window_->clear();
}

/*! Start replot of the plot canvas' contents
 *
 *  When the replot takes place outside on_expose_event this method
 *  must be called to avoid flickering
 *
 *  \sa end_replot()
 */
bool PlotCanvas::begin_replot()
{
    if (replotting_ || !window_) return false;
    //window_->freeze_updates(); 
    Gdk::Rectangle frame;
    window_->get_frame_extents(frame);
    frame.set_x(0); frame.set_y(0);
    window_->begin_paint_rect(frame);
    return replotting_= true;
}

/*! End replot of the plot canvas' contents
 *
 *  If begin_replot() was called, this method must be called after
 *  changing the plot canvas' contents
 */
void PlotCanvas::end_replot()
{
    if (!replotting_) return;
    window_->end_paint();
    //window_->thaw_updates(); 
    replotting_= false;
}

bool PlotCanvas::on_expose_event(GdkEventExpose*)
{
  return false;
}




Plot::Plot() :
    layout_(7,9),
    canvas_(this),
    title_(this,Gtk::ORIENTATION_HORIZONTAL),
    curveidx_(-1),
    draw_select_(false)

{
    axisLabel_.push_back(new PlotLabel(this,Gtk::ORIENTATION_HORIZONTAL));
    axisLabel_.push_back(new PlotLabel(this,Gtk::ORIENTATION_HORIZONTAL));
    axisLabel_.push_back(new PlotLabel(this,Gtk::ORIENTATION_VERTICAL));
    axisLabel_.push_back(new PlotLabel(this,Gtk::ORIENTATION_VERTICAL));
    tickLabel_.push_back(new HScaleLabels(Gtk::POS_TOP));
    tickLabel_.push_back(new HScaleLabels(Gtk::POS_BOTTOM));
    tickLabel_.push_back(new VScaleLabels(Gtk::POS_LEFT));
    tickLabel_.push_back(new VScaleLabels(Gtk::POS_RIGHT));
    tickMark_.push_back(new HScale(Gtk::POS_TOP,tickLabel_[0]));
    tickMark_.push_back(new HScale(Gtk::POS_BOTTOM,tickLabel_[1]));
    tickMark_.push_back(new VScale(Gtk::POS_LEFT,tickLabel_[2]));
    tickMark_.push_back(new VScale(Gtk::POS_RIGHT,tickLabel_[3]));
    layout_.attach(title_,        0,7,0,1,Gtk::FILL|Gtk::EXPAND,Gtk::SHRINK);
    layout_.attach(*axisLabel_[0],1,6,1,2,Gtk::FILL|Gtk::EXPAND,Gtk::SHRINK);
    layout_.attach(*axisLabel_[1],1,6,7,8,Gtk::FILL|Gtk::EXPAND,Gtk::SHRINK);
    layout_.attach(*axisLabel_[2],0,1,1,8,Gtk::SHRINK,Gtk::FILL|Gtk::EXPAND);
    layout_.attach(*axisLabel_[3],6,7,1,8,Gtk::SHRINK,Gtk::FILL|Gtk::EXPAND);
    layout_.attach(*tickLabel_[0],1,6,2,3,Gtk::FILL|Gtk::EXPAND,Gtk::SHRINK);
    layout_.attach(*tickLabel_[1],1,6,6,7,Gtk::FILL|Gtk::EXPAND,Gtk::SHRINK);
    layout_.attach(*tickLabel_[2],1,2,3,6,Gtk::SHRINK,Gtk::FILL|Gtk::EXPAND);
    layout_.attach(*tickLabel_[3],5,6,3,6,Gtk::SHRINK,Gtk::FILL|Gtk::EXPAND);
    layout_.attach(*tickMark_[0], 3,4,3,4,Gtk::FILL|Gtk::EXPAND,Gtk::SHRINK);
    layout_.attach(*tickMark_[1], 3,4,5,6,Gtk::FILL|Gtk::EXPAND,Gtk::SHRINK);
    layout_.attach(*tickMark_[2], 2,3,4,5,Gtk::SHRINK,Gtk::FILL|Gtk::EXPAND);
    layout_.attach(*tickMark_[3], 4,5,4,5,Gtk::SHRINK,Gtk::FILL|Gtk::EXPAND);
    layout_.attach(canvas_,3,4,4,5);
    add(layout_);
    
    int fontsize= axisLabel_[0]->font()->get_size();

    // std::cerr << "fontsize is "<<fontsize<<"\n";

    axisLabel_[0]->font()->set_size(int(fontsize*.9));
    axisLabel_[1]->font()->set_size(int(fontsize*.9));
    axisLabel_[2]->font()->set_size(int(fontsize*.9));
    axisLabel_[3]->font()->set_size(int(fontsize*.9));
    
    tickLabel_[0]->font()->set_size(int(fontsize*.8));
    tickLabel_[1]->font()->set_size(int(fontsize*.8));
    tickLabel_[2]->font()->set_size(int(fontsize*.8));
    tickLabel_[3]->font()->set_size(int(fontsize*.8));

#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 4)
    canvas_.signal_expose_event().
      connect(sigc::mem_fun(*this,&Plot::on_canvas_expose_event));
#else
    canvas_.signal_expose_event().
      connect(SigC::slot(*this,&Plot::on_canvas_expose_event));
#endif

    show_all();
}


Plot::~Plot()
{
}

/*! @brief Add a new curve to the plot
 *
 *  The plot will not be updated - call replot() manually after adding
 *  new curves.
 */
int Plot::add_curve(const Glib::RefPtr<Curve> &cv,
		   PlotAxisID xaxis,
		   PlotAxisID yaxis,bool enable)
{
    curveidx_++;
    plotDict_[curveidx_].curve= cv;
    plotDict_[curveidx_].xaxis= xaxis;
    plotDict_[curveidx_].yaxis= yaxis;
    return curveidx_;
}

bool Plot::on_canvas_expose_event(GdkEventExpose* event)
{
    replot();
}

/*! Replot all curves
 *
 *  After adding curves to the plot or changing the curves' data with
 *  Curve::setData() this method must be called to update the plot.
 *
 *  If autoscaling is activated for an axis used by any of the
 *  Curve::enabled() curves that axis will be scaled to a new range.
 */
void Plot::replot()
{
    if (!is_visible()) return;
    if (!canvas_.begin_replot()) return;
    canvas_.clear();
    //  std::cerr << "canvas was exposed!\n";
    std::map<int,CurveInfo>::iterator cv; 
    int axis;
    bool autoscale= false;
    for (axis=0; axis<4; ++axis) {
	if (tickMark_[axis]->autoscale()) {
	    tickMark_[axis]->begin_autoscale();
	    autoscale= true;
	}
    }
    if (autoscale) {
	for (cv= plotDict_.begin(); cv!=plotDict_.end(); ++cv) {
	    if (!cv->second.curve->enabled()) continue;
	    PlotAxisID xaxis= cv->second.xaxis;
	    PlotAxisID yaxis= cv->second.yaxis;
	    DoubleRect br= cv->second.curve->bounding_rect();
	    tickMark_[xaxis]->autoscale(br.get_x1(),br.get_x2());
	    tickMark_[yaxis]->autoscale(br.get_y1(),br.get_y2());
	}
	for (axis=0; axis<4; ++axis) {
	    if (tickMark_[axis]->autoscale()) {
		tickMark_[axis]->end_autoscale();
	    }
	}
    }
	
    for (cv= plotDict_.begin(); cv!=plotDict_.end(); ++cv) {
	if (!cv->second.curve->enabled()) continue;
	PlotAxisID xaxis= cv->second.xaxis;
	PlotAxisID yaxis= cv->second.yaxis;
	cv->second.curve->draw(canvas_.get_window(), 
			       tickMark_[xaxis]->scale_map(), 
			       tickMark_[yaxis]->scale_map());
    }
    if (draw_select_) draw_selection_();
    canvas_.end_replot();
}

//! Return the signal owned by plot canvas
SigC::Signal3<void,int,int,GdkEventButton*> Plot::signal_plot_mouse_press()
{
    return canvas_.signal_plot_mouse_press;
}

//! Return the signal owned by plot canvas
SigC::Signal3<void,int,int,GdkEventButton*> Plot::signal_plot_mouse_release()
{
    return canvas_.signal_plot_mouse_release;
}

//! Return the signal owned by plot canvas
SigC::Signal3<void,int,int,GdkEventMotion*> Plot::signal_plot_mouse_move()
{
    return canvas_.signal_plot_mouse_move;
}

/*! Set the selection to the given rectangle.  If selection is
 *  enabled, the old rectangle is erased and the new is drawn.  Note
 *  that replot() does not have to be called explicitly.  No other
 *  canvas contents but the rectangle are redrawn calling this method.
 */
void Plot::set_selection(const PlotMM::Rectangle &r)
{
    if (draw_select_) draw_selection_();
    select_= r;
    if (draw_select_) draw_selection_();
}

/*! Enable drawing of the selection rectangle.  The geometry can be
 *  changed using set_selection.  To actually draw the rectangle,
 *  replot() must be called.
 */
void Plot::enable_selection()
{
    if (draw_select_) return;
    draw_select_= true;
    set_selection(select_);
}

/*! Disable drawing of the selection rectangle.  To actually erase the
 *  rectangle, replot() must be called.
 */
void Plot::disable_selection()
{
    if (!draw_select_) return;
    draw_select_= false;
}

//! draws the current selection rectangle on the plot canvas.
void Plot::draw_selection_()
{
    Paint paint;
    Glib::RefPtr<Gdk::GC> gc= paint.pen(canvas_.get_window());
    gc->set_function(Gdk::EQUIV);
    canvas_.get_window()->draw_rectangle(gc, false,
					 select_.get_x_min(),
					 select_.get_y_min(),
					 select_.get_abs_width(),
					 select_.get_abs_height());
}

