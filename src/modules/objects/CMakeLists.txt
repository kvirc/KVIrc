# CMakeLists for src/modules/objects

set(
	kviobjects_SRCS
	libkviobjects.cpp
	KvsObject_button.cpp
	KvsObject_buttonGroup.cpp
	KvsObject_checkBox.cpp
	KvsObject_comboBox.cpp
	KvsObject_dateTimeEdit.cpp
	KvsObject_dialog.cpp
	KvsObject_colorDialog.cpp
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

# Qt >= 5.0.0 has no QHttp/QFtp implementation. We use the external code provided by digia.
set(
	kviobjects_SRCS
	${kviobjects_SRCS}
	qthttp/qhttp.cpp
	qthttp/qhttpauthenticator.cpp
	qtftp/qftp.cpp
	qtftp/qurlinfo.cpp
)

include_directories(qthttp/)
include_directories(qtftp/)

set(kvi_module_name kviobjects)

include(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
