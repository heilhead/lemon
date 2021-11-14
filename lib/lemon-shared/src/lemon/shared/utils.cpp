#include <locale>
#include <codecvt>
#include <string>

#include <lemon/shared/logger.h>
#include <lemon/shared/utils.h>

using namespace lemon;

void
lemon::utils::halt(gsl::czstring<> message, int code)
{
    if (message != nullptr) {
        if (code > 0) {
            logger::fatal(message);
        } else {
            logger::log(message);
        }
    }

    exit(code);
}

void
lemon::utils::terminate()
{
    std::terminate();
}

std::string
lemon::utils::ws2s(const std::wstring& str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(str);
}

std::wstring
lemon::utils::s2ws(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}
