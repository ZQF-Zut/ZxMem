#pragma once
#include <Zut/ZxMem/Traits.h>


namespace ZQF::Zut::ZxMemReflex
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
            else if constexpr (ZQF::Zut::ZxMemTraits::is_std_pair_v<field_type_t>)
            {
                Write(writer, field.first);
                Write(writer, field.second);
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
                        Write(writer, element);
                    }
                }
            }
            else if constexpr (ZQF::Zut::ZxMemTraits::is_zxmem_reflex_able<field_type_t>)
            {
                ZxMemReflex::BinaryStore(field, writer);
            }
            else
            {
                static_assert(false, "ZQF::Zut::ZxMemPrivate::Reflect::_FieldWrite(): error type!");
            }
        }

        template<typename IO_Type, typename Field_Type>
        static auto Read(IO_Type& reader, Field_Type&& field) -> void
        {
            using field_type_t = std::decay_t<Field_Type>;

            if constexpr (ZQF::Zut::ZxMemTraits::is_arithmetic_v<field_type_t> || ZQF::Zut::ZxMemTraits::is_std_span_v<field_type_t>)
            {
                reader >> field;
            }
            else if constexpr (ZQF::Zut::ZxMemTraits::is_std_pair_v<field_type_t>)
            {
                Read(reader, field.first);
                Read(reader, field.second);
            }
            else if constexpr (ZQF::Zut::ZxMemTraits::is_iter_able<field_type_t>)
            {
                const auto ele_cnt{ reader.template Get<std::uint32_t>() };

                if constexpr (ZQF::Zut::ZxMemTraits::is_span_able<field_type_t>)
                {
                    if constexpr (ZQF::Zut::ZxMemTraits::is_std_array_v<field_type_t> == false)
                    {
                        field.resize(ele_cnt);
                    }

                    Read(reader, std::span{ field });
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
                            field.try_emplace(std::move(map_key), std::move(map_val));
                        }
                        else if constexpr (ZQF::Zut::ZxMemTraits::is_std_set_v<field_type_t> || ZQF::Zut::ZxMemTraits::is_std_unordered_set_v<field_type_t>)
                        {
                            typename field_type_t::key_type set_key;
                            Read(reader, set_key);
                            field.emplace(std::move(set_key));
                        }
                        else
                        {
                            typename field_type_t::value_type normal_val;
                            Read(reader, normal_val);
                            field.push_back(std::move(normal_val));
                        }
                    }
                }
            }
            else if constexpr (ZQF::Zut::ZxMemTraits::is_zxmem_reflex_able<field_type_t>)
            {
                ZQF::Zut::ZxMemReflex::BinaryLoad(field, reader);
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
            else if constexpr (ZQF::Zut::ZxMemTraits::is_std_pair_v<field_type_t>)
            {
                return SizeBytes(field.first) + SizeBytes(field.second);
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
            else if constexpr (ZQF::Zut::ZxMemTraits::is_zxmem_reflex_able<field_type_t>)
            {
                return ZQF::Zut::ZxMemReflex::SizeBytes(field);
            }
            else
            {
                static_assert(false, "ZQF::Zut::ZxMemPrivate::Reflect::_FieldBytes(): error type!");
            }
        }
    };
}


