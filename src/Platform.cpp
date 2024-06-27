#include <ZxMem/Platform.h>
#include <span>
#include <format>

#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#endif


namespace ZQF
{
#ifdef _WIN32
    static auto PathUtf8ToWide(const std::string_view msPath) -> std::pair<std::wstring_view, std::unique_ptr<wchar_t[]>>
    {
        const auto char_count = static_cast<size_t>(::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, msPath.data(), static_cast<int>(msPath.size()), nullptr, 0));
        auto buffer = std::make_unique_for_overwrite<wchar_t[]>(char_count + 1);
        const auto char_count_real = static_cast<size_t>(::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, msPath.data(), static_cast<int>(msPath.size()), buffer.get(), static_cast<int>(char_count)));
        buffer[char_count_real] = {};
        return { { buffer.get(), char_count }, std::move(buffer) };
    }

    static auto CreateDirectories(std::pair<std::wstring_view, std::unique_ptr<wchar_t[]>>& rfWidePath) -> void
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

                const wchar_t slash_char_tmp = *cur_path_cstr;
                *cur_path_cstr = {};
                {
                    if (::GetFileAttributesW(org_path_cstr) == INVALID_FILE_ATTRIBUTES)
                    {
                        ::CreateDirectoryW(org_path_cstr, nullptr);
                    }
                }
                *cur_path_cstr = slash_char_tmp;
                cur_path_cstr++;
            }
        }
        path_wide_buffer.get()[pos + 1] = file_name_mask_char_tmp;
    }

    auto FileOpenViaReadMode(const std::string_view msPath) -> FILE_HANLDE_TYPE
    {
        const HANDLE hfile = ::CreateFileW(PathUtf8ToWide(msPath).second.get(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hfile == INVALID_HANDLE_VALUE) { throw std::runtime_error(std::format("ZxMem::RreadAllBytes(): open file error!. msPath: {}", msPath)); }
        return hfile;
    }

    auto FileGetSize(FILE_HANLDE_TYPE hFile) -> size_t
    {
        return static_cast<size_t>(::GetFileSize(hFile, nullptr));
    }

    auto FileClose(FILE_HANLDE_TYPE hFile) -> void
    {
        ::CloseHandle(hFile);
    }

    auto FileRead(FILE_HANLDE_TYPE hFile, const std::span<uint8_t> spData) -> void
    {
        DWORD read{};
        (void)::ReadFile(hFile, spData.data(), static_cast<DWORD>(spData.size_bytes()), &read, nullptr);
    }

    auto StoreBytesViaPath(const std::string_view msPath, const std::span<uint8_t> spData, bool isCoverExists, bool isCreateDirectories) -> void
    {
        auto wide_path = PathUtf8ToWide(msPath);
        if (isCreateDirectories) { CreateDirectories(wide_path); }
        const HANDLE hfile = ::CreateFileW(wide_path.first.data(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, isCoverExists ? CREATE_ALWAYS : CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hfile == INVALID_HANDLE_VALUE) { throw std::runtime_error(std::format("ZxMem::WriteAllBytes(): create file error!. msPath: {}", msPath)); }
        DWORD write{};
        (void)::WriteFile(hfile, spData.data(), static_cast<DWORD>(spData.size_bytes()), &write, nullptr);
        ::CloseHandle(hfile);
    }
#else
    auto FileOpenViaReadMode(const std::string_view msPath) -> FILE_HANLDE_TYPE
    {
        const auto file_handle = ::open(msPath.data(), O_RDONLY, 0666);
        if (file_handle == -1) { throw std::runtime_error(std::format("ZxMem::RreadAllBytes(): open file error!. msPath: {}", msPath)); }
        return file_handle;
    }

    auto FileGetSize(FILE_HANLDE_TYPE hFile) -> size_t
    {
        const auto file_size = ::lseek(hFile, 0, SEEK_END);
        ::lseek(hFile, 0, SEEK_SET);
        return static_cast<size_t>(file_size);
    }

    auto FileClose(FILE_HANLDE_TYPE hFile) -> void
    {
        ::close(hFile);
    }

    auto FileRead(FILE_HANLDE_TYPE hFile, const std::span<uint8_t> spData) -> void
    {
        ::read(hFile, spData.data(), spData.size_bytes());
    }

    static auto CreateDirectories(const std::string_view msPath) -> void
    {
        const auto pos = msPath.rfind('/');
        if ((pos == std::string_view::npos) || (pos == 1)) { return; }

        auto path_buffer = std::make_unique_for_overwrite<char[]>(msPath.size());
        memcpy(path_buffer.get(), msPath.data(), msPath.size());

        path_buffer.get()[pos + 1] = {}; // rm file_name

        char* cur_path_cstr = path_buffer.get();
        const char* org_path_cstr = path_buffer.get();

        while (*cur_path_cstr++ != '\0')
        {
            if (*cur_path_cstr != '/') { continue; }

            const char slash_char_tmp = *cur_path_cstr;
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

    auto StoreBytesViaPath(const std::string_view msPath, const std::span<uint8_t> spData, bool isCoverExists, bool isCreateDirectories) -> void
    {
        if (isCreateDirectories) { ZQF::CreateDirectories(msPath); }
        constexpr auto create_always = O_CREAT | O_WRONLY | O_TRUNC;
        constexpr auto create_new = O_CREAT | O_WRONLY | O_EXCL;
        const auto file_handle = ::open(msPath.data(), isCoverExists ? create_always : create_new, 0666);  // NOLINT
        if (file_handle == -1) { throw std::runtime_error(std::format("ZxMem::WriteAllBytes(): create file failed!", msPath)); }
        ::write(file_handle, spData.data(), spData.size_bytes());
        ::close(file_handle);
    }
#endif
} // namespace ZQF::ZxMem
