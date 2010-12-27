# CMakeLists.txt for src/kvirc/

INCLUDE_DIRECTORIES(
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
	../kvilib/net
	../kvilib/system
)

SET(kvirc_MOC_HDRS
	ui/KviActionDrawer.h
	ui/KviChannelWindow.h
	ui/KviColorSelectionWindow.h
	ui/KviConsoleWindow.h
	ui/KviCryptController.h
	ui/KviCtcpPageDialog.h
	ui/KviCustomToolBar.h
	ui/KviDebugWindow.h
	ui/KviDynamicToolTip.h
	ui/KviFileDialog.h
	ui/KviMainWindow.h
	ui/KviHistoryWindow.h
	ui/KviHtmlDialog.h
	ui/KviImageDialog.h
	ui/KviInput.h
	ui/KviInputEditor.h
	ui/KviIpEditor.h
	ui/KviIrcToolBar.h
	ui/KviIrcView.h
	ui/KviIrcView_tools.h
	ui/KviMaskEditor.h
	ui/KviMdiChild.h
	ui/KviMdiManager.h
	ui/KviMenuBar.h
	ui/KviModeEditor.h
	ui/KviModeWidget.h
	ui/KviOptionsWidget.h
	ui/KviQueryWindow.h
	ui/KviScriptButton.h
	ui/KviScriptEditor.h
	ui/KviSelectors.h
	ui/KviSplashScreen.h
	ui/KviStatusBar.h
	ui/KviStatusBarApplet.h
	ui/KviTextIconWindow.h
	ui/KviThemedComboBox.h
	ui/KviThemedLabel.h
	ui/KviThemedLineEdit.h
	ui/KviThemedTreeWidget.h
	ui/KviToolBar.h
	ui/KviWindowToolWidget.h
	ui/KviTopicWidget.h
	ui/KviUserListView.h
	ui/KviWindow.h
	ui/KviWindowListBase.h
	ui/KviTreeWindowList.h
	sparser/KviIrcServerParser.h
	module/KviMexLinkFilter.h
	module/KviMexServerImport.h
	module/KviMexToolBar.h
	module/KviModuleManager.h
	kvs/KviKvsAction.h
	kvs/KviKvsAliasManager.h
	kvs/KviKvsAsyncDnsOperation.h
	kvs/KviKvsAsyncOperation.h
	kvs/KviKvsDnsManager.h
	kvs/KviKvsPopupMenu.h
	kvs/KviKvsPopupManager.h
	kvs/KviKvsProcessManager.h
	kvs/KviKvsTimerManager.h
	kvs/KviKvsUserAction.h
	kvs/event/KviKvsEventManager.h
	kvs/object/KviKvsObject.h
	kernel/KviAction.h
	kernel/KviActionManager.h
	kernel/KviApplication.h
	kernel/KviCoreActions.h
	kernel/KviDefaultScript.h
	kernel/KviFileTransfer.h
	kernel/KviIconManager.h
	kernel/KviIpcSentinel.h
	kernel/KviIrcConnection.h
	kernel/KviIrcConnectionRequestQueue.h
	kernel/KviIrcConnectionTargetResolver.h
	kernel/KviIrcContext.h
	kernel/KviIrcLink.h
	kernel/KviIrcSocket.h
	kernel/KviLagMeter.h
	kernel/KviNotifyList.h
	kernel/KviTextIconManager.h
)

SET(kvirc_SRCS
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
	ui/KviMdiChild.cpp
	ui/KviMdiManager.cpp
	ui/KviMenuBar.cpp
	ui/KviModeEditor.cpp
	ui/KviModeWidget.cpp
	ui/KviMessageBox.cpp
	ui/KviOptionsWidget.cpp
	ui/KviQueryWindow.cpp
	ui/KviScriptButton.cpp
	ui/KviScriptEditor.cpp
	ui/KviSelectors.cpp
	ui/KviSplashScreen.cpp
	ui/KviStatusBar.cpp
	ui/KviStatusBarApplet.cpp
	ui/KviTextIconWindow.cpp
	ui/KviThemedComboBox.cpp
	ui/KviThemedLabel.cpp
	ui/KviThemedLineEdit.cpp
	ui/KviThemedTreeWidget.cpp
	ui/KviToolBar.cpp
	ui/KviWindowToolWidget.cpp
	ui/KviTopicWidget.cpp
	ui/KviUserListView.cpp
	ui/KviWindow.cpp
	ui/KviWindowListBase.cpp
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
