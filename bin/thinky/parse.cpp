// 2021.07.01 - Victor Dods

#include "parse.hpp"

#include <lvd/fmt.hpp>
#include <lvd/literal.hpp>
#include <vector>

void trim_whitespace (std::string_view &s) {
    s.remove_prefix(std::min(s.find_first_not_of("\t\n "), s.size()));
    s.remove_suffix(s.size() - std::min(s.find_last_not_of("\t\n ")+1, s.size()));
}

// // Finds matching close paren and then returns a view into the interior of the parenthesized portion of the string.
// std::string_view::size_type find_matching_close_paren (std::string_view const &s, std::string_view::size_type open_paren_pos) noexcept(false) {
//     assert(open_paren_pos < s.size() && s[open_paren_pos] == '(');
//     size_t paren_level = 0;
//     for (std::string_view::size_type i = 0; i < s.size(); ++i) {
//         if (s[i] == '(') {
//             ++paren_level;
//         } else if (s[i] == ')') {
//             if (paren_level == 0) {
//                 throw std::runtime_error("unexpected ')' -- there was no matching '(' in " + lvd::literal_of(s));
//             }
//             --paren_level;
//             if (paren_level == 0) {
//                 // We've found the matching close paren.
//                 return i;
//             }
//         }
//     }
//     throw std::runtime_error("no ')' to match '(' in " + lvd::literal_of(s));
// }

// sept::TupleTerm_c parse_tuple_interior (std::string_view const &s) noexcept(false) {
//     trim_whitespace(s);
//
//     std::vector<sept::Data> tuple_elements;
//     while (true) {
//         auto pos = s.find_first_of("\t\n ");
//         if (pos == std::string_view::npos) {
//             // Done
//             break;
//         } else {
//             // Otherwise parse the content.
//         }
//     }
// }

// // This will account for parenthesized expressions.
// std::string_view::size_type find_next_whitespace_or_end (std::string_view const &s, std::string_view::size_type pos = 0) noexcept(false) {
//     size_t paren_level = 0;
//     for ( ; pos < s.size(); ++pos) {
//         char c = s[pos];
//         if (c == '(') {
//             ++paren_level;
//         } else if (c == ')') {
//             if (paren_level == 0) {
//                 throw std::runtime_error("unexpected ')' -- there was no matching '(' in " + lvd::literal_of(s));
//             }
//             --paren_level;
//         } else if (c == '\t' || c == '\n' || c == ' ') {
//             if (paren_level == 0) {
//                 return pos;
//             }
//         }
//     }
//     return pos;
// }

// This will account for parenthesized expressions.  Delimiters are whitespace,
// or transition from parens to or from non-paren chars.
std::string_view::size_type find_next_delimiter (std::string_view const &s, std::string_view::size_type pos = 0) noexcept(false) {
//     lvd::g_log << lvd::Log::dbg() << LVD_CALL_SITE() << " - " << LVD_REFLECT(lvd::literal_of(s)) << ", " << LVD_REFLECT(pos) << '\n';
//     auto ig = lvd::IndentGuard(lvd::g_log);
    size_t paren_level = 0;
    char prev_char = '\0'; // Sentinel value
    for ( ; pos < s.size(); ++pos) {
        char c = s[pos];
//         lvd::g_log << lvd::Log::dbg() << LVD_REFLECT(prev_char) << ", " << LVD_REFLECT(c) << ", " << LVD_REFLECT(paren_level) << '\n';
        if (c == '(') {
            if (paren_level == 0 && prev_char != '\0') {
                // If we're not at the beginning and we're at paren_level 0, an open paren is a delimiter.
//                 lvd::g_log << lvd::Log::dbg() << "returning...\n";
                return pos;
            }
//             lvd::g_log << lvd::Log::dbg() << "++paren_level\n";
            ++paren_level;
        } else if (c == ')') {
            if (paren_level == 0) {
                throw std::runtime_error(LVD_FMT("unexpected ')' -- there was no matching '(' in " << lvd::literal_of(s) << " starting at position " << pos << " which is substring " << lvd::literal_of(s.substr(pos))));
            }
//             lvd::g_log << lvd::Log::dbg() << "--paren_level\n";
            --paren_level;
        } else if (c == '\t' || c == '\n' || c == ' ') {
            if (paren_level == 0) {
                // Whitespace (at paren_level 0) is definitely a delimiter.
//                 lvd::g_log << lvd::Log::dbg() << "returning...\n";
                return pos;
            }
        } else {
            // c is a non-whitespace, non-paren char, so detect empty delimiters.
            if (prev_char == ')' && paren_level == 0) {
//                 lvd::g_log << lvd::Log::dbg() << "returning...\n";
                return pos;
            }
        }
        prev_char = c;
    }
//     lvd::g_log << lvd::Log::dbg() << "returning...\n";
    return pos;
}

std::vector<std::string_view> split (std::string_view const &s) noexcept(false) {
    std::vector<std::string_view> retval;
    std::string_view::size_type pos = 0;
    while (pos < s.size()) {
        auto start = s.find_first_not_of("\t\n ", pos);
        if (start >= s.size())
            break;
        auto next = find_next_delimiter(s, start);
        assert(next > start);
        retval.emplace_back(s.substr(start, next-start));
        pos = next;
    }
    return retval;
}

sept::Data parse_data (std::string_view const &s, bool interior) noexcept(false) {
    std::string_view s2 = s;
    trim_whitespace(s2);

    auto tokens = split(s2);
    if (tokens.empty()) {
        return interior ? sept::Data{sept::Tuple()} : sept::Data{sept::Void};
    } else if (tokens.size() == 1) {
        auto &token = tokens[0];
        assert(!token.empty());
        if (token[0] == '(') {
            // Take off the parens and parse the interior.
            assert(token[token.size()-1] == ')');
            token.remove_prefix(1);
            token.remove_suffix(1);
            return interior ? sept::Tuple(parse_data(token, true)) : parse_data(token, true);
        } else {
            auto terminal = thinky_np_term_from_string(std::string{token});
            return interior ? sept::Tuple(terminal) : sept::Data{terminal};
        }
    } else {
        // Parse tuple
        std::vector<sept::Data> tuple_elements;
        tuple_elements.reserve(tokens.size());
        for (auto const &token : tokens) {
            tuple_elements.emplace_back(parse_data(token, false));
        }
        return sept::TupleTerm_c{std::move(tuple_elements)};
    }
}

// TEMP HACK
namespace std {

ostream &operator<< (ostream &out, vector<string_view> const &v) {
    lvd::Log log(out);
    log << "vector<string_view>[\n";
    {
        auto ig = lvd::IndentGuard(log);
        for (auto const &s : v) {
            log << lvd::literal_of(string(s)) << '\n';
        }
    }
    log << "]\n";
    return out;
}

} // end namespace std

void parse_test () {
    lvd::g_log << lvd::Log::dbg()
        << LVD_REFLECT(split("")) << '\n'
        << LVD_REFLECT(split(" ")) << '\n'
        << LVD_REFLECT(split("\t")) << '\n'
        << LVD_REFLECT(split("\n")) << '\n'
        << LVD_REFLECT(split("\n ")) << '\n'
        << LVD_REFLECT(split("\n\t    ")) << '\n'
        << LVD_REFLECT(split("x")) << '\n'
        << LVD_REFLECT(split(" x")) << '\n'
        << LVD_REFLECT(split("x ")) << '\n'
        << LVD_REFLECT(split("()")) << '\n'
        << LVD_REFLECT(split("(  )")) << '\n'
        << LVD_REFLECT(split(" (  )")) << '\n'
        << LVD_REFLECT(split("(  ) ")) << '\n'
        << LVD_REFLECT(split(" (  ) ")) << '\n'
        << LVD_REFLECT(split("(x)")) << '\n'
        << LVD_REFLECT(split("( x )")) << '\n'
        << LVD_REFLECT(split("( x y)")) << '\n'
        << '\n'
        << LVD_REFLECT(split("x y")) << '\n'
        << LVD_REFLECT(split(" x y")) << '\n'
        << LVD_REFLECT(split("x y ")) << '\n'
        << LVD_REFLECT(split(" x y ")) << '\n'
        << LVD_REFLECT(split(" x   \t  y zpuei")) << '\n'
        << LVD_REFLECT(split(" x   \t  (y blah) zpuei")) << '\n'
        << LVD_REFLECT(split(" x   \t  (y blah) (zpuei)")) << '\n'
        << LVD_REFLECT(split(" (x)   \t  (y blah) (zpuei)")) << '\n'
        << LVD_REFLECT(split(" ()x   \t  (y blah) (zpuei)")) << '\n'
        << LVD_REFLECT(split(" x()   \t  (y blah) (zpuei)")) << '\n'
        << LVD_REFLECT(split(" x()x   \t  (y blah) (zpuei)")) << '\n'
        << LVD_REFLECT(split("(x)   \t  (y blah) (zpuei)")) << '\n'
        << LVD_REFLECT(split("()x   \t  (y blah) (zpuei)")) << '\n'
        << '\n'
        << LVD_REFLECT(split("(x y)")) << '\n'
        << LVD_REFLECT(split("( x y)")) << '\n'
        << LVD_REFLECT(split("(x y )")) << '\n'
        << LVD_REFLECT(split("( x y )")) << '\n'
        << LVD_REFLECT(split("( x   \t  y zpuei)")) << '\n'
        << LVD_REFLECT(split("( x   \t  (y blah) zpuei)")) << '\n'
        << LVD_REFLECT(split("( x   \t  (y blah) (zpuei))")) << '\n'
        << LVD_REFLECT(split("( (x)   \t  (y blah) (zpuei))")) << '\n'
        << LVD_REFLECT(split("( ()x   \t  (y blah) (zpuei))")) << '\n'
        << LVD_REFLECT(split("( x()   \t  (y blah) (zpuei))")) << '\n'
        << LVD_REFLECT(split("( x()x   \t  (y blah) (zpuei))")) << '\n'
        << LVD_REFLECT(split("((x)   \t  (y blah) (zpuei))")) << '\n'
        << LVD_REFLECT(split("(()x   \t  (y blah) (zpuei))")) << '\n'
        << '\n'
        << LVD_REFLECT(split("(())")) << '\n'
        << LVD_REFLECT(split(" (())")) << '\n'
        << LVD_REFLECT(split("( ())")) << '\n'
        << LVD_REFLECT(split("(( ))")) << '\n'
        << LVD_REFLECT(split("(() )")) << '\n'
        << LVD_REFLECT(split("(()) ")) << '\n'
        << LVD_REFLECT(split("((()))")) << '\n'
        << '\n';

    lvd::g_log << lvd::Log::dbg()
        << LVD_REFLECT(parse_data("")) << '\n'
        << LVD_REFLECT(parse_data(" ")) << '\n'
        << LVD_REFLECT(parse_data("\t")) << '\n'
        << LVD_REFLECT(parse_data("\n")) << '\n'
        << LVD_REFLECT(parse_data("\n ")) << '\n'
        << LVD_REFLECT(parse_data("\n\t    ")) << '\n'
        << LVD_REFLECT(parse_data("Cup")) << '\n'
        << LVD_REFLECT(parse_data(" Cup")) << '\n'
        << LVD_REFLECT(parse_data("Cup ")) << '\n'
        << LVD_REFLECT(parse_data("()")) << '\n'
        << LVD_REFLECT(parse_data("(  )")) << '\n'
        << LVD_REFLECT(parse_data(" (  )")) << '\n'
        << LVD_REFLECT(parse_data("(  ) ")) << '\n'
        << LVD_REFLECT(parse_data(" (  ) ")) << '\n'
        << LVD_REFLECT(parse_data("(Cup)")) << '\n'
        << LVD_REFLECT(parse_data("( Cup )")) << '\n'
        << LVD_REFLECT(parse_data("( Cup Hat)")) << '\n'
        << '\n'
        << LVD_REFLECT(parse_data("Cup Hat")) << '\n'
        << LVD_REFLECT(parse_data(" Cup Hat")) << '\n'
        << LVD_REFLECT(parse_data("Cup Hat ")) << '\n'
        << LVD_REFLECT(parse_data(" Cup Hat ")) << '\n'
        << LVD_REFLECT(parse_data(" Cup   \t  Hat Charlie")) << '\n'
        << LVD_REFLECT(parse_data(" Cup   \t  (Hat Cool) Charlie")) << '\n'
        << LVD_REFLECT(parse_data(" Cup   \t  (Hat Cool) (Charlie)")) << '\n'
        << LVD_REFLECT(parse_data(" (Cup)   \t  (Hat Cool) (Charlie)")) << '\n'
        << LVD_REFLECT(parse_data(" ()Cup   \t  (Hat Cool) (Charlie)")) << '\n'
        << LVD_REFLECT(parse_data(" Cup()   \t  (Hat Cool) (Charlie)")) << '\n'
        << LVD_REFLECT(parse_data(" Cup()Cup   \t  (Hat Cool) (Charlie)")) << '\n'
        << LVD_REFLECT(parse_data("(Cup)   \t  (Hat Cool) (Charlie)")) << '\n'
        << LVD_REFLECT(parse_data("()Cup   \t  (Hat Cool) (Charlie)")) << '\n'
        << '\n'
        << LVD_REFLECT(parse_data("(Cup Hat)")) << '\n'
        << LVD_REFLECT(parse_data("( Cup Hat)")) << '\n'
        << LVD_REFLECT(parse_data("(Cup Hat )")) << '\n'
        << LVD_REFLECT(parse_data("( Cup Hat )")) << '\n'
        << LVD_REFLECT(parse_data("( Cup   \t  Hat Charlie)")) << '\n'
        << LVD_REFLECT(parse_data("( Cup   \t  (Hat Cool) Charlie)")) << '\n'
        << LVD_REFLECT(parse_data("( Cup   \t  (Hat Cool) (Charlie))")) << '\n'
        << LVD_REFLECT(parse_data("( (Cup)   \t  (Hat Cool) (Charlie))")) << '\n'
        << LVD_REFLECT(parse_data("( ()Cup   \t  (Hat Cool) (Charlie))")) << '\n'
        << LVD_REFLECT(parse_data("( Cup()   \t  (Hat Cool) (Charlie))")) << '\n'
        << LVD_REFLECT(parse_data("( Cup()Cup   \t  (Hat Cool) (Charlie))")) << '\n'
        << LVD_REFLECT(parse_data("((Cup)   \t  (Hat Cool) (Charlie))")) << '\n'
        << LVD_REFLECT(parse_data("(()Cup   \t  (Hat Cool) (Charlie))")) << '\n'
        << '\n'
        << LVD_REFLECT(parse_data("(())")) << '\n'
        << LVD_REFLECT(parse_data(" (())")) << '\n'
        << LVD_REFLECT(parse_data("( ())")) << '\n'
        << LVD_REFLECT(parse_data("(( ))")) << '\n'
        << LVD_REFLECT(parse_data("(() )")) << '\n'
        << LVD_REFLECT(parse_data("(()) ")) << '\n'
        << LVD_REFLECT(parse_data("((()))")) << '\n'
        << '\n';


}
