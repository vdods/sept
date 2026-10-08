// 2021.07.01 - Victor Dods

#pragma once

#include "ast.hpp"
#include "common.hpp"
#include "sept/Data.hpp"
#include <string_view>

// Basic, minimal parser for ASTs in thinky.
// Possibilities are:
// -    "" or "()" -- empty tuple.
// -    "()" -- s.
// -    "T" -- terminal.
// -    "D1 ... Dn" or "(D1 ... Dn)" tuple with n elements, where each Di is a parseable string.
sept::Data parse_data (std::string_view const &s, bool interior = false) noexcept(false);

void parse_test ();
