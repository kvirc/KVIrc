QT += xml

LIBS += -lws2_32

PRECOMPILED_HEADER  = pch.h

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
     DEFINES += USING_PCH
     }

TARGET = kviobjects
 
HEADERS += class_button.h \
            class_buttongroup.h \
		class_checkbox.h \
		class_combobox.h \
		class_dialog.h \
		class_dockwindow.h \
		class_file.h \
		class_groupbox.h \
		class_label.h \
		class_layout.h \
		class_lcd.h \
		class_lineedit.h \
		class_list.h \
		class_listbox.h \
		class_listview.h \
		class_listviewitem.h \
		class_mainwindow.h \
		class_menubar.h \
		class_multilineedit.h \
		class_painter.h \
		class_pixmap.h \
		class_popupmenu.h \
		class_process.h \
		class_progressbar.h \
		class_radiobutton.h \
		class_slider.h \
		class_socket.h \
		class_spinbox.h \
		class_tabwidget.h \
		class_tbrowser.h \
		class_toolbar.h \
		class_toolbutton.h \
		class_urllabel.h \
		class_widget.h \
		class_window.h \
		class_wizard.h \
		class_workspace.h \
		class_wrapper.h \
		class_xmlreader.h \
       	class_vbox.h \
         	class_hbox.h \
		object_macros.h

 						
SOURCES += libkviobjects.cpp \
			    class_button.cpp \
                      class_buttongroup.cpp \
			    class_checkbox.cpp \
			    class_combobox.cpp \
			    class_dialog.cpp \
			    class_dockwindow.cpp \
			    class_file.cpp \
			    class_groupbox.cpp \
			   class_label.cpp \
			    class_layout.cpp \
			    class_lcd.cpp \
			    class_lineedit.cpp \
			    class_list.cpp \
			    class_listbox.cpp \
			    class_listview.cpp \
			    class_listviewitem.cpp \
			    class_mainwindow.cpp \
			    class_menubar.cpp \
			    class_multilineedit.cpp \
			    class_popupmenu.cpp \
			    class_process.cpp \
			    class_progressbar.cpp \
			    class_radiobutton.cpp \
			    class_slider.cpp \
			    class_socket.cpp \
			    class_spinbox.cpp \
			    class_tabwidget.cpp \
			    class_toolbar.cpp \
			    class_toolbutton.cpp \
			    class_painter.cpp \
			    class_pixmap.cpp \
			    class_tbrowser.cpp \
			    class_widget.cpp \
			    class_window.cpp \
			    class_wizard.cpp \
			    class_workspace.cpp \
			    class_wrapper.cpp \
			    class_urllabel.cpp \
			    class_xmlreader.cpp \
			    class_vbox.cpp \
                      class_hbox.cpp

 						
include(../module.pri)