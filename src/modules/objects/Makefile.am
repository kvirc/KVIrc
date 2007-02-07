###############################################################################
# KVirc IRC client Makefile - 10.03.2000 Szymon Stefanek <stefanek@tin.it>
###############################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

pluglib_LTLIBRARIES      = libkviobjects.la

libkviobjects_la_LDFLAGS = -module -avoid-version $(SS_LDFLAGS) $(SS_LIBDIRS)

libkviobjects_la_SOURCES =  libkviobjects.cpp \
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


libkviobjects_la_LIBADD  = $(SS_LIBLINK) ../../kvilib/build/libkvilib.la

noinst_HEADERS=	class_button.h \
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

m_%.moc: %.h
	$(SS_QT_MOC) $< -o $@

class_checkbox.cpp: m_class_checkbox.moc
class_combobox.cpp: m_class_combobox.moc
class_button.cpp: m_class_button.moc
class_dialog.cpp: m_class_dialog.moc
class_dockwindow.cpp: m_class_dockwindow.moc
class_file.cpp: m_class_file.moc
class_groupbox.cpp: m_class_groupbox.moc
class_label.cpp: m_class_label.moc
class_layout.cpp: m_class_layout.moc
class_lcd.cpp: m_class_lcd.moc
class_lineedit.cpp: m_class_lineedit.moc
class_list.cpp: m_class_list.moc
class_listbox.cpp: m_class_listbox.moc
class_listview.cpp: m_class_listview.moc
class_listviewitem.cpp: m_class_listviewitem.moc
class_listbox.cpp: m_class_listbox.moc
class_mainwindow.cpp: m_class_mainwindow.moc
class_menubar.cpp: m_class_menubar.moc
class_multilineedit.cpp: m_class_multilineedit.moc
class_painter.cpp: m_class_painter.moc
class_pixmap.cpp: m_class_pixmap.moc
class_popupmenu.cpp: m_class_popupmenu.moc
class_process.cpp: m_class_process.moc
class_progressbar.cpp: m_class_progressbar.moc
class_radiobutton.cpp: m_class_radiobutton.moc
class_slider.cpp: m_class_slider.moc
class_socket.cpp: m_class_socket.moc
class_spinbox.cpp: m_class_spinbox.moc
class_tabwidget.cpp: m_class_tabwidget.moc
class_tbrowser.cpp: m_class_tbrowser.moc
class_toolbar.cpp: m_class_toolbar.moc
class_toolbutton.cpp: m_class_toolbutton.moc
class_urllabel.cpp: m_class_urllabel.moc
class_widget.cpp: m_class_widget.moc
class_window.cpp: m_class_window.moc
class_wizard.cpp: m_class_wizard.moc
class_workspace.cpp: m_class_workspace.moc
class_wrapper.cpp: m_class_wrapper.moc
class_xmlreader.cpp: m_class_xmlreader.moc

