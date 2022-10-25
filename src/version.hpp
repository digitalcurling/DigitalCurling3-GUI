#ifndef DIGITALCURLING3_GUI_VERSION_HPP
#define DIGITALCURLING3_GUI_VERSION_HPP

#include <cstdint>
#include <string>

namespace digitalcurling3_gui {

std::string GetVersion();
std::uint32_t GetVersionMajor();
std::uint32_t GetVersionMinor();
std::uint32_t GetVersionPatch();
std::string GetGitSHA1();
std::string GetGitDate();

} // namespace digitalcurling3_gui

#endif // DIGITALCURLING3_GUI_VERSION_HPP
