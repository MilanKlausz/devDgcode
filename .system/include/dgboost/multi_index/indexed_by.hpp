/* Copyright 2003-2013 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/multi_index for library home page.
 */

#ifndef BOOST_MULTI_INDEX_INDEXED_BY_HPP
#define BOOST_MULTI_INDEX_INDEXED_BY_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <dgboost/config.hpp> /* keep it first to prevent nasty warns in MSVC */
#include <dgboost/mpl/vector.hpp>
#include <dgboost/preprocessor/cat.hpp>
#include <dgboost/preprocessor/control/expr_if.hpp>
#include <dgboost/preprocessor/repetition/enum.hpp>
#include <dgboost/preprocessor/repetition/enum_params.hpp> 

/* An alias to mpl::vector used to hide MPL from the user.
 * indexed_by contains the index specifiers for instantiation
 * of a multi_index_container.
 */

/* This user_definable macro limits the number of elements of an index list;
 * useful for shortening resulting symbol names (MSVC++ 6.0, for instance,
 * has problems coping with very long symbol names.)
 */

#if !defined(BOOST_MULTI_INDEX_LIMIT_INDEXED_BY_SIZE)
#define BOOST_MULTI_INDEX_LIMIT_INDEXED_BY_SIZE BOOST_MPL_LIMIT_VECTOR_SIZE
#endif

#if BOOST_MULTI_INDEX_LIMIT_INDEXED_BY_SIZE<BOOST_MPL_LIMIT_VECTOR_SIZE
#define BOOST_MULTI_INDEX_INDEXED_BY_SIZE \
  BOOST_MULTI_INDEX_LIMIT_INDEXED_BY_SIZE
#else
#define BOOST_MULTI_INDEX_INDEXED_BY_SIZE BOOST_MPL_LIMIT_VECTOR_SIZE
#endif

#define BOOST_MULTI_INDEX_INDEXED_BY_TEMPLATE_PARM(z,n,var) \
  typename BOOST_PP_CAT(var,n) BOOST_PP_EXPR_IF(n,=mpl::na)

namespace dgboost {} namespace boost = dgboost; namespace dgboost{

namespace multi_index{

template<
  BOOST_PP_ENUM(
    BOOST_MULTI_INDEX_INDEXED_BY_SIZE,
    BOOST_MULTI_INDEX_INDEXED_BY_TEMPLATE_PARM,T)
>
struct indexed_by:
  mpl::vector<BOOST_PP_ENUM_PARAMS(BOOST_MULTI_INDEX_INDEXED_BY_SIZE,T)>
{
};

} /* namespace multi_index */

} /* namespace dgboost */

#undef BOOST_MULTI_INDEX_INDEXED_BY_TEMPLATE_PARM
#undef BOOST_MULTI_INDEX_INDEXED_BY_SIZE

#endif
