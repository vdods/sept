// 2021.03.27 - Victor Dods

#include "syn.hpp"

// Includes from this program's source
#include "sem.hpp"

sept::RefTerm_c const Expr_as_Ref = sept::MemRef(&Expr_as_Data);

sept::RefTerm_c const TypeExpr_as_Ref = sept::MemRef(&TypeExpr_as_Data);

sept::UnionTerm_c const ValueTerminal = sept::Union(
    sept::Array,
    sept::Bool,
    sept::FalseType,
    sept::Float32,
    sept::Float64,
    sept::Sint8,
    sept::Sint16,
    sept::Sint32,
    sept::Sint64,
    sept::Uint8,
    sept::Uint16,
    sept::Uint32,
    sept::Uint64,
    sept::TrueType,
    sept::Tuple
);
sept::UnionTerm_c const TypeTerminal = sept::Union(
    sept::ArrayES,
    sept::ArrayE,
    sept::ArrayS,
    sept::ArrayType,
    sept::BoolType,
    sept::FormalTypeOf(sept::FalseType),
    sept::Sint8Type,
    sept::Sint16Type,
    sept::Sint32Type,
    sept::Sint64Type,
    sept::Uint8Type,
    sept::Uint16Type,
    sept::Uint32Type,
    sept::Uint64Type,
    sept::Float32Type,
    sept::Float64Type,
    sept::FormalTypeOf(sept::TrueType),
    sept::TupleType
);
// TODO: Could define variables for each as well

sept::TupleTerm_c const SymbolDefn = sept::Tuple(SymbolId, sept::FormalTypeOf(DefinedAs), Expr_as_Ref);
// TODO: Eventually this must support assignment to lvalues, not just symbols.
sept::TupleTerm_c const Assignment = sept::Tuple(SymbolId, sept::FormalTypeOf(AssignFrom), Expr_as_Ref);

sept::UnionTerm_c const Stmt = sept::Union(SymbolDefn, Assignment);
sept::ArrayETerm_c const StmtArray = sept::ArrayE(Stmt);
sept::ArrayETerm_c const ExprArray = sept::ArrayE(Expr_as_Ref);
// This defines a Rust-style block expression, where the last expression is the produced value of the block.
sept::TupleTerm_c const BlockExpr = sept::Tuple(StmtArray, Expr_as_Ref);

// These are effectively structural subtypes (indistinguishable from the specific tuple terms)
sept::TupleTerm_c const BinOpExpr = sept::Tuple(Expr_as_Ref, BinOp, Expr_as_Ref);
// NOTE: This should be called LeftUnOpExpr
sept::TupleTerm_c UnOpExpr = sept::Tuple(UnOp, Expr_as_Ref);
sept::TupleTerm_c CondExpr = sept::Tuple(sept::FormalTypeOf(If), Expr_as_Ref, sept::FormalTypeOf(Then), Expr_as_Ref, sept::FormalTypeOf(Else), Expr_as_Ref);
sept::TupleTerm_c RoundExpr = sept::Tuple(sept::FormalTypeOf(RoundOpen), ExprArray, sept::FormalTypeOf(RoundClose));
sept::TupleTerm_c SquareExpr = sept::Tuple(sept::FormalTypeOf(SquareOpen), ExprArray, sept::FormalTypeOf(SquareClose));
sept::TupleTerm_c CurlyExpr = sept::Tuple(sept::FormalTypeOf(CurlyOpen), ExprArray, sept::FormalTypeOf(CurlyClose));

// TODO: SymbolId has to be std::string basically
sept::TupleTerm_c const SymbolTypeDecl = sept::Tuple(SymbolId, sept::FormalTypeOf(DeclaredAs), TypeExpr_as_Ref);
sept::TupleTerm_c const FuncType = sept::Tuple(TypeExpr_as_Ref, sept::FormalTypeOf(MapsTo), TypeExpr_as_Ref);
sept::ArrayETerm_c SymbolTypeDeclArray = sept::ArrayE(SymbolTypeDecl);
sept::TupleTerm_c const FuncPrototype = sept::Tuple(SymbolTypeDeclArray, sept::FormalTypeOf(MapsTo), TypeExpr_as_Ref);
sept::TupleTerm_c const FuncLiteral = sept::Tuple(FuncPrototype, Expr_as_Ref);
sept::TupleTerm_c const FuncEval = sept::Tuple(SymbolId, RoundExpr);
sept::TupleTerm_c const ElementEval = sept::Tuple(Expr_as_Ref, SquareExpr);
sept::TupleTerm_c const Construction = sept::Tuple(TypeExpr_as_Ref, CurlyExpr);

// This can't be const.
// NOTE: It seems that this has to be initialized down here, otherwise the order of initialization is wrong.
sept::Data Expr_as_Data{
    sept::Union(
        BinOpExpr,
        BlockExpr,
        CondExpr,
        Construction,
        ElementEval,
        FuncEval,
        RoundExpr,
        UnOpExpr,
        SymbolId,
        ValueTerminal
    )
};
sept::UnionTerm_c const &Expr = Expr_as_Data.cast<sept::UnionTerm_c const &>();

// This can't be const.
// NOTE: It seems that this has to be initialized down here, otherwise the order of initialization is wrong.
// TODO: More type constructions
sept::Data TypeExpr_as_Data{
    sept::Union(
        sept::ArrayES,
        sept::ArrayE,
        sept::ArrayS,
        sept::ArrayType,
        TypeTerminal,
        FuncType,
        FuncPrototype,
        SymbolId,
        sept::Tuple
    )
};
sept::UnionTerm_c const &TypeExpr = TypeExpr_as_Data.cast<sept::UnionTerm_c const &>();

//
// TODO: Deprecate these, since semantic term is what does evaluate and execute
//

sept::ArrayTerm_c evaluate_expr__as_ExprArray (sept::ArrayTerm_c const &a, EvalCtx &ctx) {
    return evaluate_ExprArray_Term(parse_ExprArray_Term(a), ctx);
}

sept::Data evaluate_expr__as_BinOpExpr (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    return evaluate_BinOpExpr_Term(parse_BinOpExpr_Term(t), ctx);
}

sept::Data evaluate_expr__as_BlockExpr (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    return evaluate_BlockExpr_Term(parse_BlockExpr_Term(t), ctx);
}

sept::Data evaluate_expr__as_CondExpr (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    return evaluate_CondExpr_Term(parse_CondExpr_Term(t), ctx);
}

sept::Data evaluate_expr__as_Construction (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    return evaluate_Construction_Term(parse_Construction_Term(t), ctx);
}

sept::Data evaluate_expr__as_ElementEval (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    return evaluate_ElementEval_Term(parse_ElementEval_Term(t), ctx);
}

sept::Data evaluate_expr__as_UnOpExpr (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    return evaluate_UnOpExpr_Term(parse_UnOpExpr_Term(t), ctx);
}

sept::ArrayTerm_c evaluate_expr__as_RoundExpr (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    return evaluate_RoundExpr_Term(parse_RoundExpr_Term(t), ctx);
}

sept::Data evaluate_expr__as_SymbolId (std::string const &symbol_id, EvalCtx &ctx) {
    return evaluate_SymbolId_Term(parse_SymbolId_Term(symbol_id), ctx);
}

sept::Data evaluate_expr__as_FuncEval (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    return evaluate_FuncEval_Term(parse_FuncEval_Term(t), ctx);
}

sept::Data evaluate_expr (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    // TODO: Make this efficient -- some sort of poset search.
    if (false)
        { }
    else if (inhabits(t, BinOpExpr))
        return evaluate_expr__as_BinOpExpr(t, ctx);
    else if (inhabits(t, BlockExpr))
        return evaluate_expr__as_BlockExpr(t, ctx);
    else if (inhabits(t, CondExpr))
        return evaluate_expr__as_CondExpr(t, ctx);
    else if (inhabits(t, Construction))
        return evaluate_expr__as_Construction(t, ctx);
    else if (inhabits(t, ElementEval))
        return evaluate_expr__as_ElementEval(t, ctx);
    else if (inhabits(t, FuncEval))
        return evaluate_expr__as_FuncEval(t, ctx);
    else if (inhabits(t, RoundExpr))
        return evaluate_expr__as_RoundExpr(t, ctx);
    else if (inhabits(t, UnOpExpr))
        return evaluate_expr__as_UnOpExpr(t, ctx);
    else if (inhabits(t, Expr)) {
        LVD_ABORT(LVD_FMT("unhandled Expr: " << t));
//         return t; // Just a plain Tuple.
    } else
        LVD_ABORT(LVD_FMT("attempting to evaluate_expr for a non-Expr: " << t));
}

void execute_stmt__as_SymbolDefn (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    auto symbol_defn_term = parse_SymbolDefn_Term(t);
    // Simply define the symbol in the current scope.
    ctx.current_scope()->define_symbol(symbol_defn_term.m_symbol_id, evaluate_expr_data(symbol_defn_term.m_defn, ctx));
    // If a SymbolDefn were a valid Expr, then return Void, or potentially return value, or symbol_id, or some reference.
}

void execute_stmt__as_Assignment (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    auto assignment_term = parse_Assignment_Term(t);
    // Simply assign the symbol in the current scope.
    ctx.current_scope()->resolve_symbol_nonconst(assignment_term.m_symbol_id) = evaluate_expr_data(assignment_term.m_value, ctx);
    // If an Assignment were a valid Expr, then return value, or symbol_id, or some reference.
}

void execute_stmt (sept::TupleTerm_c const &t, EvalCtx &ctx) {
    // TODO: Make this efficient -- some sort of poset search.
    if (false)
        { }
    else if (inhabits(t, Assignment))
        return execute_stmt__as_Assignment(t, ctx);
    else if (inhabits(t, SymbolDefn))
        return execute_stmt__as_SymbolDefn(t, ctx);
    else if (inhabits(t, Stmt))
        LVD_ABORT(LVD_FMT("unhandled Stmt: " << t));
    else
        LVD_ABORT(LVD_FMT("attempting to execute_stmt for a non-Stmt: " << t));
}
