#include <ZxMem/ZxMem.h>
#include <ZxMem/Platform.h>
#include <format>
#include <cstdio>
#include <filesystem>


namespace ZQF
{
    ZxMem::ZxMem()
    {

    }

    ZxMem::~ZxMem()
    {

    }

    ZxMem::ZxMem(size_t nSize)
    {
        this->Resize(nSize, true);
    }

    ZxMem::ZxMem(const ZxMem& rfOBJ)
    {
        this->operator=(rfOBJ);
    }

    ZxMem::ZxMem(ZxMem&& rfOBJ) noexcept
    {
        this->operator=(std::move(rfOBJ));
    }

    ZxMem::ZxMem(const std::string_view msPath, size_t nReadSize)
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
            m_upMemData = std::make_unique_for_overwrite<uint8_t[]>(rfOBJ.m_nSizeBytes);
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

    auto ZxMem::Resize(size_t nNewSizeBytes, bool isDiscard) -> ZxMem&
    {
        if (m_upMemData == nullptr)
        {
            m_upMemData = std::make_unique_for_overwrite<uint8_t[]>(nNewSizeBytes);
        }
        else if (m_nSizeBytes < nNewSizeBytes)
        {
            auto tmp = std::make_unique_for_overwrite<uint8_t[]>(nNewSizeBytes);
            if (isDiscard == false)
            {
                ::memcpy(tmp.get(), m_upMemData.get(), m_nSizeBytes);
            }
            m_upMemData = std::move(tmp);
        }

        m_nSizeBytes = nNewSizeBytes;
        return *this;
    }

    auto ZxMem::PosInc(size_t nBytes) -> ZxMem&
    {
        return this->PosSet<ZxMem::PosWay::Cur>(nBytes);
    }

    auto ZxMem::PosRewind() -> ZxMem&
    {
        return this->PosSet<ZxMem::PosWay::Beg>(0);
    }

    auto ZxMem::Save(const std::string_view msPath, bool isCoverExists, bool isCreateDirectories) const -> const ZxMem&
    {
        ZQF::StoreBytesViaPath(msPath, this->Span(), isCoverExists, isCreateDirectories);
        return *this;
    }

    auto ZxMem::Load(const std::string_view msPath, size_t nReadSize) -> ZxMem&
    {
        auto file_handle = ZQF::FileOpenViaReadMode(msPath);
        size_t file_size = ZQF::FileGetSize(file_handle);

        if (!file_size) { throw std::runtime_error(std::format("ZxMem::Load(): get file size error!, msPath: {}", msPath)); }

        size_t read_size_bytes{};
        if (nReadSize == ZXMEM_AUTO_READ_SIZE)
        {
            read_size_bytes = file_size;
        }
        else if (nReadSize <= file_size)
        {
            read_size_bytes = nReadSize;
        }
        else
        {
            throw std::runtime_error(std::format("ZxMem::Load(): read size larger than file size!, msPath: {}", msPath));
        }

        ZQF::FileRead(file_handle, this->Resize(read_size_bytes, true).Span());
        ZQF::FileClose(file_handle);
        return this->PosRewind();
    }
}
