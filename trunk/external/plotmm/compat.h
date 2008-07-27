#ifndef _compat_h_
#define _compat_h_

#include <gtkmmconfig.h>
#include <assert.h>

#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 4)
namespace PlotMM {
  class ObjectBase : public sigc::trackable
  {
  protected:
    ObjectBase() 
      : _counter(1)
#ifdef DEBUG
	,_id(serial++)
	,_magic(1234)
#endif
    {};

    ObjectBase(const ObjectBase &a) 
      : sigc::trackable(),
        _counter(1)
#ifdef DEBUG
	,_id(serial++)
	,_magic(1234)
#endif
    {};

    virtual ~ObjectBase() {
#ifdef DEBUG
      is_to_be_deleted();
      _magic=5678;
#endif
    };
  public:

    void reference() const {_counter++; 
#ifdef DEBUG
    is_valid();
#endif
    };

    void unreference() const {if (!--_counter) delete this; 
#ifdef DEBUG
    else is_valid();
#endif
    };

  private:
    mutable unsigned int _counter;
#ifdef DEBUG
    void is_valid()         const {assert (_counter>0); assert(_magic==1234);};
    void is_to_be_deleted() const {assert (_counter==0); assert(_magic==1234);};
    unsigned int _magic;
#endif
  };
};

#else

namespace PlotMM {
  typedef sigc::ObjectBase ObjectBase;
}

#endif
#endif
