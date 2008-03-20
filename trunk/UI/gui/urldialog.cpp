#include "urldialog.h"

#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/table.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>

urlDialog::urlDialog()
{  
   label1 = Gtk::manage(new class Gtk::Label("Enter URL and filename"));
   entry2 = Gtk::manage(new class Gtk::Entry());
   entry1 = Gtk::manage(new class Gtk::Entry());
   label3 = Gtk::manage(new class Gtk::Label("Filename"));
   label2 = Gtk::manage(new class Gtk::Label("URL"));
   table1 = Gtk::manage(new class Gtk::Table(2, 2, true));
   vbox1 = Gtk::manage(new class Gtk::VBox(false, 0));
   button1 = Gtk::manage(new class Gtk::Button(Gtk::StockID("gtk-cancel")));
   button2 = Gtk::manage(new class Gtk::Button(Gtk::StockID("gtk-apply")));
   label1->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
   entry2->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
   entry2->set_flags(Gtk::CAN_FOCUS);
   entry2->set_max_length(512);
   entry1->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
   entry1->set_flags(Gtk::CAN_FOCUS);
   entry1->set_max_length(512);
   label3->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
   label2->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
   table1->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
   table1->attach(*entry2, 1, 2, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::EXPAND|Gtk::FILL, 0, 0);
   table1->attach(*entry1, 1, 2, -1, -1, Gtk::EXPAND|Gtk::FILL, Gtk::EXPAND|Gtk::FILL, 0, 0);
   table1->attach(*label3, -1, -1, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::EXPAND|Gtk::FILL, 0, 0);
   table1->attach(*label2, -1, -1, -1, -1, Gtk::EXPAND|Gtk::FILL, Gtk::EXPAND|Gtk::FILL, 0, 0);
   vbox1->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
   vbox1->pack_start(*label1);
   vbox1->pack_start(*table1, Gtk::PACK_EXPAND_PADDING, 0);
   button1->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
   button1->set_flags(Gtk::CAN_FOCUS);
   button2->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
   button2->set_flags(Gtk::CAN_FOCUS);
   get_action_area()->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
   get_action_area()->property_layout_style().set_value(Gtk::BUTTONBOX_CENTER);
   get_vbox()->set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
   get_vbox()->set_spacing(2);
   get_vbox()->pack_start(*vbox1);
   set_events(Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
   set_border_width(5);
   property_window_position().set_value(Gtk::WIN_POS_CENTER_ON_PARENT);
   set_has_separator(false);
   add_action_widget(*button1, 0);
   add_action_widget(*button2, 0);
   label1->show();
   entry2->show();
   entry1->show();
   label3->show();
   label2->show();
   table1->show();
   vbox1->show();
   button1->show();
   button2->show();
}

urlDialog::~urlDialog()
{

}
