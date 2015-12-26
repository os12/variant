// Copyright Michael Park 2015
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VARIANT_DETAIL_TYPE_TRAITS_HPP
#define VARIANT_DETAIL_TYPE_TRAITS_HPP

#include <algorithm>
#include <array>
#include <type_traits>
#include <utility>

namespace std {
namespace experimental {
namespace detail {

// C++17 `bool_constant`
template <size_t B>
using bool_constant = integral_constant<bool, B>;

// `size_constant`
template <size_t I>
using size_constant = integral_constant<size_t, I>;

/* C++17 `std::conjunction` */

template <typename... Ts>
struct conjunction;

template <>
struct conjunction<> : true_type {};

template <typename B>
struct conjunction<B> : B {};

template <typename B, typename... Bs>
struct conjunction<B, Bs...>
    : conditional_t<B::value, conjunction<Bs...>, B> {};

/* C++17 `std::disjunction` */

template <typename... Ts>
struct disjunction;

template <>
struct disjunction<> : false_type {};

template <typename B>
struct disjunction<B> : B {};

template <typename B, typename... Bs>
struct disjunction<B, Bs...>
    : conditional_t<B::value, B, disjunction<Bs...>> {};

/* C++17 `std::negation` */

template <typename B>
struct negation : bool_constant<!B::value> {};

/* `false_t` */
template <typename... Ts>
struct false_t : false_type {};

/* `id` */
template <typename T>
struct id { using type = T; };

/* `is_nothrow_swappable` (N4511) */
template <typename T, typename U = T>
struct is_nothrow_swappable
    : bool_constant<noexcept(swap(declval<T &>(), declval<U &>()))> {};

/* `repack` */
template <typename From, template <typename... Ts> class To>
struct repack;

template <typename From, template <typename...> class To>
using repack_t = typename repack<From, To>::type;

template <template <typename... Ts> class From,
          typename... Ts,
          template <typename...> class To>
struct repack<From<Ts...>, To> : id<To<Ts...>> {};

/* `priority` */
template <size_t I>
struct priority;

template <>
struct priority<0> {};

template <size_t I>
struct priority : priority<I - 1> {};

/* `same_type` */
template <typename... Ts>
struct same_type;

template <typename... Ts>
using same_type_t = typename same_type<Ts...>::type;

template <>
struct same_type<> {};

template <typename T, typename... Ts>
struct same_type<T, Ts...> : enable_if<conjunction<is_same<T, Ts>...>{}, T> {};

/* `overload_set` */
template <typename... Ts>
struct overload_set;

template <>
struct overload_set<> {
  void operator()() const;
};

template <typename T, typename... Ts>
struct overload_set<T, Ts...> : overload_set<Ts...> {
  using super = overload_set<Ts...>;

  using super::operator();

  id<T> operator()(T) const;

  template <typename U = T, typename = enable_if_t<is_lvalue_reference<U>{}>>
  void operator()(remove_reference_t<T> &&) const = delete;
};

/* `get_best_match` */
template <typename T, typename... Ts>
using get_best_match = typename result_of_t<overload_set<Ts...>(T)>::type;

}  // namespace detail
}  // namespace experimental
}  // namespace std

#endif  // VARIANT_DETAIL_TYPE_TRAITS_HPP
