#pragma once
#include <span>
#include <cstdint>
#include <optional>
#include <string_view>


namespace ZQF::Zut::ZxMemPlat
{
#ifdef _WIN32
    using FILE_HANLDE_TYPE = void*;
#elif __linux__
    using FILE_HANLDE_TYPE = int;
#endif

    auto FileOpenViaReadMode(const std::string_view msPath) -> std::optional<FILE_HANLDE_TYPE>;
    auto FileGetSize(const FILE_HANLDE_TYPE hFile) -> std::optional<std::uint64_t>;
    auto FileClose(const FILE_HANLDE_TYPE hFile) -> bool;
    auto FileRead(const FILE_HANLDE_TYPE hFile, const std::span<std::uint8_t> spBuffer) -> std::optional<std::size_t>;
    auto SaveDataViaPathImp(const std::string_view msPath, const std::span<const std::uint8_t> spData, const bool isCoverExists, const bool isCreateDirectories) -> bool;
} // namespace ZQF::ZxJson
