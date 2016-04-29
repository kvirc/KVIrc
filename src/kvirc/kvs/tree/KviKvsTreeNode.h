#ifndef _KVI_KVS_TREENODE_H_
#define _KVI_KVS_TREENODE_H_
//=============================================================================
//
//   File : KviKvsTreeNode.h
//   Creation date : Thu 25 Sep 2003 05.12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "KviKvsTreeNodeBase.h"

#include "KviKvsTreeNodeAliasFunctionCall.h"
#include "KviKvsTreeNodeAliasSimpleCommand.h"
#include "KviKvsTreeNodeArrayCount.h"
#include "KviKvsTreeNodeArrayElement.h"
#include "KviKvsTreeNodeArrayOrHashElement.h"
#include "KviKvsTreeNodeArrayReferenceAssert.h"
#include "KviKvsTreeNodeBaseObjectFunctionCall.h"
#include "KviKvsTreeNodeCallbackCommand.h"
#include "KviKvsTreeNodeCommand.h"
#include "KviKvsTreeNodeCommandEvaluation.h"
#include "KviKvsTreeNodeCommandWithParameters.h"
#include "KviKvsTreeNodeCompositeData.h"
#include "KviKvsTreeNodeConstantData.h"
#include "KviKvsTreeNodeCoreCallbackCommand.h"
#include "KviKvsTreeNodeCoreFunctionCall.h"
#include "KviKvsTreeNodeCoreSimpleCommand.h"
#include "KviKvsTreeNodeData.h"
#include "KviKvsTreeNodeDataList.h"
#include "KviKvsTreeNodeExpression.h"
#include "KviKvsTreeNodeExpressionReturn.h"
#include "KviKvsTreeNodeExtendedScopeVariable.h"
#include "KviKvsTreeNodeFunctionCall.h"
#include "KviKvsTreeNodeGlobalVariable.h"
#include "KviKvsTreeNodeHashCount.h"
#include "KviKvsTreeNodeHashElement.h"
#include "KviKvsTreeNodeHashReferenceAssert.h"
#include "KviKvsTreeNodeIndirectData.h"
#include "KviKvsTreeNodeInstruction.h"
#include "KviKvsTreeNodeInstructionBlock.h"
#include "KviKvsTreeNodeLocalVariable.h"
#include "KviKvsTreeNodeModuleCallbackCommand.h"
#include "KviKvsTreeNodeModuleFunctionCall.h"
#include "KviKvsTreeNodeModuleSimpleCommand.h"
#include "KviKvsTreeNodeMultipleParameterIdentifier.h"
#include "KviKvsTreeNodeObjectField.h"
#include "KviKvsTreeNodeObjectFunctionCall.h"
#include "KviKvsTreeNodeOperation.h"
#include "KviKvsTreeNodeParameterCount.h"
#include "KviKvsTreeNodeParameterReturn.h"
#include "KviKvsTreeNodeRebindingSwitch.h"
#include "KviKvsTreeNodeScopeOperator.h"
#include "KviKvsTreeNodeSimpleCommand.h"
#include "KviKvsTreeNodeSingleParameterIdentifier.h"
#include "KviKvsTreeNodeSpecialCommand.h"
#include "KviKvsTreeNodeSpecialCommandBreak.h"
#include "KviKvsTreeNodeSpecialCommandClass.h"
#include "KviKvsTreeNodeSpecialCommandContinue.h"
#include "KviKvsTreeNodeSpecialCommandDefpopup.h"
#include "KviKvsTreeNodeSpecialCommandDo.h"
#include "KviKvsTreeNodeSpecialCommandFor.h"
#include "KviKvsTreeNodeSpecialCommandForeach.h"
#include "KviKvsTreeNodeSpecialCommandIf.h"
#include "KviKvsTreeNodeSpecialCommandSwitch.h"
#include "KviKvsTreeNodeSpecialCommandUnset.h"
#include "KviKvsTreeNodeSpecialCommandWhile.h"
#include "KviKvsTreeNodeStringCase.h"
#include "KviKvsTreeNodeSwitchList.h"
#include "KviKvsTreeNodeThisObjectFunctionCall.h"
#include "KviKvsTreeNodeVariable.h"
#include "KviKvsTreeNodeVoidFunctionCall.h"

//
// Hierarchy (the nodes that have some missing implementation have the leading '?' char)
//
//
// TreeNode(OK) (never instantiated)
// |
// +--Instruction(OK) (never instantiated) (used as interface by InstructionBlock) : instruction has bool execute() = 0;
// |  |
// |  +--InstructionBlock(OK)
// |  |
// |  +--ExpressionReturn(OK)
// |  |
// |  +--ParameterReturn(OK)
// |  |
// |  |--Command(OK) (never instantiated)
// |  |  |
// |  |  +--RebindingSwitch(OK)
// |  |  |
// |  |  +--CommandWithParameters(OK) (never instantiated)
// |  |  |  |
// |  |  |  +--SimpleCommand(OK) (never instantiated)
// |  |  |  |  |
// |  |  |  |  +--CoreSimpleCommand(OK)
// |  |  |  |  |
// |  |  |  |  +--ModuleSimpleCommand(OK)
// |  |  |  |  |
// |  |  |  |  +--AliasSimpleCommand(OK)
// |  |  |  |
// |  |  |  +--CallbackCommand(OK) (never instantiated)
// |  |  |     |
// |  |  |     +--CoreCallbackCommand(OK)
// |  |  |     |
// |  |  |     +--ModuleCallbackCommand(OK)
// |  |  |
// |  |  +--SpecialCommand(OK) (never instantiated)
// |  |     |
// |  |     +--SpecialCommandIf(OK)
// |  |     |
// |  |     +--SpecialCommandWhile(OK)
// |  |     |
// |  |     +--SpecialCommandDo(OK)
// |  |     |
// |  |     +--SpecialCommandForeach(OK)
// |  |     |
// |  |     +--SpecialCommandFor(OK)
// |  |     |
// |  |     +--SpecialCommandBreak(OK)
// |  |     |
// |  |     +--SpecialCommandContinue(OK)
// |  |     |
// |  |     +--SpecialCommandSwitch(OK)
// |  |     |
// |  |     +--SpecialCommandDefpopup(OK)
// |  |     |
// |  |     +--SpecialCommandClass(OK)
// |  |
// |  +--VoidFunctionCall(OK)
// |  |
// |  +--Operation(OK) : has a m_pTargetData pointer (never instantiated)
// |     |
// |     +--OperationAssignment(OK)
// |     |
// |     +--OperationIncrement(OK)
// |     |
// |     +--OperationDecrement(OK)
// |     |
// |     +--OperationSelfSum(OK)
// |     |
// |     +--OperationSelfSubtraction(OK)
// |     |
// |     +--OperationSelfDivision(OK)
// |     |
// |     +--OperationSelfMultiplication(OK)
// |     |
// |     +--OperationSelfModulus(OK)
// |     |
// |     +--OperationSelfOr(OK)
// |     |
// |     +--OperationSelfAnd(OK)
// |     |
// |     +--OperationSelfXor(OK)
// |     |
// |     +--OperationSelfShl(OK)
// |     |
// |     +--OperationSelfShr(OK)
// |     |
// |     +--OperationStringAppend(OK)
// |     |
// |     +--OperationStringAppendWithComma(OK)
// |     |
// |     +--OperationStringAppendWithSpace(OK)
// |     |
// |     +--OperationStringTransliteration(OK)
// |     |
// |     +--OperationStringSubstitution(OK)
// |
// +--Data(OK)  data has
// |  |         bool evaluateReadOnly(KviKvsRunTimeContext * c,KviKvsVariant * pBuffer) <-- this MUST be implemented
// |  |         KviKvsVariant * evaluateReadWrite(KviKvsRunTimeContext * c) <-- this MUST be implemented if isReadOnly returns false
// |  |
// |  |         This class is NEVER instantiated directly
// |  |
// |  |         isReadOnly(); true by default
// |  |         canEvaluateToObjectReference(); false by default
// |  |         isFunctionCall(); false by default
// |  |         canEvaluateInObjectScope(); false by default
// |  |         (DEAD)canReleaseResult(); false by default (can the result be simply "taken" ?)
// |  |
// |  +--StringCast(OK)    has some kind of data inside that is converted to string "on the fly"
// |  |
// |  +--ConstantData(OK)  have real constant data
// |  |                    everything inherited from data
// |  |                    only evaluateReadOnly is implemented and it never fails
// |  |
// |  +--CompositeData(OK) have real data composed of constants, it is an implicit StringCast too (it isn't inherited from StringCast since it operates in a different way)
// |  |                    everything inherited from data
// |  |                    only evaluateReadOnly is implemented and it never fails
// |  |
// |  +--ScopeOperator(  ) Has a left side that must evaluate to ob reference
// |  |                    Has a right side that must be a variable reference or function call
// |  |                    isReadOnly() depends on the right child
// |  |                    canEvaluateToObjectReference() depends on last child
// |  |                    isFunctionCall() depends on last child
// |  |                    canEvaluateInObjectScope() depends on last child
// |  |                    (DEAD)canReleaseResult() depends on last child
// |  |
// |  +--SingleParameterIdentifier(OK)  this is $n
// |  |                                 canEvaluateToObjectReference() = true
// |  |
// |  +--MultipleParameterIdentifier(OK)  this is $n-[n]
// |  |
// |  +--CommandEvaluation(OK) this is ${ ... }
// |  |                        canEvaluateToObjectReference() = true
// |  |
// |  +--Variable(OK)     this is something related to %xxx (never instantiated)
// |  |  |                canEvaluateInObjectScope() = true
// |  |  |                isReadOnly() = false
// |  |  |                canEvaluateToObjectReference() = true
// |  |  |
// |  |  +--LocalVariable(OK)
// |  |  |
// |  |  +--GlobalVariable(OK)
// |  |  |
// |  |  +--ExtendedScopeVariable(OK)
// |  |  |
// |  |  +--ObjectField(OK)
// |  |
// |  +--IndirectData(OK)    takes a Data as source (never instantiated)
// |  |  |                   canEvaluateInObjectScope() depends on the parameter data
// |  |  |
// |  |  +--ArrayOrHashElement(OK) isReadOnly() depends on the source data (never instantiated)
// |  |  |  |                      canEvaluateToObjectReference() = true
// |  |  |  |
// |  |  |  +--ArrayElement(OK) the source Data must evaluate to an array or be writable in order to convert it
// |  |  |  |                   and an index that must evaluate to an integer (evaluate ok, result inherited ok)
// |  |  |  |
// |  |  |  +--HashElement(OK)  the source Data must evaluate to a hash or be writable in order to convert it
// |  |  |                      and an index that must evaluate to a non-empty hash key (evaluate ok, result inherited ok)
// |  |  |
// |  |  +--HashCount(OK)    the source Data must evaluate to a hash or be writable in order to convert it
// |  |  |
// |  |  +--ArrayCount(OK)   the source Data must evaluate to an array or be writable in order to convert it
// |  |  |
// |  |  +--HashReferenceAssert(OK)   the source Data must evaluate to a hash or be writable in order to convert it (otherwise asserts)
// |  |  |                            isReadOnly() depends on the parameter data
// |  |  |
// |  |  +--ArrayReferenceAssert(OK)  the source Data must evaluate to an array or be writable in order to convert it (otherwise asserts)
// |  |                               isReadOnly() depends on the parameter data
// |  |
// |  +--Expression(OK) (never instantiated)
// |  |  |
// |  |  +--ExpressionVariableOperand(OK) (evaluate and result ok)
// |  |  |
// |  |  +--ExpressionConstantOperand(OK) (evaluate and result ok)
// |  |  |
// |  |  +--ExpressionOperator(OK) (never instantiated)
// |  |     |
// |  |     +--ExpressionBinaryOperator(OK) (never instantiated)
// |  |     |  |
// |  |     |  +--ExpressionBinaryOperatorSum(OK) (evaluate and result ok)
// |  |     |  |
// |  |     |  +--ExpressionBinaryOperatorSubtraction(OK) (evaluate and result ok)
// |  |     |  |
// |  |     |  +--ExpressionBinaryOperatorMultiplication(OK) (evaluate and result ok)
// |  |     |  |
// |  |     |  +--ExpressionBinaryOperatorDivision(OK) (evaluate and result ok)
// |  |     |  |
// |  |     |  +--ExpressionBinaryOperatorBitwiseAnd(OK) (evaluate and result ok)
// |  |     |  |
// |  |     |  +--ExpressionBinaryOperatorBitwiseOr(OK) (evaluate and result ok)
// |  |     |  |
// |  |     |  +--ExpressionBinaryOperatorBitwiseXor(OK) (evaluate and result ok)
// |  |     |  |
// |  |     |  +--ExpressionBinaryOperatorShiftLeft(OK) (evaluate and result ok)
// |  |     |  |
// |  |     |  +--ExpressionBinaryOperatorShiftRight(OK) (evaluate and result ok)
// |  |     |  |
// |  |     |  +--ExpressionBinaryOperatorAnd(OK) (evaluate and result ok)
// |  |     |  |
// |  |     |  +--ExpressionBinaryOperatorOr(OK) (evaluate and result ok)
// |  |     |  |
// |  |     |  +--ExpressionBinaryOperatorXor(OK) (evaluate and result ok)
// |  |     |  |
// |  |     |  +--ExpressionBinaryOperatorLowerThan(OK) (evaluate and result ok)
// |  |     |  |
// |  |     |  +--ExpressionBinaryOperatorGreaterThan(OK) (evaluate and result ok)
// |  |     |  |
// |  |     |  +--ExpressionBinaryOperatorLowerOrEqualTo(OK) (evaluate and result ok)
// |  |     |  |
// |  |     |  +--ExpressionBinaryOperatorGreaterOrEqualTo(OK) (evaluate and result ok)
// |  |     |  |
// |  |     |  +--ExpressionBinaryOperatorEqualTo(OK) (evaluate and result ok)
// |  |     |  |
// |  |     |  +--ExpressionBinaryOperatorNotEqualTo(OK) (evaluate and result ok)
// |  |     |
// |  |     +--ExpressionUnaryOperator(OK) (never instantiated)
// |  |        |
// |  |        +--ExpressionUnaryOperatorNegate(OK) (evaluate and result ok)
// |  |        |
// |  |        +--ExpressionUnaryOperatorBitwiseNot(OK) (evaluate and result ok)
// |  |        |
// |  |        +--ExpressionUnaryOperatorLogicalNot(OK) (evaluate and result ok)
// |  |
// |  +--FunctionCall(OK)  this is something like $xxx and has a name. Can be called as a void instruction
// |     |                 canEvaluateToObjectReference() = true
// |     |                 isFunctionCall() = true
// |     |
// |     +--ModuleFunctionCall(OK)
// |     |
// |     +--AliasFunctionCall(OK)
// |     |
// |     +--CoreFunctionCall(OK)
// |     |
// |     +--ObjectFunctionCall(  )    this is the only function call type appended to a scope operator
// |        |                         canEvaluateInObjectScope() = true (never instantiated)
// |        |
// |        +--ThisObjectFunctionCall(  )
// |        |
// |        +--BaseObjectFunctionCall(  )
// |
// +--DataList(OK)
// |
// +--SwitchList(OK)
// |
// +--SpecialCommandSwitchLabel(Never instantiated)
// |  |
// |  +--SpecialCommandSwitchLabelCase(OK)
// |  |
// |  +--SpecialCommandSwitchLabelMatch(OK)
// |  |
// |  +--SpecialCommandSwitchLabelRegexp(OK)
// |  |
// |  +--SpecialCommandSwitchLabelDefault(OK)
// |
// +--SpecialCommandDefpopupLabel(Never instantiated)
//    |
//    +--SpecialCommandDefpopupLabelPrologue(OK)
//    |
//    +--SpecialCommandDefpopupLabelEpilogue(OK)
//    |
//    +--SpecialCommandDefpopupConditionalLabel(Never instantiated)
//       |
//       +--SpecialCommandDefpopupLabelSeparator(OK)
//       |
//       +--SpecialCommandDefpopupConditionalLabelWithTextAndIcon (Never instantiated)
//          |
//          +--SpecialCommandDefpopupLabelPopup(OK)
//          |
//          +--SpecialCommandDefpopupLabelExtpopup(OK)
//          |
//          +--SpecialCommandDefpopupLabelItem(OK)
//          |
//          +--SpecialCommandDefpopupLabelLabel(OK)
//
//
//

//
// Tree morphology
//
// InstructionBlock
//     [Instruction]
//
// ScopeOperator
//     [Data]->[Data]
//

#endif //!_KVI_KVS_TREENODE_H_
