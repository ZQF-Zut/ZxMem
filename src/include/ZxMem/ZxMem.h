#pragma once
#include <span>
#include <string>
#include <memory>
#include <cstring>
#include <cassert>
#include <stdexcept>


namespace ZQF
{
    class ZxMem
    {
    private:
        std::size_t m_nPos{};
        std::size_t m_nSizeBytes{};
        std::unique_ptr<std::uint8_t[]> m_upMemData;
    public:
        enum class PosWay
        {
            Beg,
            Cur,
            End
        };

    public:
        ZxMem();
        ~ZxMem();
        ZxMem(const std::size_t nSize);
        ZxMem(const ZxMem& rfOBJ);
        ZxMem(ZxMem&& rfOBJ) noexcept;
        ZxMem(const std::string_view msPath, std::size_t nReadSize = static_cast<size_t>(-1));
        auto operator=(const ZxMem& rfOBJ)->ZxMem&;
        auto operator=(ZxMem&& rfOBJ) noexcept -> ZxMem&;

    public:
        auto Resize(const std::size_t nNewSizeBytes, const bool isDiscard = false) -> ZxMem&;

        template <class T = std::size_t>
        auto SizeBytes() const noexcept -> T;

        template <class T = std::uint8_t*>
        auto Ptr() const noexcept -> T;

        template <class T = std::uint8_t*>
        auto PtrCur() const noexcept -> T;

        template <class T = std::uint8_t>
        auto Span() const noexcept -> std::span<T>;

        template <class T = size_t>
        auto Pos() const->T;

        template <ZxMem::PosWay eWay = ZxMem::PosWay::Beg>
        auto PosSet(const std::size_t nBytes = 0) -> ZxMem&;

        auto PosInc(const std::size_t nBytes) -> ZxMem&;
        auto PosRewind() -> ZxMem&;

    public:
        template <class T, std::size_t S>
        auto Read(const std::span<T, S> spData) -> void;

        template <class T, std::size_t S>
        auto Write(const std::span<T, S> spData) -> void;

        template<class T>
        auto operator>>(T& rfData)->ZxMem&;

        template<class T>
        auto operator<<(T&& rfData)->ZxMem&;

        template <class T>
        auto Get() -> T;

        template <class T>
        auto Put(const T& rfData) -> ZxMem&;

    public:
        auto Save(const std::string_view msPath, const bool isCoverExists = true, const bool isCreateDirectories = true) const -> const ZxMem&;
        auto Load(const std::string_view msPath, const std::size_t nReadSize = static_cast<size_t>(-1)) -> ZxMem&;
    };

    template <class T>
    inline auto ZxMem::Ptr() const noexcept -> T
    {
        if constexpr (std::is_pointer_v<T>)
        {
            return reinterpret_cast<T>(m_upMemData.get());
        }
        else
        {
            static_assert(false, "ZxMem::Ptr<T>(): not pointer type!");
        }
    }

    template<class T>
    inline auto ZxMem::PtrCur() const noexcept -> T
    {
        assert(m_nPos <= m_nSizeBytes);
        if constexpr (std::is_pointer_v<T>)
        {
            return reinterpret_cast<T>(this->Ptr<uint8_t*>() + m_nPos);
        }
        else
        {
            static_assert(false, "ZxMem::PtrCur<T>(): not pointer type!");
        }
    }

    template <class T>
    inline auto ZxMem::Span() const noexcept -> std::span<T>
    {
        return std::span{ this->Ptr<T*>(), this->SizeBytes() / sizeof(T) };
    }

    template <class T>
    inline auto ZxMem::Pos() const -> T
    {
        if constexpr (std::is_integral_v<T>)
        {
            return static_cast<T>(m_nPos);
        }
        else
        {
            static_assert(false, "ZxMem::Pos<T>(): not integral type!");
        }
    }

    template <ZxMem::PosWay eWay>
    inline auto ZxMem::PosSet(const std::size_t nBytes) -> ZxMem&
    {
        if constexpr (eWay == ZxMem::PosWay::Beg)
        {
            m_nPos = nBytes;
        }
        else if constexpr (eWay == ZxMem::PosWay::Cur)
        {
            m_nPos += nBytes;
        }
        else if constexpr (eWay == ZxMem::PosWay::End)
        {
            m_nPos = m_nSizeBytes;
            m_nPos += nBytes;
        }

        if (m_nPos > m_nSizeBytes)
        {
            throw std::runtime_error("ZxMem::PosSet<T>(): out of size!");
        }

        return *this;
    }

    template <class T>
    inline auto ZxMem::SizeBytes() const noexcept -> T
    {
        if constexpr (std::is_integral_v<T>)
        {
            return static_cast<T>(m_nSizeBytes);
        }
        else
        {
            static_assert(false, "ZxMem::SizeBytes<T>(): not integral type!");
        }
    }

    template <class T, std::size_t S>
    inline auto ZxMem::Read(const std::span<T, S> spData) -> void
    {
        if (spData.empty()) { return; }
        std::memcpy(spData.data(), this->PtrCur<std::uint8_t*>(), spData.size_bytes());
        this->PosSet<ZxMem::PosWay::Cur>(spData.size_bytes());
    }

    template <class T, std::size_t S>
    inline auto ZxMem::Write(const std::span<T, S> spData) -> void
    {
        if (spData.empty()) { return; }
        std::memcpy(this->PtrCur<std::uint8_t*>(), spData.data(), spData.size_bytes());
        this->PosSet<ZxMem::PosWay::Cur>(spData.size_bytes());
    }

    template<class T>
    inline auto ZxMem::operator>>(T& rfData) -> ZxMem&
    {
        using T_decay = std::decay_t<decltype(rfData)>;

        if constexpr (std::is_integral_v<T_decay> || std::is_floating_point_v<T_decay>)
        {
            this->Read(std::span{ &rfData, 1 });
        }
        else
        {
            this->Read(std::span{ rfData });
        }

        return *this;
    }

    template<class T>
    inline auto ZxMem::operator<<(T&& rfData) -> ZxMem&
    {
        using T_decay = std::decay_t<decltype(rfData)>;

        if constexpr (std::is_integral_v<T_decay> || std::is_floating_point_v<T_decay>)
        {
            this->Write(std::span{ &rfData, 1 });
        }
        else
        {
            this->Write(std::span{ rfData });
        }

        return *this;
    }

    template <class T>
    inline auto ZxMem::Get() -> T
    {
        T tmp;
        this->operator>>(tmp);
        return tmp;
    }

    template <class T>
    inline auto ZxMem::Put(const T& rfData) -> ZxMem&
    {
        return this->operator<<(rfData);
    }
}
