
//  (C) Copyright John Maddock 2015.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef BOOST_TT_IS_CONSTRUCTIBLE_HPP_INCLUDED
#define BOOST_TT_IS_CONSTRUCTIBLE_HPP_INCLUDED

#include <dgboost/type_traits/integral_constant.hpp>
#include <dgboost/detail/workaround.hpp>

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) && !defined(BOOST_NO_CXX11_DECLTYPE) && !BOOST_WORKAROUND(BOOST_MSVC, < 1800) && !BOOST_WORKAROUND(BOOST_GCC_VERSION, < 40500)

#include <dgboost/type_traits/is_destructible.hpp>
#include <dgboost/type_traits/is_default_constructible.hpp>
#include <dgboost/type_traits/detail/yes_no_type.hpp>
#include <dgboost/type_traits/declval.hpp>
#include <dgboost/type_traits/is_complete.hpp>
#include <dgboost/static_assert.hpp>

#define BOOST_TT_IS_CONSTRUCTIBLE_CONFORMING 1

namespace dgboost {} namespace boost = dgboost; namespace dgboost{

   namespace detail{

      struct is_constructible_imp
      {
         template<typename T, typename ...TheArgs, typename = decltype(T(dgboost::declval<TheArgs>()...))>
         static dgboost::type_traits::yes_type test(int);
         template<typename, typename...>
         static dgboost::type_traits::no_type test(...);

         template<typename T, typename Arg, typename = decltype(::new T(dgboost::declval<Arg>()))>
         static dgboost::type_traits::yes_type test1(int);
         template<typename, typename>
         static dgboost::type_traits::no_type test1(...);

         template <typename T>
         static dgboost::type_traits::yes_type ref_test(T);
         template <typename T>
         static dgboost::type_traits::no_type ref_test(...);
      };

   }

   template <class T, class ...Args> struct is_constructible : public integral_constant<bool, sizeof(detail::is_constructible_imp::test<T, Args...>(0)) == sizeof(dgboost::type_traits::yes_type)>
   {
      BOOST_STATIC_ASSERT_MSG(::dgboost::is_complete<T>::value, "The target type must be complete in order to test for constructibility");
   };
   template <class T, class Arg> struct is_constructible<T, Arg> : public integral_constant<bool, is_destructible<T>::value && sizeof(dgboost::detail::is_constructible_imp::test1<T, Arg>(0)) == sizeof(dgboost::type_traits::yes_type)>
   {
      BOOST_STATIC_ASSERT_MSG(::dgboost::is_complete<T>::value, "The target type must be complete in order to test for constructibility");
   };
   template <class Ref, class Arg> struct is_constructible<Ref&, Arg> : public integral_constant<bool, sizeof(detail::is_constructible_imp::ref_test<Ref&>(dgboost::declval<Arg>())) == sizeof(dgboost::type_traits::yes_type)>{};
   template <class Ref, class Arg> struct is_constructible<Ref&&, Arg> : public integral_constant<bool, sizeof(detail::is_constructible_imp::ref_test<Ref&&>(dgboost::declval<Arg>())) == sizeof(dgboost::type_traits::yes_type)>{};

   template <> struct is_constructible<void> : public false_type{};
   template <> struct is_constructible<void const> : public false_type{};
   template <> struct is_constructible<void const volatile> : public false_type{};
   template <> struct is_constructible<void volatile> : public false_type{};

   template <class T> struct is_constructible<T> : public is_default_constructible<T>{};

#else

#include <dgboost/type_traits/is_convertible.hpp>
#include <dgboost/type_traits/is_default_constructible.hpp>

namespace dgboost {} namespace boost = dgboost; namespace dgboost{

   // We don't know how to implement this:
   template <class T, class U = void> struct is_constructible : public is_convertible<U, T>{};
   template <class T> struct is_constructible<T, void> : public is_default_constructible<T>{};
   template <> struct is_constructible<void, void> : public false_type{};
   template <> struct is_constructible<void const, void> : public false_type{};
   template <> struct is_constructible<void const volatile, void> : public false_type{};
   template <> struct is_constructible<void volatile, void> : public false_type{};
   template <class Ref> struct is_constructible<Ref&, void> : public false_type{};
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
   template <class Ref> struct is_constructible<Ref&&, void> : public false_type{};
#endif
#endif

} // namespace dgboost

#endif // BOOST_TT_IS_CONSTRUCTIBLE_HPP_INCLUDED
