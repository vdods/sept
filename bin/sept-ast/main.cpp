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
SEPT__REGISTER__PRINT(BinOp_c)
SEPT__REGISTER__PRINT(Expr_Term_c)
SEPT__REGISTER__PRINT(SymbolId_c)
SEPT__REGISTER__PRINT(UnOp_c)
SEPT__REGISTER__PRINT__GIVE_ID(char const *, __char_const_ptr__)

SEPT__REGISTER__EQ(BinOp_c)
SEPT__REGISTER__EQ(UnOp_c)
SEPT__REGISTER__EQ(ASTNPTerm)
SEPT__REGISTER__EQ(SymbolId_c)

SEPT__REGISTER__ABSTRACT_TYPE_OF(BinOp_c)
SEPT__REGISTER__ABSTRACT_TYPE_OF(UnOp_c)

SEPT__REGISTER__INHABITS__NONDATA(ASTNPTerm,     BinOp_c)
SEPT__REGISTER__INHABITS__NONDATA(ASTNPTerm,     UnOp_c)
SEPT__REGISTER__INHABITS__NONDATA(ASTNPTerm,     FormalTypeOf_Term_c)
SEPT__REGISTER__INHABITS__GIVE_ID__NONDATA(std::string,   SymbolId_c, __std_string___SymbolId_c__)
SEPT__REGISTER__INHABITS__GIVE_ID__NONDATA(char const *,  SymbolId_c, __char_const_ptr___SymbolId_c__)
// TODO there are probably some missing

SEPT__REGISTER__COMPARE__SINGLETON(BinOp_c)
SEPT__REGISTER__COMPARE__SINGLETON(UnOp_c)
SEPT__REGISTER__COMPARE(ASTNPTerm, ASTNPTerm)

SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(BinOp_c,      ASTNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(UnOp_c,       ASTNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(FormalTypeOf_Term_c, ASTNPTerm)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF___GIVE_ID__ABSTRACT_TYPE(SymbolId_c, std::string, __SymbolId_c___std_string__)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF___GIVE_ID__ABSTRACT_TYPE(SymbolId_c, char const *, __SymbolId_c___char_const_ptr__)

SEPT__REGISTER__EVALUATE_EXPR(bool)
SEPT__REGISTER__EVALUATE_EXPR(True_c)
SEPT__REGISTER__EVALUATE_EXPR(False_c)
SEPT__REGISTER__EVALUATE_EXPR(double)
SEPT__REGISTER__EVALUATE_EXPR__GIVE_ID(std::string, __std_string__)
SEPT__REGISTER__EVALUATE_EXPR(TupleTerm_c)
SEPT__REGISTER__EVALUATE_EXPR(Array_c)
SEPT__REGISTER__EVALUATE_EXPR(ArrayESTerm_c)
SEPT__REGISTER__EVALUATE_EXPR(ArrayETerm_c)
SEPT__REGISTER__EVALUATE_EXPR(ArraySTerm_c)
SEPT__REGISTER__EVALUATE_EXPR(ArrayTerm_c)
SEPT__REGISTER__EVALUATE_EXPR(ValueTerminal_Term_c)
SEPT__REGISTER__EVALUATE_EXPR(ExprArray_Term_c)
SEPT__REGISTER__EVALUATE_EXPR(Expr_Term_c)

SEPT__REGISTER__EXECUTE_STMT(TupleTerm_c)

// TEMP HACK
SEPT__REGISTER__INHABITS__NONDATA(ASTNPTerm, UnionTerm_c)
SEPT__REGISTER__CONSTRUCT_INHABITANT_OF__ABSTRACT_TYPE(UnionTerm_c, ASTNPTerm)
} // end namespace sept

int main (int argc, char **argv) {
//     lvd::g_log.set_log_level_threshold(lvd::LogLevel::DBG);
    lvd::g_log.out().precision(std::numeric_limits<double>::max_digits10+1);
    lvd::g_log.out().setf(std::ios_base::boolalpha, std::ios_base::boolalpha);

    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(BinOp) << '\n'
               << LVD_REFLECT(BinOpExpr) << '\n'
               << LVD_REFLECT(CondExpr) << '\n'
               << LVD_REFLECT(Expr) << '\n'
               << LVD_REFLECT(UnOp) << '\n'
               << LVD_REFLECT(UnOpExpr) << '\n'
               << '\n';
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(inhabits(Add, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Sub, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Mul, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Div, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Pow, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Neg, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(And, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Or, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Xor, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(Not, BinOp)) << '\n'
               << '\n'
               << LVD_REFLECT(inhabits(Add, UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Sub, UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Mul, UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Div, UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Pow, UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Neg, UnOp)) << '\n'
               << LVD_REFLECT(inhabits(And, UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Or, UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Xor, UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Not, UnOp)) << '\n'
               << '\n'
               << LVD_REFLECT(inhabits(MapsTo, sept::FormalTypeOf(MapsTo))) << '\n'
               << LVD_REFLECT(inhabits(MapsTo, UnOp)) << '\n'
               << LVD_REFLECT(inhabits(Xor, sept::FormalTypeOf(MapsTo))) << '\n'
               << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(123.4, Pow, 2.0), Expr)) << '\n'
               << '\n'
               << LVD_REFLECT(inhabits(sept::Float64, TypeExpr)) << '\n'
               << LVD_REFLECT(inhabits("hippo1", SymbolId)) << '\n'
               << LVD_REFLECT(inhabits(std::string("hippo2"), SymbolId)) << '\n'
               << LVD_REFLECT(inhabits(SymbolId("hippo3"), SymbolId)) << '\n'
               << LVD_REFLECT(inhabits(SymbolId(std::string("hippo4")), SymbolId)) << '\n'
               << LVD_REFLECT(inhabits(SymbolTypeDecl(sept::TupleTerm_c(std::string("hippo5"), DeclaredAs, sept::Float64)), SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(SymbolTypeDecl(sept::TupleTerm_c(SymbolId(std::string("hippo6")), DeclaredAs, sept::Float64)), SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(SymbolTypeDecl(sept::TupleTerm_c("hippo7", DeclaredAs, sept::Float64)), SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(SymbolTypeDecl(sept::TupleTerm_c(SymbolId("hippo8"), DeclaredAs, sept::Float64)), SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c("hippo9", DeclaredAs, sept::Float64), SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(SymbolId("hippo9"), DeclaredAs, sept::Float64), SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(std::string("hippo10"), DeclaredAs, sept::Float64), SymbolTypeDecl)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(sept::Float64, MapsTo, sept::Bool), FuncType)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(sept::Float64, MapsTo, sept::TupleTerm_c(sept::Bool, MapsTo, sept::Uint32)), FuncType)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(SymbolTypeDeclArray(sept::TupleTerm_c("hippo11", DeclaredAs, sept::Float64)), MapsTo, sept::Bool), FuncPrototype)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(SymbolTypeDeclArray(sept::TupleTerm_c(std::string("hippo12"), DeclaredAs, sept::Float64)), MapsTo, sept::Bool), FuncPrototype)) << '\n'
               << LVD_REFLECT(inhabits(sept::TupleTerm_c(SymbolTypeDeclArray(sept::TupleTerm_c(std::string("hippo13"), DeclaredAs, sept::Float64), sept::TupleTerm_c(std::string("ostrich44"), DeclaredAs, sept::Uint32)), MapsTo, sept::Bool), FuncPrototype)) << '\n'
               << '\n';
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(ValueTerminal(123.45)) << '\n'
               << LVD_REFLECT(Expr(123.45)) << '\n'
               << LVD_REFLECT(BinOpExpr(123.45, Add, 0.05)) << '\n'
               << '\n';

    EvalCtx ctx;

    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(evaluate_expr_data(123.45, ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(true, ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(BinOpExpr(123.45, Add, 0.05), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(CondExpr(If, true, Then, 1.2, Else, 0.05), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(RoundExpr(RoundOpen, ExprArray(1.2), RoundClose), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(RoundExpr(RoundOpen, ExprArray(1.2, 3.4, 5.6), RoundClose), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr_data(RoundExpr(RoundOpen, ExprArray(RoundExpr(RoundOpen, ExprArray(false), RoundClose)), RoundClose), ctx)) << '\n'
               << '\n';
    lvd::g_log << lvd::Log::trc()
               << LVD_REFLECT(inhabits_data(RoundOpen, sept::FormalTypeOf(RoundOpen))) << '\n'
               << LVD_REFLECT(inhabits_data(SquareOpen, sept::FormalTypeOf(RoundOpen))) << '\n'
               << LVD_REFLECT(inhabits_data(CurlyOpen, sept::FormalTypeOf(RoundOpen))) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(RoundOpen, ExprArray(1.0), RoundClose), RoundExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(RoundOpen, ExprArray(1.0), RoundClose), SquareExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(RoundOpen, ExprArray(1.0), RoundClose), CurlyExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(SquareOpen, 1.0, SquareClose), RoundExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(SquareOpen, 1.0, SquareClose), SquareExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(SquareOpen, 1.0, SquareClose), CurlyExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(CurlyOpen, 1.0, CurlyClose), RoundExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(CurlyOpen, 1.0, CurlyClose), SquareExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(sept::Tuple(CurlyOpen, 1.0, CurlyClose), CurlyExpr)) << '\n'
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
               << LVD_REFLECT(BinOpExpr(x, Pow, 3.0)) << '\n'
               << LVD_REFLECT(evaluate_expr(BinOpExpr(x, Pow, 3.0), ctx)) << '\n'
               << '\n';
    // sin(x) = x - x^3 / 3! + x^5 / 5! - x^7 / 7! + x^9 / 9! - ...
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(evaluate_expr(BinOpExpr(x, Pow, 9.0), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr(nine_factorial, ctx)) << '\n'
               << LVD_REFLECT(inhabits(BinOpExpr(x, Pow, 9.0), Expr)) << '\n'
               << LVD_REFLECT(inhabits(Div, BinOp)) << '\n'
               << LVD_REFLECT(inhabits(nine_factorial, Expr)) << '\n'
               << LVD_REFLECT(sept::inhabits_data(BinOpExpr(x, Pow, 9.0), Expr)) << '\n'
               << LVD_REFLECT(sept::inhabits_data(BinOpExpr(x, Pow, 9.0), Expr)) << '\n'
               << LVD_REFLECT(sept::inhabits_data(Div, BinOp)) << '\n'
               << LVD_REFLECT(sept::inhabits_data(nine_factorial, Expr)) << '\n'
               << LVD_REFLECT(inhabits(BinOpExpr(x, Pow, 9.0), BinOpExpr)) << '\n'
               << LVD_REFLECT(inhabits(BinOpExpr(x, Pow, 9.0), CondExpr)) << '\n'
               << LVD_REFLECT(inhabits(BinOpExpr(x, Pow, 9.0), Expr)) << '\n'
               << '\n';
    lvd::g_log << lvd::Log::dbg() << evaluate_expr(
        BinOpExpr(
            BinOpExpr(x, Pow, 9.0),
            Div,
            nine_factorial
        ),
        ctx
    ) << '\n';

    lvd::g_log << lvd::Log::dbg() << evaluate_expr(
        BinOpExpr(
            BinOpExpr(
                BinOpExpr(
                    BinOpExpr(
                        x,
                        Sub,
                        BinOpExpr(
                            BinOpExpr(x, Pow, 3.0),
                            Div,
                            three_factorial
                        )
                    ),
                    Add,
                    BinOpExpr(
                        BinOpExpr(x, Pow, 5.0),
                        Div,
                        five_factorial
                    )
                ),
                Sub,
                BinOpExpr(
                    BinOpExpr(x, Pow, 7.0),
                    Div,
                    seven_factorial
                )
            ),
            Add,
            BinOpExpr(
                BinOpExpr(x, Pow, 9.0),
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
               << LVD_REFLECT(inhabits(sept::Tuple(std::string("f"), sept::Tuple(std::string("x"))), FuncEval)) << '\n'
               << LVD_REFLECT(evaluate_expr__as_SymbolId(std::string("hippa"), ctx)) << '\n'
               << LVD_REFLECT(evaluate_expr__as_SymbolId(std::string("ostrich"), ctx)) << '\n'
               << '\n';

    // Define a global function.
    ctx.current_scope()->define_symbol(
        SymbolId("square"),
        FuncLiteral(
            FuncPrototype(
                SymbolTypeDeclArray(
                    SymbolTypeDecl(SymbolId("x"), DeclaredAs, sept::Float64)
                ),
                MapsTo,
                sept::Float64
            ),
            BinOpExpr(SymbolId("x"), Mul, SymbolId("x"))
        )
    );
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(ExprArray(sept::Float64(100.1))) << '\n'
               << LVD_REFLECT(ExprArray(sept::Float64(100.1), sept::Uint32(1234))) << '\n'
               << LVD_REFLECT(RoundExpr(RoundOpen, ExprArray(sept::Float64(100.1)), RoundClose)) << '\n'
               << LVD_REFLECT(FuncEval(SymbolId("square"), RoundExpr(RoundOpen, ExprArray(sept::Float64(100.1)), RoundClose))) << '\n'
               << LVD_REFLECT(evaluate_expr__as_FuncEval(FuncEval(SymbolId("square"), RoundExpr(RoundOpen, ExprArray(sept::Float64(100.1)), RoundClose)), ctx)) << '\n'
               << LVD_REFLECT(Stmt) << '\n'
               << LVD_REFLECT(StmtArray) << '\n'
               << LVD_REFLECT(ExprArray) << '\n'
               << LVD_REFLECT(BlockExpr) << '\n'
               << '\n';

    auto stmt_array = StmtArray(
        SymbolDefn(SymbolId("fwee"), DefinedAs, sept::True),
        SymbolDefn(SymbolId("gwaa"), DefinedAs, sept::Float64(40302.01))
    );
    lvd::g_log << lvd::Log::dbg() << LVD_CALL_SITE() << '\n';
    auto blah = sept::ArrayE(sept::Union(SymbolDefn))(
        SymbolDefn(SymbolId("fwee"), DefinedAs, sept::True),
        SymbolDefn(SymbolId("gwaa"), DefinedAs, sept::Float64(40302.01))
    );
    lvd::g_log << lvd::Log::dbg() << LVD_CALL_SITE() << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(inhabits_data(stmt_array, StmtArray)) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(inhabits_data(blah, StmtArray)) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(stmt_array) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(blah) << '\n';
    bool eq_val = stmt_array == blah;

    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(SymbolDefn) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(SymbolDefn == SymbolDefn) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(Stmt) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(Stmt == Stmt) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(sept::Union(SymbolDefn) == Stmt) << '\n';


    lvd::g_log << lvd::Log::dbg() << "stmt_array == blah" << " = " << eq_val << '\n';
    auto block_expr = BlockExpr(stmt_array, SymbolId("fwee"));
    lvd::g_log << lvd::Log::dbg() << LVD_CALL_SITE() << '\n'
               << LVD_REFLECT(block_expr) << '\n'
               << LVD_REFLECT(inhabits(block_expr, BlockExpr)) << '\n'
               << LVD_REFLECT(inhabits_data(block_expr, BlockExpr)) << '\n';
    lvd::g_log << lvd::Log::dbg() << LVD_CALL_SITE() << '\n';
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(
                    StmtArray(
                        SymbolDefn(SymbolId("fwee"), DefinedAs, sept::True),
                        SymbolDefn(SymbolId("gwaa"), DefinedAs, sept::Float64(40302.01))
                    )
                  ) << '\n'
               << LVD_REFLECT(
                    BlockExpr(
                        StmtArray(
                            SymbolDefn(SymbolId("fwee"), DefinedAs, sept::True),
                            SymbolDefn(SymbolId("gwaa"), DefinedAs, sept::Float64(40302.01)),
                            Assignment(SymbolId("fwee"), AssignFrom, sept::Float64(8888.55))
                        ),
                        SymbolId("fwee")
                    )
                  ) << '\n'
               << LVD_REFLECT(
                    evaluate_expr__as_BlockExpr(
                        BlockExpr(
                            StmtArray(
                                SymbolDefn(SymbolId("fwee"), DefinedAs, sept::True),
                                SymbolDefn(SymbolId("gwaa"), DefinedAs, sept::Float64(40302.01)),
                                Assignment(SymbolId("fwee"), AssignFrom, sept::Float64(8888.55))
                            ),
                            SymbolId("fwee")
                        ),
                        ctx
                    ).deref()
                  ) << '\n'
               << '\n';

    // Define another global function.
    ctx.current_scope()->define_symbol(
        SymbolId("exp"),
        FuncLiteral(
            FuncPrototype(
                SymbolTypeDeclArray(
                    SymbolTypeDecl(SymbolId("x"), DeclaredAs, sept::Float64)
                ),
                MapsTo,
                sept::Float64
            ),
            BlockExpr(
                StmtArray(
                    SymbolDefn(SymbolId("retval"), DefinedAs, 0.0),
                    SymbolDefn(SymbolId("i"), DefinedAs, 0.0),
                    SymbolDefn(SymbolId("accumulator"), DefinedAs, 1.0),

                    // Just do an unrolled loop for now.

                    Assignment(SymbolId("retval"), AssignFrom, BinOpExpr(SymbolId("retval"), Add, SymbolId("accumulator"))),
                    Assignment(SymbolId("i"), AssignFrom, BinOpExpr(SymbolId("i"), Add, 1.0)),
                    Assignment(SymbolId("accumulator"), AssignFrom, BinOpExpr(BinOpExpr(SymbolId("accumulator"), Mul, SymbolId("x")), Div, SymbolId("i"))),

                    Assignment(SymbolId("retval"), AssignFrom, BinOpExpr(SymbolId("retval"), Add, SymbolId("accumulator"))),
                    Assignment(SymbolId("i"), AssignFrom, BinOpExpr(SymbolId("i"), Add, 1.0)),
                    Assignment(SymbolId("accumulator"), AssignFrom, BinOpExpr(BinOpExpr(SymbolId("accumulator"), Mul, SymbolId("x")), Div, SymbolId("i"))),

                    Assignment(SymbolId("retval"), AssignFrom, BinOpExpr(SymbolId("retval"), Add, SymbolId("accumulator"))),
                    Assignment(SymbolId("i"), AssignFrom, BinOpExpr(SymbolId("i"), Add, 1.0)),
                    Assignment(SymbolId("accumulator"), AssignFrom, BinOpExpr(BinOpExpr(SymbolId("accumulator"), Mul, SymbolId("x")), Div, SymbolId("i"))),

                    Assignment(SymbolId("retval"), AssignFrom, BinOpExpr(SymbolId("retval"), Add, SymbolId("accumulator"))),
                    Assignment(SymbolId("i"), AssignFrom, BinOpExpr(SymbolId("i"), Add, 1.0)),
                    Assignment(SymbolId("accumulator"), AssignFrom, BinOpExpr(BinOpExpr(SymbolId("accumulator"), Mul, SymbolId("x")), Div, SymbolId("i"))),

                    Assignment(SymbolId("retval"), AssignFrom, BinOpExpr(SymbolId("retval"), Add, SymbolId("accumulator"))),
                    Assignment(SymbolId("i"), AssignFrom, BinOpExpr(SymbolId("i"), Add, 1.0)),
                    Assignment(SymbolId("accumulator"), AssignFrom, BinOpExpr(BinOpExpr(SymbolId("accumulator"), Mul, SymbolId("x")), Div, SymbolId("i"))),

                    Assignment(SymbolId("retval"), AssignFrom, BinOpExpr(SymbolId("retval"), Add, SymbolId("accumulator"))),
                    Assignment(SymbolId("i"), AssignFrom, BinOpExpr(SymbolId("i"), Add, 1.0)),
                    Assignment(SymbolId("accumulator"), AssignFrom, BinOpExpr(BinOpExpr(SymbolId("accumulator"), Mul, SymbolId("x")), Div, SymbolId("i"))),

                    Assignment(SymbolId("retval"), AssignFrom, BinOpExpr(SymbolId("retval"), Add, SymbolId("accumulator"))),
                    Assignment(SymbolId("i"), AssignFrom, BinOpExpr(SymbolId("i"), Add, 1.0)),
                    Assignment(SymbolId("accumulator"), AssignFrom, BinOpExpr(BinOpExpr(SymbolId("accumulator"), Mul, SymbolId("x")), Div, SymbolId("i")))
                ),
                SymbolId("retval")
            )
        )
    );
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(evaluate_expr_data(FuncEval(SymbolId("exp"), RoundExpr(RoundOpen, ExprArray(0.1), RoundClose)), ctx)) << '\n'
               << LVD_REFLECT(std::exp(0.1)) << '\n';

    // Testing ElementEval
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(evaluate_expr_data(ElementEval(sept::Array(0.5, 1.5, 2.5, 3.5), SquareExpr(SquareOpen, ExprArray(sept::Uint32(2)), SquareClose)), ctx)) << '\n';

    // Testing Construction
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(evaluate_expr_data(Construction(sept::ArrayES(sept::Float64,sept::Uint32(2)), CurlyExpr(CurlyOpen, ExprArray(sept::Array(1.5, 3.5)), CurlyClose)), ctx)) << '\n';


    // Attempt to use a typedef
    ctx.current_scope()->define_symbol(SymbolId("Complex"), sept::ArrayES(sept::Float64, sept::Uint32(2)));
    lvd::g_log << lvd::Log::dbg()
               << LVD_REFLECT(evaluate_expr_data(SymbolId("Complex"), ctx).deref()) << '\n'
               << LVD_REFLECT(evaluate_expr_data(SymbolId("Complex"), ctx).deref()) << '\n'
               << LVD_REFLECT(evaluate_expr_data(Construction(SymbolId("Complex"), CurlyExpr(CurlyOpen, ExprArray(sept::Array(8.1, 9.2)), CurlyClose)), ctx)) << '\n'
               << LVD_REFLECT(
                    inhabits_data(
                        evaluate_expr_data(Construction(SymbolId("Complex"), CurlyExpr(CurlyOpen, ExprArray(sept::Array(8.1, 9.2)), CurlyClose)), ctx),
                        SymbolId("Complex")
                    )
                ) << '\n'
               << LVD_REFLECT(
                    inhabits_data(
                        sept::Array(8.1, 9.2),
                        evaluate_expr_data(SymbolId("Complex"), ctx)
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
                        evaluate_expr_data(SymbolId("Complex"), ctx)
                    )
                ) << '\n'
               << LVD_REFLECT(
                    inhabits_data(
                        sept::ArrayES(sept::Float64, sept::Uint32(2))(8.1, 9.2),
                        evaluate_expr_data(SymbolId("Complex"), ctx)
                    )
                ) << '\n'
               << LVD_REFLECT(
                    evaluate_expr_data(sept::ArrayES(sept::Float64, sept::Uint32(2))(8.1, 9.2), ctx)
                ) << '\n'
               << '\n';

    ctx.current_scope()->define_symbol(
        SymbolId("Complex_square"),
        FuncLiteral(
            FuncPrototype(
                SymbolTypeDeclArray(
                    SymbolTypeDecl(SymbolId("z"), DeclaredAs, SymbolId("Complex"))
                ),
                MapsTo,
                SymbolId("Complex")
            ),
            BlockExpr(
                StmtArray(
                    SymbolDefn(SymbolId("re"), DefinedAs, ElementEval(SymbolId("z"), SquareExpr(SquareOpen, ExprArray(sept::Uint32(0)), SquareClose))),
                    SymbolDefn(SymbolId("im"), DefinedAs, ElementEval(SymbolId("z"), SquareExpr(SquareOpen, ExprArray(sept::Uint32(1)), SquareClose)))
                ),
                Construction(
                    SymbolId("Complex"),
                    CurlyExpr(
                        CurlyOpen,
                        ExprArray(
                            sept::Array(
                                BinOpExpr(BinOpExpr(SymbolId("re"), Mul, SymbolId("re")), Sub, BinOpExpr(SymbolId("im"), Mul, SymbolId("im"))),
                                BinOpExpr(2.0, Mul, BinOpExpr(SymbolId("re"), Mul, SymbolId("im")))
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
                        FuncEval(
                            SymbolId("Complex_square"),
                            RoundExpr(
                                RoundOpen,
                                ExprArray(
                                    Construction(
                                        SymbolId("Complex"),
                                        CurlyExpr(
                                            CurlyOpen,
                                            ExprArray(
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
        SymbolId("Complex_mul"),
        FuncLiteral(
            FuncPrototype(
                SymbolTypeDeclArray(
                    SymbolTypeDecl(SymbolId("w"), DeclaredAs, SymbolId("Complex")),
                    SymbolTypeDecl(SymbolId("z"), DeclaredAs, SymbolId("Complex"))
                ),
                MapsTo,
                SymbolId("Complex")
            ),
            BlockExpr(
                StmtArray(
                    SymbolDefn(SymbolId("w_re"), DefinedAs, ElementEval(SymbolId("w"), SquareExpr(SquareOpen, ExprArray(sept::Uint32(0)), SquareClose))),
                    SymbolDefn(SymbolId("w_im"), DefinedAs, ElementEval(SymbolId("w"), SquareExpr(SquareOpen, ExprArray(sept::Uint32(1)), SquareClose))),
                    SymbolDefn(SymbolId("z_re"), DefinedAs, ElementEval(SymbolId("z"), SquareExpr(SquareOpen, ExprArray(sept::Uint32(0)), SquareClose))),
                    SymbolDefn(SymbolId("z_im"), DefinedAs, ElementEval(SymbolId("z"), SquareExpr(SquareOpen, ExprArray(sept::Uint32(1)), SquareClose)))
                ),
                Construction(
                    SymbolId("Complex"),
                    CurlyExpr(
                        CurlyOpen,
                        ExprArray(
                            sept::Array(
                                BinOpExpr(BinOpExpr(SymbolId("w_re"), Mul, SymbolId("z_re")), Sub, BinOpExpr(SymbolId("w_im"), Mul, SymbolId("z_im"))),
                                BinOpExpr(BinOpExpr(SymbolId("w_re"), Mul, SymbolId("z_im")), Add, BinOpExpr(SymbolId("w_im"), Mul, SymbolId("z_re")))
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
                        FuncEval(
                            SymbolId("Complex_mul"),
                            RoundExpr(
                                RoundOpen,
                                ExprArray(
                                    Construction(
                                        SymbolId("Complex"),
                                        CurlyExpr(
                                            CurlyOpen,
                                            ExprArray(
                                                sept::Array(3.0, 4.0)
                                            ),
                                            CurlyClose
                                        )
                                    ),
                                    Construction(
                                        SymbolId("Complex"),
                                        CurlyExpr(
                                            CurlyOpen,
                                            ExprArray(
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
                    parse_Expr_Term(
                        FuncEval(
                            SymbolId("Complex_mul"),
                            RoundExpr(
                                RoundOpen,
                                ExprArray(
                                    Construction(
                                        SymbolId("Complex"),
                                        CurlyExpr(
                                            CurlyOpen,
                                            ExprArray(
                                                sept::Array(3.0, 4.0)
                                            ),
                                            CurlyClose
                                        )
                                    ),
                                    Construction(
                                        SymbolId("Complex"),
                                        CurlyExpr(
                                            CurlyOpen,
                                            ExprArray(
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
               << LVD_REFLECT(parse_Expr_Term(sept::Data{sept::Float64(12.34)})) << '\n'
               << LVD_REFLECT(parse_Expr_Term(sept::Data{sept::Uint32(1234)})) << '\n'
               << LVD_REFLECT(evaluate_BinOpExpr_Term(BinOpExpr_Term_c{123.4, Add, 456.7}, ctx)) << '\n'
               << LVD_REFLECT(evaluate_Expr_Term(BinOpExpr_Term_c{123.4, Add, 456.7}, ctx)) << '\n'
               << '\n';

    return 0;
}
