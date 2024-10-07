#include <cmath>
#include <print>
#include <array>
#include <format>
#include <iostream>
#include <Zut/ZxMem.h>
#include <vector>
#include <list>
#include <map>
#include <set>


struct foo
{
public:
    std::uint32_t a{};
    std::uint64_t b{};
    std::double_t c{};
    std::string d;
    std::array<char, 16> e{};
    std::vector<int> f;
    std::list<std::string> g;
    std::set<int> h;
    std::map<std::string, std::size_t> i;

public:

    auto BinaryStore(ZxMem& mem) const -> void
    {
        this->_BinaryStore(mem, a, b, c, d, e, f, g, h, i);
    }

    auto BinaryStore() const -> ZxMem
    {
        ZxMem mem{ this->SizeBytes() };
        this->BinaryStore(mem); return mem;
    }

    auto BinaryLoad(ZxMem& mem) -> void
    {
        this->_BinaryLoad(mem, a, b, c, d, e, f, g, h, i);
    }

    constexpr auto SizeBytes() const -> std::size_t
    {
        return this->_SizeBytes(a, b, c, d, e, f, g, h, i);
    }

private:
    template<typename IO_Type, typename Field_Type>
    auto _FieldWrite(IO_Type& writer, Field_Type&& field) const -> void
    {
        using field_type_t = std::decay_t<Field_Type>;

        /* char short int uint32_t uint64_t double float ... std::spn */
        if constexpr (ZQF::Zut::ZxMemTraits::is_arithmetic<field_type_t> || ZQF::Zut::ZxMemTraits::is_std_span<field_type_t>)
        {
            writer << field;
        }
        /* container */
        else if constexpr (ZQF::Zut::ZxMemTraits::is_iter_able<field_type_t>)
        {
            /* container size */
            writer << static_cast<std::uint32_t>(field.size());

            /* span able */
            if constexpr (ZQF::Zut::ZxMemTraits::is_span_able<field_type_t>)
            {
                /* string, vector, array */
                this->_FieldWrite(writer, std::span{ field });
            }
            /* not span able */
            else
            {
                for (const auto& element : field)
                {
                    /* map, unordered_map*/
                    if constexpr (ZQF::Zut::ZxMemTraits::is_std_map<field_type_t>)
                    {
                        this->_FieldWrite(writer, element.first);
                        this->_FieldWrite(writer, element.second);
                    }
                    /* list, set, unordered_set, forward_list*/
                    else
                    {
                        this->_FieldWrite(writer, element);
                    }
                }
            }
        }
        else
        {
            static_assert(false, "ZQF::Zut::ZxMemPrivate::Reflect::_FieldWrite(): error type!");
        }
    }

    template<typename IO_Type, typename Field_Type>
    auto _FieldRead(IO_Type& reader, Field_Type&& filed) -> void
    {
        using field_type_t = std::decay_t<Field_Type>;

        if constexpr (ZQF::Zut::ZxMemTraits::is_arithmetic<field_type_t> || ZQF::Zut::ZxMemTraits::is_std_span<field_type_t>)
        {
            reader >> filed;
        }
        else if constexpr (ZQF::Zut::ZxMemTraits::is_iter_able<field_type_t>)
        {
            const auto ele_cnt{ reader.Get<std::uint32_t>() };

            if constexpr (ZQF::Zut::ZxMemTraits::is_span_able<field_type_t>)
            {
                if constexpr (ZQF::Zut::ZxMemTraits::is_std_array<field_type_t> == false)
                {
                    filed.resize(ele_cnt);
                }

                this->_FieldRead(reader, std::span{ filed });
            }
            else
            {
                for (std::size_t idx{}; idx < ele_cnt; idx++)
                {
                    if constexpr (ZQF::Zut::ZxMemTraits::is_std_map<field_type_t>)
                    {
                        typename field_type_t::key_type map_key;
                        typename field_type_t::mapped_type map_val;
                        this->_FieldRead(reader, map_key);
                        this->_FieldRead(reader, map_val);
                        filed.try_emplace(std::move(map_key), std::move(map_val));
                    }
                    else if constexpr (ZQF::Zut::ZxMemTraits::is_std_set<field_type_t>)
                    {
                        typename field_type_t::key_type set_key;
                        this->_FieldRead(reader, set_key);
                        filed.emplace(std::move(set_key));
                    }
                    else
                    {
                        typename field_type_t::value_type normal_val;
                        this->_FieldRead(reader, normal_val);
                        filed.push_back(std::move(normal_val));
                    }
                }
            }
        }
        else
        {
            static_assert(false, "ZQF::Zut::ZxMemPrivate::Reflect::_FieldRead(): error type!");
        }
    }

    template<typename Field_Type>
    auto _FieldBytes(Field_Type&& field) const -> std::size_t
    {
        using field_type_t = std::decay_t<Field_Type>;

        if constexpr (ZQF::Zut::ZxMemTraits::is_arithmetic<field_type_t>)
        {
            return sizeof(field);
        }
        else if constexpr (ZQF::Zut::ZxMemTraits::is_std_span<field_type_t>)
        {
            return field.size_bytes();
        }
        else if constexpr (ZQF::Zut::ZxMemTraits::is_iter_able<field_type_t>)
        {
            std::size_t bytes{ 4 };
            if constexpr (ZQF::Zut::ZxMemTraits::is_span_able<field_type_t>)
            {
                bytes += this->_FieldBytes(std::span{ field });
            }
            else
            {
                for (const auto& element : field)
                {
                    if constexpr (ZQF::Zut::ZxMemTraits::is_std_map<field_type_t>)
                    {
                        bytes += this->_FieldBytes(element.first);
                        bytes += this->_FieldBytes(element.second);
                    }
                    else
                    {
                        bytes += this->_FieldBytes(element);
                    }
                }
            }
            return bytes;
        }
        else
        {
            static_assert(false, "ZQF::Zut::ZxMemPrivate::Reflect::_FieldBytes(): error type!");
        }
    }

    template<typename ...Arg> constexpr auto _SizeBytes(const Arg&... args) const -> std::size_t
    {
        std::size_t bytes{};
        ((bytes += this->_FieldBytes(args)), ...);
        return bytes;
    }

    template<typename ...Arg>
    auto _BinaryStore(ZxMem& mem, const Arg&... args) const -> void
    {
        (this->_FieldWrite(mem, args), ...);
    }

    template<typename ...Arg>
    auto _BinaryLoad(ZxMem& mem, Arg&... args) -> void
    {
        (this->_FieldRead(mem, args), ...);
    }

public:
    // ZXMEM_REFLRECT(a, b, c, d, e, f)
};

struct foo2
{
    std::uint32_t a{};
    std::uint64_t b{};
    std::double_t c{};
    std::string d;
    std::array<char, 16> e{};
    std::vector<int> f;
    std::list<std::string> g;
    std::set<int> h;
    std::map<std::string, std::size_t> i;


    ZXMEM_REFLRECT(a, b, c, d, e, f, g, h, i)
};

auto main(void) -> int
{
    try
    {
        foo2 fool2_store
        {
            .a = 0xAABBCCDD,
            .b = 0x1122334455667788,
            .c = 1.03,
            .d = "test st::string",
            .e = "std::array",
            .f = {1,2,3,4,5,6,7},
            .g = {"123","321","666","777","888"},
            .h = {0xAA,0xBB,0xCC,0xEE,0xFF},
            .i = {{"1008611",0x01008611},{"AAVV", 0xAABB},{"shfuis",0x22}}
        };

        fool2_store.BinaryStore().Save("2.bin");

        ZxMem mem{ "2.bin" };
        foo2 foo2_load;
        foo2_load.BinaryLoad(mem);

        //foo foo1;
        //foo1.BinaryStore().Save("1.bin");

        //ZxMem mem{ "1.bin" };
        //foo foo;
        //foo.BinaryLoad(mem);

    }
    catch (const std::exception& err)
    {
        std::println(std::cerr, "std::exception: {}", err.what());
    }
}
