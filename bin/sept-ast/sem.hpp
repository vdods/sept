// 2021.04.19 - Victor Dods

// Includes from this program's source
#include "core.hpp"
#include "syn.hpp"

#include <functional>
#include <lvd/variant.hpp>
#include "sept/ArrayTerm.hpp"
#include "sept/Data.hpp"

//
// StaticAssociation_t for evaluate_expr_data
//

class EvalCtx;

using EvaluateExprFunction = std::function<sept::Data(sept::Data const &expr, EvalCtx &ctx)>;
using DataEvaluateExprFunctionMap = std::unordered_map<std::type_index,EvaluateExprFunction>;
LVD_STATIC_ASSOCIATION_DEFINE(EvaluateExpr, DataEvaluateExprFunctionMap)

#define SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID__EVALUATOR(ExprType, unique_id, evaluator) \
    LVD_STATIC_ASSOCIATION_REGISTER( \
        EvaluateExpr, \
        unique_id, \
        std::type_index(typeid(ExprType)), \
        evaluator \
    )
#define SEPT__REGISTER__EVALUATE_EXPR__EVALUATOR(ExprType, evaluator) \
    SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID__EVALUATOR(ExprType, ExprType, evaluator)
#define SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID(ExprType, unique_id) \
    SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID__EVALUATOR( \
        ExprType, \
        unique_id, \
        [](sept::Data const &expr_data, EvalCtx &ctx) -> sept::Data { \
            auto const &expr = expr_data.cast<ExprType const &>(); \
            return evaluate_expr(expr, ctx); \
        } \
    )
#define SEPT__REGISTER__EVALUATE_EXPR(ExprType) \
    SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID(ExprType, ExprType)

sept::Data evaluate_expr_data (sept::Data const &expr_data, EvalCtx &ctx);

//
// StaticAssociation_t for execute_stmt_data
//

using ExecuteStmtFunction = std::function<void(sept::Data const &stmt, EvalCtx &ctx)>;
using DataExecuteStmtFunctionMap = std::unordered_map<std::type_index,ExecuteStmtFunction>;
LVD_STATIC_ASSOCIATION_DEFINE(ExecuteStmt, DataExecuteStmtFunctionMap)

#define SEPT__REGISTER__EXECUTE_STMT__GIVE_ID__EVALUATOR(StmtType, unique_id, evaluator) \
    LVD_STATIC_ASSOCIATION_REGISTER( \
        ExecuteStmt, \
        unique_id, \
        std::type_index(typeid(StmtType)), \
        evaluator \
    )
#define SEPT__REGISTER__EXECUTE_STMT__EVALUATOR(StmtType, evaluator) \
    SEPT__REGISTER__EXECUTE_STMT__GIVE_ID__EVALUATOR(StmtType, StmtType, evaluator)
#define SEPT__REGISTER__EXECUTE_STMT__GIVE_ID(StmtType, unique_id) \
    SEPT__REGISTER__EXECUTE_STMT__GIVE_ID__EVALUATOR( \
        StmtType, \
        unique_id, \
        [](sept::Data const &stmt_data, EvalCtx &ctx) -> void { \
            auto const &stmt = stmt_data.cast<StmtType const &>(); \
            execute_stmt(stmt, ctx); \
        } \
    )
#define SEPT__REGISTER__EXECUTE_STMT(StmtType) \
    SEPT__REGISTER__EXECUTE_STMT__GIVE_ID(StmtType, StmtType)

void execute_stmt_data (sept::Data const &stmt_data, EvalCtx &ctx);

//
// Semantic term parsing from syntactic term.
//
// NOTE: For now, these all use values instead of references.  Ideally they would use references
// so that copying isn't necessary, but there's difficulty with invalid access to freed memory.
//

class BinOpExpr_Term_c;
class BlockExpr_Term_c;
class CondExpr_Term_c;
class Construction_Term_c;
class ElementEval_Term_c;
class FuncEval_Term_c;
class RoundExpr_Term_c;
class SymbolId_Term_c;
class UnOpExpr_Term_c;
class ValueTerminal_Term_c;

using Expr_Term_c = std::variant<
    BinOpExpr_Term_c,
    BlockExpr_Term_c,
    CondExpr_Term_c,
    Construction_Term_c,
    ElementEval_Term_c,
    FuncEval_Term_c,
    RoundExpr_Term_c,
    SymbolId_Term_c,
    UnOpExpr_Term_c,
    ValueTerminal_Term_c
>;
using ExprArray_Term_c = std::vector<Expr_Term_c>;

struct BinOpExpr_Term_c {
    sept::Data m_lhs_expr;
    ASTNPTerm m_bin_op;
    sept::Data m_rhs_expr;
};

struct UnOpExpr_Term_c {
    ASTNPTerm m_un_op;
    sept::Data m_operand;
};

struct BlockExpr_Term_c {
    sept::ArrayTerm_c m_stmt_array;
    // TODO: Eventually this would be something like Expr_Term_c, which would be a variant.
    sept::Data m_final_expr;
};

struct CondExpr_Term_c {
    sept::Data m_condition;
    sept::Data m_positive_expr;
    sept::Data m_negative_expr;
};

struct RoundExpr_Term_c {
    ExprArray_Term_c m_expr_array;
};

struct SquareExpr_Term_c {
    ExprArray_Term_c m_expr_array;
};

struct CurlyExpr_Term_c {
    ExprArray_Term_c m_expr_array;
};

struct FuncEval_Term_c {
    std::string m_func_symbol_id;
    ExprArray_Term_c m_params;
};

struct ElementEval_Term_c {
    // TODO: This would eventually be destructured into some Expr_Term_c type which is probably a std::variant.
    sept::Data m_container;
    ExprArray_Term_c m_params;
};

struct Construction_Term_c {
    // TODO: This would eventually be destructured into some TypeExpr_Term_c type which is probably a std::variant.
    sept::Data m_type_to_construct;
    ExprArray_Term_c m_params;
};

// TODO
// - Add `sept::TupleTerm_c syntactical_form () const` so this can easily be turned back into the syntactical form (though
//   this would not be able to use references and would necessarily copy, unless sept::MemRef were used).
// - Maybe distinguish `SymbolTypeDecl = sept::Tuple(SymbolId, sept::FormalTypeOf(DeclaredAs), TypeExpr_as_Ref)` by calling it
//   something like `SyntacticalSymbolTypeDecl`, since the syntactical one is just meant as a syntactical representation.
struct SymbolTypeDecl_Term_c {
    std::string m_symbol_id;
    // TODO: If some sort of validation is done, this could potentially be destructured into TypeExpr or something.
    sept::Data m_decl_type;
};

using SymbolTypeDeclArray_Term_c = std::vector<SymbolTypeDecl_Term_c>;

struct SymbolDefn_Term_c {
    std::string m_symbol_id;
    sept::Data m_defn;
};

struct FuncPrototype_Term_c {
    SymbolTypeDeclArray_Term_c m_param_decls;
    // TODO: This would eventually be TypeExpr
    sept::Data m_codomain;
};

struct FuncLiteral_Term_c {
    FuncPrototype_Term_c m_prototype;
    sept::Data m_body_expr;
};

struct Assignment_Term_c {
    std::string m_symbol_id;
    sept::Data m_value;
};

struct SymbolId_Term_c {
    std::string m_symbol_id;
};

// TODO: Make this a std::variant -- or not?  sept::Data is more flexible and extensible,
// and the memory footprint is probably smaller (though it uses heap memory for larger types)
struct ValueTerminal_Term_c {
    sept::Data m_value;
};

std::ostream &operator<< (std::ostream &out, Expr_Term_c const &expr_term);
std::ostream &operator<< (std::ostream &out, ExprArray_Term_c const &expr_array_term);
std::ostream &operator<< (std::ostream &out, BinOpExpr_Term_c const &bin_op_expr_term);
std::ostream &operator<< (std::ostream &out, UnOpExpr_Term_c const &un_op_expr_term);
std::ostream &operator<< (std::ostream &out, BlockExpr_Term_c const &block_expr_term);
std::ostream &operator<< (std::ostream &out, CondExpr_Term_c const &cond_expr_term);
std::ostream &operator<< (std::ostream &out, RoundExpr_Term_c const &round_expr_term);
std::ostream &operator<< (std::ostream &out, SquareExpr_Term_c const &square_expr_term);
std::ostream &operator<< (std::ostream &out, CurlyExpr_Term_c const &curly_expr_term);
std::ostream &operator<< (std::ostream &out, FuncEval_Term_c const &func_eval_term);
std::ostream &operator<< (std::ostream &out, ElementEval_Term_c const &element_eval_term);
std::ostream &operator<< (std::ostream &out, Construction_Term_c const &construction_term);
std::ostream &operator<< (std::ostream &out, SymbolTypeDecl_Term_c const &symbol_type_decl_term);
std::ostream &operator<< (std::ostream &out, SymbolDefn_Term_c const &symbol_defn_term);
std::ostream &operator<< (std::ostream &out, SymbolTypeDeclArray_Term_c const &v);
std::ostream &operator<< (std::ostream &out, FuncPrototype_Term_c const &func_prototype_term);
std::ostream &operator<< (std::ostream &out, FuncLiteral_Term_c const &func_literal_term);
std::ostream &operator<< (std::ostream &out, Assignment_Term_c const &assignment_term);
std::ostream &operator<< (std::ostream &out, SymbolId_Term_c const &symbol_id_term);
std::ostream &operator<< (std::ostream &out, ValueTerminal_Term_c const &value_terminal_term);
std::ostream &operator<< (std::ostream &out, ExprArray_Term_c const &expr_array_term);

ExprArray_Term_c parse_ExprArray_Term (sept::ArrayTerm_c const &a);
ExprArray_Term_c parse_ExprArray_Term (sept::Data const &d);
BinOpExpr_Term_c parse_BinOpExpr_Term (sept::TupleTerm_c const &t);
BinOpExpr_Term_c parse_BinOpExpr_Term (sept::Data const &d);
UnOpExpr_Term_c parse_UnOpExpr_Term (sept::TupleTerm_c const &t);
UnOpExpr_Term_c parse_UnOpExpr_Term (sept::Data const &d);
BlockExpr_Term_c parse_BlockExpr_Term (sept::TupleTerm_c const &t);
BlockExpr_Term_c parse_BlockExpr_Term (sept::Data const &d);
CondExpr_Term_c parse_CondExpr_Term (sept::TupleTerm_c const &t);
CondExpr_Term_c parse_CondExpr_Term (sept::Data const &d);
RoundExpr_Term_c parse_RoundExpr_Term (sept::Data const &d);
SquareExpr_Term_c parse_SquareExpr_Term (sept::Data const &d);
CurlyExpr_Term_c parse_CurlyExpr_Term (sept::Data const &d);
FuncEval_Term_c parse_FuncEval_Term (sept::Data const &d);
ElementEval_Term_c parse_ElementEval_Term (sept::Data const &d);
Construction_Term_c parse_Construction_Term (sept::TupleTerm_c const &t);
Construction_Term_c parse_Construction_Term (sept::Data const &d);
SymbolTypeDecl_Term_c parse_SymbolTypeDecl_Term (sept::Data const &d);
SymbolDefn_Term_c parse_SymbolDefn_Term (sept::TupleTerm_c const &t);
SymbolDefn_Term_c parse_SymbolDefn_Term (sept::Data const &d);
SymbolTypeDeclArray_Term_c parse_SymbolTypeDeclArray_Term (sept::Data const &d);
FuncPrototype_Term_c parse_FuncPrototype_Term (sept::Data const &d);
FuncLiteral_Term_c parse_FuncLiteral_Term (sept::Data const &d);
Assignment_Term_c parse_Assignment_Term (sept::TupleTerm_c const &t);
Assignment_Term_c parse_Assignment_Term (sept::Data const &d);
SymbolId_Term_c parse_SymbolId_Term (std::string const &s);
SymbolId_Term_c parse_SymbolId_Term (sept::Data const &d);
ValueTerminal_Term_c parse_ValueTerminal_Term (sept::Data const &d);
Expr_Term_c parse_Expr_Term (sept::Data const &d);
ExprArray_Term_c parse_ExprArray_Term (sept::ArrayTerm_c const &a);

inline bool inhabits (ValueTerminal_Term_c const &vt, sept::Data const &d) {
    return inhabits_data(vt.m_value, d);
}

//
// evaluate
//

sept::Data evaluate_BinOpExpr_Term (BinOpExpr_Term_c const &bin_op_expr_term, EvalCtx &ctx);
sept::Data evaluate_BlockExpr_Term (BlockExpr_Term_c const &block_expr_term, EvalCtx &ctx);
sept::Data evaluate_CondExpr_Term (CondExpr_Term_c const &cond_expr_term, EvalCtx &ctx);
sept::Data evaluate_Construction_Term (Construction_Term_c const &construction_term, EvalCtx &ctx);
sept::Data evaluate_ElementEval_Term (ElementEval_Term_c const &element_eval_term, EvalCtx &ctx);
sept::ArrayTerm_c evaluate_ExprArray_Term (ExprArray_Term_c const &expr_array_term, EvalCtx &ctx);
sept::Data evaluate_FuncEval_Term (FuncEval_Term_c const &func_eval_term, EvalCtx &ctx);
sept::ArrayTerm_c evaluate_RoundExpr_Term (RoundExpr_Term_c const &round_expr_term, EvalCtx &ctx);
sept::Data evaluate_SymbolId_Term (SymbolId_Term_c const &symbol_id_term, EvalCtx &ctx);
sept::Data evaluate_UnOpExpr_Term (UnOpExpr_Term_c const &un_op_expr_term, EvalCtx &ctx);
sept::Data evaluate_ValueTerminal_Term (ValueTerminal_Term_c const &value_terminal_term, EvalCtx &ctx);
sept::Data evaluate_Expr_Term (Expr_Term_c const &expr_term, EvalCtx &ctx);

inline bool evaluate_expr (bool const &expr, EvalCtx &ctx) {
    return expr;
}
inline sept::BoolTerm_c evaluate_expr (sept::BoolTerm_c const &expr, EvalCtx &ctx) {
    return expr;
}
inline double evaluate_expr (double const &expr, EvalCtx &ctx) {
    return expr;
}
inline sept::Array_c evaluate_expr (sept::Array_c const &expr, EvalCtx &ctx) {
    return expr;
}
inline sept::ArrayESTerm_c evaluate_expr (sept::ArrayESTerm_c const &expr, EvalCtx &ctx) {
    // TODO: Evaluate the E and S components, since they could be expressions themselves.
    return expr;
}
inline sept::ArrayETerm_c evaluate_expr (sept::ArrayETerm_c const &expr, EvalCtx &ctx) {
    // TODO: Evaluate the E component, since it could be an expression itself.
    return expr;
}
inline sept::ArraySTerm_c evaluate_expr (sept::ArraySTerm_c const &expr, EvalCtx &ctx) {
    // TODO: Evaluate the S component, since it could be an expression itself.
    return expr;
}
inline sept::ArrayTerm_c evaluate_expr (sept::ArrayTerm_c const &expr, EvalCtx &ctx) {
    return evaluate_expr__as_ExprArray(expr, ctx);
}
inline sept::Tuple_c evaluate_expr (sept::Tuple_c const &expr, EvalCtx &ctx) {
    return expr;
}
inline sept::Data evaluate_expr (std::string const &expr, EvalCtx &ctx) {
    return evaluate_expr__as_SymbolId(expr, ctx);
}
inline sept::Data evaluate_expr (ExprArray_Term_c const &expr, EvalCtx &ctx) {
    return evaluate_ExprArray_Term(expr, ctx);
}
inline sept::Data evaluate_expr (ValueTerminal_Term_c const &expr, EvalCtx &ctx) {
    return evaluate_ValueTerminal_Term(expr, ctx);
}
inline sept::Data evaluate_expr (Expr_Term_c const &expr, EvalCtx &ctx) {
    return evaluate_Expr_Term(expr, ctx);
}

//
// execute
//

void execute_stmt__as_StmtArray (sept::ArrayTerm_c const &stmt_array, EvalCtx &ctx);
void execute_stmt__as_StmtArray (sept::Data const &stmt, EvalCtx &ctx);
