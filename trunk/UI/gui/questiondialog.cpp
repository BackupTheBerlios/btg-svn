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
            : status_(false)
         {
            class Gtk::Dialog* dialog     = this;
            class Gtk::Button* noButton   = Gtk::manage(new class Gtk::Button("No!"));
            class Gtk::Button* yesButton  = Gtk::manage(new class Gtk::Button("Yes"));
            class Gtk::Label*  quitLabel  = Gtk::manage(new class Gtk::Label(_question));
            class Gtk::Viewport* viewport = Gtk::manage(
                                                        new class Gtk::Viewport(*manage(
                                                                                        new Gtk::Adjustment(0,0,1)),
                                                                                *manage(new Gtk::Adjustment(0,0,1))
                                                                                )
                                                        );
            class Gtk::ScrolledWindow* scrolledwindow = Gtk::manage(new class Gtk::ScrolledWindow());

            noButton->set_flags(Gtk::CAN_FOCUS);
            noButton->set_flags(Gtk::CAN_DEFAULT);
            noButton->set_border_width(10);
            noButton->set_relief(Gtk::RELIEF_NORMAL);

            yesButton->set_flags(Gtk::CAN_FOCUS);
            yesButton->set_flags(Gtk::CAN_DEFAULT);
            yesButton->set_border_width(10);
            yesButton->set_relief(Gtk::RELIEF_NORMAL);

            dialog->get_action_area()->property_layout_style().set_value(Gtk::BUTTONBOX_SPREAD);
            quitLabel->set_alignment(0.5,0.5);
            quitLabel->set_padding(0,0);
            quitLabel->set_justify(Gtk::JUSTIFY_LEFT);
            quitLabel->set_line_wrap(false);
            quitLabel->set_use_markup(false);
            quitLabel->set_selectable(false);
            viewport->set_shadow_type(Gtk::SHADOW_IN);
            viewport->add(*quitLabel);
            scrolledwindow->set_flags(Gtk::CAN_FOCUS);
            scrolledwindow->set_border_width(15);
            scrolledwindow->set_shadow_type(Gtk::SHADOW_NONE);
            scrolledwindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
            scrolledwindow->property_window_placement().set_value(Gtk::CORNER_TOP_LEFT);
            scrolledwindow->add(*viewport);
            dialog->get_vbox()->set_homogeneous(false);
            dialog->get_vbox()->set_spacing(0);
            dialog->get_vbox()->pack_start(*scrolledwindow, Gtk::PACK_SHRINK, 0);
            dialog->set_title(_title);
            dialog->set_modal(true);
            dialog->property_window_position().set_value(Gtk::WIN_POS_CENTER);
            dialog->set_resizable(true);
            dialog->property_destroy_with_parent().set_value(false);
            dialog->set_has_separator(true);
            dialog->add_action_widget(*noButton, 0);
            dialog->add_action_widget(*yesButton, 0);
            noButton->show();
            yesButton->show();
            quitLabel->show();
            viewport->show();
            scrolledwindow->show();

            noButton->signal_clicked().connect(sigc::mem_fun(*this, &questionDialog::on_no_clicked));
            yesButton->signal_clicked().connect(sigc::mem_fun(*this, &questionDialog::on_yes_clicked));

            dialog->show();
         }

         void questionDialog::on_no_clicked()
         {
            response(0);
            status_ = false;
         }

         void questionDialog::on_yes_clicked()
         {
            response(0);
            status_ = true;
         }

         bool questionDialog::status() const
         {
            return status_;
         }

         bool questionDialog::showAndDie(std::string const& _title, std::string const& _question)
         {
            questionDialog* qd = new questionDialog(_title, _question);
            qd->run();
            qd->hide();
            bool result = qd->status();
            delete qd;
            qd = 0;

            return result;
         }

         questionDialog::~questionDialog()
         {

         }

      } // namespace gui
   } // namespace UI
} // namespace btg
