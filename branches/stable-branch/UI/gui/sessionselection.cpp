#include <gdk/gdkkeysyms.h>
#include <gtkmm/accelgroup.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/label.h>
#include <gtkmm/box.h>

#include "sessionselection.h"
#include <bcore/t_string.h>
#include <bcore/command/command.h>
#include <gtkmm/main.h>

#include <string>

namespace btg
{
   namespace UI
   {
      namespace gui
      {
         using namespace btg::core;

         sessionSelectionDialog::sessionSelectionDialog(std::string const& _title,
                                                        t_longList const& _sessionIDs, 
                                                        t_strList const& _sessionsNames,
                                                        bool const _disableSelection)
            : Gtk::Dialog(_title, true /* modal */, true /* separator */),
              disableSelection_(_disableSelection),
              selected(false),
              session(Command::INVALID_SESSION),
              cbt(0)
         {
            Gtk::Button* cancelbutton = Gtk::manage(new class Gtk::Button(Gtk::StockID("gtk-cancel")));
            Gtk::Button* okbutton     = Gtk::manage(new class Gtk::Button(Gtk::StockID("gtk-ok")));
            Gtk::Label* label = Gtk::manage(new class Gtk::Label(_title));

            cbt                   = Gtk::manage(new class Gtk::ComboBoxText());
            Gtk::VBox* vbox       = Gtk::manage(new class Gtk::VBox(false, 0));

            cancelbutton->set_flags(Gtk::CAN_FOCUS);
            cancelbutton->set_relief(Gtk::RELIEF_NORMAL);
            okbutton->set_flags(Gtk::CAN_FOCUS);
            okbutton->set_relief(Gtk::RELIEF_NORMAL);
            get_action_area()->property_layout_style().set_value(Gtk::BUTTONBOX_END);
            label->set_alignment(0.5,0.5);
            label->set_padding(0,0);
            label->set_justify(Gtk::JUSTIFY_LEFT);
            label->set_line_wrap(false);
            label->set_use_markup(false);
            label->set_selectable(false);

            vbox->pack_start(*label, Gtk::PACK_SHRINK, 0);
            vbox->pack_start(*cbt);
            get_vbox()->set_homogeneous(false);
            get_vbox()->set_spacing(0);
            get_vbox()->pack_start(*vbox);

            property_window_position().set_value(Gtk::WIN_POS_NONE);
            set_resizable(true);
            property_destroy_with_parent().set_value(false);
            add_action_widget(*cancelbutton, -6);
            add_action_widget(*okbutton, -5);

            /// Fill the combobox with session ids.
            t_strListCI sessionNameIter = _sessionsNames.begin();
            for (t_longListCI sessionIter = _sessionIDs.begin();
                 sessionIter != _sessionIDs.end();
                 sessionIter++)
               {
                  std::string session_descr = convertToString<t_long>(*sessionIter);
                  session_descr += " (";
                  session_descr += *sessionNameIter;
                  session_descr += ")";

                  cbt->append_text(session_descr);
                  sessionNameIter++;
               }

            // Select the first session id.
            cbt->set_active(0);

            if (!disableSelection_)
               {
                  cancelbutton->show();
               }

            // Connect buttons to handlers.
            okbutton->signal_clicked().connect(sigc::mem_fun(*this, &sessionSelectionDialog::on_ok_clicked));
            if (!disableSelection_)
               {
                  cancelbutton->signal_clicked().connect(sigc::mem_fun(*this, &sessionSelectionDialog::on_cancel_clicked));
               }
            
            show_all(); // show all elements
            hide(); // hide toplevel window
         }

         void sessionSelectionDialog::on_ok_clicked()
         {
            std::string active = cbt->get_active_text();
            if (active != "")
               {
                  std::string::size_type pos = active.find_first_of("(");
                  if (pos != std::string::npos)
                     {
                        std::string session_descr = active.substr(0, pos);
                        this->session  = convertStringTo<t_long>(session_descr);
                        this->selected = true;
                     }
               }
            else
               {
                  this->selected = false;
               }

            this->hide();
         }

         void sessionSelectionDialog::on_cancel_clicked()
         {
            this->selected = false;
            this->hide();
         }

         bool sessionSelectionDialog::getSelectedSession(t_long & _session)
         {
            if (this->selected)
               {
                  _session = this->session;
               }
            else
               {
                  _session = Command::INVALID_SESSION;
               }

            return this->selected;
         }

         sessionSelectionDialog::~sessionSelectionDialog()
         {
         }

      } // namespace gui
   } // namespace UI
} // namespace btg
