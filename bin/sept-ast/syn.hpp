// 2021.03.27 - Victor Dods

#pragma once

// Includes from this program's source
#include "common.hpp"
#include "EvalCtx.hpp"

#include <array>
#include "sept/ArrayTerm.hpp"
#include "sept/MemRef.hpp"
#include "sept/Tuple.hpp"
#include "sept/Union.hpp"
#include <string>

namespace syn {

// This can't be const.
// TODO: Figure out how to keep this as sept::UnionTerm_c so that its usage doesn't require Data indirection.
extern sept::Data Expr_as_Data;
extern sept::RefTerm_c const Expr_as_Ref;
extern sept::UnionTerm_c const &Expr;

// This can't be const.
// TODO: Figure out how to keep this as sept::UnionTerm_c so that its usage doesn't require Data indirection.
extern sept::Data TypeExpr_as_Data;
extern sept::RefTerm_c const TypeExpr_as_Ref;
extern sept::UnionTerm_c const &TypeExpr;

extern sept::UnionTerm_c const ValueTerminal;
extern sept::UnionTerm_c const TypeTerminal;
// TODO: Could define variables for each as well

extern sept::TupleTerm_c const SymbolDefn;
// TODO: Eventually this must support assignment to lvalues, not just symbols.
extern sept::TupleTerm_c const Assignment;

extern sept::UnionTerm_c const Stmt;
extern sept::ArrayETerm_c const StmtArray;
extern sept::ArrayETerm_c const ExprArray;
// This defines a Rust-style block expression, where the last expression is the produced value of the block.
extern sept::TupleTerm_c const BlockExpr;

// These are effectively structural subtypes (indistinguishable from the specific tuple terms)
extern sept::TupleTerm_c const BinOpExpr;
// NOTE: This should be called LeftUnOpExpr, and there should also be RightUnOpExpr
// TODO: Shouldn't these be const?
extern sept::TupleTerm_c UnOpExpr;
extern sept::TupleTerm_c CondExpr;
extern sept::TupleTerm_c RoundExpr;
extern sept::TupleTerm_c SquareExpr;
extern sept::TupleTerm_c CurlyExpr;

// TODO: SymbolId has to be std::string basically
extern sept::TupleTerm_c const SymbolTypeDecl;
extern sept::TupleTerm_c const FuncType;
extern sept::ArrayETerm_c SymbolTypeDeclArray;
extern sept::TupleTerm_c const FuncPrototype;
extern sept::TupleTerm_c const FuncLiteral;
extern sept::TupleTerm_c const FuncEval;
extern sept::TupleTerm_c const ElementEval;
extern sept::TupleTerm_c const Construction;

//
// TODO: Deprecate these, since semantic terms are what do evaluate and execute
//

sept::ArrayTerm_c evaluate_expr__as_ExprArray (sept::ArrayTerm_c const &a, sem::EvalCtx &ctx);
sept::Data evaluate_expr__as_BinOpExpr (sept::TupleTerm_c const &t, sem::EvalCtx &ctx);
sept::Data evaluate_expr__as_BlockExpr (sept::TupleTerm_c const &t, sem::EvalCtx &ctx);
sept::Data evaluate_expr__as_CondExpr (sept::TupleTerm_c const &t, sem::EvalCtx &ctx);
sept::Data evaluate_expr__as_Construction (sept::TupleTerm_c const &t, sem::EvalCtx &ctx);
sept::Data evaluate_expr__as_ElementEval (sept::TupleTerm_c const &t, sem::EvalCtx &ctx);
sept::Data evaluate_expr__as_UnOpExpr (sept::TupleTerm_c const &t, sem::EvalCtx &ctx);
sept::ArrayTerm_c evaluate_expr__as_RoundExpr (sept::TupleTerm_c const &t, sem::EvalCtx &ctx);
sept::Data evaluate_expr__as_SymbolId (std::string const &symbol_id, sem::EvalCtx &ctx);
sept::Data evaluate_expr__as_FuncEval (sept::TupleTerm_c const &t, sem::EvalCtx &ctx);

void execute_stmt__as_SymbolDefn (sept::TupleTerm_c const &t, sem::EvalCtx &ctx);
void execute_stmt__as_Assignment (sept::TupleTerm_c const &t, sem::EvalCtx &ctx);

} // end namespace syn
