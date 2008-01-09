/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * PlotMM Widget Library
 * Copyright (C) 2004   Andy Thaller
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 *****************************************************************************/

#ifndef PLOTMM_PLOTSTYLE_H
#define PLOTMM_PLOTSTYLE_H

#include "compat.h"
#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION > 4)
#include <glibmm/objectbase.h>
#else
#include <sigc++/object.h>
#endif

#include <glibmm/refptr.h>
#include <gdkmm/color.h>

namespace Gdk {
    class GC;
    class Color;
    class Point;
    class Drawable;
    class Rectangle;
}

namespace PlotMM {

    //! @brief A class to define pens and brushes for painting
    class Paint : public PlotMM::ObjectBase
    {
    public:
	Paint();
	Paint(const Paint &c);
	virtual ~Paint();
    
	bool operator!=(const Paint &) const;
	bool operator==(const Paint &) const;

	void set_pen_color(const Gdk::Color &);
	void set_brush_color(const Gdk::Color &);

	Glib::RefPtr<Gdk::GC> pen(const Glib::RefPtr<Gdk::Drawable> &) const;
	Glib::RefPtr<Gdk::GC> brush(const Glib::RefPtr<Gdk::Drawable> &) const;
	bool filled() const { return filled_; }

    private:
	Gdk::Color pColor_, bColor_;
	Glib::RefPtr<Gdk::GC> pGC_, bGC_;
	int size_, sizeB_;
	bool filled_;
    };

}

#endif
