# CMakeLists for src/modules/objects

# From qmake
#QT += xml

IF(WIN32)
	LIST(APPEND LIBS -lws2_32)
ENDIF()

SET(kviobjects_MOC_HDRS
	KvsObject_button.h
	KvsObject_checkBox.h
	KvsObject_comboBox.h
	KvsObject_dateTimeEdit.h
	KvsObject_dockWindow.h
	KvsObject_ftp.h
	KvsObject_http.h
	KvsObject_lineEdit.h
	KvsObject_listWidget.h
	KvsObject_multiLineEdit.h
	KvsObject_painter.h
	KvsObject_pixmap.h
	KvsObject_popupMenu.h
	KvsObject_process.h
	KvsObject_radioButton.h
	KvsObject_slider.h
	KvsObject_socket.h
	KvsObject_spinBox.h
	KvsObject_tabWidget.h
	KvsObject_tableWidget.h
	KvsObject_trayIcon.h
	KvsObject_textBrowser.h
	KvsObject_toolButton.h
	KvsObject_treeWidget.h
	KvsObject_webView.h
	KvsObject_widget.h
	KvsObject_window.h
	KvsObject_wizard.h
)

SET(kviobjects_SRCS
	libkviobjects.cpp
	KvsObject_button.cpp
	KvsObject_buttonGroup.cpp
	KvsObject_checkBox.cpp
	KvsObject_comboBox.cpp
	KvsObject_dateTimeEdit.cpp
	KvsObject_dialog.cpp
	KvsObject_dockWindow.cpp
	KvsObject_file.cpp
	KvsObject_ftp.cpp
	KvsObject_groupBox.cpp
	KvsObject_hBox.cpp
	KvsObject_http.cpp
	KvsObject_label.cpp
	KvsObject_layout.cpp
	KvsObject_lcd.cpp
	KvsObject_lineEdit.cpp
	KvsObject_list.cpp
	KvsObject_listWidget.cpp
	KvsObject_mainWindow.cpp
	KvsObject_memoryBuffer.cpp
	KvsObject_menuBar.cpp
	KvsObject_multiLineEdit.cpp
	KvsObject_painter.cpp
	KvsObject_pixmap.cpp
	KvsObject_popupMenu.cpp
	KvsObject_process.cpp
	KvsObject_progressBar.cpp
	KvsObject_radioButton.cpp
	KvsObject_slider.cpp
	KvsObject_socket.cpp
	KvsObject_spinBox.cpp
	KvsObject_sql.cpp
	KvsObject_tabWidget.cpp
	KvsObject_tableWidget.cpp
	KvsObject_trayIcon.cpp
	KvsObject_textBrowser.cpp
	KvsObject_toolBar.cpp
	KvsObject_toolButton.cpp
	KvsObject_treeWidget.cpp
	KvsObject_treeWidgeteItem.cpp
	KvsObject_vBox.cpp
	KvsObject_webView.cpp
	KvsObject_widget.cpp
	KvsObject_window.cpp
	KvsObject_wizard.cpp
	KvsObject_workspace.cpp
	KvsObject_wrapper.cpp
	KvsObject_xmlreader.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kviobjects_MOC_SRCS ${kviobjects_MOC_HDRS})

SET(kvi_module_name kviobjects)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
