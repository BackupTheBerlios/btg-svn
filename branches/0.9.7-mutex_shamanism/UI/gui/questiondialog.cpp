#include "questiondialog.h"

#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/label.h>
#include <gtkmm/viewport.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>

#include <gtkmmconfig.h>
#include <gdk/gdkkeysyms.h>
#include <gtkmm/accelgroup.h>
#include <gtkmm/adjustment.h>

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         questionDialog::questionDialog(std::string const& _title, std::string const& _question)
            : Gtk::MessageDialog(_question.c_str(), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO, true)
         {
            set_title(_title.c_str());
            status_ = run() == Gtk::RESPONSE_YES;
         }

         bool questionDialog::status() const
         {
            return status_;
         }

         bool questionDialog::showAndDie(std::string const& _title, std::string const& _question)
         {
            std::auto_ptr<questionDialog> qd(new questionDialog(_title, _question));
            return qd->status();
         }

         questionDialog::~questionDialog()
         {
         }

      } // namespace gui
   } // namespace UI
} // namespace btg
