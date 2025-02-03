#ifndef TOOLPEX_MIME_TYPE_H_
#define TOOLPEX_MIME_TYPE_H_

#include <string_view>
#include <filesystem>

namespace toolpex 
{

::std::string_view get_mime_type(const ::std::filesystem::path& file_name);

} // namespace toolpex

#endif
