/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Plot Widget Library
 * Copyright (C) 2004   Andy Thaller
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 *****************************************************************************/

#include <gdkmm/gc.h>
#include <gdkmm/color.h>

#include <plotmm/paint.h>

using namespace PlotMM;


Paint::Paint() :
    size_(1),
    sizeB_(-1),
    filled_(false)
{
}

Paint::Paint(const Paint &s)
{
    pGC_= s.pGC_;
    bGC_= s.bGC_;
    size_= s.size_;
    sizeB_= s.sizeB_;
    filled_= s.filled_;
}

Paint::~Paint()
{
}

bool Paint::operator!=(const Paint &) const
{
}

bool Paint::operator==(const Paint &) const
{
}

/*! Set the color of the pen
 *
 *  The pen is used to draw lines or outlines of objects
 */
void Paint::set_pen_color(const Gdk::Color &c)
{
    pColor_= c;
}

/*! Set the color of the brush
 *
 *  The pen is used to fill objects
 */
void Paint::set_brush_color(const Gdk::Color &c)
{
    bColor_= c;
    filled_= true;
}

//! Return Pen (only outline)
Glib::RefPtr<Gdk::GC> Paint::pen(const Glib::RefPtr<Gdk::Drawable> &w) const
{
    Glib::RefPtr<Gdk::GC> gc= Gdk::GC::create(w);
    gc->set_rgb_fg_color(pColor_);
    return gc;
}

//! Return Brush (no fill)
Glib::RefPtr<Gdk::GC> Paint::brush(const Glib::RefPtr<Gdk::Drawable> &w) const
{
    Glib::RefPtr<Gdk::GC> gc= Gdk::GC::create(w);
    gc->set_rgb_fg_color(bColor_);
    return gc;
}

