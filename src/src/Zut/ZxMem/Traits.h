#pragma once
#include <cstdint>
#include <span>
#include <array>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>


namespace ZQF::Zut::ZxMemTraits
{
    template <typename T>
    concept is_iter_able = requires(T & t)
    {
        *std::begin(t);
        std::begin(t) != std::end(t);
        ++std::declval<decltype(std::begin(t))&>();
    };

    template <typename T>
    concept is_span_able = requires(T & t)
    {
        std::span{ t };
    };

    template <typename T>
    concept is_sequence_container = requires(T & t)
    {
        t.data();
        t.size();
    };

    template<typename T>
    concept is_std_span = requires
    {
        std::same_as<std::decay_t<T>, std::span<typename std::decay_t<T>::element_type, std::decay_t<T>::extent>>;
    };

    template<typename T>
    concept is_std_pair = requires
    {
        std::same_as<std::decay_t<T>, std::pair<typename std::decay_t<T>::first_type, typename std::decay_t<T>::second_type>>;
    };

    template <typename T>
    struct is_std_array_imp : std::false_type{};

    template <typename T, size_t N>
    struct is_std_array_imp<std::array<T, N>> : public std::true_type{};

    template <typename T>
    concept is_std_array = is_std_array_imp<T>::value;

    template<typename T>
    concept is_std_set = requires
    {
        std::same_as<std::decay_t<T>, std::set<typename std::decay_t<T>::key_type>> || std::same_as<std::decay_t<T>, std::unordered_set<typename std::decay_t<T>::key_type>>;
    };

    template<typename T>
    concept is_std_map = requires
    {
        std::same_as<std::decay_t<T>, std::map<typename std::decay_t<T>::key_type, typename std::decay_t<T>::mapped_type>> || std::same_as<std::decay_t<T>, std::unordered_map<typename std::decay_t<T>::key_type, typename std::decay_t<T>::mapped_type>>;
    };

    template<typename T>
    concept is_arithmetic = requires
    {
        requires std::is_arithmetic_v<T>;
    };
}
