########################################################################################################################
#
# KVIrc executable Makefile
# Szymon Stefanek 21.09.2001
#
########################################################################################################################

AM_CPPFLAGS = -I$(SS_TOPSRCDIR)/src/kvilib/include/ -I$(SS_TOPSRCDIR)/src/kvirc/include/ \
$(SS_INCDIRS) $(SS_CPPFLAGS) -DGLOBAL_KVIRC_DIR=\"$(globalkvircdir)\"

kvirc4_LDFLAGS = -export-dynamic $(SS_LDFLAGS) $(SS_LIBDIRS) $(SS_RPATH)
kvirc4_LDADD = $(SS_LIBLINK) ../../kvilib/build/libkvirclib.la

bin_PROGRAMS = kvirc4

#
# Command useful to find out all the sources
# SRC=`find ../ -name *.cpp | sort`
# echo $SRC
#

nodist_kvirc4_SOURCES = ../kernel/moc_kvi_action.cpp \
../kernel/moc_kvi_actionmanager.cpp \
../kernel/moc_kvi_coreactions.cpp \
../kernel/moc_kvi_filetransfer.cpp \
../kernel/moc_kvi_ircconnection.cpp \
../kernel/moc_kvi_irccontext.cpp \
../kernel/moc_kvi_irclink.cpp \
../kernel/moc_kvi_ircsocket.cpp \
../kernel/moc_kvi_ircconnectiontargetresolver.cpp \
../kvs/moc_kvi_kvs_action.cpp \
../kvs/moc_kvi_kvs_asyncdnsoperation.cpp \
../kvs/moc_kvi_kvs_asyncoperation.cpp \
../kvs/moc_kvi_kvs_dnsmanager.cpp \
../kvs/moc_kvi_kvs_timermanager.cpp \
../kvs/moc_kvi_kvs_object.cpp \
../kvs/moc_kvi_kvs_popupmenu.cpp \
../kvs/moc_kvi_kvs_processmanager.cpp \
../kvs/moc_kvi_kvs_useraction.cpp \
../module/moc_kvi_mexlinkfilter.cpp \
../module/moc_kvi_mexserverimport.cpp \
../module/moc_kvi_mextoolbar.cpp \
../module/moc_kvi_modulemanager.cpp \
../ui/moc_kvi_actiondrawer.cpp \
../ui/moc_kvi_customtoolbar.cpp \
../ui/moc_kvi_debugwindow.cpp \
../ui/moc_kvi_htmldialog.cpp \
../ui/moc_kvi_imagedialog.cpp \
../ui/moc_kvi_ircviewtools.cpp \
../ui/moc_kvi_mdicaption.cpp \
../ui/moc_kvi_mdichild.cpp \
../ui/moc_kvi_mdimanager.cpp \
../ui/moc_kvi_statusbar.cpp \
../ui/moc_kvi_statusbarapplet.cpp \
../ui/moc_kvi_toolwindows_container.cpp \
../ui/moc_kvi_listview.cpp

kvirc4_SOURCES = ../kernel/kvi_action.cpp \
../kernel/kvi_actionmanager.cpp \
../kernel/kvi_app.cpp \
../kernel/kvi_coreactions.cpp \
../kernel/kvi_filetransfer.cpp \
../kernel/kvi_app_fs.cpp \
../kernel/kvi_app_setup.cpp \
../kernel/kvi_asynchronousconnectiondata.cpp \
../kernel/kvi_customtoolbardescriptor.cpp \
../kernel/kvi_customtoolbarmanager.cpp \
../kernel/kvi_iconmanager.cpp \
../kernel/kvi_internalcmd.cpp \
../kernel/kvi_ipc.cpp \
../kernel/kvi_ircdatastreammonitor.cpp \
../kernel/kvi_ircconnection.cpp \
../kernel/kvi_ircconnectionantictcpflooddata.cpp \
../kernel/kvi_ircconnectionasyncwhoisdata.cpp \
../kernel/kvi_ircconnectionnetsplitdetectordata.cpp \
../kernel/kvi_ircconnectiontarget.cpp \
../kernel/kvi_ircconnectiontargetresolver.cpp \
../kernel/kvi_ircconnectionserverinfo.cpp \
../kernel/kvi_ircconnectionstatedata.cpp \
../kernel/kvi_ircconnectionstatistics.cpp \
../kernel/kvi_ircconnectionuserinfo.cpp \
../kernel/kvi_irccontext.cpp \
../kernel/kvi_irclink.cpp \
../kernel/kvi_ircsocket.cpp \
../kernel/kvi_ircurl.cpp \
../kernel/kvi_main.cpp \
../kernel/kvi_lagmeter.cpp \
../kernel/kvi_notifylist.cpp \
../kernel/kvi_options.cpp \
../kernel/kvi_sslmaster.cpp \
../kernel/kvi_texticonmanager.cpp \
../kernel/kvi_theme.cpp \
../kernel/kvi_useraction.cpp \
../kernel/kvi_userinput.cpp \
../kvs/kvi_kvs.cpp \
../kvs/kvi_kvs_action.cpp \
../kvs/kvi_kvs_aliasmanager.cpp \
../kvs/kvi_kvs_array.cpp \
../kvs/kvi_kvs_arraycast.cpp \
../kvs/kvi_kvs_asyncdnsoperation.cpp \
../kvs/kvi_kvs_asyncoperation.cpp \
../kvs/kvi_kvs_callbackobject.cpp \
../kvs/kvi_kvs_corecallbackcommands.cpp \
../kvs/kvi_kvs_corefunctions.cpp \
../kvs/kvi_kvs_corefunctions_af.cpp \
../kvs/kvi_kvs_corefunctions_gl.cpp \
../kvs/kvi_kvs_corefunctions_mr.cpp \
../kvs/kvi_kvs_corefunctions_sz.cpp \
../kvs/kvi_kvs_coresimplecommands.cpp \
../kvs/kvi_kvs_coresimplecommands_af.cpp \
../kvs/kvi_kvs_coresimplecommands_gl.cpp \
../kvs/kvi_kvs_coresimplecommands_mr.cpp \
../kvs/kvi_kvs_coresimplecommands_sz.cpp \
../kvs/kvi_kvs_dnsmanager.cpp \
../kvs/kvi_kvs_event.cpp \
../kvs/kvi_kvs_eventhandler.cpp \
../kvs/kvi_kvs_eventmanager.cpp \
../kvs/kvi_kvs_eventtable.cpp \
../kvs/kvi_kvs_hash.cpp \
../kvs/kvi_kvs_kernel.cpp \
../kvs/kvi_kvs_moduleinterface.cpp \
../kvs/kvi_kvs_object.cpp \
../kvs/kvi_kvs_object_class.cpp \
../kvs/kvi_kvs_object_controller.cpp \
../kvs/kvi_kvs_object_functioncall.cpp \
../kvs/kvi_kvs_object_functionhandler.cpp \
../kvs/kvi_kvs_object_functionhandlerimpl.cpp \
../kvs/kvi_kvs_parameterprocessor.cpp \
../kvs/kvi_kvs_parser.cpp \
../kvs/kvi_kvs_parser_command.cpp \
../kvs/kvi_kvs_parser_comment.cpp \
../kvs/kvi_kvs_parser_dollar.cpp \
../kvs/kvi_kvs_parser_expression.cpp \
../kvs/kvi_kvs_parser_lside.cpp \
../kvs/kvi_kvs_parser_specialcommands.cpp \
../kvs/kvi_kvs_popupmanager.cpp \
../kvs/kvi_kvs_popupmenu.cpp \
../kvs/kvi_kvs_processmanager.cpp \
../kvs/kvi_kvs_report.cpp \
../kvs/kvi_kvs_runtimecall.cpp \
../kvs/kvi_kvs_runtimecontext.cpp \
../kvs/kvi_kvs_rwevaluationresult.cpp \
../kvs/kvi_kvs_script.cpp \
../kvs/kvi_kvs_scriptaddonmanager.cpp \
../kvs/kvi_kvs_switchlist.cpp \
../kvs/kvi_kvs_timermanager.cpp \
../kvs/kvi_kvs_treenode_aliasfunctioncall.cpp \
../kvs/kvi_kvs_treenode_aliassimplecommand.cpp \
../kvs/kvi_kvs_treenode_arraycount.cpp \
../kvs/kvi_kvs_treenode_arrayelement.cpp \
../kvs/kvi_kvs_treenode_arrayorhashelement.cpp \
../kvs/kvi_kvs_treenode_arrayreferenceassert.cpp \
../kvs/kvi_kvs_treenode_base.cpp \
../kvs/kvi_kvs_treenode_baseobjectfunctioncall.cpp \
../kvs/kvi_kvs_treenode_callbackcommand.cpp \
../kvs/kvi_kvs_treenode_command.cpp \
../kvs/kvi_kvs_treenode_commandevaluation.cpp \
../kvs/kvi_kvs_treenode_commandwithparameters.cpp \
../kvs/kvi_kvs_treenode_compositedata.cpp \
../kvs/kvi_kvs_treenode_constantdata.cpp \
../kvs/kvi_kvs_treenode_corecallbackcommand.cpp \
../kvs/kvi_kvs_treenode_corefunctioncall.cpp \
../kvs/kvi_kvs_treenode_coresimplecommand.cpp \
../kvs/kvi_kvs_treenode_data.cpp \
../kvs/kvi_kvs_treenode_datalist.cpp \
../kvs/kvi_kvs_treenode_expression.cpp \
../kvs/kvi_kvs_treenode_expressionreturn.cpp \
../kvs/kvi_kvs_treenode_extendedscopevariable.cpp \
../kvs/kvi_kvs_treenode_functioncall.cpp \
../kvs/kvi_kvs_treenode_globalvariable.cpp \
../kvs/kvi_kvs_treenode_hashcount.cpp \
../kvs/kvi_kvs_treenode_hashelement.cpp \
../kvs/kvi_kvs_treenode_hashreferenceassert.cpp \
../kvs/kvi_kvs_treenode_indirectdata.cpp \
../kvs/kvi_kvs_treenode_instruction.cpp \
../kvs/kvi_kvs_treenode_instructionblock.cpp \
../kvs/kvi_kvs_treenode_localvariable.cpp \
../kvs/kvi_kvs_treenode_modulecallbackcommand.cpp \
../kvs/kvi_kvs_treenode_modulefunctioncall.cpp \
../kvs/kvi_kvs_treenode_modulesimplecommand.cpp \
../kvs/kvi_kvs_treenode_multipleparameteridentifier.cpp \
../kvs/kvi_kvs_treenode_objectfield.cpp \
../kvs/kvi_kvs_treenode_objectfunctioncall.cpp \
../kvs/kvi_kvs_treenode_operation.cpp \
../kvs/kvi_kvs_treenode_parameterreturn.cpp \
../kvs/kvi_kvs_treenode_rebindingswitch.cpp \
../kvs/kvi_kvs_treenode_scopeoperator.cpp \
../kvs/kvi_kvs_treenode_simplecommand.cpp \
../kvs/kvi_kvs_treenode_singleparameteridentifier.cpp \
../kvs/kvi_kvs_treenode_specialcommand.cpp \
../kvs/kvi_kvs_treenode_specialcommandbreak.cpp \
../kvs/kvi_kvs_treenode_specialcommandclass.cpp \
../kvs/kvi_kvs_treenode_specialcommanddefpopup.cpp \
../kvs/kvi_kvs_treenode_specialcommanddo.cpp \
../kvs/kvi_kvs_treenode_specialcommandfor.cpp \
../kvs/kvi_kvs_treenode_specialcommandforeach.cpp \
../kvs/kvi_kvs_treenode_specialcommandif.cpp \
../kvs/kvi_kvs_treenode_specialcommandswitch.cpp \
../kvs/kvi_kvs_treenode_specialcommandunset.cpp \
../kvs/kvi_kvs_treenode_specialcommandwhile.cpp \
../kvs/kvi_kvs_treenode_stringcast.cpp \
../kvs/kvi_kvs_treenode_switchlist.cpp \
../kvs/kvi_kvs_treenode_thisobjectfunctioncall.cpp \
../kvs/kvi_kvs_treenode_variable.cpp \
../kvs/kvi_kvs_useraction.cpp \
../kvs/kvi_kvs_variant.cpp \
../kvs/kvi_kvs_variantlist.cpp \
../kvs/kvi_kvs_treenode_voidfunctioncall.cpp \
../module/kvi_mexlinkfilter.cpp \
../module/kvi_mexserverimport.cpp ../module/kvi_mextoolbar.cpp \
../module/kvi_module.cpp ../module/kvi_moduleextension.cpp ../module/kvi_modulemanager.cpp \
../sparser/kvi_antispam.cpp ../sparser/kvi_ircmessage.cpp ../sparser/kvi_sparser.cpp \
../sparser/kvi_sp_ctcp.cpp ../sparser/kvi_sp_literal.cpp ../sparser/kvi_sp_numeric.cpp \
../sparser/kvi_sp_tables.cpp \
../ui/kvi_actiondrawer.cpp \
../ui/kvi_channel.cpp \
../ui/kvi_colorwin.cpp \
../ui/kvi_console.cpp \
../ui/kvi_cryptcontroller.cpp \
../ui/kvi_ctcppagedialog.cpp \
../ui/kvi_customtoolbar.cpp \
../ui/kvi_debugwindow.cpp \
../ui/kvi_dynamictooltip.cpp \
../ui/kvi_filedialog.cpp \
../ui/kvi_frame.cpp \
../ui/kvi_historywin.cpp \
../ui/kvi_htmldialog.cpp \
../ui/kvi_imagedialog.cpp \
../ui/kvi_input.cpp \
../ui/kvi_ipeditor.cpp \
../ui/kvi_irctoolbar.cpp \
../ui/kvi_ircview.cpp \
../ui/kvi_ircviewtools.cpp \
../ui/kvi_listview.cpp \
../ui/kvi_maskeditor.cpp \
../ui/kvi_mdicaption.cpp \
../ui/kvi_mdichild.cpp \
../ui/kvi_mdimanager.cpp \
../ui/kvi_menubar.cpp \
../ui/kvi_modeeditor.cpp \
../ui/kvi_modew.cpp \
../ui/kvi_msgbox.cpp \
../ui/kvi_optionswidget.cpp \
../ui/kvi_query.cpp \
../ui/kvi_scriptbutton.cpp\
../ui/kvi_scripteditor.cpp \
../ui/kvi_selectors.cpp \
../ui/kvi_splash.cpp \
../ui/kvi_statusbar.cpp \
../ui/kvi_statusbarapplet.cpp \
../ui/kvi_taskbar.cpp \
../ui/kvi_texticonwin.cpp \
../ui/kvi_themedlabel.cpp \
../ui/kvi_toolbar.cpp \
../ui/kvi_toolwindows_container.cpp \
../ui/kvi_topicw.cpp \
../ui/kvi_styled_controls.cpp \
../ui/kvi_userlistview.cpp \
../ui/kvi_window.cpp

#
# Command useful to find out all the headers
# rm -f ../include/*.h
# SRC=`find ../ -name *.h | sort`
# echo $SRC
#


headers_HEADERS = ../kernel/kvi_action.h \
../kernel/kvi_actionmanager.h \
../kernel/kvi_app.h \
../kernel/kvi_asynchronousconnectiondata.h \
../kernel/kvi_coreactions.h \
../kernel/kvi_coreactionnames.h \
../kernel/kvi_customtoolbardescriptor.h \
../kernel/kvi_customtoolbarmanager.h \
../kernel/kvi_filetransfer.h \
../kernel/kvi_iconmanager.h \
../kernel/kvi_internalcmd.h \
../kernel/kvi_ipc.h \
../kernel/kvi_ircdatastreammonitor.h \
../kernel/kvi_ircconnection.h \
../kernel/kvi_ircconnectionantictcpflooddata.h \
../kernel/kvi_ircconnectionasyncwhoisdata.h \
../kernel/kvi_ircconnectionnetsplitdetectordata.h \
../kernel/kvi_ircconnectiontarget.h \
../kernel/kvi_ircconnectiontargetresolver.h \
../kernel/kvi_ircconnectionserverinfo.h \
../kernel/kvi_ircconnectionstatedata.h \
../kernel/kvi_ircconnectionstatistics.h \
../kernel/kvi_ircconnectionuserinfo.h \
../kernel/kvi_irccontext.h \
../kernel/kvi_irclink.h \
../kernel/kvi_ircsocket.h \
../kernel/kvi_ircurl.h \
../kernel/kvi_lagmeter.h \
../kernel/kvi_notifylist.h \
../kernel/kvi_options.h \
../kernel/kvi_out.h \
../kernel/kvi_sslmaster.h \
../kernel/kvi_texticonmanager.h \
../kernel/kvi_theme.h \
../sparser/kvi_antispam.h \
../sparser/kvi_ircmessage.h \
../kernel/kvi_useraction.h \
../kernel/kvi_userinput.h \
../kvs/kvi_kvs.h \
../kvs/kvi_kvs_action.h \
../kvs/kvi_kvs_aliasmanager.h \
../kvs/kvi_kvs_array.h \
../kvs/kvi_kvs_arraycast.h \
../kvs/kvi_kvs_asyncdnsoperation.h \
../kvs/kvi_kvs_asyncoperation.h \
../kvs/kvi_kvs_callbackobject.h \
../kvs/kvi_kvs_corecallbackcommands.h \
../kvs/kvi_kvs_corefunctions.h \
../kvs/kvi_kvs_coresimplecommands.h \
../kvs/kvi_kvs_dnsmanager.h \
../kvs/kvi_kvs_event.h \
../kvs/kvi_kvs_eventhandler.h \
../kvs/kvi_kvs_eventmanager.h \
../kvs/kvi_kvs_eventtriggers.h \
../kvs/kvi_kvs_eventtable.h \
../kvs/kvi_kvs_hash.h \
../kvs/kvi_kvs_kernel.h \
../kvs/kvi_kvs_moduleinterface.h \
../kvs/kvi_kvs_object.h \
../kvs/kvi_kvs_object_class.h \
../kvs/kvi_kvs_object_controller.h \
../kvs/kvi_kvs_object_functioncall.h \
../kvs/kvi_kvs_object_functionhandler.h \
../kvs/kvi_kvs_object_functionhandlerimpl.h \
../kvs/kvi_kvs_parameterprocessor.h \
../kvs/kvi_kvs_parser.h \
../kvs/kvi_kvs_parser_macros.h \
../kvs/kvi_kvs_popupmanager.h \
../kvs/kvi_kvs_popupmenu.h \
../kvs/kvi_kvs_processmanager.h \
../kvs/kvi_kvs_report.h \
../kvs/kvi_kvs_runtimecall.h \
../kvs/kvi_kvs_runtimecontext.h \
../kvs/kvi_kvs_rwevaluationresult.h \
../kvs/kvi_kvs_script.h \
../kvs/kvi_kvs_scriptaddonmanager.h \
../kvs/kvi_kvs_switchlist.h \
../kvs/kvi_kvs_timermanager.h \
../kvs/kvi_kvs_treenode.h \
../kvs/kvi_kvs_treenode_aliasfunctioncall.h \
../kvs/kvi_kvs_treenode_aliassimplecommand.h \
../kvs/kvi_kvs_treenode_arraycount.h \
../kvs/kvi_kvs_treenode_arrayelement.h \
../kvs/kvi_kvs_treenode_arrayorhashelement.h \
../kvs/kvi_kvs_treenode_arrayreferenceassert.h \
../kvs/kvi_kvs_treenode_base.h \
../kvs/kvi_kvs_treenode_baseobjectfunctioncall.h \
../kvs/kvi_kvs_treenode_callbackcommand.h \
../kvs/kvi_kvs_treenode_command.h \
../kvs/kvi_kvs_treenode_commandevaluation.h \
../kvs/kvi_kvs_treenode_commandwithparameters.h \
../kvs/kvi_kvs_treenode_compositedata.h \
../kvs/kvi_kvs_treenode_constantdata.h \
../kvs/kvi_kvs_treenode_corecallbackcommand.h \
../kvs/kvi_kvs_treenode_corefunctioncall.h \
../kvs/kvi_kvs_treenode_coresimplecommand.h \
../kvs/kvi_kvs_treenode_data.h \
../kvs/kvi_kvs_treenode_datalist.h \
../kvs/kvi_kvs_treenode_expression.h \
../kvs/kvi_kvs_treenode_expressionreturn.h \
../kvs/kvi_kvs_treenode_extendedscopevariable.h \
../kvs/kvi_kvs_treenode_functioncall.h \
../kvs/kvi_kvs_treenode_globalvariable.h \
../kvs/kvi_kvs_treenode_hashcount.h \
../kvs/kvi_kvs_treenode_hashelement.h \
../kvs/kvi_kvs_treenode_hashreferenceassert.h \
../kvs/kvi_kvs_treenode_indirectdata.h \
../kvs/kvi_kvs_treenode_instruction.h \
../kvs/kvi_kvs_treenode_instructionblock.h \
../kvs/kvi_kvs_treenode_localvariable.h \
../kvs/kvi_kvs_treenode_modulecallbackcommand.h \
../kvs/kvi_kvs_treenode_modulefunctioncall.h \
../kvs/kvi_kvs_treenode_modulesimplecommand.h \
../kvs/kvi_kvs_treenode_multipleparameteridentifier.h \
../kvs/kvi_kvs_treenode_objectfield.h \
../kvs/kvi_kvs_treenode_objectfunctioncall.h \
../kvs/kvi_kvs_treenode_operation.h \
../kvs/kvi_kvs_treenode_parameterreturn.h \
../kvs/kvi_kvs_treenode_rebindingswitch.h \
../kvs/kvi_kvs_treenode_scopeoperator.h \
../kvs/kvi_kvs_treenode_simplecommand.h \
../kvs/kvi_kvs_treenode_singleparameteridentifier.h \
../kvs/kvi_kvs_treenode_specialcommand.h \
../kvs/kvi_kvs_treenode_specialcommandbreak.h \
../kvs/kvi_kvs_treenode_specialcommandclass.h \
../kvs/kvi_kvs_treenode_specialcommanddefpopup.h \
../kvs/kvi_kvs_treenode_specialcommanddo.h \
../kvs/kvi_kvs_treenode_specialcommandfor.h \
../kvs/kvi_kvs_treenode_specialcommandforeach.h \
../kvs/kvi_kvs_treenode_specialcommandif.h \
../kvs/kvi_kvs_treenode_specialcommandswitch.h \
../kvs/kvi_kvs_treenode_specialcommandunset.h \
../kvs/kvi_kvs_treenode_specialcommandwhile.h \
../kvs/kvi_kvs_treenode_stringcast.h \
../kvs/kvi_kvs_treenode_switchlist.h \
../kvs/kvi_kvs_treenode_thisobjectfunctioncall.h \
../kvs/kvi_kvs_treenode_variable.h \
../kvs/kvi_kvs_treenode_voidfunctioncall.h \
../kvs/kvi_kvs_types.h \
../kvs/kvi_kvs_useraction.h \
../kvs/kvi_kvs_variant.h \
../kvs/kvi_kvs_variantlist.h \
../module/kvi_mexlinkfilter.h ../module/kvi_mexserverimport.h ../module/kvi_mextoolbar.h \
../module/kvi_module.h ../module/kvi_moduleextension.h ../module/kvi_modulemanager.h \
../sparser/kvi_numeric.h \
../sparser/kvi_sparser.h \
../ui/kvi_actiondrawer.h \
../ui/kvi_channel.h \
../ui/kvi_colorwin.h \
../ui/kvi_console.h \
../ui/kvi_cryptcontroller.h \
../ui/kvi_ctcppagedialog.h \
../ui/kvi_customtoolbar.h \
../ui/kvi_debugwindow.h \
../ui/kvi_dynamictooltip.h \
../ui/kvi_filedialog.h \
../ui/kvi_frame.h \
../ui/kvi_historywin.h \
../ui/kvi_htmldialog.h \
../ui/kvi_imagedialog.h \
../ui/kvi_input.h \
../ui/kvi_ipeditor.h \
../ui/kvi_irctoolbar.h \
../ui/kvi_ircview.h \
../ui/kvi_ircviewprivate.h \
../ui/kvi_ircviewtools.h \
../ui/kvi_listview.h \
../ui/kvi_maskeditor.h \
../ui/kvi_mdicaption.h \
../ui/kvi_mdichild.h \
../ui/kvi_mdimanager.h \
../ui/kvi_menubar.h \
../ui/kvi_modeeditor.h \
../ui/kvi_msgbox.h \
../ui/kvi_optionswidget.h \
../ui/kvi_query.h \
../ui/kvi_scriptbutton.h \
../ui/kvi_scripteditor.h \
../ui/kvi_selectors.h \
../ui/kvi_splash.h \
../ui/kvi_statusbar.h \
../ui/kvi_statusbarapplet.h \
../ui/kvi_taskbar.h ../ui/kvi_texticonwin.h ../ui/kvi_themedlabel.h \
../ui/kvi_toolbar.h ../ui/kvi_topicw.h ../ui/kvi_userlistview.h \
../ui/kvi_window.h \
../ui/kvi_modew.h \
../ui/kvi_styled_controls.h \
../ui/kvi_toolwindows_container.h

%.moc: %.h
	$(SS_QT_MOC) $(SS_QT_MOC_FLAGS) $< -o $@

# Command useful to find the moc dependancies
# DIRS="kernel sparser ui uparser"
# for dire in $DIRS; do grep "\.moc" ../$dire/*.cpp | sort | sed -e 's/[[:space:]]*#include//' | sed -e s/\\\"/..\\\/$dire\\\// | sed -e 's/\"//'; done

../kernel/kvi_app.cpp: ../kernel/kvi_app.moc
../kernel/kvi_iconmanager.cpp: ../kernel/kvi_iconmanager.moc
../kernel/kvi_ipc.cpp: ../kernel/kvi_ipc.moc
../kernel/kvi_lagmeter.cpp: ../kernel/kvi_lagmeter.moc
../kernel/kvi_notifylist.cpp: ../kernel/kvi_notifylist.moc
../kernel/kvi_texticonmanager.cpp: ../kernel/kvi_texticonmanager.moc
../sparser/kvi_sparser.cpp: ../sparser/kvi_sparser.moc
../ui/kvi_channel.cpp: ../ui/kvi_channel.moc
../ui/kvi_colorwin.cpp: ../ui/kvi_colorwin.moc
../ui/kvi_console.cpp: ../ui/kvi_console.moc
../ui/kvi_cryptcontroller.cpp: ../ui/kvi_cryptcontroller.moc
../ui/kvi_ctcppagedialog.cpp: ../ui/kvi_ctcppagedialog.moc
../ui/kvi_dynamictooltip.cpp: ../ui/kvi_dynamictooltip.moc
../ui/kvi_filedialog.cpp: ../ui/kvi_filedialog.moc
../ui/kvi_frame.cpp: ../ui/kvi_frame.moc
../ui/kvi_historywin.cpp: ../ui/kvi_historywin.moc
../ui/kvi_input.cpp: ../ui/kvi_input.moc
../ui/kvi_ipeditor.cpp: ../ui/kvi_ipeditor.moc
../ui/kvi_irctoolbar.cpp: ../ui/kvi_irctoolbar.moc
../ui/kvi_ircview.cpp: ../ui/kvi_ircview.moc
../ui/kvi_maskeditor.cpp: ../ui/kvi_maskeditor.moc
../ui/kvi_menubar.cpp: ../ui/kvi_menubar.moc
../ui/kvi_modeeditor.cpp: ../ui/kvi_modeeditor.moc
../ui/kvi_optionswidget.cpp: ../ui/kvi_optionswidget.moc
../ui/kvi_query.cpp: ../ui/kvi_query.moc
../ui/kvi_scriptbutton.cpp: ../ui/kvi_scriptbutton.moc
../ui/kvi_scripteditor.cpp: ../ui/kvi_scripteditor.moc
../ui/kvi_selectors.cpp: ../ui/kvi_selectors.moc
../ui/kvi_splash.cpp: ../ui/kvi_splash.moc
../ui/kvi_taskbar.cpp: ../ui/kvi_taskbar.moc
../ui/kvi_texticonwin.cpp: ../ui/kvi_texticonwin.moc
../ui/kvi_themedlabel.cpp: ../ui/kvi_themedlabel.moc
../ui/kvi_toolbar.cpp: ../ui/kvi_toolbar.moc
../ui/kvi_topicw.cpp: ../ui/kvi_topicw.moc
../ui/kvi_modew.cpp: ../ui/kvi_modew.moc
../ui/kvi_styled_controls.cpp: ../ui/kvi_styled_controls.moc
../ui/kvi_userlistview.cpp: ../ui/kvi_userlistview.moc
../ui/kvi_window.cpp: ../ui/kvi_window.moc


../kvs/moc_%.cpp: ../kvs/%.h
	$(SS_QT_MOC) $(SS_QT_MOC_FLAGS) $< -o $@

../kernel/moc_%.cpp: ../kernel/%.h
	$(SS_QT_MOC) $(SS_QT_MOC_FLAGS) $< -o $@

../module/moc_%.cpp: ../module/%.h
	$(SS_QT_MOC) $(SS_QT_MOC_FLAGS) $< -o $@

../ui/moc_%.cpp: ../ui/%.h
	$(SS_QT_MOC) $(SS_QT_MOC_FLAGS) $< -o $@


#
# All the cpp files depend on symlinks beging up-to-date
#

../*/kvi_*.cpp: symlinks
../*/*/kvi_*.cpp: symlinks

#
# Symlinks depend on all the header files: symlink is rebuilt whenewer an *.h file changes
#

symlinks: ../kernel/*.h ../kvs/*.h ../module/*h ../sparser/*.h ../ui/*.h
	cd ../include; for i in kvi_*.h; do if [ ! -f $$i ]; then rm -f $$i; fi; done; for i in $(SS_TOPSRCDIR)/src/kvirc/*/kvi_*.h; do $(LN_S) $$i . >/dev/null 2>&1; echo "Created link to $$i"; done
	touch symlinks

#
# When cleaning...remove also the symlinks
#

CLEANFILES = ../include/*.h symlinks ../*/*.moc ../*/moc_kvi_*.cpp
