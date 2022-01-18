#if !defined(BOOST_PP_IS_ITERATING)

// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

# ifndef TARGET_DWA2002521_HPP
#  define TARGET_DWA2002521_HPP

#  include <dgboost/python/detail/preprocessor.hpp>

#  include <dgboost/type.hpp>

#  include <dgboost/preprocessor/comma_if.hpp>
#  include <dgboost/preprocessor/if.hpp>
#  include <dgboost/preprocessor/iterate.hpp>
#  include <dgboost/preprocessor/debug/line.hpp>
#  include <dgboost/preprocessor/enum_params.hpp>
#  include <dgboost/preprocessor/repetition/enum_trailing_params.hpp>

namespace dgboost {} namespace boost = dgboost; namespace dgboost { namespace python { namespace detail {

#  define BOOST_PP_ITERATION_PARAMS_1                                                                   \
    (4, (0, BOOST_PYTHON_MAX_ARITY, <dgboost/python/detail/target.hpp>, BOOST_PYTHON_FUNCTION_POINTER))
#  include BOOST_PP_ITERATE()

#  define BOOST_PP_ITERATION_PARAMS_1                                                                    \
    (4, (0, BOOST_PYTHON_CV_COUNT - 1, <dgboost/python/detail/target.hpp>, BOOST_PYTHON_POINTER_TO_MEMBER))
#  include BOOST_PP_ITERATE()

template <class R, class T>
T& (* target(R (T::*)) )() { return 0; }

}}} // namespace dgboost::python::detail

# endif // TARGET_DWA2002521_HPP

/* --------------- function pointers --------------- */
// For gcc 4.4 compatability, we must include the
// BOOST_PP_ITERATION_DEPTH test inside an #else clause.
#else // BOOST_PP_IS_ITERATING
#if BOOST_PP_ITERATION_DEPTH() == 1 && BOOST_PP_ITERATION_FLAGS() == BOOST_PYTHON_FUNCTION_POINTER
# if !(BOOST_WORKAROUND(__MWERKS__, > 0x3100)                      \
        && BOOST_WORKAROUND(__MWERKS__, BOOST_TESTED_AT(0x3201)))
#  line BOOST_PP_LINE(__LINE__, target.hpp(function_pointers))
# endif 

# define N BOOST_PP_ITERATION()

template <class R BOOST_PP_ENUM_TRAILING_PARAMS_Z(1, N, class A)>
BOOST_PP_IF(N, A0, void)(* target(R (*)(BOOST_PP_ENUM_PARAMS_Z(1, N, A))) )()
{
    return 0;
}

# undef N

/* --------------- pointers-to-members --------------- */
#elif BOOST_PP_ITERATION_DEPTH() == 1 && BOOST_PP_ITERATION_FLAGS() == BOOST_PYTHON_POINTER_TO_MEMBER
// Outer over cv-qualifiers

# define BOOST_PP_ITERATION_PARAMS_2 (3, (0, BOOST_PYTHON_MAX_ARITY, <dgboost/python/detail/target.hpp>))
# include BOOST_PP_ITERATE()

#elif BOOST_PP_ITERATION_DEPTH() == 2
# if !(BOOST_WORKAROUND(__MWERKS__, > 0x3100)                      \
        && BOOST_WORKAROUND(__MWERKS__, BOOST_TESTED_AT(0x3201)))
#  line BOOST_PP_LINE(__LINE__, target.hpp(pointers-to-members))
# endif 
// Inner over arities

# define N BOOST_PP_ITERATION()
# define Q BOOST_PYTHON_CV_QUALIFIER(BOOST_PP_RELATIVE_ITERATION(1))

template <class R, class T BOOST_PP_ENUM_TRAILING_PARAMS_Z(1, N, class A)>
T& (* target(R (T::*)(BOOST_PP_ENUM_PARAMS_Z(1, N, A)) Q) )()
{
    return 0;
}

# undef N
# undef Q

#endif // BOOST_PP_ITERATION_DEPTH()
#endif
