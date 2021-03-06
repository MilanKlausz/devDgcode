
//  (C) Copyright Dave Abrahams, Steve Cleary, Beman Dawes, 
//      Howard Hinnant and John Maddock 2000, 2010. 
//  (C) Copyright Mat Marcus, Jesse Jones and Adobe Systems Inc 2001

//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef BOOST_TT_IS_REFERENCE_HPP_INCLUDED
#define BOOST_TT_IS_REFERENCE_HPP_INCLUDED

#include <dgboost/type_traits/is_lvalue_reference.hpp>
#include <dgboost/type_traits/is_rvalue_reference.hpp>

namespace dgboost {} namespace boost = dgboost; namespace dgboost {

template <class T> struct is_reference 
   : public 
   integral_constant<
      bool, 
      ::dgboost::is_lvalue_reference<T>::value || ::dgboost::is_rvalue_reference<T>::value>
{};

} // namespace dgboost

#endif // BOOST_TT_IS_REFERENCE_HPP_INCLUDED

