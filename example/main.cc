#include <iomanip>
#include <iostream>
#include <ranges>
#include <string_view>
 
int main()
{
    std::string_view words{"Hello^_^C++^_^20^_^!"};
    std::string_view delim{"^_^"};
 
    for (const auto word : std::views::split(words, delim))
        std::cout << std::quoted(std::string_view{word.begin(), word.end()}) << ' ';
}
