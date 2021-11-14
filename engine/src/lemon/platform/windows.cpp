#include <lemon/platform/windows.h>
#include <shlobj_core.h>

using namespace lemon::platform;

GUID
convertSystemPath(SystemPath id)
{
    switch (id) {
    case SystemPath::UserDocuments:
        return FOLDERID_Documents;
    case SystemPath::UserAppData:
        return FOLDERID_LocalAppData;
    case SystemPath::UserSavedGames:
        return FOLDERID_SavedGames;
    case SystemPath::UserScreenshots:
        return FOLDERID_Screenshots;
    case SystemPath::SharedAppData:
        return FOLDERID_ProgramData;
    case SystemPath::Fonts:
        return FOLDERID_Fonts;
    case SystemPath::OSRoot:
        return FOLDERID_Windows;
    default:
        LEMON_UNREACHABLE();
    }

    return GUID{};
}

std::filesystem::path
lemon::platform::getSystemPath(SystemPath sysPath)
{
    std::filesystem::path dir;
    PWSTR pDir;

    if (SUCCEEDED(SHGetKnownFolderPath(convertSystemPath(sysPath), 0, nullptr, &pDir))) {
        dir = pDir;
    }

    CoTaskMemFree(pDir);

    return dir;
}
