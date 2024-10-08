#pragma once
#include <span>
#include <string>
#include <string_view>
#include <memory>
#include <cstring>
#include <cassert>
#include <stdexcept>
#include <Zut/ZxMem/Traits.h>


namespace ZQF::Zut
{
    class ZxMem
    {
    private:
        std::size_t m_nPos{};
        std::size_t m_nSizeBytes{};
        std::size_t m_nMaxSizeBytes{};
        std::unique_ptr<std::uint8_t[]> m_upMemData;

    public:
        enum class PosWay
        {
            Set,
            Cur,
            End
        };

    public:
        ZxMem();
        ~ZxMem();
        ZxMem(const ZxMem& rfOBJ);
        ZxMem(ZxMem&& rfOBJ) noexcept;
        ZxMem(const std::size_t nSize);
        ZxMem(const std::string_view msPath, std::size_t nReadSize = static_cast<size_t>(-1));

    public:
        auto operator=(const ZxMem& rfOBJ)->ZxMem&;
        auto operator=(ZxMem&& rfOBJ) noexcept -> ZxMem&;
        template <class T> auto operator>>(T&& rfData)->ZxMem&;
        template <class T> auto operator<<(T&& rfData)->ZxMem&;

    public:
        template <class T = std::size_t>     auto PosCur() const -> T;
        template <class T = std::uint8_t*>   auto Ptr() const noexcept -> T;
        template <class T = std::uint8_t*>   auto PtrCur() const noexcept -> T;
        template <class T = std::size_t>     auto SizeBytes() const noexcept -> T;
        template <class T = std::size_t>     auto SizeBytesCur() const noexcept -> T;
        template <class T = std::uint8_t>    auto Span() const noexcept -> std::span<T>;
        template <class T = std::uint8_t>    auto SpanCur() const noexcept -> std::span<T>;

    public:
        auto PosRewind() -> ZxMem&;
        auto PosInc(const std::size_t nBytes) -> ZxMem&;
        template <ZxMem::PosWay eWay = ZxMem::PosWay::Set> auto PosSet(const std::size_t nBytes = 0) -> ZxMem&;

    public:
        auto Resize(const std::size_t nNewSizeBytes, const bool isDiscard = true, const bool isRewindPos = true) -> ZxMem&;

    public:
        template <class T> auto Get() -> T;
        template <class T> auto Put(T&& rfData) -> ZxMem&;
        template <class T, std::size_t S> auto Read(const std::span<T, S> spData) -> void;
        template <class T, std::size_t S> auto Write(const std::span<T, S> spData) -> void;

    public:
        auto Load(const std::string_view msPath, const std::size_t nReadSize = static_cast<std::size_t>(-1)) -> ZxMem&;
        auto Save(const std::string_view msPath, const bool isCoverExists = true, const bool isCreateDirectories = true) const -> const ZxMem&;
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
            static_assert(false, "ZxMem::Ptr<T>() const: not pointer type!");
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
            static_assert(false, "ZxMem::PtrCur<T>() const: not pointer type!");
        }
    }

    template <class T>
    inline auto ZxMem::Span() const noexcept -> std::span<T>
    {
        return std::span{ this->Ptr<T*>(), this->SizeBytes() / sizeof(T) };
    }

    template <class T>
    inline auto ZxMem::SpanCur() const noexcept -> std::span<T>
    {
        return std::span{ this->PtrCur<T*>(), this->SizeBytesCur() / sizeof(T) };
    }

    template <class T>
    inline auto ZxMem::PosCur() const -> T
    {
        if constexpr (std::is_integral_v<T>)
        {
            return static_cast<T>(m_nPos);
        }
        else
        {
            static_assert(false, "ZxMem::PosCur<T>() const: not integral type!");
        }
    }

    template <ZxMem::PosWay eWay>
    inline auto ZxMem::PosSet(const std::size_t nBytes) -> ZxMem&
    {
        if constexpr (eWay == ZxMem::PosWay::Set)
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
            static_assert(false, "ZxMem::SizeBytes<T>() const: not integral type!");
        }
    }

    template <class T>
    inline auto ZxMem::SizeBytesCur() const noexcept -> T
    {
        if constexpr (std::is_integral_v<T>)
        {
            return static_cast<T>(m_nSizeBytes - m_nPos);
        }
        else
        {
            static_assert(false, "ZxMem::SizeBytesCur<T>() const: not integral type!");
        }
    }

    template <class T, std::size_t S>
    inline auto ZxMem::Read(const std::span<T, S> spData) -> void
    {
        if (spData.empty()) { return; }
        assert(this->SizeBytesCur() >= spData.size_bytes());
        std::memcpy(spData.data(), this->PtrCur<std::uint8_t*>(), spData.size_bytes());
        this->PosInc(spData.size_bytes());
    }

    template <class T, std::size_t S>
    inline auto ZxMem::Write(const std::span<T, S> spData) -> void
    {
        if (spData.empty()) { return; }
        assert(this->SizeBytesCur() >= spData.size_bytes());
        std::memcpy(this->PtrCur<std::uint8_t*>(), spData.data(), spData.size_bytes());
        this->PosInc(spData.size_bytes());
    }

    template<class T>
    inline auto ZxMem::operator>>(T&& rfData) -> ZxMem&
    {
        using data_type = std::decay_t<decltype(rfData)>;

        if constexpr (std::is_same_v<data_type, ZxMem>)
        {
            static_assert(false, "ZxMem::operator>>(): self type disabled!");
        }
        else if constexpr (ZxMemTraits::is_std_span_v<data_type>)
        {
            this->Read(rfData);
        }
        else
        {
            this->Read(std::span{ &rfData, 1 });
        }

        return *this;
    }

    template<class T>
    inline auto ZxMem::operator<<(T&& rfData) -> ZxMem&
    {
        using data_type = std::decay_t<decltype(rfData)>;

        if constexpr (std::is_same_v<data_type, ZxMem>)
        {
            static_assert(false, "ZxMem::operator>>(): self type disabled!");
        }
        else if constexpr (ZxMemTraits::is_std_span_v<data_type>)
        {
            this->Write(rfData);
        }
        else
        {
            this->Write(std::span{ &rfData, 1 });
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
    inline auto ZxMem::Put(T&& rfData) -> ZxMem&
    {
        return this->operator<<(std::forward<T>(rfData));
    }
}
