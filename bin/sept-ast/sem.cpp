// 2021.04.19 - Victor Dods

#include "sem.hpp"

#include <cmath>
#include <lvd/comma.hpp>

namespace sem {

sept::Data evaluate_expr_data (sept::Data const &expr_data, EvalCtx &ctx) {
    // Look up the type pair in the evaluator map.
    auto const &evaluator_map = lvd::static_association_singleton<EvaluateExpr>();
    auto it = evaluator_map.find(std::type_index(expr_data.type()));
    if (it == evaluator_map.end()) {
//         throw std::runtime_error(LVD_FMT("Data type " << expr_data.type().name() << " is not registered in EvaluateExpr for use in evaluate_expr_data"));
        // Evaluation is the identity.  TODO: Maybe don't do this, because it makes missing registrations harder to detect.
        lvd::g_log << lvd::Log::wrn() << LVD_FMT("Data type " << expr_data.type().name() << " is not registered in EvaluateExpr for use in evaluate_expr_data; returning data unchanged.") << '\n';
        return expr_data;
    }

    return it->second(expr_data, ctx);
}

void execute_stmt_data (sept::Data const &stmt_data, EvalCtx &ctx) {
    // Look up the type pair in the evaluator map.
    auto const &evaluator_map = lvd::static_association_singleton<ExecuteStmt>();
    auto it = evaluator_map.find(std::type_index(stmt_data.type()));
    if (it == evaluator_map.end())
        throw std::runtime_error(LVD_FMT("Data type " << stmt_data.type().name() << " is not registered in ExecuteStmt for use in execute_stmt_data"));

    it->second(stmt_data, ctx);
}

std::ostream &operator<< (std::ostream &out, ExprArray_Term_c const &expr_array_term) {
    auto cspace_delim = lvd::make_comma_space_delimiter();
    out << "ExprArray_Term_c(";
    for (auto const &expr : expr_array_term)
        out << cspace_delim << expr;
    return out << ')';
}

std::ostream &operator<< (std::ostream &out, BinOpExpr_Term_c const &bin_op_expr_term) {
    return out << "BinOpExpr_Term_c(" << bin_op_expr_term.m_lhs_expr << ", " << bin_op_expr_term.m_bin_op << ", " << bin_op_expr_term.m_rhs_expr << ')';
}

std::ostream &operator<< (std::ostream &out, BlockExpr_Term_c const &block_expr_term) {
    return out << "BlockExpr_Term_c(" << block_expr_term.m_stmt_array << ", " << block_expr_term.m_final_expr << ')';
}

std::ostream &operator<< (std::ostream &out, CondExpr_Term_c const &cond_expr_term) {
    return out << "CondExpr_Term_c(" << cond_expr_term.m_condition << ", " << cond_expr_term.m_positive_expr << ", " << cond_expr_term.m_negative_expr << ')';
}

std::ostream &operator<< (std::ostream &out, FuncEval_Term_c const &func_eval_term) {
    return out << "FuncEval_Term_c(" << func_eval_term.m_func_symbol_id << ", " << func_eval_term.m_params << ')';
}

std::ostream &operator<< (std::ostream &out, ElementEval_Term_c const &element_eval_term) {
    return out << "ElementEval_Term_c(" << element_eval_term.m_container << ", " << element_eval_term.m_params << ')';
}

std::ostream &operator<< (std::ostream &out, Construction_Term_c const &construction_term) {
    return out << "Construction_Term_c(" << construction_term.m_type_to_construct << ", " << construction_term.m_params << ')';
}

std::ostream &operator<< (std::ostream &out, SymbolTypeDecl_Term_c const &symbol_type_decl_term) {
    return out << "SymbolTypeDecl_Term_c(" << symbol_type_decl_term.m_symbol_id << ", " << symbol_type_decl_term.m_decl_type << ')';
}

std::ostream &operator<< (std::ostream &out, SymbolDefn_Term_c const &symbol_defn_term) {
    return out << "SymbolDefn_Term_c(" << symbol_defn_term.m_symbol_id << ", " << symbol_defn_term.m_defn << ')';
}

std::ostream &operator<< (std::ostream &out, SymbolTypeDeclArray_Term_c const &v) {
    auto cspace_delim = lvd::make_comma_space_delimiter();
    out << "SymbolTypeDeclArray_Term_c(";
    for (auto const &element : v)
        out << cspace_delim << element;
    return out << ')';
}

std::ostream &operator<< (std::ostream &out, FuncPrototype_Term_c const &func_prototype_term) {
    return out << "FuncPrototype_Term_c(" << func_prototype_term.m_param_decls << ", " << func_prototype_term.m_codomain << ')';
}

std::ostream &operator<< (std::ostream &out, FuncLiteral_Term_c const &func_literal_term) {
    return out << "FuncLiteral_Term_c(" << func_literal_term.m_prototype << ", " << func_literal_term.m_body_expr << ')';
}

std::ostream &operator<< (std::ostream &out, Assignment_Term_c const &assignment_term) {
    return out << "Assignment_Term_c(" << assignment_term.m_symbol_id << ", " << assignment_term.m_value << ')';
}

std::ostream &operator<< (std::ostream &out, SymbolId_Term_c const &symbol_id_term) {
    return out << "SymbolId_Term_c(" << symbol_id_term.m_symbol_id << ')';
}

std::ostream &operator<< (std::ostream &out, ValueTerminal_Term_c const &value_terminal_term) {
    return out << "ValueTerminal_Term_c(" << value_terminal_term.m_value << ')';
}

std::ostream &operator<< (std::ostream &out, UnOpExpr_Term_c const &un_op_expr_term) {
    return out << "UnOpExpr_Term_c(" << un_op_expr_term.m_un_op << ", " << un_op_expr_term.m_operand << ')';
}

std::ostream &operator<< (std::ostream &out, RoundExpr_Term_c const &round_expr_term) {
    return out << "RoundExpr_Term_c(" << round_expr_term.m_expr_array << ')';
}

std::ostream &operator<< (std::ostream &out, SquareExpr_Term_c const &square_expr_term) {
    return out << "SquareExpr_Term_c(" << square_expr_term.m_expr_array << ')';
}

std::ostream &operator<< (std::ostream &out, CurlyExpr_Term_c const &curly_expr_term) {
    return out << "CurlyExpr_Term_c(" << curly_expr_term.m_expr_array << ')';
}

std::ostream &operator<< (std::ostream &out, Expr_Term_c const &expr_term) {
    return std::visit(
        lvd::Visitor_t{
            [&out](BinOpExpr_Term_c const &expr) -> std::ostream& { return out << expr; },
            [&out](BlockExpr_Term_c const &expr) -> std::ostream& { return out << expr; },
            [&out](CondExpr_Term_c const &expr) -> std::ostream& { return out << expr; },
            [&out](Construction_Term_c const &expr) -> std::ostream& { return out << expr; },
            [&out](ElementEval_Term_c const &expr) -> std::ostream& { return out << expr; },
            [&out](FuncEval_Term_c const &expr) -> std::ostream& { return out << expr; },
            [&out](RoundExpr_Term_c const &expr) -> std::ostream& { return out << expr; },
            [&out](SymbolId_Term_c const &expr) -> std::ostream& { return out << expr; },
            [&out](UnOpExpr_Term_c const &expr) -> std::ostream& { return out << expr; },
            [&out](ValueTerminal_Term_c const &expr) -> std::ostream& { return out << expr; }
        },
        expr_term
    );
}

ExprArray_Term_c parse_ExprArray_Term (sept::ArrayTerm_c const &a) {
    assert(inhabits(a, syn::ExprArray));
    ExprArray_Term_c retval;
    retval.reserve(a.size());
    for (auto const &element : a.elements())
        retval.emplace_back(parse_Expr_Term(element));
    return retval;
}

ExprArray_Term_c parse_ExprArray_Term (sept::Data const &d) {
    assert(inhabits_data(d, syn::ExprArray));
    return parse_ExprArray_Term(d.cast<sept::ArrayTerm_c const &>());
}

BinOpExpr_Term_c parse_BinOpExpr_Term (sept::TupleTerm_c const &t) {
    assert(inhabits(t, syn::BinOpExpr));
    return BinOpExpr_Term_c{t[0], t[1].cast<ASTNPTerm>(), t[2]};
}

BinOpExpr_Term_c parse_BinOpExpr_Term (sept::Data const &d) {
    assert(inhabits_data(d, sept::Tuple));
    return parse_BinOpExpr_Term(d.cast<sept::TupleTerm_c const &>());
}

UnOpExpr_Term_c parse_UnOpExpr_Term (sept::TupleTerm_c const &t) {
    assert(inhabits(t, syn::UnOpExpr));
    return UnOpExpr_Term_c{t[0].cast<ASTNPTerm>(), t[1]};
}

UnOpExpr_Term_c parse_UnOpExpr_Term (sept::Data const &d) {
    assert(inhabits_data(d, sept::Tuple));
    return parse_UnOpExpr_Term(d.cast<sept::TupleTerm_c const &>());
}

BlockExpr_Term_c parse_BlockExpr_Term (sept::TupleTerm_c const &t) {
    assert(inhabits(t, syn::BlockExpr));
    return BlockExpr_Term_c{
        t[0].cast<sept::ArrayTerm_c const &>(),
        t[1]
    };
}

BlockExpr_Term_c parse_BlockExpr_Term (sept::Data const &d) {
    assert(inhabits_data(d, sept::Tuple));
    return parse_BlockExpr_Term(d.cast<sept::TupleTerm_c const &>());
}

CondExpr_Term_c parse_CondExpr_Term (sept::TupleTerm_c const &t) {
    assert(inhabits(t, syn::CondExpr));
    // t[0], t[2], t[4] are If, Then, Else respectively.
    return CondExpr_Term_c{t[1], t[3], t[5]};
}

CondExpr_Term_c parse_CondExpr_Term (sept::Data const &d) {
    assert(inhabits_data(d, sept::Tuple));
    return parse_CondExpr_Term(d.cast<sept::TupleTerm_c const &>());
}

RoundExpr_Term_c parse_RoundExpr_Term (sept::Data const &d) {
    assert(inhabits_data(d, syn::RoundExpr));
    auto const &t = d.cast<sept::TupleTerm_c const &>();
    // t[0] and t[2] are the Round brackets, i.e. (), and are only used for syntactical distinction.
    return RoundExpr_Term_c{parse_ExprArray_Term(t[1])};
}

SquareExpr_Term_c parse_SquareExpr_Term (sept::Data const &d) {
    assert(inhabits_data(d, syn::SquareExpr));
    auto const &t = d.cast<sept::TupleTerm_c const &>();
    // t[0] and t[2] are the Square brackets, i.e. [], and are only used for syntactical distinction.
    return SquareExpr_Term_c{parse_ExprArray_Term(t[1])};
}

CurlyExpr_Term_c parse_CurlyExpr_Term (sept::Data const &d) {
    assert(inhabits_data(d, syn::CurlyExpr));
    auto const &t = d.cast<sept::TupleTerm_c const &>();
    // t[0] and t[2] are the Curly brackets, i.e. {}, and are only used for syntactical distinction.
    return CurlyExpr_Term_c{parse_ExprArray_Term(t[1])};
}

FuncEval_Term_c parse_FuncEval_Term (sept::Data const &d) {
    assert(inhabits_data(d, syn::FuncEval));
    auto const &t = d.cast<sept::TupleTerm_c const &>();
    return FuncEval_Term_c{
        t[0].cast<std::string const &>(),
        std::move(parse_RoundExpr_Term(t[1]).m_expr_array)
    };
}

ElementEval_Term_c parse_ElementEval_Term (sept::Data const &d) {
    assert(inhabits_data(d, syn::ElementEval));
    auto const &t = d.cast<sept::TupleTerm_c const &>();
    return ElementEval_Term_c{
        t[0],
        std::move(parse_SquareExpr_Term(t[1]).m_expr_array)
    };
}

Construction_Term_c parse_Construction_Term (sept::TupleTerm_c const &t) {
    assert(inhabits_data(t, syn::Construction));
    return Construction_Term_c{
        t[0],
        std::move(parse_CurlyExpr_Term(t[1]).m_expr_array)
    };
}

Construction_Term_c parse_Construction_Term (sept::Data const &d) {
    assert(inhabits_data(d, sept::Tuple));
    return parse_Construction_Term(d.cast<sept::TupleTerm_c const &>());
}

SymbolTypeDecl_Term_c parse_SymbolTypeDecl_Term (sept::Data const &d) {
    assert(inhabits_data(d, syn::SymbolTypeDecl));
    auto const &t = d.cast<sept::TupleTerm_c const &>();
    // Note that t[1] is DeclaredAs, which is only used for syntactical distinction.
    return SymbolTypeDecl_Term_c{
        t[0].cast<std::string const &>(),
        t[2]
    };
}

SymbolDefn_Term_c parse_SymbolDefn_Term (sept::TupleTerm_c const &t) {
    assert(inhabits_data(t, syn::SymbolDefn));
    // Note that t[1] is DefinedAs, which is only used for syntactical distinction.
    return SymbolDefn_Term_c{
        t[0].cast<std::string const &>(),
        t[2]
    };
}

SymbolDefn_Term_c parse_SymbolDefn_Term (sept::Data const &d) {
    assert(inhabits_data(d, sept::Tuple));
    return parse_SymbolDefn_Term(d.cast<sept::TupleTerm_c const &>());
}

SymbolTypeDeclArray_Term_c parse_SymbolTypeDeclArray_Term (sept::Data const &d) {
    assert(inhabits_data(d, syn::SymbolTypeDeclArray));
    auto const &a = d.cast<sept::ArrayTerm_c const &>();
    SymbolTypeDeclArray_Term_c retval;
    retval.reserve(a.size());
    for (auto const &symbol_type_decl : a)
        retval.emplace_back(parse_SymbolTypeDecl_Term(symbol_type_decl));
    return retval;
}

FuncPrototype_Term_c parse_FuncPrototype_Term (sept::Data const &d) {
    assert(inhabits_data(d, syn::FuncPrototype));
    auto const &t = d.cast<sept::TupleTerm_c const &>();
    // t[1] is MapsTo, which is only used for syntactical distinction.
    return FuncPrototype_Term_c{
        parse_SymbolTypeDeclArray_Term(t[0]),
        t[2]
    };
}

FuncLiteral_Term_c parse_FuncLiteral_Term (sept::Data const &d) {
    assert(inhabits_data(d, syn::FuncLiteral));
    auto const &t = d.cast<sept::TupleTerm_c const &>();
    return FuncLiteral_Term_c{
        parse_FuncPrototype_Term(t[0]),
        t[1]
    };
}

Assignment_Term_c parse_Assignment_Term (sept::TupleTerm_c const &t) {
    assert(inhabits_data(t, syn::Assignment));
    // Note that t[1] is AssignFrom, which is only used for syntactical distinction.
    return Assignment_Term_c{
        t[0].cast<std::string const &>(),
        t[2]
    };
}

Assignment_Term_c parse_Assignment_Term (sept::Data const &d) {
    assert(inhabits_data(d, sept::Tuple));
    return parse_Assignment_Term(d.cast<sept::TupleTerm_c const &>());
}

SymbolId_Term_c parse_SymbolId_Term (std::string const &s) {
    assert(inhabits(s, SymbolId));
    return SymbolId_Term_c{s};
}

SymbolId_Term_c parse_SymbolId_Term (sept::Data const &d) {
    assert(inhabits_data(d, SymbolId));
    return parse_SymbolId_Term(d.cast<std::string const &>());
}

ValueTerminal_Term_c parse_ValueTerminal_Term (sept::Data const &d) {
    assert(inhabits_data(d, syn::ValueTerminal));
    return ValueTerminal_Term_c{d};
}

Expr_Term_c parse_Expr_Term (sept::Data const &d) {
    assert(inhabits_data(d, syn::Expr));
    // TODO: Implement this search using a poset
    if (false)
        { }
    else if (inhabits_data(d, syn::BinOpExpr))
        return Expr_Term_c{parse_BinOpExpr_Term(d)};
    else if (inhabits_data(d, syn::BlockExpr))
        return Expr_Term_c{parse_BlockExpr_Term(d)};
    else if (inhabits_data(d, syn::CondExpr))
        return Expr_Term_c{parse_CondExpr_Term(d)};
    else if (inhabits_data(d, syn::Construction))
        return Expr_Term_c{parse_Construction_Term(d)};
    else if (inhabits_data(d, syn::ElementEval))
        return Expr_Term_c{parse_ElementEval_Term(d)};
    else if (inhabits_data(d, syn::FuncEval))
        return Expr_Term_c{parse_FuncEval_Term(d)};
    else if (inhabits_data(d, syn::RoundExpr))
        return Expr_Term_c{parse_RoundExpr_Term(d)};
    else if (inhabits_data(d, SymbolId))
        return Expr_Term_c{parse_SymbolId_Term(d)};
    else if (inhabits_data(d, syn::UnOpExpr))
        return Expr_Term_c{parse_UnOpExpr_Term(d)};
    else if (inhabits_data(d, syn::ValueTerminal))
        return Expr_Term_c{parse_ValueTerminal_Term(d)};
    else
        LVD_ABORT(LVD_FMT("data not recognized as Expr: " << d));
}

sept::Data evaluate_BinOpExpr_Term (BinOpExpr_Term_c const &bin_op_expr_term, EvalCtx &ctx) {
    auto evaled_lhs_expr = evaluate_expr_data(bin_op_expr_term.m_lhs_expr, ctx);
    auto evaled_rhs_expr = evaluate_expr_data(bin_op_expr_term.m_rhs_expr, ctx);
    switch (bin_op_expr_term.m_bin_op) {
        case ASTNPTerm::AND: return evaled_lhs_expr.cast<bool>() && evaled_rhs_expr.cast<bool>();
        case ASTNPTerm::OR:  return evaled_lhs_expr.cast<bool>() || evaled_rhs_expr.cast<bool>();
        case ASTNPTerm::XOR: return evaled_lhs_expr.cast<bool>() != evaled_rhs_expr.cast<bool>();
        case ASTNPTerm::ADD: return evaled_lhs_expr.cast<double>() + evaled_rhs_expr.cast<double>();
        case ASTNPTerm::SUB: return evaled_lhs_expr.cast<double>() - evaled_rhs_expr.cast<double>();
        case ASTNPTerm::MUL: return evaled_lhs_expr.cast<double>() * evaled_rhs_expr.cast<double>();
        case ASTNPTerm::DIV: return evaled_lhs_expr.cast<double>() / evaled_rhs_expr.cast<double>();
        case ASTNPTerm::POW: return std::pow(evaled_lhs_expr.cast<double>(), evaled_rhs_expr.cast<double>());
        default: LVD_ABORT(LVD_FMT("invalid ASTNPTerm for use as a BinOp: " << uint32_t(bin_op_expr_term.m_bin_op)));
    }
}

sept::Data evaluate_BlockExpr_Term (BlockExpr_Term_c const &block_expr_term, EvalCtx &ctx) {
    // Push a new scope for this block.
    auto scope_guard = ctx.push_scope();
    // Execute all the statements in the block before the final expression.
    execute_stmt__as_StmtArray(block_expr_term.m_stmt_array, ctx);
    // Evaluate the final expression, which is what renders the value of the BlockExpr
    return evaluate_expr_data(block_expr_term.m_final_expr, ctx);
}

sept::Data evaluate_CondExpr_Term (CondExpr_Term_c const &cond_expr_term, EvalCtx &ctx) {
    auto condition = evaluate_expr_data(cond_expr_term.m_condition, ctx).cast<bool>();
    return evaluate_expr_data(condition ? cond_expr_term.m_positive_expr : cond_expr_term.m_negative_expr, ctx);
}

sept::Data evaluate_Construction_Term (Construction_Term_c const &construction_term, EvalCtx &ctx) {
    auto evaled_type_to_construct = evaluate_expr_data(construction_term.m_type_to_construct, ctx);
    auto evaled_param_array = evaluate_ExprArray_Term(construction_term.m_params, ctx);
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << '\n' << lvd::IndentGuard()
//                << LVD_REFLECT(evaled_type_to_construct) << '\n'
//                << LVD_REFLECT(construction_term.m_params.m_expr_array) << '\n'
//                << LVD_REFLECT(evaled_param_array) << '\n';
    // TEMP HACK -- this unwraps the ExprArray, which is assumed to contain only a single element for now.
    assert(evaled_param_array.size() == 1);
    return evaled_type_to_construct(evaled_param_array[0]).deref();
}

sept::Data evaluate_ElementEval_Term (ElementEval_Term_c const &element_eval_term, EvalCtx &ctx) {
    auto evaled_container = evaluate_expr_data(element_eval_term.m_container, ctx);
    auto evaled_param_array = evaluate_ExprArray_Term(element_eval_term.m_params, ctx);
    // TEMP HACK -- this unwraps the ExprArray, which is assumed to contain only a single element for now.
    assert(evaled_param_array.size() == 1);
    return evaled_container[evaled_param_array[0]].deref();
}

sept::ArrayTerm_c evaluate_ExprArray_Term (ExprArray_Term_c const &expr_array_term, EvalCtx &ctx) {
    sept::DataVector evaluated_elements;
    evaluated_elements.reserve(expr_array_term.size());
    for (auto const &element : expr_array_term)
        evaluated_elements.emplace_back(evaluate_expr_data(element, ctx));
    return sept::ArrayTerm_c{std::move(evaluated_elements)};
}

sept::Data evaluate_FuncEval_Term (FuncEval_Term_c const &func_eval_term, EvalCtx &ctx) {
    lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << '\n'
               << LVD_REFLECT(func_eval_term.m_func_symbol_id) << '\n'
               << LVD_REFLECT(func_eval_term.m_params) << '\n';

    // Resolve the function symbol
    // TODO: Later this will turn into an expression that produces a function.
    auto func_literal = parse_FuncLiteral_Term(ctx.current_scope()->resolve_symbol_const(func_eval_term.m_func_symbol_id));
//     lvd::g_log << lvd::Log::trc() << '\n' << LVD_CALL_SITE() << " - " << LVD_REFLECT(func_literal) << '\n';

//     assert(sept::inhabits_data(func, FuncLiteral));
    auto evaled_param_array = evaluate_ExprArray_Term(func_eval_term.m_params, ctx);
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << " - " << LVD_REFLECT(evaled_param_array) << '\n';

    // Identify the parameter symbol decl
    // NOTE: This probably copy-constructs, instead of returning a const ref.
    // TODO: Could have the stuff return a MemRef.
    auto const &func_prototype = func_literal.m_prototype;
    // Check that there are the expected number of parameters in the FuncEval
    // TODO: Could check this earlier.
    if (func_prototype.m_param_decls.size() != evaled_param_array.size())
        throw std::runtime_error(LVD_FMT("Expected " << func_prototype.m_param_decls.size() << " parameters in call to function " << func_eval_term.m_func_symbol_id << ", but got " << evaled_param_array.size()));
    // Now check that all the param types are as expected.
    for (size_t i = 0; i < func_prototype.m_param_decls.size(); ++i) {
        auto const &evaled_param = evaled_param_array[i];
        auto const &param_decl = func_prototype.m_param_decls[i];
        // NOTE/TEMP HACK: This will resolve any symbolic references in the param types using the local symbol table,
        // and not the symbol table of the scope that the function was declared in.  TODO: Fix this.
        auto evaled_param_decl_type = evaluate_expr_data(param_decl.m_decl_type, ctx);
        if (!inhabits_data(evaled_param, evaled_param_decl_type))
            throw std::runtime_error(LVD_FMT("In parameter " << i << " in call to function " << func_eval_term.m_func_symbol_id << ": Expected a value of type " << evaled_param_decl_type << " but got " << evaled_param << " (which has abstract type " << sept::abstract_type_of_data(evaled_param) << ')'));
    }

    auto return_type_eval = evaluate_expr_data(func_prototype.m_codomain, ctx);

    // Push a context, define the function param(s).
    auto scope_guard = ctx.push_scope();
    for (size_t i = 0; i < func_prototype.m_param_decls.size(); ++i) {
        auto const &param_symbol_id = func_prototype.m_param_decls[i].m_symbol_id;
        auto &evaled_param = evaled_param_array[i];
        ctx.current_scope()->define_symbol(param_symbol_id, std::move(evaled_param));
    }
    // Evaluate the function body using the new scope.
    // Have to deref the data, since this might be a reference.
    auto retval = evaluate_expr_data(func_literal.m_body_expr, ctx).deref();
    // Check that the parameter is the correct type
    if (!sept::inhabits_data(retval, return_type_eval))
        throw std::runtime_error(LVD_FMT("Expected return value " << retval << " to evaluate to a term of type " << func_prototype.m_codomain << " but it didn't; retval: " << retval.deref()));
    return retval;
}

sept::ArrayTerm_c evaluate_RoundExpr_Term (RoundExpr_Term_c const &round_expr_term, EvalCtx &ctx) {
    return evaluate_ExprArray_Term(round_expr_term.m_expr_array, ctx);
}

sept::Data evaluate_SymbolId_Term (SymbolId_Term_c const &symbol_id_term, EvalCtx &ctx) {
    return sept::LocalSymRef(symbol_id_term.m_symbol_id, ctx.current_scope());
}

sept::Data evaluate_UnOpExpr_Term (UnOpExpr_Term_c const &un_op_expr_term, EvalCtx &ctx) {
    auto evaled_operand = evaluate_expr_data(un_op_expr_term.m_operand, ctx);
    switch (un_op_expr_term.m_un_op) {
        case ASTNPTerm::NOT: return !evaled_operand.cast<bool>();
        case ASTNPTerm::NEG: return -evaled_operand.cast<double>();
        default: LVD_ABORT(LVD_FMT("invalid ASTNPTerm for use as an UnOp: " << uint32_t(un_op_expr_term.m_un_op)));
    }
}

sept::Data evaluate_ValueTerminal_Term (ValueTerminal_Term_c const &value_terminal_term, EvalCtx &ctx) {
    return evaluate_expr_data(value_terminal_term.m_value, ctx);
}

sept::Data evaluate_Expr_Term (Expr_Term_c const &expr_term, EvalCtx &ctx) {
    return std::visit(
        lvd::Visitor_t{
            [&ctx](BinOpExpr_Term_c const &expr) -> sept::Data { return evaluate_BinOpExpr_Term(expr, ctx); },
            [&ctx](BlockExpr_Term_c const &expr) -> sept::Data { return evaluate_BlockExpr_Term(expr, ctx); },
            [&ctx](CondExpr_Term_c const &expr) -> sept::Data { return evaluate_CondExpr_Term(expr, ctx); },
            [&ctx](Construction_Term_c const &expr) -> sept::Data { return evaluate_Construction_Term(expr, ctx); },
            [&ctx](ElementEval_Term_c const &expr) -> sept::Data { return evaluate_ElementEval_Term(expr, ctx); },
            [&ctx](FuncEval_Term_c const &expr) -> sept::Data { return evaluate_FuncEval_Term(expr, ctx); },
            [&ctx](RoundExpr_Term_c const &expr) -> sept::Data { return evaluate_RoundExpr_Term(expr, ctx); },
            [&ctx](SymbolId_Term_c const &expr) -> sept::Data { return evaluate_SymbolId_Term(expr, ctx); },
            [&ctx](UnOpExpr_Term_c const &expr) -> sept::Data { return evaluate_UnOpExpr_Term(expr, ctx); },
            [&ctx](ValueTerminal_Term_c const &expr) -> sept::Data { return evaluate_ValueTerminal_Term(expr, ctx); }
        },
        expr_term
    );
}

void execute_stmt__as_StmtArray (sept::ArrayTerm_c const &stmt_array, EvalCtx &ctx) {
    assert(inhabits(stmt_array, syn::StmtArray));
    for (auto const &stmt : stmt_array.elements())
        execute_stmt_data(stmt, ctx);
}

void execute_stmt__as_StmtArray (sept::Data const &stmt, EvalCtx &ctx) {
    assert(inhabits_data(stmt, sept::Array));
    execute_stmt__as_StmtArray(stmt.cast<sept::ArrayTerm_c const &>(), ctx);
}

//
// BEGIN TEMP HACK
//

sept::Data evaluate_expr (sept::TupleTerm_c const &t, sem::EvalCtx &ctx) {
    // TODO: Make this efficient -- some sort of poset search.
    if (false)
        { }
    else if (inhabits(t, syn::BinOpExpr))
        return syn::evaluate_expr__as_BinOpExpr(t, ctx);
    else if (inhabits(t, syn::BlockExpr))
        return syn::evaluate_expr__as_BlockExpr(t, ctx);
    else if (inhabits(t, syn::CondExpr))
        return syn::evaluate_expr__as_CondExpr(t, ctx);
    else if (inhabits(t, syn::Construction))
        return syn::evaluate_expr__as_Construction(t, ctx);
    else if (inhabits(t, syn::ElementEval))
        return syn::evaluate_expr__as_ElementEval(t, ctx);
    else if (inhabits(t, syn::FuncEval))
        return syn::evaluate_expr__as_FuncEval(t, ctx);
    else if (inhabits(t, syn::RoundExpr))
        return syn::evaluate_expr__as_RoundExpr(t, ctx);
    else if (inhabits(t, syn::UnOpExpr))
        return syn::evaluate_expr__as_UnOpExpr(t, ctx);
    else if (inhabits(t, syn::Expr)) {
        LVD_ABORT(LVD_FMT("unhandled Expr: " << t));
//         return t; // Just a plain Tuple.
    } else
        LVD_ABORT(LVD_FMT("attempting to evaluate_expr for a non-Expr: " << t));
}

void execute_stmt (sept::TupleTerm_c const &t, sem::EvalCtx &ctx) {
    // TODO: Make this efficient -- some sort of poset search.
    if (false)
        { }
    else if (inhabits(t, syn::Assignment))
        return syn::execute_stmt__as_Assignment(t, ctx);
    else if (inhabits(t, syn::SymbolDefn))
        return syn::execute_stmt__as_SymbolDefn(t, ctx);
    else if (inhabits(t, syn::Stmt))
        LVD_ABORT(LVD_FMT("unhandled Stmt: " << t));
    else
        LVD_ABORT(LVD_FMT("attempting to execute_stmt for a non-Stmt: " << t));
}

//
// END TEMP HACK
//
} // end namespace sem
