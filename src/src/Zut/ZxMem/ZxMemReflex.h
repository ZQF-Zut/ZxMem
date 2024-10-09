#pragma once
#include <Zut/ZxMem/Reflex/ZxReflex.h>
#include <Zut/ZxMem/Reflex/BinaryField.h>


namespace ZQF::Zut::ZxMemReflex::Private
{
    static constexpr auto SizeBytesImp(const auto& object) -> std::size_t
    {
        std::size_t bytes{};
        ZQF::Zut::ZxMemReflex::ZxReflex::FotEachField
        (
            object,
            [&bytes](auto&& v)
            {
                bytes += ZQF::Zut::ZxMemReflex::BinaryField::SizeBytes(v);
            }
        );
        return bytes;
    }

    static auto BinaryStoreImp(const auto& object, ZxMem& mem) -> void
    {
        ZQF::Zut::ZxMemReflex::ZxReflex::FotEachField
        (
            object,
            [&mem](auto&& v)
            {
                ZQF::Zut::ZxMemReflex::BinaryField::Write(mem, v);
            }
        );
    }

    static auto BinaryLoadImp(auto& object, ZxMem& mem) -> void
    {
        ZQF::Zut::ZxMemReflex::ZxReflex::FotEachField
        (
            object,
            [&mem](auto&& v)
            {
                ZQF::Zut::ZxMemReflex::BinaryField::Read(mem, v);
            }
        );
    }
}

namespace ZQF::Zut::ZxMemReflex
{
    static constexpr auto SizeBytes(const auto& object) -> std::size_t
    {
        return Private::SizeBytesImp(object);
    }

    static auto BinaryStore(const auto& object, ZxMem& mem) -> void
    {
        Private::BinaryStoreImp(object, mem);
    }

    static auto BinaryStore(const auto& object) -> ZxMem
    {
        ZxMem mem{ ZxMemReflex::SizeBytes(object) };
        ZxMemReflex::BinaryStore(object, mem);
        return mem;
    }

    static auto BinaryLoad(auto& object, const std::string_view msPath) -> void
    {
        ZxMem mem{ msPath };
        Private::BinaryLoadImp(object, mem);
    }

    static auto BinaryLoad(auto& object, ZxMem& mem) -> void
    {
        Private::BinaryLoadImp(object, mem);
    }
}


#define ZXMEM_REFLEX(...)                                                                                                                                                                                     \
auto ReflexBinaryStore() const -> ZxMem { ZxMem mem{ this->ReflexSizeBytes() }; this->ReflexBinaryStore(mem); return mem; }                                                                                   \
auto ReflexBinaryStore(ZxMem& mem) const -> void { this->_ReflexBinaryStore(mem, __VA_ARGS__); }                                                                                                              \
auto ReflexBinaryLoad(const std::string_view msPath) -> void { ZxMem mem{ msPath }; this->_ReflexBinaryLoad(mem, __VA_ARGS__); }                                                                              \
auto ReflexBinaryLoad(ZxMem& mem) -> void { this->_ReflexBinaryLoad(mem, __VA_ARGS__); }                                                                                                                      \
constexpr auto ReflexSizeBytes() const -> std::size_t { return this->_SizeBytes(__VA_ARGS__); }                                                                                                               \
private:                                                                                                                                                                                                      \
template<typename ...Arg> constexpr auto _SizeBytes(const Arg&... args) const -> std::size_t { std::size_t bytes{};  ((bytes += ZQF::Zut::ZxMemReflex::BinaryField::SizeBytes(args)), ...);  return bytes; } \
template<typename ...Arg> auto _ReflexBinaryStore(ZxMem& mem, const Arg&... args) const -> void{ (ZQF::Zut::ZxMemReflex::BinaryField::Write(mem, args), ...); }                                              \
template<typename ...Arg> auto _ReflexBinaryLoad(ZxMem& mem, Arg&... args) -> void { (ZQF::Zut::ZxMemReflex::BinaryField::Read(mem, args), ...); }                                                           \
                                                                                                                                                                                                                  


#define ZXMEM_REFLEX_ALL                                                                                                             \
auto ReflexBinaryStore() const -> ZxMem { ZxMem mem{ this->ReflexSizeBytes() }; this->ReflexBinaryStore(mem); return mem; }          \
auto ReflexBinaryStore(ZxMem& mem) const -> void { ZQF::Zut::ZxMemReflex::BinaryStore(*this, mem); }                                 \
auto ReflexBinaryLoad(const std::string_view msPath) -> void { ZxMem mem{ msPath }; ZQF::Zut::ZxMemReflex::BinaryLoad(*this, mem); } \
auto ReflexBinaryLoad(ZxMem& mem) -> void { ZQF::Zut::ZxMemReflex::BinaryLoad(*this, mem); }                                         \
constexpr auto ReflexSizeBytes() const -> std::size_t { return ZQF::Zut::ZxMemReflex::SizeBytes(*this); }                            \

