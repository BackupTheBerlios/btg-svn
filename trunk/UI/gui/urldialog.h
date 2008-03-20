#ifndef URLDIALOG_H
#define URLDIALOG_H

#include <gtkmm/dialog.h>

class urlDialog : public Gtk::Dialog
{  
 public:
   urlDialog();
   virtual ~urlDialog();

 protected:
   class Gtk::Label* label1;
   class Gtk::Entry* entry2;
   class Gtk::Entry* entry1;
   class Gtk::Label* label3;
   class Gtk::Label* label2;
   class Gtk::Table* table1;
   class Gtk::VBox* vbox1;
   class Gtk::Button* button1;
   class Gtk::Button * button2;
};

#endif // URLDIALOG_H

