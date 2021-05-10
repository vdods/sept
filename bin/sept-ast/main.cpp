// 2021.03.27 - Victor Dods

// Includes from this program's source
#include "core.hpp"
#include "sem.hpp"
#include "syn.hpp"

#include <cmath>

/*
AST design notes
-   Make a simple functional language with arithmetic operations, functions, conditionals, etc.
-   Goal is to have sept GUI be able to view and work with these AST classes natively and naturally.
-   Terms
    -   LogicalBinOp, which has inhabitants (this could be made into a Rust-style enum)
        -   And
        -   Or
        -   Xor
    -   LogicalUnOp, which has inhabitants (this could be made into a Rust-style enum)
        -   Not
    -   LogicalBinOpExpr := Tuple(LogicalExpr, LogicalBinOp, LogicalExpr)
    -   LogicalUnOpExpr := Tuple(LogicalUnOp, LogicalExpr)
    -   LogicalParenExpr := Tuple(LogicalExpr)
    -   LogicalCondExpr := Tuple(LogicalExpr, LogicalExpr, LogicalExpr) // (condition, positive_expr, negative_expr)
    -   LogicalExpr := Union(Bool, LogicalBinOpExpr, LogicalCondExpr, LogicalParenExpr, LogicalUnOpExpr)
    -   NumericBinOp, which has inhabitants (this could be made into a Rust-style enum)
        -   Add
        -   Sub
        -   Mul
        -   Div
        -   Pow
    -   NumericUnOp, which has inhabitants (this could be made into a Rust-style enum)
        -   Neg
    -   NumericBinOpExpr := Tuple(NumericExpr, NumericBinOp, NumericExpr)
    -   NumericUnOpExpr := Tuple(NumericUnOp, NumericExpr)
    -   NumericParenExpr := Tuple(NumericExpr)
    -   NumericCondExpr := Tuple(LogicalExpr, NumericExpr, NumericExpr)
    -   NumericExpr := Union(Float64, NumericBinOpExpr, NumericUnOpExpr, NumericParenExpr)
-   Can use Array instead of Tuple for now, since Array is a weakly-typed Tuple.
    Note that it is only because the various Tuple-based type definitions are mutually disjoint
    that a type without a semantic subtype tag (e.g. LogicalParenExpr(LogicalExpr)) can be used.
-   Now that the type system is much more free and flexible, can define Expr and CondExpr to not
    be Numeric- or Logical-, but rather form a type that overlaps both.
    -   ExprKind := TypeSet(LogicalExpr, NumericExpr)
    -   Expr := UnionOfTypeSet(ExprKind)
    -   CondExpr := Tuple(LogicalExpr, X, X) where X:ExprKind
    The latter definition, for CondExpr, is not possible (yet) declaratively, but it is possible
    by a definition of `inhabits`.
-   Other stuff that could be useful
    -   BinOp := Union(LogicalBinOp, NumericBinOp)
    -   LogicalThing := Union(LogicalBinOp, LogicalBinOpExpr, LogicalCondExpr, LogicalExpr, LogicalParenExpr, LogicalUnOp, LogicalUnOpExpr)
    -   NumericThing := Union(NumericBinOp, NumericBinOpExpr, NumericCondExpr, NumericExpr, NumericParenExpr, NumericUnOp, NumericUnOpExpr)
-   Idea for improvement: Just make a single enum for BinOpTerm_c and a single enum for UnOpTerm_c,
    containing all classes of BinOps and UnOps respectively, and then define semantic classes
    that select out subsets of those binops.  Since there is no reason to have distinct concrete
    types for these things.  In fact, because the ops are just non-parametric terms, there could
    potentially be a single enum for all ops, and semantic classes are used to select different
    semantic subtypes.
-   Notes on adding an `inhabits` binop to LogicalBinOp
    -   So far, it was assumed that LogicalBinOps accept LogicalExpr (i.e. something that evaluates to LogicalValueTerminal)
        and produce LogicalValueTerminal.
    -   But `x inhabits T` has differently typed arguments.  `x` can be any term (including a type), and `T` can be any type.
    -   So the name "LogicalBinOp" is rather underspecified.  It really needs to specify that both arguments are
        LogicalExpr and the output is LogicalValueTerminal.  Then there is "room" for another type name which `harbors`
        BinOps taking other types but producing LogicalValueTerminal, and then LogicalBinOpExpr would be any BinOpExpr
        that produces LogicalValueTerminal.
-   Adding functions and type constructions and type declarations
    -   SymbolTypeDecl is Tuple(SymbolId, DeclaredAs, TypeExpr) i.e. `x : T`
        Eventually include `where` clauses, i.e. `x : T where <LogicalExpr in x>`; this would implicitly declare a subtype of T.
    -   FuncType is Tuple(TypeExpr, MapsTo, TypeExpr) i.e. `T1 -> T2`
    -   FuncPrototype Tuple(Parenthesized(SymbolTypeDecl), MapsTo, TypeExpr) i.e. `(x:T1) -> T2`
        or maybe also include Tuple(Parenthesized(SymbolTypeDecl), MapsTo, Parenthesized(SymbolTypeDecl)) i.e. `(x:T1) -> (y:T2)`
    -   TypeExpr := Union(TypeTerminal, FuncType) -- add more later, such as tuples, arrays, etc
    -   FuncValue := Tuple(FuncPrototype, Expr) -- e.g. `(x:Float64) -> Float64 { x - x^3.0 / factorial(3.0) + x^5.0 / factorial(5.0) }
    -   Later, do dependent functions.  This is a lot more complicated though and isn't a good first step.
    -   In order to implement evaluate for SymbolId, it's required to implement SymbolicRef
-   Idea: Use round brackets, i.e. parens, exclusively for syntactical grouping, and not to indicate data types.
-   Now that there's a distinction between syntactical terms and semantic terms, formalize that:
    -   Use namespaces syn and sem and sort things out.
    -   Make a type hierarchy for sem terms just like how syn terms have theirs.
    -   Make sem terms be geared toward efficiency and have them be reasonable C++ patterns.
*/

//
// sept data model registrations
//

namespace sept {
SEPT__REGISTER__PRINT(ASTNPTerm)
SEPT__REGISTER__PRINT__GIVE_ID(syn::BinOp_c, __syn__BinOp_c__)
SEPT__REGISTER__PRINT__GIVE_ID(sem::Expr_Term_c, __sem__Expr_Term_c__)
SEPT__REGISTER__PRINT__GIVE_ID(syn::SymbolId_c, __syn__SymbolId_c__)
SEPT__REGISTER__PRINT__GIVE_ID(syn::UnOp_c, __syn__UnOp_c__)
SEPT__REGISTER__PRINT__GIVE_ID(char const *, __char_const_ptr__)

SEPT__REGISTER__EQ__GIVE_ID(syn::BinOp_c, __syn__BinOp_c__)
SEPT__REGISTER__EQ__GIVE_ID(syn::UnOp_c, __syn__UnOp_c__)
SEPT__REGISTER__EQ(ASTNPTerm)
SEPT__REGISTER__EQ__GIVE_ID(syn::SymbolId_c, __syn__SymbolId_c__)

SEPT__REGISTER__ABSTRACT_TYPE_OF__GIVE_ID(syn::BinOp_c, __syn__BinOp_c__)
SEPT__REGISTER__ABSTRACT_TYPE_OF__GIVE_ID(syn::UnOp_c, __syn__UnOp_c__)

SEPT__REGISTER__INHABITS__GIVE_ID__NONDATA(ASTNPTerm,     syn::BinOp_c, __ASTNPTerm___syn__BinOp_c__)
SEPT__REGISTER__INHABITS__GIVE_ID__NONDATA(ASTNPTerm,     syn::UnOp_c, __ASTNPTerm___syn__UnOp_c__)
SEPT__REGISTER__INHABITS__GIVE_ID__NONDATA(ASTNPTerm,     FormalTypeOf_Term_c, __ASTNPTerm___sem__FormalTypeOf_Term_c__)
SEPT__REGISTER__INHABITS__GIVE_ID__NONDATA(std::string,   syn::SymbolId_c, __std_string___syn__SymbolId_c__)
SEPT__REGISTER__INHABITS__GIVE_ID__NONDATA(char const *,  syn::SymbolId_c, __char_const_ptr___syn__SymbolId_c__)
// TODO there are probably some missing

SEPT__REGISTER__COMPARE__GIVE_ID__SINGLETON(syn::BinOp_c, __syn_BinOp_c__)
SEPT__REGISTER__COMPARE__GIVE_ID__SINGLETON(syn::UnOp_c, __syn_UnOp_c__)
SEPT__REGISTER__COMPARE(ASTNPTerm, ASTNPTerm)

SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__GIVE_ID__ABSTRACT_TYPE(syn::BinOp_c,      ASTNPTerm, __syn__BinOp_c___ASTNPTerm__)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__GIVE_ID__ABSTRACT_TYPE(syn::UnOp_c,       ASTNPTerm, __syn__UnOp_c___ASTNPTerm__)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(FormalTypeOf_Term_c, ASTNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__GIVE_ID__ABSTRACT_TYPE(syn::SymbolId_c, std::string, __syn__SymbolId_c___std_string__)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__GIVE_ID__ABSTRACT_TYPE(syn::SymbolId_c, char const *, __syn__SymbolId_c___char_const_ptr__)

// TEMP HACK
SEPT__REGISTER__INHABITS__NONDATA(ASTNPTerm, UnionTerm_c)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(UnionTerm_c, ASTNPTerm)
} // end namespace sept

namespace sem {

SEPT__REGISTER__EVALUATE_EXPR(bool)
SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID(sept::True_c, __sept__True_c__)
SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID(sept::False_c, __sept__False_c__)
SEPT__REGISTER__EVALUATE_EXPR(double)
SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID(std::string, __std_string__)
SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID(sept::TupleTerm_c, __sept__TupleTerm_c__)
SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID(sept::Array_c, __sept__Array_c__)
SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID(sept::ArrayESTerm_c, __sept__ArrayESTerm_c__)
SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID(sept::ArrayETerm_c, __sept__ArrayETerm_c__)
SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID(sept::ArraySTerm_c, __sept__ArraySTerm_c__)
SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID(sept::ArrayTerm_c, __sept__ArrayTerm_c__)
SEPT__REGISTER__EVALUATE_EXPR(ValueTerminal_Term_c)
SEPT__REGISTER__EVALUATE_EXPR(ExprArray_Term_c)
SEPT__REGISTER__EVALUATE_EXPR(Expr_Term_c)

SEPT__REGISTER__EXECUTE_STMT__GIVE_ID(sept::TupleTerm_c, __sept__TupleTerm_c__)

} // end namespace sem

int main (int argc, char **argv) {
//     lvd::g_log.set_log_level_threshold(lvd::LogLevel::DBG);
    lvd::g_log.out().precision(std::numeric_limits<double>::max_digits10+1);
    lvd::g_log.out().setf(std::ios_base::boolalpha, std::ios_base::boolalpha);

    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(syn::BinOp) << '\n'
               << LVD_REFLECT(syn::BinOpExpr) << '\n'
               << LVD_REFLECT(syn::CondExpr) << '\n'
               << LVD_REFLECT(syn::Expr) << '\n'
               << LVD_REFLECT(syn::UnOp) << '\n'
               << LVD_REFLECT(syn::UnOpExpr) << '\n'
               << '\n';
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(inhabits(Add, syn::BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Sub, syn::BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Mul, syn::BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Div, syn::BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Pow, syn::BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Neg, syn::BinOp)) << '\n'
               << LVD_REFLECT(inhabits(And, syn::BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Or,  syn::BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Xor, syn::BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Not, syn::BinOp)) << '\n'
               << '\n'
               << LVD_REFLECT(inhabits(Add, syn::UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Sub, syn::UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Mul, syn::UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Div, syn::UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Pow, syn::UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Neg, syn::UnOp)) << '\n'
               << LVD_REFLECT(inhabits(And, syn::UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Or,  syn::UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Xor, syn::UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Not, syn::UnOp)) << '\n'
               << '\n'
               << LVD_REFLECT(inhabits(MapsTo, sept::FormalTypeOf(MapsTo))) << '\n'
               << LVD_REFLECT(inhabits(MapsTo, syn::UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Xor, sept::FormalTypeOf(MapsTo))) << '\n'
               << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(123.4, Pow, 2.0), syn::Expr)) << '\n'
               << '\n'
               << LVD_REFLECT(inhabits(sept::Float64, syn::TypeExpr)) << '\n'
               << LVD_REFLECT(inhabits("hippo1", syn::SymbolId)) << '\n'
               << LVD_REFLECT(inhabits(std::string("hippo2"), syn::SymbolId)) << '\n'
               << LVD_REFLECT(inhabits(syn::SymbolId("hippo3"), syn::SymbolId)) << '\n'
               << LVD_REFLECT(inhabits(syn::SymbolId(std::string("hippo4")), syn::SymbolId)) << '\n'
               << LVD_REFLECT(inhabits(syn::SymbolTypeDecl(sept::TupleTerm_c(std::string("hippo5"), DeclaredAs, sept::Float64)), syn::SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(syn::SymbolTypeDecl(sept::TupleTerm_c(syn::SymbolId(std::string("hippo6")), DeclaredAs, sept::Float64)), syn::SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(syn::SymbolTypeDecl(sept::TupleTerm_c("hippo7", DeclaredAs, sept::Float64)), syn::SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(syn::SymbolTypeDecl(sept::TupleTerm_c(syn::SymbolId("hippo8"), DeclaredAs, sept::Float64)), syn::SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c("hippo9", DeclaredAs, sept::Float64), syn::SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(syn::SymbolId("hippo9"), DeclaredAs, sept::Float64), syn::SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(std::string("hippo10"), DeclaredAs, sept::Float64), syn::SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(sept::Float64, MapsTo, sept::Bool), syn::FuncType)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(sept::Float64, MapsTo, sept::TupleTerm_c(sept::Bool, MapsTo, sept::Uint32)), syn::FuncType)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(syn::SymbolTypeDeclArray(sept::TupleTerm_c("hippo11", DeclaredAs, sept::Float64)), MapsTo, sept::Bool), syn::FuncPrototype)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(syn::SymbolTypeDeclArray(sept::TupleTerm_c(std::string("hippo12"), DeclaredAs, sept::Float64)), MapsTo, sept::Bool), syn::FuncPrototype)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(syn::SymbolTypeDeclArray(sept::TupleTerm_c(std::string("hippo13"), DeclaredAs, sept::Float64), sept::TupleTerm_c(std::string("ostrich44"), DeclaredAs, sept::Uint32)), MapsTo, sept::Bool), syn::FuncPrototype)) << '\n'
               << '\n';
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(syn::ValueTerminal(123.45)) << '\n'
               << LVD_REFLECT(syn::Expr(123.45)) << '\n'
               << LVD_REFLECT(syn::BinOpExpr(123.45, Add, 0.05)) << '\n'
               << '\n';

    sem::EvalCtx ctx;

    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(evaluate_expr_data(123.45, ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(true, ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(syn::BinOpExpr(123.45, Add, 0.05), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(syn::CondExpr(If, true, Then, 1.2, Else, 0.05), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(syn::RoundExpr(RoundOpen, syn::ExprArray(1.2), RoundClose), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(syn::RoundExpr(RoundOpen, syn::ExprArray(1.2, 3.4, 5.6), RoundClose), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(syn::RoundExpr(RoundOpen, syn::ExprArray(syn::RoundExpr(RoundOpen, syn::ExprArray(false), RoundClose)), RoundClose), ctx)) << '\n'
               << '\n';
    lvd::g_log << lvd::Log::trc()
               << LVD_REFLECT(inhabits_data(RoundOpen, sept::FormalTypeOf(RoundOpen))) << '\n'
               << LVD_REFLECT(inhabits_data(SquareOpen, sept::FormalTypeOf(RoundOpen))) << '\n'
               << LVD_REFLECT(inhabits_data(CurlyOpen, sept::FormalTypeOf(RoundOpen))) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(RoundOpen, syn::ExprArray(1.0), RoundClose), syn::RoundExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(RoundOpen, syn::ExprArray(1.0), RoundClose), syn::SquareExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(RoundOpen, syn::ExprArray(1.0), RoundClose), syn::CurlyExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(SquareOpen, 1.0, SquareClose), syn::RoundExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(SquareOpen, 1.0, SquareClose), syn::SquareExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(SquareOpen, 1.0, SquareClose), syn::CurlyExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(CurlyOpen, 1.0, CurlyClose), syn::RoundExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(CurlyOpen, 1.0, CurlyClose), syn::SquareExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(CurlyOpen, 1.0, CurlyClose), syn::CurlyExpr)) << '\n'
               << '\n';

    // Silly, but hey.
    auto x = 0.1;
    auto three_factorial = 3.0*2.0*1.0;
    auto five_factorial = 5.0*4.0*three_factorial;
    auto seven_factorial = 7.0*6.0*five_factorial;
    auto nine_factorial = 9.0*8.0*seven_factorial;
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(x) << '\n'
               << LVD_REFLECT(three_factorial) << '\n'
               << LVD_REFLECT(five_factorial) << '\n'
               << LVD_REFLECT(seven_factorial) << '\n'
               << LVD_REFLECT(nine_factorial) << '\n'
               << LVD_REFLECT(syn::BinOpExpr(x, Pow, 3.0)) << '\n'
               << LVD_REFLECT(evaluate_expr(syn::BinOpExpr(x, Pow, 3.0), ctx)) << '\n'
               << '\n';
    // sin(x) = x - x^3 / 3! + x^5 / 5! - x^7 / 7! + x^9 / 9! - ...
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(evaluate_expr(syn::BinOpExpr(x, Pow, 9.0), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr(nine_factorial, ctx)) << '\n'
               << LVD_REFLECT(inhabits(syn::BinOpExpr(x, Pow, 9.0), syn::Expr)) << '\n'
               << LVD_REFLECT(inhabits(Div, syn::BinOp)) << '\n'
               << LVD_REFLECT(inhabits(nine_factorial, syn::Expr)) << '\n'
               << LVD_REFLECT(sept::inhabits_data(syn::BinOpExpr(x, Pow, 9.0), syn::Expr)) << '\n'
               << LVD_REFLECT(sept::inhabits_data(syn::BinOpExpr(x, Pow, 9.0), syn::Expr)) << '\n'
               << LVD_REFLECT(sept::inhabits_data(Div, syn::BinOp)) << '\n'
               << LVD_REFLECT(sept::inhabits_data(nine_factorial, syn::Expr)) << '\n'
               << LVD_REFLECT(inhabits(syn::BinOpExpr(x, Pow, 9.0), syn::BinOpExpr)) << '\n'
               << LVD_REFLECT(inhabits(syn::BinOpExpr(x, Pow, 9.0), syn::CondExpr)) << '\n'
               << LVD_REFLECT(inhabits(syn::BinOpExpr(x, Pow, 9.0), syn::Expr)) << '\n'
               << '\n';
    lvd::g_log << lvd::Log::dbg() << evaluate_expr(
        syn::BinOpExpr(
            syn::BinOpExpr(x, Pow, 9.0),
            Div,
            nine_factorial
        ),
        ctx
    ) << '\n';

    lvd::g_log << lvd::Log::dbg() << evaluate_expr(
        syn::BinOpExpr(
            syn::BinOpExpr(
                syn::BinOpExpr(
                    syn::BinOpExpr(
                        x,
                        Sub,
                        syn::BinOpExpr(
                            syn::BinOpExpr(x, Pow, 3.0),
                            Div,
                            three_factorial
                        )
                    ),
                    Add,
                    syn::BinOpExpr(
                        syn::BinOpExpr(x, Pow, 5.0),
                        Div,
                        five_factorial
                    )
                ),
                Sub,
                syn::BinOpExpr(
                    syn::BinOpExpr(x, Pow, 7.0),
                    Div,
                    seven_factorial
                )
            ),
            Add,
            syn::BinOpExpr(
                syn::BinOpExpr(x, Pow, 9.0),
                Div,
                nine_factorial
            )
        ),
        ctx
    ) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(std::sin(x)) << '\n';

    auto stuff = sept::Data{sept::Array(sept::True, sept::False, sept::Uint32(456))};
    // Define some global symbols
    ctx.current_scope()->define_symbol(std::string("hippa"), sept::True);
    ctx.current_scope()->define_symbol(std::string("ostrich"), sept::Float64(123.0));
    // Add a reference to the symbol table, demonstrating that nested references work.
    ctx.current_scope()->define_symbol(std::string("donkey"), sept::MemRef(&stuff));
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(sept::GlobalSymRef("ostrich")) << '\n'
               << LVD_REFLECT(sept::Data{sept::GlobalSymRef("ostrich")}) << '\n'
               << LVD_REFLECT(stuff) << '\n'
               << LVD_REFLECT(sept::GlobalSymRef("donkey")) << '\n'
               << LVD_REFLECT(sept::Data{sept::GlobalSymRef("donkey")}) << '\n'
               << '\n'
               << LVD_REFLECT(inhabits(sept::Tuple(std::string("f"), sept::Tuple(std::string("x"))), syn::FuncEval)) << '\n'
               << LVD_REFLECT(syn::evaluate_expr__as_SymbolId(std::string("hippa"), ctx)) << '\n'
               << LVD_REFLECT(syn::evaluate_expr__as_SymbolId(std::string("ostrich"), ctx)) << '\n'
               << '\n';

    // Define a global function.
    ctx.current_scope()->define_symbol(
        syn::SymbolId("square"),
        syn::FuncLiteral(
            syn::FuncPrototype(
                syn::SymbolTypeDeclArray(
                    syn::SymbolTypeDecl(syn::SymbolId("x"), DeclaredAs, sept::Float64)
                ),
                MapsTo,
                sept::Float64
            ),
            syn::BinOpExpr(syn::SymbolId("x"), Mul, syn::SymbolId("x"))
        )
    );
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(syn::ExprArray(sept::Float64(100.1))) << '\n'
               << LVD_REFLECT(syn::ExprArray(sept::Float64(100.1), sept::Uint32(1234))) << '\n'
               << LVD_REFLECT(syn::RoundExpr(RoundOpen, syn::ExprArray(sept::Float64(100.1)), RoundClose)) << '\n'
               << LVD_REFLECT(syn::FuncEval(syn::SymbolId("square"), syn::RoundExpr(RoundOpen, syn::ExprArray(sept::Float64(100.1)), RoundClose))) << '\n'
               << LVD_REFLECT(syn::evaluate_expr__as_FuncEval(syn::FuncEval(syn::SymbolId("square"), syn::RoundExpr(RoundOpen, syn::ExprArray(sept::Float64(100.1)), RoundClose)), ctx)) << '\n'
               << LVD_REFLECT(syn::Stmt) << '\n'
               << LVD_REFLECT(syn::StmtArray) << '\n'
               << LVD_REFLECT(syn::ExprArray) << '\n'
               << LVD_REFLECT(syn::BlockExpr) << '\n'
               << '\n';

    auto stmt_array = syn::StmtArray(
        syn::SymbolDefn(syn::SymbolId("fwee"), DefinedAs, sept::True),
        syn::SymbolDefn(syn::SymbolId("gwaa"), DefinedAs, sept::Float64(40302.01))
    );
    lvd::g_log << lvd::Log::dbg() << LVD_CALL_SITE() << '\n';
    auto blah = sept::ArrayE(sept::Union(syn::SymbolDefn))(
        syn::SymbolDefn(syn::SymbolId("fwee"), DefinedAs, sept::True),
        syn::SymbolDefn(syn::SymbolId("gwaa"), DefinedAs, sept::Float64(40302.01))
    );
    lvd::g_log << lvd::Log::dbg() << LVD_CALL_SITE() << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(inhabits_data(stmt_array, syn::StmtArray)) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(inhabits_data(blah, syn::StmtArray)) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(stmt_array) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(blah) << '\n';
    bool eq_val = stmt_array == blah;

    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(syn::SymbolDefn) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(syn::SymbolDefn == syn::SymbolDefn) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(syn::Stmt) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(syn::Stmt == syn::Stmt) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(sept::Union(syn::SymbolDefn) == syn::Stmt) << '\n';


    lvd::g_log << lvd::Log::dbg() << "stmt_array == blah" << " = " << eq_val << '\n';
    auto block_expr = syn::BlockExpr(stmt_array, syn::SymbolId("fwee"));
    lvd::g_log << lvd::Log::dbg() << LVD_CALL_SITE() << '\n'
               << LVD_REFLECT(block_expr) << '\n'
               << LVD_REFLECT(inhabits(block_expr, syn::BlockExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(block_expr, syn::BlockExpr)) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_CALL_SITE() << '\n';
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(
                    syn::StmtArray(
                        syn::SymbolDefn(syn::SymbolId("fwee"), DefinedAs, sept::True),
                        syn::SymbolDefn(syn::SymbolId("gwaa"), DefinedAs, sept::Float64(40302.01))
                    )
                  ) << '\n'
               << LVD_REFLECT(
                    syn::BlockExpr(
                        syn::StmtArray(
                            syn::SymbolDefn(syn::SymbolId("fwee"), DefinedAs, sept::True),
                            syn::SymbolDefn(syn::SymbolId("gwaa"), DefinedAs, sept::Float64(40302.01)),
                            syn::Assignment(syn::SymbolId("fwee"), AssignFrom, sept::Float64(8888.55))
                        ),
                        syn::SymbolId("fwee")
                    )
                  ) << '\n'
               << LVD_REFLECT(
                    syn::evaluate_expr__as_BlockExpr(
                        syn::BlockExpr(
                            syn::StmtArray(
                                syn::SymbolDefn(syn::SymbolId("fwee"), DefinedAs, sept::True),
                                syn::SymbolDefn(syn::SymbolId("gwaa"), DefinedAs, sept::Float64(40302.01)),
                                syn::Assignment(syn::SymbolId("fwee"), AssignFrom, sept::Float64(8888.55))
                            ),
                            syn::SymbolId("fwee")
                        ),
                        ctx
                    ).deref()
                  ) << '\n'
               << '\n';

    // Define another global function.
    ctx.current_scope()->define_symbol(
        syn::SymbolId("exp"),
        syn::FuncLiteral(
            syn::FuncPrototype(
                syn::SymbolTypeDeclArray(
                    syn::SymbolTypeDecl(syn::SymbolId("x"), DeclaredAs, sept::Float64)
                ),
                MapsTo,
                sept::Float64
            ),
            syn::BlockExpr(
                syn::StmtArray(
                    syn::SymbolDefn(syn::SymbolId("retval"), DefinedAs, 0.0),
                    syn::SymbolDefn(syn::SymbolId("i"), DefinedAs, 0.0),
                    syn::SymbolDefn(syn::SymbolId("accumulator"), DefinedAs, 1.0),

                    // Just do an unrolled loop for now.

                    syn::Assignment(syn::SymbolId("retval"), AssignFrom, syn::BinOpExpr(syn::SymbolId("retval"), Add, syn::SymbolId("accumulator"))),
                    syn::Assignment(syn::SymbolId("i"), AssignFrom, syn::BinOpExpr(syn::SymbolId("i"), Add, 1.0)),
                    syn::Assignment(syn::SymbolId("accumulator"), AssignFrom, syn::BinOpExpr(syn::BinOpExpr(syn::SymbolId("accumulator"), Mul, syn::SymbolId("x")), Div, syn::SymbolId("i"))),

                    syn::Assignment(syn::SymbolId("retval"), AssignFrom, syn::BinOpExpr(syn::SymbolId("retval"), Add, syn::SymbolId("accumulator"))),
                    syn::Assignment(syn::SymbolId("i"), AssignFrom, syn::BinOpExpr(syn::SymbolId("i"), Add, 1.0)),
                    syn::Assignment(syn::SymbolId("accumulator"), AssignFrom, syn::BinOpExpr(syn::BinOpExpr(syn::SymbolId("accumulator"), Mul, syn::SymbolId("x")), Div, syn::SymbolId("i"))),

                    syn::Assignment(syn::SymbolId("retval"), AssignFrom, syn::BinOpExpr(syn::SymbolId("retval"), Add, syn::SymbolId("accumulator"))),
                    syn::Assignment(syn::SymbolId("i"), AssignFrom, syn::BinOpExpr(syn::SymbolId("i"), Add, 1.0)),
                    syn::Assignment(syn::SymbolId("accumulator"), AssignFrom, syn::BinOpExpr(syn::BinOpExpr(syn::SymbolId("accumulator"), Mul, syn::SymbolId("x")), Div, syn::SymbolId("i"))),

                    syn::Assignment(syn::SymbolId("retval"), AssignFrom, syn::BinOpExpr(syn::SymbolId("retval"), Add, syn::SymbolId("accumulator"))),
                    syn::Assignment(syn::SymbolId("i"), AssignFrom, syn::BinOpExpr(syn::SymbolId("i"), Add, 1.0)),
                    syn::Assignment(syn::SymbolId("accumulator"), AssignFrom, syn::BinOpExpr(syn::BinOpExpr(syn::SymbolId("accumulator"), Mul, syn::SymbolId("x")), Div, syn::SymbolId("i"))),

                    syn::Assignment(syn::SymbolId("retval"), AssignFrom, syn::BinOpExpr(syn::SymbolId("retval"), Add, syn::SymbolId("accumulator"))),
                    syn::Assignment(syn::SymbolId("i"), AssignFrom, syn::BinOpExpr(syn::SymbolId("i"), Add, 1.0)),
                    syn::Assignment(syn::SymbolId("accumulator"), AssignFrom, syn::BinOpExpr(syn::BinOpExpr(syn::SymbolId("accumulator"), Mul, syn::SymbolId("x")), Div, syn::SymbolId("i"))),

                    syn::Assignment(syn::SymbolId("retval"), AssignFrom, syn::BinOpExpr(syn::SymbolId("retval"), Add, syn::SymbolId("accumulator"))),
                    syn::Assignment(syn::SymbolId("i"), AssignFrom, syn::BinOpExpr(syn::SymbolId("i"), Add, 1.0)),
                    syn::Assignment(syn::SymbolId("accumulator"), AssignFrom, syn::BinOpExpr(syn::BinOpExpr(syn::SymbolId("accumulator"), Mul, syn::SymbolId("x")), Div, syn::SymbolId("i"))),

                    syn::Assignment(syn::SymbolId("retval"), AssignFrom, syn::BinOpExpr(syn::SymbolId("retval"), Add, syn::SymbolId("accumulator"))),
                    syn::Assignment(syn::SymbolId("i"), AssignFrom, syn::BinOpExpr(syn::SymbolId("i"), Add, 1.0)),
                    syn::Assignment(syn::SymbolId("accumulator"), AssignFrom, syn::BinOpExpr(syn::BinOpExpr(syn::SymbolId("accumulator"), Mul, syn::SymbolId("x")), Div, syn::SymbolId("i")))
                ),
                syn::SymbolId("retval")
            )
        )
    );
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(evaluate_expr_data(syn::FuncEval(syn::SymbolId("exp"), syn::RoundExpr(RoundOpen, syn::ExprArray(0.1), RoundClose)), ctx)) << '\n'
               << LVD_REFLECT(std::exp(0.1)) << '\n';

    // Testing ElementEval
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(evaluate_expr_data(syn::ElementEval(sept::Array(0.5, 1.5, 2.5, 3.5), syn::SquareExpr(SquareOpen, syn::ExprArray(sept::Uint32(2)), SquareClose)), ctx)) << '\n';

    // Testing syn::Construction
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(evaluate_expr_data(syn::Construction(sept::ArrayES(sept::Float64,sept::Uint32(2)), syn::CurlyExpr(CurlyOpen, syn::ExprArray(sept::Array(1.5, 3.5)), CurlyClose)), ctx)) << '\n';


    // Attempt to use a typedef
    ctx.current_scope()->define_symbol(syn::SymbolId("Complex"), sept::ArrayES(sept::Float64, sept::Uint32(2)));
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(evaluate_expr_data(syn::SymbolId("Complex"), ctx).deref()) << '\n'
               << LVD_REFLECT(evaluate_expr_data(syn::SymbolId("Complex"), ctx).deref()) << '\n'
               << LVD_REFLECT(evaluate_expr_data(syn::Construction(syn::SymbolId("Complex"), syn::CurlyExpr(CurlyOpen, syn::ExprArray(sept::Array(8.1, 9.2)), CurlyClose)), ctx)) << '\n'
               << LVD_REFLECT(
                    inhabits_data(
                        evaluate_expr_data(syn::Construction(syn::SymbolId("Complex"), syn::CurlyExpr(CurlyOpen, syn::ExprArray(sept::Array(8.1, 9.2)), CurlyClose)), ctx),
                        syn::SymbolId("Complex")
                    )
                ) << '\n'
               << LVD_REFLECT(
                    inhabits_data(
                        sept::Array(8.1, 9.2),
                        evaluate_expr_data(syn::SymbolId("Complex"), ctx)
                    )
                ) << '\n'
               << LVD_REFLECT(sept::Array(8.1, 9.2)) << '\n'
               << LVD_REFLECT(sept::ArrayES(sept::Float64, sept::Uint32(2))(8.1, 9.2)) << '\n'
               << LVD_REFLECT(
                    inhabits_data(
                        sept::Array(8.1, 9.2),
                        sept::Array
                    )
                ) << '\n'
               << LVD_REFLECT(
                    inhabits_data(
                        sept::Array(8.1, 9.2),
                        sept::ArrayES(sept::Float64, sept::Uint32(2))
                    )
                ) << '\n'
               << LVD_REFLECT(
                    inhabits_data(
                        sept::ArrayES(sept::Float64, sept::Uint32(2))(8.1, 9.2),
                        sept::Array
                    )
                ) << '\n'
               << LVD_REFLECT(
                    inhabits_data(
                        sept::ArrayES(sept::Float64, sept::Uint32(2))(8.1, 9.2),
                        sept::ArrayES(sept::Float64, sept::Uint32(2))
                    )
                ) << '\n'
               << LVD_REFLECT(
                    inhabits_data(
                        sept::Array(8.1, 9.2),
                        evaluate_expr_data(syn::SymbolId("Complex"), ctx)
                    )
                ) << '\n'
               << LVD_REFLECT(
                    inhabits_data(
                        sept::ArrayES(sept::Float64, sept::Uint32(2))(8.1, 9.2),
                        evaluate_expr_data(syn::SymbolId("Complex"), ctx)
                    )
                ) << '\n'
               << LVD_REFLECT(
                    evaluate_expr_data(sept::ArrayES(sept::Float64, sept::Uint32(2))(8.1, 9.2), ctx)
                ) << '\n'
               << '\n';

    ctx.current_scope()->define_symbol(
        syn::SymbolId("Complex_square"),
        syn::FuncLiteral(
            syn::FuncPrototype(
                syn::SymbolTypeDeclArray(
                    syn::SymbolTypeDecl(syn::SymbolId("z"), DeclaredAs, syn::SymbolId("Complex"))
                ),
                MapsTo,
                syn::SymbolId("Complex")
            ),
            syn::BlockExpr(
                syn::StmtArray(
                    syn::SymbolDefn(syn::SymbolId("re"), DefinedAs, syn::ElementEval(syn::SymbolId("z"), syn::SquareExpr(SquareOpen, syn::ExprArray(sept::Uint32(0)), SquareClose))),
                    syn::SymbolDefn(syn::SymbolId("im"), DefinedAs, syn::ElementEval(syn::SymbolId("z"), syn::SquareExpr(SquareOpen, syn::ExprArray(sept::Uint32(1)), SquareClose)))
                ),
                syn::Construction(
                    syn::SymbolId("Complex"),
                    syn::CurlyExpr(
                        CurlyOpen,
                        syn::ExprArray(
                            sept::Array(
                                syn::BinOpExpr(syn::BinOpExpr(syn::SymbolId("re"), Mul, syn::SymbolId("re")), Sub, syn::BinOpExpr(syn::SymbolId("im"), Mul, syn::SymbolId("im"))),
                                syn::BinOpExpr(2.0, Mul, syn::BinOpExpr(syn::SymbolId("re"), Mul, syn::SymbolId("im")))
                            )
                        ),
                        CurlyClose
                    )
                )
            )
        )
    );
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(
                    evaluate_expr_data(
                        syn::FuncEval(
                            syn::SymbolId("Complex_square"),
                            syn::RoundExpr(
                                RoundOpen,
                                syn::ExprArray(
                                    syn::Construction(
                                        syn::SymbolId("Complex"),
                                        syn::CurlyExpr(
                                            CurlyOpen,
                                            syn::ExprArray(
                                                sept::Array(3.0, 4.0)
                                            ),
                                            CurlyClose
                                        )
                                    )
                                ),
                                RoundClose
                            )
                        ),
                        ctx
                    )
                ) << '\n';

    ctx.current_scope()->define_symbol(
        syn::SymbolId("Complex_mul"),
        syn::FuncLiteral(
            syn::FuncPrototype(
                syn::SymbolTypeDeclArray(
                    syn::SymbolTypeDecl(syn::SymbolId("w"), DeclaredAs, syn::SymbolId("Complex")),
                    syn::SymbolTypeDecl(syn::SymbolId("z"), DeclaredAs, syn::SymbolId("Complex"))
                ),
                MapsTo,
                syn::SymbolId("Complex")
            ),
            syn::BlockExpr(
                syn::StmtArray(
                    syn::SymbolDefn(syn::SymbolId("w_re"), DefinedAs, syn::ElementEval(syn::SymbolId("w"), syn::SquareExpr(SquareOpen, syn::ExprArray(sept::Uint32(0)), SquareClose))),
                    syn::SymbolDefn(syn::SymbolId("w_im"), DefinedAs, syn::ElementEval(syn::SymbolId("w"), syn::SquareExpr(SquareOpen, syn::ExprArray(sept::Uint32(1)), SquareClose))),
                    syn::SymbolDefn(syn::SymbolId("z_re"), DefinedAs, syn::ElementEval(syn::SymbolId("z"), syn::SquareExpr(SquareOpen, syn::ExprArray(sept::Uint32(0)), SquareClose))),
                    syn::SymbolDefn(syn::SymbolId("z_im"), DefinedAs, syn::ElementEval(syn::SymbolId("z"), syn::SquareExpr(SquareOpen, syn::ExprArray(sept::Uint32(1)), SquareClose)))
                ),
                syn::Construction(
                    syn::SymbolId("Complex"),
                    syn::CurlyExpr(
                        CurlyOpen,
                        syn::ExprArray(
                            sept::Array(
                                syn::BinOpExpr(syn::BinOpExpr(syn::SymbolId("w_re"), Mul, syn::SymbolId("z_re")), Sub, syn::BinOpExpr(syn::SymbolId("w_im"), Mul, syn::SymbolId("z_im"))),
                                syn::BinOpExpr(syn::BinOpExpr(syn::SymbolId("w_re"), Mul, syn::SymbolId("z_im")), Add, syn::BinOpExpr(syn::SymbolId("w_im"), Mul, syn::SymbolId("z_re")))
                            )
                        ),
                        CurlyClose
                    )
                )
            )
        )
    );
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(
                    evaluate_expr_data(
                        syn::FuncEval(
                            syn::SymbolId("Complex_mul"),
                            syn::RoundExpr(
                                RoundOpen,
                                syn::ExprArray(
                                    syn::Construction(
                                        syn::SymbolId("Complex"),
                                        syn::CurlyExpr(
                                            CurlyOpen,
                                            syn::ExprArray(
                                                sept::Array(3.0, 4.0)
                                            ),
                                            CurlyClose
                                        )
                                    ),
                                    syn::Construction(
                                        syn::SymbolId("Complex"),
                                        syn::CurlyExpr(
                                            CurlyOpen,
                                            syn::ExprArray(
                                                sept::Array(1.0, -2.0)
                                            ),
                                            CurlyClose
                                        )
                                    )
                                ),
                                RoundClose
                            )
                        ),
                        ctx
                    )
                ) << '\n';

    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(
                    sem::parse_Expr_Term(
                        syn::FuncEval(
                            syn::SymbolId("Complex_mul"),
                            syn::RoundExpr(
                                RoundOpen,
                                syn::ExprArray(
                                    syn::Construction(
                                        syn::SymbolId("Complex"),
                                        syn::CurlyExpr(
                                            CurlyOpen,
                                            syn::ExprArray(
                                                sept::Array(3.0, 4.0)
                                            ),
                                            CurlyClose
                                        )
                                    ),
                                    syn::Construction(
                                        syn::SymbolId("Complex"),
                                        syn::CurlyExpr(
                                            CurlyOpen,
                                            syn::ExprArray(
                                                sept::Array(1.0, -2.0)
                                            ),
                                            CurlyClose
                                        )
                                    )
                                ),
                                RoundClose
                            )
                        )
                    )
                ) << '\n';


    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(sem::parse_Expr_Term(sept::Data{sept::Float64(12.34)})) << '\n'
               << LVD_REFLECT(sem::parse_Expr_Term(sept::Data{sept::Uint32(1234)})) << '\n'
               << LVD_REFLECT(sem::evaluate_BinOpExpr_Term(sem::BinOpExpr_Term_c{123.4, Add, 456.7}, ctx)) << '\n'
               << LVD_REFLECT(sem::evaluate_Expr_Term(sem::BinOpExpr_Term_c{123.4, Add, 456.7}, ctx)) << '\n'
               << '\n';

    return 0;
}
