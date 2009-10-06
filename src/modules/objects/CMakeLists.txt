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
	class_datetimeedit.h
	class_dockwindow.h
	class_ftp.h
	class_http.h
	class_lineedit.h
	class_listwidget.h
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
	class_tablewidget.h
	class_tbrowser.h
	class_toolbutton.h
	class_treewidget.h
	class_widget.h
	class_window.h
	class_wizard.h
)

SET(kviobjects_SRCS
	libkviobjects.cpp
	class_button.cpp
	class_buttongroup.cpp
	class_checkbox.cpp
	class_combobox.cpp
	class_datetimeedit.cpp
	class_dialog.cpp
	class_dockwindow.cpp
	class_file.cpp
	class_ftp.cpp
	class_groupbox.cpp
	class_hbox.cpp
	class_http.cpp
	class_label.cpp
	class_layout.cpp
	class_lcd.cpp
	class_lineedit.cpp
	class_list.cpp
	class_listwidget.cpp
	class_mainwindow.cpp
	class_memorybuffer.cpp
	class_menubar.cpp
	class_multilineedit.cpp
	class_painter.cpp
	class_pixmap.cpp
	class_popupmenu.cpp
	class_process.cpp
	class_progressbar.cpp
	class_radiobutton.cpp
	class_slider.cpp
	class_socket.cpp
	class_spinbox.cpp
	class_sql.cpp
	class_tabwidget.cpp
	class_tablewidget.cpp
	class_tbrowser.cpp
	class_toolbar.cpp
	class_toolbutton.cpp
	class_treewidget.cpp
	class_treewidgetitem.cpp
	class_vbox.cpp
	class_widget.cpp
	class_window.cpp
	class_wizard.cpp
	class_workspace.cpp
	class_wrapper.cpp
	class_xmlreader.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviobjects_MOC_SRCS ${kviobjects_MOC_HDRS})

SET(kvi_module_name kviobjects)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
