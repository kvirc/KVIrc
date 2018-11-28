# CMakeLists.txt for src/kvirc/

include_directories(
	kernel
	kvs
	kvs/event
	kvs/object
	kvs/parser
	kvs/tree
	module
	sparser
	ui
	../kvilib/tal
	../kvilib/config
	../kvilib/core
	../kvilib/ext
	../kvilib/file
	../kvilib/irc
	../kvilib/locale
	../kvilib/net
	../kvilib/system
)

set(kvirc_SRCS
	ui/KviActionDrawer.cpp
	ui/KviChannelWindow.cpp
	ui/KviColorSelectionWindow.cpp
	ui/KviConsoleWindow.cpp
	ui/KviCryptController.cpp
	ui/KviCtcpPageDialog.cpp
	ui/KviCustomToolBar.cpp
	ui/KviDebugWindow.cpp
	ui/KviDynamicToolTip.cpp
	ui/KviFileDialog.cpp
	ui/KviMainWindow.cpp
	ui/KviHistoryWindow.cpp
	ui/KviHtmlDialog.cpp
	ui/KviImageDialog.cpp
	ui/KviInput.cpp
	ui/KviInputEditor.cpp
	ui/KviInputHistory.cpp
	ui/KviIpEditor.cpp
	ui/KviIrcToolBar.cpp
	ui/KviIrcView.cpp
	ui/KviIrcView_events.cpp
	ui/KviIrcView_getTextLine.cpp
	ui/KviIrcView_loghandling.cpp
	ui/KviIrcView_tools.cpp
	ui/KviMaskEditor.cpp
	ui/KviMenuBar.cpp
	ui/KviModeEditor.cpp
	ui/KviModeWidget.cpp
	ui/KviMessageBox.cpp
	ui/KviOptionsWidget.cpp
	ui/KviQueryWindow.cpp
	ui/KviScriptButton.cpp
	ui/KviScriptEditor.cpp
	ui/KviSelectors.cpp
	ui/KviStatusBar.cpp
	ui/KviStatusBarApplet.cpp
	ui/KviTextIconWindow.cpp
	ui/KviThemedComboBox.cpp
	ui/KviThemedLabel.cpp
	ui/KviThemedLineEdit.cpp
	ui/KviThemedTreeWidget.cpp
	ui/KviToolBar.cpp
	ui/KviWebPackageManagementDialog.cpp
	ui/KviWindowToolWidget.cpp
	ui/KviTopicWidget.cpp
	ui/KviUserListView.cpp
	ui/KviWindow.cpp
	ui/KviWindowListBase.cpp
	ui/KviWindowStack.cpp
	ui/KviTreeWindowList.cpp
	sparser/KviAntiSpam.cpp
	sparser/KviIrcMessage.cpp
	sparser/KviIrcServerParser_ctcp.cpp
	sparser/KviIrcServerParser_literalHandlers.cpp
	sparser/KviIrcServerParser_numericHandlers.cpp
	sparser/KviIrcServerParser_tables.cpp
	sparser/KviIrcServerParser.cpp
	module/KviMexLinkFilter.cpp
	module/KviMexServerImport.cpp
	module/KviMexToolBar.cpp
	module/KviModule.cpp
	module/KviModuleExtension.cpp
	module/KviModuleManager.cpp
	kvs/KviKvs.cpp
	kvs/KviKvsAction.cpp
	kvs/KviKvsAliasManager.cpp
	kvs/KviKvsArray.cpp
	kvs/KviKvsArrayCast.cpp
	kvs/KviKvsAsyncDnsOperation.cpp
	kvs/KviKvsAsyncOperation.cpp
	kvs/KviKvsCallbackObject.cpp
	kvs/KviKvsCoreCallbackCommands.cpp
	kvs/KviKvsCoreFunctions.cpp
	kvs/KviKvsCoreFunctions_af.cpp
	kvs/KviKvsCoreFunctions_gl.cpp
	kvs/KviKvsCoreFunctions_mr.cpp
	kvs/KviKvsCoreFunctions_sz.cpp
	kvs/KviKvsCoreSimpleCommands.cpp
	kvs/KviKvsCoreSimpleCommands_af.cpp
	kvs/KviKvsCoreSimpleCommands_gl.cpp
	kvs/KviKvsCoreSimpleCommands_mr.cpp
	kvs/KviKvsCoreSimpleCommands_sz.cpp
	kvs/KviKvsDnsManager.cpp
	kvs/KviKvsHash.cpp
	kvs/KviKvsKernel.cpp
	kvs/KviKvsModuleInterface.cpp
	kvs/KviKvsParameterProcessor.cpp
	kvs/KviKvsPopupManager.cpp
	kvs/KviKvsPopupMenu.cpp
	kvs/KviKvsProcessManager.cpp
	kvs/KviKvsReport.cpp
	kvs/KviKvsRunTimeCall.cpp
	kvs/KviKvsRunTimeContext.cpp
	kvs/KviKvsRWEvaluationResult.cpp
	kvs/KviKvsScript.cpp
	kvs/KviKvsScriptAddonManager.cpp
	kvs/KviKvsSwitchList.cpp
	kvs/KviKvsTimerManager.cpp
	kvs/KviKvsUserAction.cpp
	kvs/KviKvsVariant.cpp
	kvs/KviKvsVariantList.cpp
	kvs/event/KviKvsEvent.cpp
	kvs/event/KviKvsEventHandler.cpp
	kvs/event/KviKvsEventManager.cpp
	kvs/event/KviKvsEventTable.cpp
	kvs/object/KviKvsObject.cpp
	kvs/object/KviKvsObjectClass.cpp
	kvs/object/KviKvsObjectController.cpp
	kvs/object/KviKvsObjectFunctionCall.cpp
	kvs/object/KviKvsObjectFunctionHandler.cpp
	kvs/object/KviKvsObjectFunctionHandlerImpl.cpp
	kvs/parser/KviKvsParser.cpp
	kvs/parser/KviKvsParser_command.cpp
	kvs/parser/KviKvsParser_comment.cpp
	kvs/parser/KviKvsParser_dollar.cpp
	kvs/parser/KviKvsParser_expression.cpp
	kvs/parser/KviKvsParser_lside.cpp
	kvs/parser/KviKvsParser_specialCommands.cpp
	kvs/tree/KviKvsTreeNodeAliasFunctionCall.cpp
	kvs/tree/KviKvsTreeNodeAliasSimpleCommand.cpp
	kvs/tree/KviKvsTreeNodeArrayCount.cpp
	kvs/tree/KviKvsTreeNodeArrayElement.cpp
	kvs/tree/KviKvsTreeNodeArrayOrHashElement.cpp
	kvs/tree/KviKvsTreeNodeArrayReferenceAssert.cpp
	kvs/tree/KviKvsTreeNodeBase.cpp
	kvs/tree/KviKvsTreeNodeBaseObjectFunctionCall.cpp
	kvs/tree/KviKvsTreeNodeCallbackCommand.cpp
	kvs/tree/KviKvsTreeNodeCommand.cpp
	kvs/tree/KviKvsTreeNodeCommandEvaluation.cpp
	kvs/tree/KviKvsTreeNodeCommandWithParameters.cpp
	kvs/tree/KviKvsTreeNodeCompositeData.cpp
	kvs/tree/KviKvsTreeNodeConstantData.cpp
	kvs/tree/KviKvsTreeNodeCoreCallbackCommand.cpp
	kvs/tree/KviKvsTreeNodeCoreFunctionCall.cpp
	kvs/tree/KviKvsTreeNodeCoreSimpleCommand.cpp
	kvs/tree/KviKvsTreeNodeData.cpp
	kvs/tree/KviKvsTreeNodeDataList.cpp
	kvs/tree/KviKvsTreeNodeExpression.cpp
	kvs/tree/KviKvsTreeNodeExpressionReturn.cpp
	kvs/tree/KviKvsTreeNodeExtendedScopeVariable.cpp
	kvs/tree/KviKvsTreeNodeFunctionCall.cpp
	kvs/tree/KviKvsTreeNodeGlobalVariable.cpp
	kvs/tree/KviKvsTreeNodeHashCount.cpp
	kvs/tree/KviKvsTreeNodeHashElement.cpp
	kvs/tree/KviKvsTreeNodeHashReferenceAssert.cpp
	kvs/tree/KviKvsTreeNodeIndirectData.cpp
	kvs/tree/KviKvsTreeNodeInstruction.cpp
	kvs/tree/KviKvsTreeNodeInstructionBlock.cpp
	kvs/tree/KviKvsTreeNodeLocalVariable.cpp
	kvs/tree/KviKvsTreeNodeModuleCallbackCommand.cpp
	kvs/tree/KviKvsTreeNodeModuleFunctionCall.cpp
	kvs/tree/KviKvsTreeNodeModuleSimpleCommand.cpp
	kvs/tree/KviKvsTreeNodeMultipleParameterIdentifier.cpp
	kvs/tree/KviKvsTreeNodeObjectField.cpp
	kvs/tree/KviKvsTreeNodeObjectFunctionCall.cpp
	kvs/tree/KviKvsTreeNodeOperation.cpp
	kvs/tree/KviKvsTreeNodeParameterCount.cpp
	kvs/tree/KviKvsTreeNodeParameterReturn.cpp
	kvs/tree/KviKvsTreeNodeRebindingSwitch.cpp
	kvs/tree/KviKvsTreeNodeScopeOperator.cpp
	kvs/tree/KviKvsTreeNodeSimpleCommand.cpp
	kvs/tree/KviKvsTreeNodeSingleParameterIdentifier.cpp
	kvs/tree/KviKvsTreeNodeSpecialCommand.cpp
	kvs/tree/KviKvsTreeNodeSpecialCommandBreak.cpp
	kvs/tree/KviKvsTreeNodeSpecialCommandClass.cpp
	kvs/tree/KviKvsTreeNodeSpecialCommandContinue.cpp
	kvs/tree/KviKvsTreeNodeSpecialCommandDefpopup.cpp
	kvs/tree/KviKvsTreeNodeSpecialCommandDo.cpp
	kvs/tree/KviKvsTreeNodeSpecialCommandFor.cpp
	kvs/tree/KviKvsTreeNodeSpecialCommandForeach.cpp
	kvs/tree/KviKvsTreeNodeSpecialCommandIf.cpp
	kvs/tree/KviKvsTreeNodeSpecialCommandSwitch.cpp
	kvs/tree/KviKvsTreeNodeSpecialCommandUnset.cpp
	kvs/tree/KviKvsTreeNodeSpecialCommandWhile.cpp
	kvs/tree/KviKvsTreeNodeStringCase.cpp
	kvs/tree/KviKvsTreeNodeSwitchList.cpp
	kvs/tree/KviKvsTreeNodeThisObjectFunctionCall.cpp
	kvs/tree/KviKvsTreeNodeVariable.cpp
	kvs/tree/KviKvsTreeNodeVoidFunctionCall.cpp
	kernel/KviAction.cpp
	kernel/KviActionManager.cpp
	kernel/KviApplication.cpp
	kernel/KviApplication_filesystem.cpp
	kernel/KviApplication_setup.cpp
	kernel/KviAsynchronousConnectionData.cpp
	kernel/KviCoreActions.cpp
	kernel/KviCustomToolBarDescriptor.cpp
	kernel/KviCustomToolBarManager.cpp
	kernel/KviDefaultScript.cpp
	kernel/KviFileTransfer.cpp
	kernel/KviHtmlGenerator.cpp
	kernel/KviIconManager.cpp
	kernel/KviInternalCommand.cpp
	kernel/KviIpcSentinel.cpp
	kernel/KviIrcConnection.cpp
	kernel/KviIrcConnectionAntiCtcpFloodData.cpp
	kernel/KviIrcConnectionAsyncWhoisData.cpp
	kernel/KviIrcConnectionNetsplitDetectorData.cpp
	kernel/KviIrcConnectionRequestQueue.cpp
	kernel/KviIrcConnectionServerInfo.cpp
	kernel/KviIrcConnectionStateData.cpp
	kernel/KviIrcConnectionStatistics.cpp
	kernel/KviIrcConnectionTarget.cpp
	kernel/KviIrcConnectionTargetResolver.cpp
	kernel/KviIrcConnectionUserInfo.cpp
	kernel/KviIrcContext.cpp
	kernel/KviIrcDataStreamMonitor.cpp
	kernel/KviIrcLink.cpp
	kernel/KviIrcSocket.cpp
	kernel/KviIrcUrl.cpp
	kernel/KviLagMeter.cpp
	kernel/KviMain.cpp
	kernel/KviNotifyList.cpp
	kernel/KviOptions.cpp
	kernel/KviSSLMaster.cpp
	kernel/KviTextIconManager.cpp
	kernel/KviTheme.cpp
	kernel/KviUserAction.cpp
	kernel/KviUserInput.cpp
)

if(MINGW)
	list(APPEND LIBS -lshlwapi)
	# resource compilation for mingw
	set(CMAKE_RC_COMPILER_INIT windres)
	enable_language(RC)
	set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")

	list(APPEND kvirc_SRCS ${CMAKE_SOURCE_DIR}/data/resources_win32/kvirc.rc)
	list(APPEND kvirc_SRCS ${CMAKE_SOURCE_DIR}/data/resources_win32/kvirc.exe.manifest)
endif()

if(MSVC)
	list(APPEND LIBS shlwapi.lib)
	list(APPEND kvirc_SRCS ${CMAKE_SOURCE_DIR}/data/resources_win32/kvirc.rc)
	list(APPEND kvirc_SRCS ${CMAKE_SOURCE_DIR}/data/resources_win32/kvirc.exe.manifest)
endif()

# Link libraries
# don't mind the WIN32 flag, its needed to not show a console on non-debug windows builds and is ignored anywhere else.
if(NOT WANT_DEBUG)
	add_executable(${KVIRC_BINARYNAME} WIN32 ${kvirc_SRCS} ${kvirc_MOC_SRCS})
else()
	add_executable(${KVIRC_BINARYNAME} ${kvirc_SRCS} ${kvirc_MOC_SRCS})
endif()

target_link_libraries(${KVIRC_BINARYNAME} ${KVILIB_BINARYNAME} ${LIBS})

# Enable C++17
set_property(TARGET ${KVIRC_BINARYNAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${KVIRC_BINARYNAME} PROPERTY CXX_STANDARD_REQUIRED ON)

if(Qt5Widgets_FOUND)
	target_link_libraries(${KVIRC_BINARYNAME} ${qt5_kvirc_modules})
endif()

if(MINGW)
	# For some reason CMake doesn't do this itself...
	# In case of MSVC, it does add /implib:kvirc.lib though
	set_target_properties(${KVIRC_BINARYNAME} PROPERTIES LINK_FLAGS "${ADDITIONAL_LINK_FLAGS} -Wl,--out-implib=libkvirc.dll.a")
else()
	set_target_properties(${KVIRC_BINARYNAME} PROPERTIES LINK_FLAGS "${ADDITIONAL_LINK_FLAGS}")
endif()

if(CMAKE_HOST_SYSTEM MATCHES "FreeBSD")
	set_target_properties(${KVIRC_BINARYNAME} PROPERTIES LINK_FLAGS -Wl,-export-dynamic)
	message(STATUS "Patching for FreeBSD...")
endif()

# This is needed for linking the modules (not actually used by gcc on linux... but it SHOULD be)
set_target_properties(${KVIRC_BINARYNAME} PROPERTIES ENABLE_EXPORTS ON)

if(WIN32)
	# We need this defined when mingw will compile moc files
	add_definitions(-D_WANT_KVIRC_)
endif()

if(WANT_STRIP)
	add_custom_command(
		TARGET ${KVIRC_BINARYNAME}
		POST_BUILD
		COMMAND ${STRIP_EXECUTABLE} $<TARGET_FILE:${KVIRC_BINARYNAME}>
	)
endif()

# Installation directives
install(TARGETS ${KVIRC_BINARYNAME} RUNTIME DESTINATION "${KVIRC_BIN_PATH}")
if(WIN32)
	install(FILES $<TARGET_PDB_FILE:${KVIRC_BINARYNAME}> DESTINATION ${KVIRC_BIN_PATH} OPTIONAL)
endif()

if(APPLE)
	set(plugin_dest_dir ${CMAKE_INSTALL_PREFIX}/Contents/Plugins)
	set(qtconf_dest_dir ${CMAKE_INSTALL_PREFIX}/Contents/Resources)
	#fixme dirty hack
	set(QT_PLUGINS_DIR "${Qt5Widgets_DIR}/../../../plugins")
	set(QT_LIBRARY_DIR "${Qt5Widgets_DIR}/../../")
	get_filename_component(QT_LIBRARY_DIR ${QT_LIBRARY_DIR} PATH)
	get_filename_component(QT_LIBRARY_DIR "${QT_LIBRARY_DIR}/.." ABSOLUTE)

    # qt5: audio, iconengines, imageformats, mediaservice, platforms, sqldrivers
    install(DIRECTORY "${QT_PLUGINS_DIR}/" DESTINATION ${plugin_dest_dir} COMPONENT Runtime
        FILES_MATCHING
        PATTERN "*.dSYM" EXCLUDE
        PATTERN "*_debug.dylib" EXCLUDE
        PATTERN "audio/*.dylib"
        PATTERN "iconengines/*.dylib"
        PATTERN "imageformats/*.dylib"
        PATTERN "mediaservice/*.dylib"
        PATTERN "platforms/*.dylib"
        PATTERN "sqldrivers/*.dylib"
    )

	install(CODE "
	    file(WRITE \"${qtconf_dest_dir}/qt.conf\" \"[Paths]
Plugins = Plugins\")
	    " COMPONENT Runtime)

	install(CODE "
	    file(GLOB_RECURSE QTPLUGINS
	      \"${plugin_dest_dir}/*.dylib\")
	    set(BU_CHMOD_BUNDLE_ITEMS ON)
	    include(BundleUtilities)
	    fixup_bundle(\"${CMAKE_INSTALL_PREFIX}\" \"\${QTPLUGINS}\" \"${QT_LIBRARY_DIR}\")
	    " COMPONENT Runtime)
endif()
