#include <gdk/gdkkeysyms.h>
#include <gtkmm/accelgroup.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/label.h>
#include <gtkmm/box.h>

#include "sessiondialog.h"
#include <bcore/t_string.h>
#include <bcore/command/command.h>
#include <gtkmm/main.h>

namespace btg
{
   namespace UI
   {
      namespace gui
      {
         using namespace std;
         using namespace btg::core;

         sessionDialog::sessionDialog(t_longList const& _sessionIDs, 
                                      t_strList const& _sessionsNames)
            : quitstate(sessionDialog::QUIT),
              selected(false),
              session(Command::INVALID_SESSION),
              create_session(false),
              cbt(0),
              attachbutton(0)
         {
            Gtk::Button* quitbutton = Gtk::manage(new class Gtk::Button(Gtk::StockID("gtk-cancel")));
            Gtk::Button* newbutton    = Gtk::manage(new class Gtk::Button(Gtk::StockID("gtk-new")));
            attachbutton              = Gtk::manage(new class Gtk::Button(Gtk::StockID("gtk-ok")));
            Gtk::Label*  label        = Gtk::manage(new class Gtk::Label("Session: attach or create"));

            cbt                   = Gtk::manage(new class Gtk::ComboBoxText());
            Gtk::VBox *vbox       = Gtk::manage(new class Gtk::VBox(false, 0));

            quitbutton->set_flags(Gtk::CAN_FOCUS);
            quitbutton->set_relief(Gtk::RELIEF_NORMAL);
            quitbutton->set_label("Quit");

            newbutton->set_flags(Gtk::CAN_FOCUS);
            newbutton->set_relief(Gtk::RELIEF_NORMAL);
            newbutton->set_label("Create session");

            attachbutton->set_flags(Gtk::CAN_FOCUS);
            attachbutton->set_relief(Gtk::RELIEF_NORMAL);
            attachbutton->set_label("Attach");

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
            set_title("BTG sessions");

            set_modal(true);
            property_window_position().set_value(Gtk::WIN_POS_NONE);
            set_resizable(true);
            property_destroy_with_parent().set_value(false);
            set_has_separator(true);
            add_action_widget(*quitbutton, -6);
            add_action_widget(*newbutton, -4);
            add_action_widget(*attachbutton, -5);

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
            // cbt->set_active(0);
            quitbutton->show();

            // attachbutton->show();
            newbutton->show();
            label->show();
            cbt->show();
            vbox->show();
            show();

            // Connect buttons to handlers.
            attachbutton->signal_clicked().connect(sigc::mem_fun(*this, &sessionDialog::on_ok_clicked));
            quitbutton->signal_clicked().connect(sigc::mem_fun(*this, &sessionDialog::on_quit_clicked));
            newbutton->signal_clicked().connect(sigc::mem_fun(*this, &sessionDialog::on_create_session_clicked));

            newbutton->signal_clicked().connect(sigc::mem_fun(*this, &sessionDialog::on_create_session_clicked));

            cbt->signal_changed().connect(sigc::mem_fun(*this, &sessionDialog::on_combo_changed));
         }

         sessionDialog::QUIT_STATE sessionDialog::getResult()
         {
            return quitstate;
         }

         void sessionDialog::on_combo_changed()
         {
            string active = cbt->get_active_text();
            if (active != "")
               {
                  attachbutton->show();                  
               }
            else
               {
                  attachbutton->hide();
               }
         }

         void sessionDialog::on_ok_clicked()
         {
            string active = cbt->get_active_text();
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

            quitstate = sessionDialog::SELECTED;

            this->hide();
         }

         void sessionDialog::on_create_session_clicked()
         {
            this->create_session = false;

            quitstate = sessionDialog::NEW_SESSION;

            this->hide();
         }

         void sessionDialog::on_quit_clicked()
         {
            this->selected = false;

            quitstate = sessionDialog::QUIT;

            this->hide();
         }

         bool sessionDialog::getSelectedSession(t_long & _session)
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

         sessionDialog::~sessionDialog()
         {
         }

      } // namespace gui
   } // namespace UI
} // namespace btg
