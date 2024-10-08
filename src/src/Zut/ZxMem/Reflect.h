#pragma once
#include <tuple>
#include <string_view>
#include <source_location>
#include <Zut/ZxMem/Traits.h>


namespace ZQF::Zut::ZxMemPrivate
{
    class ZxReflex
    {
    private:
        struct AnyType
        {
            template <typename T>
            operator T() {};
        };

        template <typename Object_Type>
        struct StaticWarp
        {
            inline static Object_Type value;
        };

        template <typename T>
        static consteval auto CountMember(auto&&... Args) -> std::size_t
        {
            if constexpr (!requires { T{ Args... }; })
            {
                return sizeof...(Args) - 1;
            }
            else
            {
                return CountMember<T>(Args..., AnyType{});
            }
        }

        template<auto>
        static consteval auto GetFieldSigNameImp() -> const char*
        {
#if __cpp_lib_source_location >= 201907L
            return std::source_location::current().function_name();
#elif defined(_MSC_VER)
            return __FUNCSIG__;
#else
            return __PRETTY_FUNCTION__;
#endif
        }

        template<typename Object_Type, std::size_t IDX>
        static consteval auto GetFieldSigName() -> std::string_view
        {
            return ZxReflex::GetFieldSigNameImp<std::get<IDX>(ZxReflex::GetTuple(StaticWarp<Object_Type>::value))>();
        }

        template <typename Object_Type>
        static constexpr auto GetTuple(Object_Type&& Object) -> auto
        {
            constexpr auto field_num{ ZxReflex::CountMember<std::decay_t<Object_Type>>() };

            if constexpr (field_num == 0)
            {
                static_assert(false, "ZxReflex::GetTuple(): zero field.");
            }
            else if constexpr (field_num == 1)
            {
                auto&& [_1] = Object;
                return std::tuple{ &_1 };
            }
            else if constexpr (field_num == 2)
            {
                auto&& [_1, _2] = Object;
                return std::tuple{ &_1, &_2 };
            }
            else if constexpr (field_num == 3)
            {
                auto&& [_1, _2, _3] = Object;
                return std::tuple{ &_1, &_2, &_3 };
            }
            else if constexpr (field_num == 4)
            {
                auto&& [_1, _2, _3, _4] = Object;
                return std::tuple{ &_1, &_2, &_3, &_4 };
            }
            else if constexpr (field_num == 5)
            {
                auto&& [_1, _2, _3, _4, _5] = Object;
                return std::tuple{ &_1, &_2, &_3, &_4, &_5 };
            }
            else if constexpr (field_num == 6)
            {
                auto&& [_1, _2, _3, _4, _5, _6] = Object;
                return std::tuple{ &_1, &_2, &_3, &_4, &_5, &_6 };
            }
            else if constexpr (field_num == 7)
            {
                auto&& [_1, _2, _3, _4, _5, _6, _7] = Object;
                return std::tuple{ &_1, &_2, &_3, &_4, &_5, &_6, &_7 };
            }
            else if constexpr (field_num == 8)
            {
                auto&& [_1, _2, _3, _4, _5, _6, _7, _8] = Object;
                return std::tuple{ &_1, &_2, &_3, &_4, &_5, &_6, &_7, &_8 };
            }
            else if constexpr (field_num == 9)
            {
                auto&& [_1, _2, _3, _4, _5, _6, _7, _8, _9] = Object;
                return std::tuple{ &_1, &_2, &_3, &_4, &_5, &_6, &_7, &_8, &_9 };
            }
            else if constexpr (field_num == 10)
            {
                auto&& [_1, _2, _3, _4, _5, _6, _7, _8, _9, _10] = Object;
                return std::tuple{ &_1, &_2, &_3, &_4, &_5, &_6, &_7, &_8, &_9, &_10 };
            }
            else if constexpr (field_num == 11)
            {
                auto&& [_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11] = Object;
                return std::tuple{ &_1, &_2, &_3, &_4, &_5, &_6, &_7, &_8, &_9, &_10, &_11 };
            }
            else if constexpr (field_num == 12)
            {
                auto&& [_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12] = Object;
                return std::tuple{ &_1, &_2, &_3, &_4, &_5, &_6, &_7, &_8, &_9, &_10, &_11, &_12 };
            }
            else if constexpr (field_num == 13)
            {
                auto&& [_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13] = Object;
                return std::tuple{ &_1, &_2, &_3, &_4, &_5, &_6, &_7, &_8, &_9, &_10, &_11, &_12, &_13 };
            }
            else if constexpr (field_num == 14)
            {
                auto&& [_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14] = Object;
                return std::tuple{ &_1, &_2, &_3, &_4, &_5, &_6, &_7, &_8, &_9, &_10, &_11, &_12, &_13, &_14 };
            }
            else if constexpr (field_num == 15)
            {
                auto&& [_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15] = Object;
                return std::tuple{ &_1, &_2, &_3, &_4, &_5, &_6, &_7, &_8, &_9, &_10, &_11, &_12, &_13, &_14, &_15 };
            }
            else if constexpr (field_num == 16)
            {
                auto&& [_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16] = Object;
                return std::tuple{ &_1, &_2, &_3, &_4, &_5, &_6, &_7, &_8, &_9, &_10, &_11, &_12, &_13, &_14, &_15, &_16 };
            }
            else
            {
                static_assert(false, "ZxReflex::GetTuple(): max field count.");
            }
        }

    public:
        template<typename Object_Type>
        static consteval auto GetFieldCount() -> std::size_t
        {
            return std::tuple_size_v<decltype(ZxReflex::GetTuple(StaticWarp<Object_Type>::value))>;
        }

        template<typename Object_Type, std::size_t IDX>
        static consteval auto GetFieldName() -> std::string_view
        {
            constexpr std::string_view symbol_name{ ZxReflex::GetFieldSigName<Object_Type, IDX>() };
#if defined(__clang__)
#elif defined(__GNUC__)
#elif defined(_MSC_VER)
            constexpr auto field_name_beg{ symbol_name.find("&value->") + 8 };
            constexpr auto field_name_end{ symbol_name.find(">", field_name_beg) };
            constexpr auto filed_name_size{ field_name_end - field_name_beg };
#else
            static_assert(false, "ZxReflect::GetFieldName(): unknown compiler.");
#endif
            return symbol_name.substr(field_name_beg, filed_name_size);
        }

        template<typename Object_Type>
        static constexpr auto FotEach(Object_Type& Object, auto&& Functor) -> void
        {
            auto tuple{ ZxReflex::GetTuple(std::forward<Object_Type>(Object)) };
            [&tuple, &Functor] <auto... Ns>(std::index_sequence<Ns...>) {
                (Functor(ZxReflex::GetFieldName<Object_Type, Ns>(), Ns, *std::get<Ns>(tuple)), ...);
            }
            (std::make_index_sequence<ZxReflex::GetFieldCount<Object_Type>()>{});
        }

        template<typename Object_Type>
        static constexpr auto FotEachField(Object_Type& Object, auto&& Functor) -> void
        {
            auto tuple{ ZxReflex::GetTuple(std::forward<Object_Type>(Object)) };
            [&tuple, &Functor] <auto... Ns>(std::index_sequence<Ns...>) {
                (Functor(*std::get<Ns>(tuple)), ...);
            }
            (std::make_index_sequence<ZxReflex::GetFieldCount<Object_Type>()>{});
        }

        template<typename Object_Type, std::size_t IDX>
        using GetFieldType = std::remove_pointer_t<std::decay_t<decltype(std::get<IDX>(ZxReflex::GetTuple(StaticWarp<Object_Type>::value)))>>;
    };
}


namespace ZQF::Zut::ZxMemPrivate
{
    class BinaryField
    {
    public:
        template<typename IO_Type, typename Field_Type>
        static auto Write(IO_Type& writer, Field_Type&& field) -> void
        {
            using field_type_t = std::decay_t<Field_Type>;

            /* char short int uint32_t uint64_t double float ... std::spn */
            if constexpr (ZQF::Zut::ZxMemTraits::is_arithmetic_v<field_type_t> || ZQF::Zut::ZxMemTraits::is_std_span_v<field_type_t>)
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
                    Write(writer, std::span{ field });
                }
                /* not span able */
                else
                {
                    for (const auto& element : field)
                    {
                        /* map, unordered_map*/
                        if constexpr (ZQF::Zut::ZxMemTraits::is_std_map_v<field_type_t> || ZQF::Zut::ZxMemTraits::is_std_unordered_map_v<field_type_t>)
                        {
                            Write(writer, element.first);
                            Write(writer, element.second);
                        }
                        /* list, set, unordered_set, forward_list*/
                        else
                        {
                            Write(writer, element);
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
        static auto Read(IO_Type& reader, Field_Type&& filed) -> void
        {
            using field_type_t = std::decay_t<Field_Type>;

            if constexpr (ZQF::Zut::ZxMemTraits::is_arithmetic_v<field_type_t> || ZQF::Zut::ZxMemTraits::is_std_span_v<field_type_t>)
            {
                reader >> filed;
            }
            else if constexpr (ZQF::Zut::ZxMemTraits::is_iter_able<field_type_t>)
            {
                const auto ele_cnt{ reader.Get<std::uint32_t>() };

                if constexpr (ZQF::Zut::ZxMemTraits::is_span_able<field_type_t>)
                {
                    if constexpr (ZQF::Zut::ZxMemTraits::is_std_array_v<field_type_t> == false)
                    {
                        filed.resize(ele_cnt);
                    }

                    Read(reader, std::span{ filed });
                }
                else
                {
                    for (std::size_t idx{}; idx < ele_cnt; idx++)
                    {
                        if constexpr (ZQF::Zut::ZxMemTraits::is_std_map_v<field_type_t> || ZQF::Zut::ZxMemTraits::is_std_unordered_map_v<field_type_t>)
                        {
                            typename field_type_t::key_type map_key;
                            typename field_type_t::mapped_type map_val;
                            Read(reader, map_key);
                            Read(reader, map_val);
                            filed.try_emplace(std::move(map_key), std::move(map_val));
                        }
                        else if constexpr (ZQF::Zut::ZxMemTraits::is_std_set_v<field_type_t> || ZQF::Zut::ZxMemTraits::is_std_unordered_set_v<field_type_t>)
                        {
                            typename field_type_t::key_type set_key;
                            Read(reader, set_key);
                            filed.emplace(std::move(set_key));
                        }
                        else
                        {
                            typename field_type_t::value_type normal_val;
                            Read(reader, normal_val);
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
        static auto SizeBytes(Field_Type&& field) -> std::size_t
        {
            using field_type_t = std::decay_t<Field_Type>;

            if constexpr (ZQF::Zut::ZxMemTraits::is_arithmetic_v<field_type_t>)
            {
                return sizeof(field);
            }
            else if constexpr (ZQF::Zut::ZxMemTraits::is_std_span_v<field_type_t>)
            {
                return field.size_bytes();
            }
            else if constexpr (ZQF::Zut::ZxMemTraits::is_iter_able<field_type_t>)
            {
                std::size_t bytes{ 4 };
                if constexpr (ZQF::Zut::ZxMemTraits::is_span_able<field_type_t>)
                {
                    bytes += SizeBytes(std::span{ field });
                }
                else
                {
                    for (const auto& element : field)
                    {
                        if constexpr (ZQF::Zut::ZxMemTraits::is_std_map_v<field_type_t> || ZQF::Zut::ZxMemTraits::is_std_unordered_map_v<field_type_t>)
                        {
                            bytes += SizeBytes(element.first);
                            bytes += SizeBytes(element.second);
                        }
                        else
                        {
                            bytes += SizeBytes(element);
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
    };



#define ZXMEM_REFLEX(...)                                                                                                                                                                                         \
    auto ReflexBinaryStore() const -> ZxMem { ZxMem mem{ this->ReflexSizeBytes() }; this->ReflexBinaryStore(mem); return mem; }                                                                                   \
    auto ReflexBinaryStore(ZxMem& mem) const -> void { this->_ReflexBinaryStore(mem, __VA_ARGS__); }                                                                                                              \
    auto ReflexBinaryLoad(const std::string_view msPath) -> void { ZxMem mem{ msPath }; this->_ReflexBinaryLoad(mem, __VA_ARGS__); }                                                                              \
    auto ReflexBinaryLoad(ZxMem& mem) -> void { this->_ReflexBinaryLoad(mem, __VA_ARGS__); }                                                                                                                      \
    constexpr auto ReflexSizeBytes() const -> std::size_t { return this->_SizeBytes(__VA_ARGS__); }                                                                                                               \
private:                                                                                                                                                                                                          \
    template<typename ...Arg> constexpr auto _SizeBytes(const Arg&... args) const -> std::size_t { std::size_t bytes{};  ((bytes += ZQF::Zut::ZxMemPrivate::BinaryField::SizeBytes(args)), ...);  return bytes; } \
    template<typename ...Arg>                                                                                                                                                                                     \
    auto _ReflexBinaryStore(ZxMem& mem, const Arg&... args) const -> void{ (ZQF::Zut::ZxMemPrivate::BinaryField::Write(mem, args), ...); }                                                                        \
    template<typename ...Arg>                                                                                                                                                                                     \
    auto _ReflexBinaryLoad(ZxMem& mem, Arg&... args) -> void { (ZQF::Zut::ZxMemPrivate::BinaryField::Read(mem, args), ...); }                                                                                     \
                                                                                                                                                                                                                  


#define ZXMEM_REFLEX_ALL                                                                                                                                                                                                               \
    auto ReflexBinaryStore() const -> ZxMem { ZxMem mem{ this->ReflexSizeBytes() }; this->ReflexBinaryStore(mem); return mem; }                                                                                                        \
    auto ReflexBinaryStore(ZxMem& mem) const -> void { this->_ReflexBinaryStore(mem); }                                                                                                                                                \
    auto ReflexBinaryLoad(const std::string_view msPath) -> void { ZxMem mem{ msPath }; this->_ReflexBinaryLoad(mem); }                                                                                                                \
    auto ReflexBinaryLoad(ZxMem& mem) -> void { this->_ReflexBinaryLoad(mem); }                                                                                                                                                        \
    constexpr auto ReflexSizeBytes() const -> std::size_t { return this->_SizeBytes(); }                                                                                                                                               \
private:                                                                                                                                                                                                                               \
    constexpr auto _SizeBytes() const -> std::size_t { std::size_t bytes{}; ZQF::Zut::ZxMemPrivate::ZxReflex::FotEachField(*this, [&bytes](auto&& v) { bytes += ZQF::Zut::ZxMemPrivate::BinaryField::SizeBytes(v); }); return bytes; } \
    auto _ReflexBinaryStore(ZxMem& mem) const -> void { ZQF::Zut::ZxMemPrivate::ZxReflex::FotEachField(*this, [&mem](auto&& v) { ZQF::Zut::ZxMemPrivate::BinaryField::Write(mem, v); }); }                                             \
    auto _ReflexBinaryLoad(ZxMem& mem) -> void { ZQF::Zut::ZxMemPrivate::ZxReflex::FotEachField(*this, [&mem](auto&& v) { ZQF::Zut::ZxMemPrivate::BinaryField::Read(mem, v); }); }                                                     \
                                                                                                                                               \

}

