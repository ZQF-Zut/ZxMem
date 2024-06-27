#pragma once
#include <span>
#include <memory>
#include <string_view>


namespace ZQF
{
#ifdef _WIN32
#include <windef.h>
    using FILE_HANLDE_TYPE = HADNLE;
#elif __linux__
    using FILE_HANLDE_TYPE = int;
#endif

    auto FileOpenViaReadMode(const std::string_view msPath) -> FILE_HANLDE_TYPE;
    auto FileGetSize(FILE_HANLDE_TYPE hFile) -> size_t;
    auto FileClose(FILE_HANLDE_TYPE hFile) -> void;
    auto FileRead(FILE_HANLDE_TYPE hFile, const std::span<uint8_t> spData) -> void;
    auto StoreBytesViaPath(const std::string_view msPath, const std::span<uint8_t> spData, bool isCoverExists, bool isCreateDirectories) -> void;
} // namespace ZQF::ZxJson
