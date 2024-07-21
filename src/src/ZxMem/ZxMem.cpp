#include <ZxMem/ZxMem.h>
#include <ZxMem/Platform.h>
#include <format>


namespace ZQF
{
    ZxMem::ZxMem()
    {

    }

    ZxMem::~ZxMem()
    {

    }

    ZxMem::ZxMem(const std::size_t nSize)
    {
        this->Resize(nSize, true, true);
    }

    ZxMem::ZxMem(const ZxMem& rfOBJ)
    {
        this->operator=(rfOBJ);
    }

    ZxMem::ZxMem(ZxMem&& rfOBJ) noexcept
    {
        this->operator=(std::move(rfOBJ));
    }

    ZxMem::ZxMem(const std::string_view msPath, const std::size_t nReadSize)
    {
        this->Load(msPath, nReadSize);
    }

    auto ZxMem::operator=(const ZxMem& rfOBJ) -> ZxMem&
    {
        assert(this != &rfOBJ);

        m_nPos = rfOBJ.m_nPos;
        m_nSizeBytes = rfOBJ.m_nSizeBytes;
        if (rfOBJ.m_upMemData != nullptr)
        {
            m_upMemData = std::make_unique_for_overwrite<std::uint8_t[]>(rfOBJ.m_nSizeBytes);
            std::memcpy(m_upMemData.get(), rfOBJ.m_upMemData.get(), m_nSizeBytes);
        }

        return *this;
    }

    auto ZxMem::operator=(ZxMem&& rfOBJ) noexcept -> ZxMem&
    {
        assert(this != &rfOBJ);

        m_nPos = rfOBJ.m_nPos;
        m_nSizeBytes = rfOBJ.m_nSizeBytes;
        m_upMemData = std::move(rfOBJ.m_upMemData);

        rfOBJ.m_nPos = 0;
        rfOBJ.m_nSizeBytes = 0;

        return *this;
    }

    auto ZxMem::Resize(const std::size_t nNewSizeBytes, const bool isDiscard , const bool isRewindPos) -> ZxMem&
    {
        if (m_upMemData == nullptr)
        {
            m_upMemData = std::make_unique_for_overwrite<uint8_t[]>(nNewSizeBytes);
            m_nMaxSizeBytes = nNewSizeBytes;
        }
        else if (m_nMaxSizeBytes < nNewSizeBytes)
        {
            auto tmp = std::make_unique_for_overwrite<uint8_t[]>(nNewSizeBytes);
            if (isDiscard == false)
            {
                std::memcpy(tmp.get(), m_upMemData.get(), m_nSizeBytes);
            }
            m_upMemData = std::move(tmp);
            m_nMaxSizeBytes = nNewSizeBytes;
        }

        if (isRewindPos)
        {
            this->PosRewind();
        }

        m_nSizeBytes = nNewSizeBytes;
        return *this;
    }

    auto ZxMem::PosInc(const std::size_t nBytes) -> ZxMem&
    {
        return this->PosSet<ZxMem::PosWay::Cur>(nBytes);
    }

    auto ZxMem::PosRewind() -> ZxMem&
    {
        return this->PosSet<ZxMem::PosWay::Beg>(0);
    }

    auto ZxMem::Save(const std::string_view msPath, const bool isCoverExists, const bool isCreateDirectories) const -> const ZxMem&
    {
        bool status = ZxMemPrivate::SaveDataViaPathImp(msPath, this->Span(), isCoverExists, isCreateDirectories);
        if (status == false) { throw std::runtime_error(std::format("ZxMem::Save(): save data error! -> msPath: {}", msPath)); }
        return *this;
    }

    auto ZxMem::Load(const std::string_view msPath, const std::size_t nReadSize) -> ZxMem&
    {
        if (const auto file_handle_opt = ZxMemPrivate::FileOpenViaReadMode(msPath))
        {
            const auto file_handle = *file_handle_opt;
            if (const auto file_size_opt = ZxMemPrivate::FileGetSize(file_handle))
            {
                const std::size_t file_size = static_cast<std::size_t>(*file_size_opt);
                const std::size_t read_size_bytes = nReadSize == static_cast<size_t>(-1) ? file_size : nReadSize;
                this->Resize(read_size_bytes, true, true);
                if (const auto read_bytes_opt = ZxMemPrivate::FileRead(file_handle, this->Span()))
                {
                    if (*read_bytes_opt == read_size_bytes)
                    {
                        ZxMemPrivate::FileClose(file_handle);
                        return this->PosRewind();
                    }
                }
            }

            ZxMemPrivate::FileClose(file_handle);
        }

        throw std::runtime_error(std::format("ZxMem::Load(): error! -> msPath: {}", msPath));
    }
}
