# CMakeLists for src/modules/objects

# From qmake
#QT += xml

IF(WIN32)
	LIST(APPEND LIBS -lws2_32)
ENDIF()

SET(kviobjects_MOC_HDRS
	class_button.h
	class_checkbox.h
	class_combobox.h
	class_dockwindow.h
	class_lineedit.h
	class_listbox.h
	class_listview.h
	class_listviewitem.h
	class_multilineedit.h
	class_painter.h
	class_pixmap.h
	class_popupmenu.h
	class_process.h
	class_radiobutton.h
	class_slider.h
	class_socket.h
	class_spinbox.h
	class_tabwidget.h
	class_tbrowser.h
	class_toolbar.h
	class_toolbutton.h
	class_widget.h
	class_window.h
	class_wizard.h
	class_wrapper.h
	class_xmlreader.h
)

SET(kviobjects_SRCS
	libkviobjects.cpp
	class_button.cpp
	class_buttongroup.cpp
	class_checkbox.cpp
	class_combobox.cpp
	class_dialog.cpp
	class_dockwindow.cpp
	class_file.cpp
	class_groupbox.cpp
	class_label.cpp
	class_layout.cpp
	class_lcd.cpp
	class_lineedit.cpp
	class_list.cpp
	class_listbox.cpp
	class_listview.cpp
	class_listviewitem.cpp
	class_mainwindow.cpp
	class_menubar.cpp
	class_multilineedit.cpp
	class_popupmenu.cpp
	class_process.cpp
	class_progressbar.cpp
	class_radiobutton.cpp
	class_slider.cpp
	class_socket.cpp
	class_spinbox.cpp
	class_tabwidget.cpp
	class_toolbar.cpp
	class_toolbutton.cpp
	class_painter.cpp
	class_pixmap.cpp
	class_tbrowser.cpp
	class_widget.cpp
	class_window.cpp
	class_wizard.cpp
	class_workspace.cpp
	class_wrapper.cpp
	class_urllabel.cpp
	class_xmlreader.cpp
	class_vbox.cpp
	class_hbox.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviobjects_MOC_SRCS ${kviobjects_MOC_HDRS})

SET(kvi_module_name kviobjects)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
