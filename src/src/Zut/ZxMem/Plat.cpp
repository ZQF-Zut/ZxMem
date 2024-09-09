#include "Plat.h"
#include <span>
#include <memory>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif __linux__
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#endif


namespace ZQF::Zut::ZxMemPlat
{
#ifdef _WIN32
    static auto PathUtf8ToWide(const std::string_view msPath) -> std::pair<std::wstring_view, std::unique_ptr<wchar_t[]>>
    {
        const auto buffer_max_chars = ((msPath.size() * sizeof(char)) + 1) * 2;
        auto buffer = std::make_unique_for_overwrite<wchar_t[]>(buffer_max_chars);
        const auto char_count_real = static_cast<std::size_t>(::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, msPath.data(), static_cast<int>(msPath.size()), buffer.get(), static_cast<int>(buffer_max_chars)));
        buffer[char_count_real] = {};
        std::wstring_view cvt_sv{ buffer.get(), char_count_real };
        return { cvt_sv, std::move(buffer) };
    }

    static auto CreateDirectories(const std::pair<std::wstring_view, std::unique_ptr<wchar_t[]>>& rfWidePath) -> void
    {
        auto& [path_wide_sv, path_wide_buffer] = rfWidePath;

        const auto pos = path_wide_sv.rfind(L'/');
        if ((pos == std::wstring_view::npos) || (pos == 1)) { return; }

        const auto file_name_mask_char_tmp = path_wide_buffer.get()[pos + 1];
        path_wide_buffer.get()[pos + 1] = {};
        {
            wchar_t* cur_path_cstr = path_wide_buffer.get();
            const wchar_t* org_path_cstr = path_wide_buffer.get();

            while (*cur_path_cstr++ != L'\0')
            {
                if (*cur_path_cstr != L'/') { continue; }

                *cur_path_cstr = {};
                {
                    if (::GetFileAttributesW(org_path_cstr) == INVALID_FILE_ATTRIBUTES)
                    {
                        ::CreateDirectoryW(org_path_cstr, nullptr);
                    }
                }
                *cur_path_cstr = L'/';

                cur_path_cstr++;
            }
        }
        path_wide_buffer.get()[pos + 1] = file_name_mask_char_tmp;
    }

    auto FileOpenViaReadMode(const std::string_view msPath) -> std::optional<FILE_HANLDE_TYPE>
    {
        const auto hfile = ::CreateFileW(PathUtf8ToWide(msPath).second.get(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        return (hfile == INVALID_HANDLE_VALUE) ? std::nullopt : std::optional{ hfile };
    }

    auto FileGetSize(FILE_HANLDE_TYPE hFile) -> std::optional<std::uint64_t>
    {
        LARGE_INTEGER file_size_large{};
        const auto status = ::GetFileSizeEx(hFile, &file_size_large);
        return status != FALSE ? std::optional{ static_cast<std::size_t>(file_size_large.QuadPart) } : std::nullopt;
    }

    auto FileClose(FILE_HANLDE_TYPE hFile) -> bool
    {
        const auto status = ::CloseHandle(hFile);
        return status != FALSE ? true : false;
    }

    auto FileRead(FILE_HANLDE_TYPE hFile, const std::span<std::uint8_t> spBuffer) -> std::optional<std::size_t>
    {
        DWORD read_bytes{};
        const auto status = ::ReadFile(hFile, spBuffer.data(), static_cast<DWORD>(spBuffer.size_bytes()), &read_bytes, nullptr);
        return status != FALSE ? std::optional{ static_cast<std::size_t>(read_bytes) } : std::nullopt;
    }

    auto SaveDataViaPathImp(const std::string_view msPath, const std::span<const std::uint8_t> spData, const bool isCoverExists, const bool isCreateDirectories) -> bool
    {
        auto wide_path = PathUtf8ToWide(msPath);
        if (isCreateDirectories) { ZxMemPlat::CreateDirectories(wide_path); }
        const auto hfile = ::CreateFileW(wide_path.first.data(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, isCoverExists ? CREATE_ALWAYS : CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hfile == INVALID_HANDLE_VALUE) { return false; }
        DWORD write{};
        const auto staus = ::WriteFile(hfile, spData.data(), static_cast<DWORD>(spData.size_bytes()), &write, nullptr);
        if (staus == FALSE)
        {
            (void)::CloseHandle(hfile);
            return false;
        }
        return ::CloseHandle(hfile) != FALSE;
    }
#else
    auto FileOpenViaReadMode(const std::string_view msPath) -> std::optional<FILE_HANLDE_TYPE>
    {
        const auto file_handle{ ::open(msPath.data(), O_RDONLY, 0666) };
        return (file_handle == -1) ? std::nullopt : std::optional{ file_handle };
    }

    auto FileGetSize(const FILE_HANLDE_TYPE hFile) -> std::optional<std::uint64_t>
    {
        struct ::stat s;
        return (::fstat(hFile, &s) == -1) ? std::nullopt : std::optional{ static_cast<std::uint64_t>(s.st_size) };
    }

    auto FileClose(const FILE_HANLDE_TYPE hFile) -> bool
    {
        return (::close(hFile) == -1) ? false : true;
    }

    auto FileRead(const FILE_HANLDE_TYPE hFile, const std::span<std::uint8_t> spBuffer) -> std::optional<std::size_t>
    {
        const auto read_bytes{ ::read(hFile, spBuffer.data(), spBuffer.size_bytes()) };
        return (read_bytes == -1) ? std::nullopt : std::optional{ static_cast<std::size_t>(read_bytes) };
    }

    static auto CreateDirectories(const std::string_view msPath) -> void
    {
        const auto pos{ msPath.rfind('/') };
        if ((pos == std::string_view::npos) || (pos == 1)) { return; }

        const auto path_buffer{ std::make_unique_for_overwrite<char[]>(msPath.size()) };
        std::memcpy(path_buffer.get(), msPath.data(), msPath.size());

        path_buffer.get()[pos + 1] = {}; // rm file_name

        char* cur_path_cstr{ path_buffer.get() };
        const char* org_path_cstr{ path_buffer.get() };

        while (*cur_path_cstr++ != '\0')
        {
            if (*cur_path_cstr != '/') { continue; }

            const char slash_char_tmp{ *cur_path_cstr };
            *cur_path_cstr = {};
            {
                if (::access(org_path_cstr, X_OK) == -1)
                {
                    ::mkdir(org_path_cstr, 0777);
                }
            }
            *cur_path_cstr = slash_char_tmp;
            cur_path_cstr++;
        }
    }

    auto SaveDataViaPathImp(const std::string_view msPath, const std::span<const std::uint8_t> spData, const bool isCoverExists, const bool isCreateDirectories) -> bool
    {
        if (isCreateDirectories) { ZxMemPlat::CreateDirectories(msPath); }
        constexpr auto create_always{ O_CREAT | O_WRONLY | O_TRUNC };
        constexpr auto create_new{ O_CREAT | O_WRONLY | O_EXCL };
        const auto file_handle{ ::open(msPath.data(), isCoverExists ? create_always : create_new, 0666) };
        if (file_handle == -1) { return false; }
        const auto written_bytes{ ::write(file_handle, spData.data(), spData.size_bytes()) };
        if (written_bytes == -1) { ::close(file_handle); return false; }
        return (::close(file_handle) == -1) ? false : true;
    }
#endif
} // namespace ZQF::ZxMemPrivate
