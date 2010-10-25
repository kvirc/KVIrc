# CMakeLists.txt for src/kvirc/

INCLUDE_DIRECTORIES(
	kernel
	kvs
	module
	sparser
	ui
	../kvilib/tal
	../kvilib/config
	../kvilib/core
	../kvilib/ext
	../kvilib/file
	../kvilib/irc
	../kvilib/net
	../kvilib/system
)

SET(kvirc_MOC_HDRS
	ui/kvi_actiondrawer.h
	ui/kvi_channel.h
	ui/kvi_colorwin.h
	ui/kvi_console.h
	ui/kvi_cryptcontroller.h
	ui/kvi_ctcppagedialog.h
	ui/kvi_customtoolbar.h
	ui/kvi_debugwindow.h
	ui/kvi_dynamictooltip.h
	ui/kvi_filedialog.h
	ui/kvi_frame.h
	ui/kvi_historywin.h
	ui/kvi_htmldialog.h
	ui/kvi_imagedialog.h
	ui/kvi_input.h
	ui/kvi_input_editor.h
	ui/kvi_ipeditor.h
	ui/kvi_irctoolbar.h
	ui/kvi_ircview.h
	ui/kvi_ircviewtools.h
	ui/kvi_maskeditor.h
	ui/kvi_mdichild.h
	ui/kvi_mdimanager.h
	ui/kvi_menubar.h
	ui/kvi_modeeditor.h
	ui/kvi_modew.h
	ui/kvi_optionswidget.h
	ui/kvi_query.h
	ui/kvi_scriptbutton.h
	ui/kvi_scripteditor.h
	ui/kvi_selectors.h
	ui/kvi_splash.h
	ui/kvi_statusbar.h
	ui/kvi_statusbarapplet.h
	ui/kvi_texticonwin.h
	ui/kvi_themedcombobox.h
	ui/kvi_themedlabel.h
	ui/kvi_themedlineedit.h
	ui/kvi_toolbar.h
	ui/kvi_toolwindows_container.h
	ui/kvi_topicw.h
	ui/kvi_userlistview.h
	ui/kvi_window.h
	ui/kvi_windowlist.h
	ui/kvi_windowlist_tree.h
	sparser/kvi_sparser.h
	module/kvi_mexlinkfilter.h
	module/kvi_mexserverimport.h
	module/kvi_mextoolbar.h
	module/kvi_modulemanager.h
	kvs/kvi_kvs_action.h
	kvs/kvi_kvs_aliasmanager.h
	kvs/kvi_kvs_asyncdnsoperation.h
	kvs/kvi_kvs_asyncoperation.h
	kvs/kvi_kvs_dnsmanager.h
	kvs/kvi_kvs_object.h
	kvs/kvi_kvs_popupmenu.h
	kvs/kvi_kvs_processmanager.h
	kvs/kvi_kvs_timermanager.h
	kvs/kvi_kvs_useraction.h
	kernel/kvi_action.h
	kernel/kvi_actionmanager.h
	kernel/kvi_app.h
	kernel/kvi_coreactions.h
	kernel/kvi_filetransfer.h
	kernel/kvi_iconmanager.h
	kernel/kvi_ipc.h
	kernel/kvi_ircconnection.h
	kernel/kvi_ircconnectionrequestqueue.h
	kernel/kvi_ircconnectiontargetresolver.h
	kernel/kvi_irccontext.h
	kernel/kvi_irclink.h
	kernel/kvi_ircsocket.h
	kernel/kvi_lagmeter.h
	kernel/kvi_notifylist.h
	kernel/kvi_texticonmanager.h
)

SET(kvirc_SRCS
	ui/kvi_actiondrawer.cpp
	ui/kvi_channel.cpp
	ui/kvi_colorwin.cpp
	ui/kvi_console.cpp
	ui/kvi_cryptcontroller.cpp
	ui/kvi_ctcppagedialog.cpp
	ui/kvi_customtoolbar.cpp
	ui/kvi_debugwindow.cpp
	ui/kvi_dynamictooltip.cpp
	ui/kvi_filedialog.cpp
	ui/kvi_frame.cpp
	ui/kvi_historywin.cpp
	ui/kvi_htmldialog.cpp
	ui/kvi_imagedialog.cpp
	ui/kvi_input.cpp
	ui/kvi_input_editor.cpp
	ui/kvi_input_history.cpp
	ui/kvi_ipeditor.cpp
	ui/kvi_irctoolbar.cpp
	ui/kvi_ircview.cpp
	ui/kvi_ircview_events.cpp
	ui/kvi_ircview_gettextline.cpp
	ui/kvi_ircview_loghandling.cpp
	ui/kvi_ircviewtools.cpp
	ui/kvi_maskeditor.cpp
	ui/kvi_mdichild.cpp
	ui/kvi_mdimanager.cpp
	ui/kvi_menubar.cpp
	ui/kvi_modeeditor.cpp
	ui/kvi_modew.cpp
	ui/kvi_msgbox.cpp
	ui/kvi_optionswidget.cpp
	ui/kvi_query.cpp
	ui/kvi_scriptbutton.cpp
	ui/kvi_scripteditor.cpp
	ui/kvi_selectors.cpp
	ui/kvi_splash.cpp
	ui/kvi_statusbar.cpp
	ui/kvi_statusbarapplet.cpp
	ui/kvi_texticonwin.cpp
	ui/kvi_themedcombobox.cpp
	ui/kvi_themedlabel.cpp
	ui/kvi_themedlineedit.cpp
	ui/kvi_toolbar.cpp
	ui/kvi_toolwindows_container.cpp
	ui/kvi_topicw.cpp
	ui/kvi_userlistview.cpp
	ui/kvi_window.cpp
	ui/kvi_windowlist.cpp
	ui/kvi_windowlist_tree.cpp
	sparser/kvi_antispam.cpp
	sparser/kvi_ircmessage.cpp
	sparser/kvi_sp_ctcp.cpp
	sparser/kvi_sp_literal.cpp
	sparser/kvi_sp_numeric.cpp
	sparser/kvi_sp_tables.cpp
	sparser/kvi_sparser.cpp
	module/kvi_mexlinkfilter.cpp
	module/kvi_mexserverimport.cpp
	module/kvi_mextoolbar.cpp
	module/kvi_module.cpp
	module/kvi_moduleextension.cpp
	module/kvi_modulemanager.cpp
	kvs/kvi_kvs.cpp
	kvs/kvi_kvs_action.cpp
	kvs/kvi_kvs_aliasmanager.cpp
	kvs/kvi_kvs_array.cpp
	kvs/kvi_kvs_arraycast.cpp
	kvs/kvi_kvs_asyncdnsoperation.cpp
	kvs/kvi_kvs_asyncoperation.cpp
	kvs/kvi_kvs_callbackobject.cpp
	kvs/kvi_kvs_corecallbackcommands.cpp
	kvs/kvi_kvs_corefunctions.cpp
	kvs/kvi_kvs_corefunctions_af.cpp
	kvs/kvi_kvs_corefunctions_gl.cpp
	kvs/kvi_kvs_corefunctions_mr.cpp
	kvs/kvi_kvs_corefunctions_sz.cpp
	kvs/kvi_kvs_coresimplecommands.cpp
	kvs/kvi_kvs_coresimplecommands_af.cpp
	kvs/kvi_kvs_coresimplecommands_gl.cpp
	kvs/kvi_kvs_coresimplecommands_mr.cpp
	kvs/kvi_kvs_coresimplecommands_sz.cpp
	kvs/kvi_kvs_dnsmanager.cpp
	kvs/kvi_kvs_event.cpp
	kvs/kvi_kvs_eventhandler.cpp
	kvs/kvi_kvs_eventmanager.cpp
	kvs/kvi_kvs_eventtable.cpp
	kvs/kvi_kvs_hash.cpp
	kvs/kvi_kvs_kernel.cpp
	kvs/kvi_kvs_moduleinterface.cpp
	kvs/kvi_kvs_object.cpp
	kvs/kvi_kvs_object_class.cpp
	kvs/kvi_kvs_object_controller.cpp
	kvs/kvi_kvs_object_functioncall.cpp
	kvs/kvi_kvs_object_functionhandler.cpp
	kvs/kvi_kvs_object_functionhandlerimpl.cpp
	kvs/kvi_kvs_parameterprocessor.cpp
	kvs/kvi_kvs_parser.cpp
	kvs/kvi_kvs_parser_command.cpp
	kvs/kvi_kvs_parser_comment.cpp
	kvs/kvi_kvs_parser_dollar.cpp
	kvs/kvi_kvs_parser_expression.cpp
	kvs/kvi_kvs_parser_lside.cpp
	kvs/kvi_kvs_parser_specialcommands.cpp
	kvs/kvi_kvs_popupmanager.cpp
	kvs/kvi_kvs_popupmenu.cpp
	kvs/kvi_kvs_processmanager.cpp
	kvs/kvi_kvs_report.cpp
	kvs/kvi_kvs_runtimecall.cpp
	kvs/kvi_kvs_runtimecontext.cpp
	kvs/kvi_kvs_rwevaluationresult.cpp
	kvs/kvi_kvs_script.cpp
	kvs/kvi_kvs_scriptaddonmanager.cpp
	kvs/kvi_kvs_switchlist.cpp
	kvs/kvi_kvs_timermanager.cpp
	kvs/kvi_kvs_treenode_aliasfunctioncall.cpp
	kvs/kvi_kvs_treenode_aliassimplecommand.cpp
	kvs/kvi_kvs_treenode_arraycount.cpp
	kvs/kvi_kvs_treenode_arrayelement.cpp
	kvs/kvi_kvs_treenode_arrayorhashelement.cpp
	kvs/kvi_kvs_treenode_arrayreferenceassert.cpp
	kvs/kvi_kvs_treenode_base.cpp
	kvs/kvi_kvs_treenode_baseobjectfunctioncall.cpp
	kvs/kvi_kvs_treenode_callbackcommand.cpp
	kvs/kvi_kvs_treenode_command.cpp
	kvs/kvi_kvs_treenode_commandevaluation.cpp
	kvs/kvi_kvs_treenode_commandwithparameters.cpp
	kvs/kvi_kvs_treenode_compositedata.cpp
	kvs/kvi_kvs_treenode_constantdata.cpp
	kvs/kvi_kvs_treenode_corecallbackcommand.cpp
	kvs/kvi_kvs_treenode_corefunctioncall.cpp
	kvs/kvi_kvs_treenode_coresimplecommand.cpp
	kvs/kvi_kvs_treenode_data.cpp
	kvs/kvi_kvs_treenode_datalist.cpp
	kvs/kvi_kvs_treenode_expression.cpp
	kvs/kvi_kvs_treenode_expressionreturn.cpp
	kvs/kvi_kvs_treenode_extendedscopevariable.cpp
	kvs/kvi_kvs_treenode_functioncall.cpp
	kvs/kvi_kvs_treenode_globalvariable.cpp
	kvs/kvi_kvs_treenode_hashcount.cpp
	kvs/kvi_kvs_treenode_hashelement.cpp
	kvs/kvi_kvs_treenode_hashreferenceassert.cpp
	kvs/kvi_kvs_treenode_indirectdata.cpp
	kvs/kvi_kvs_treenode_instruction.cpp
	kvs/kvi_kvs_treenode_instructionblock.cpp
	kvs/kvi_kvs_treenode_localvariable.cpp
	kvs/kvi_kvs_treenode_modulecallbackcommand.cpp
	kvs/kvi_kvs_treenode_modulefunctioncall.cpp
	kvs/kvi_kvs_treenode_modulesimplecommand.cpp
	kvs/kvi_kvs_treenode_multipleparameteridentifier.cpp
	kvs/kvi_kvs_treenode_objectfield.cpp
	kvs/kvi_kvs_treenode_objectfunctioncall.cpp
	kvs/kvi_kvs_treenode_operation.cpp
	kvs/kvi_kvs_treenode_parameterreturn.cpp
	kvs/kvi_kvs_treenode_rebindingswitch.cpp
	kvs/kvi_kvs_treenode_scopeoperator.cpp
	kvs/kvi_kvs_treenode_simplecommand.cpp
	kvs/kvi_kvs_treenode_singleparameteridentifier.cpp
	kvs/kvi_kvs_treenode_specialcommand.cpp
	kvs/kvi_kvs_treenode_specialcommandbreak.cpp
	kvs/kvi_kvs_treenode_specialcommandclass.cpp
	kvs/kvi_kvs_treenode_specialcommandcontinue.cpp
	kvs/kvi_kvs_treenode_specialcommanddefpopup.cpp
	kvs/kvi_kvs_treenode_specialcommanddo.cpp
	kvs/kvi_kvs_treenode_specialcommandfor.cpp
	kvs/kvi_kvs_treenode_specialcommandforeach.cpp
	kvs/kvi_kvs_treenode_specialcommandif.cpp
	kvs/kvi_kvs_treenode_specialcommandswitch.cpp
	kvs/kvi_kvs_treenode_specialcommandunset.cpp
	kvs/kvi_kvs_treenode_specialcommandwhile.cpp
	kvs/kvi_kvs_treenode_stringcast.cpp
	kvs/kvi_kvs_treenode_switchlist.cpp
	kvs/kvi_kvs_treenode_thisobjectfunctioncall.cpp
	kvs/kvi_kvs_treenode_variable.cpp
	kvs/kvi_kvs_treenode_voidfunctioncall.cpp
	kvs/kvi_kvs_useraction.cpp
	kvs/kvi_kvs_variant.cpp
	kvs/kvi_kvs_variantlist.cpp
	kernel/kvi_action.cpp
	kernel/kvi_actionmanager.cpp
	kernel/kvi_app.cpp
	kernel/kvi_app_fs.cpp
	kernel/kvi_app_setup.cpp
	kernel/kvi_asynchronousconnectiondata.cpp
	kernel/kvi_coreactions.cpp
	kernel/kvi_customtoolbardescriptor.cpp
	kernel/kvi_customtoolbarmanager.cpp
	kernel/kvi_filetransfer.cpp
	kernel/kvi_htmlgenerator.cpp
	kernel/kvi_iconmanager.cpp
	kernel/kvi_internalcmd.cpp
	kernel/kvi_ipc.cpp
	kernel/kvi_ircconnection.cpp
	kernel/kvi_ircconnectionantictcpflooddata.cpp
	kernel/kvi_ircconnectionasyncwhoisdata.cpp
	kernel/kvi_ircconnectionnetsplitdetectordata.cpp
	kernel/kvi_ircconnectionrequestqueue.cpp
	kernel/kvi_ircconnectionserverinfo.cpp
	kernel/kvi_ircconnectionstatedata.cpp
	kernel/kvi_ircconnectionstatistics.cpp
	kernel/kvi_ircconnectiontarget.cpp
	kernel/kvi_ircconnectiontargetresolver.cpp
	kernel/kvi_ircconnectionuserinfo.cpp
	kernel/kvi_irccontext.cpp
	kernel/kvi_ircdatastreammonitor.cpp
	kernel/kvi_irclink.cpp
	kernel/kvi_ircsocket.cpp
	kernel/kvi_ircurl.cpp
	kernel/kvi_lagmeter.cpp
	kernel/kvi_main.cpp
	kernel/kvi_notifylist.cpp
	kernel/kvi_options.cpp
	kernel/kvi_sslmaster.cpp
	kernel/kvi_texticonmanager.cpp
	kernel/kvi_theme.cpp
	kernel/kvi_useraction.cpp
	kernel/kvi_userinput.cpp
)

# After this call, files will be moc'ed to moc_kvi_*.cpp
QT4_WRAP_CPP(kvirc_MOC_SRCS ${kvirc_MOC_HDRS})

IF(MINGW)
	# resource compilation for mingw
	ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/win32_resource.o
		COMMAND windres.exe -I${CMAKE_SOURCE_DIR}/data/resources_win32/ -i${CMAKE_SOURCE_DIR}/data/resources_win32/kvirc.rc -o${CMAKE_CURRENT_BINARY_DIR}/win32_resource.o
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/data/resources_win32
	)
	LIST(APPEND kvirc_SRCS ${CMAKE_CURRENT_BINARY_DIR}/win32_resource.o)
ENDIF()

IF(MSVC)
	LIST(APPEND LIBS shlwapi.lib)
ENDIF()

# Link libraries
ADD_EXECUTABLE(${KVIRC_BINARYNAME} ${kvirc_SRCS} ${kvirc_MOC_SRCS})
TARGET_LINK_LIBRARIES(${KVIRC_BINARYNAME} ${KVILIB_BINARYNAME} ${LIBS})
SET_TARGET_PROPERTIES(${KVIRC_BINARYNAME} PROPERTIES LINK_FLAGS "${ADDITIONAL_LINK_FLAGS}")

IF(CMAKE_HOST_SYSTEM MATCHES "FreeBSD")
	SET_TARGET_PROPERTIES(${KVIRC_BINARYNAME} PROPERTIES LINK_FLAGS -export-dynamic)
	MESSAGE(STATUS "Patching for FreeBSD...")
ENDIF()

IF(WIN32)
	# Needed for linking
	SET_TARGET_PROPERTIES(${KVIRC_BINARYNAME} PROPERTIES ENABLE_EXPORTS ON)

	# We need this defined when mingw will compile moc files
	ADD_DEFINITIONS(-D__KVIRC__)
	ADD_DEFINITIONS(-DCOMPILE_USE_STANDALONE_MOC_SOURCES)
ENDIF()

IF(WANT_STRIP)
	GET_TARGET_PROPERTY(KVIRC_LOCATION ${KVIRC_BINARYNAME} LOCATION)
	INSTALL(CODE "EXEC_PROGRAM(${STRIP_EXECUTABLE} ARGS -s \"${KVIRC_LOCATION}\")")
ENDIF()

# Installation directives
IF(UNIX)
	IF(APPLE)
		INSTALL(TARGETS ${KVIRC_BINARYNAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/Contents/MacOS)
	ELSE()
		# Assume linux
		INSTALL(TARGETS ${KVIRC_BINARYNAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	ENDIF()
ELSEIF(WIN32)
	INSTALL(TARGETS ${KVIRC_BINARYNAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
ENDIF()
