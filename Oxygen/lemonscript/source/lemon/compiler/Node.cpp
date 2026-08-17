/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "lemon/pch.h"
#include "lemon/compiler/Node.h"

#if defined(__CELLOS_LV2__) || defined(__SNC__)
namespace lemon
{
	genericmanager::detail::ElementClassImpl<Node, UndefinedNode, (uint32)Node::Type::UNDEFINED> UndefinedNode::CLASS;
	genericmanager::detail::ElementClassImpl<Node, BlockNode, (uint32)Node::Type::BLOCK> BlockNode::CLASS;
	genericmanager::detail::ElementClassImpl<Node, PragmaNode, (uint32)Node::Type::PRAGMA> PragmaNode::CLASS;
	genericmanager::detail::ElementClassImpl<Node, FunctionNode, (uint32)Node::Type::FUNCTION> FunctionNode::CLASS;
	genericmanager::detail::ElementClassImpl<Node, LabelNode, (uint32)Node::Type::LABEL> LabelNode::CLASS;
	genericmanager::detail::ElementClassImpl<Node, JumpNode, (uint32)Node::Type::JUMP> JumpNode::CLASS;
	genericmanager::detail::ElementClassImpl<Node, JumpIndirectNode, (uint32)Node::Type::JUMP_INDIRECT> JumpIndirectNode::CLASS;
	genericmanager::detail::ElementClassImpl<Node, BreakNode, (uint32)Node::Type::BREAK> BreakNode::CLASS;
	genericmanager::detail::ElementClassImpl<Node, ContinueNode, (uint32)Node::Type::CONTINUE> ContinueNode::CLASS;
	genericmanager::detail::ElementClassImpl<Node, ReturnNode, (uint32)Node::Type::RETURN> ReturnNode::CLASS;
	genericmanager::detail::ElementClassImpl<Node, ExternalNode, (uint32)Node::Type::EXTERNAL> ExternalNode::CLASS;
	genericmanager::detail::ElementClassImpl<Node, StatementNode, (uint32)Node::Type::STATEMENT> StatementNode::CLASS;
	genericmanager::detail::ElementClassImpl<Node, IfStatementNode, (uint32)Node::Type::IF_STATEMENT> IfStatementNode::CLASS;
	genericmanager::detail::ElementClassImpl<Node, WhileStatementNode, (uint32)Node::Type::WHILE_STATEMENT> WhileStatementNode::CLASS;
	genericmanager::detail::ElementClassImpl<Node, ForStatementNode, (uint32)Node::Type::FOR_STATEMENT> ForStatementNode::CLASS;
}
#endif
