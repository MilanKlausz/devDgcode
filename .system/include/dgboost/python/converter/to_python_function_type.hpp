// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef TO_PYTHON_FUNCTION_TYPE_DWA200236_HPP
# define TO_PYTHON_FUNCTION_TYPE_DWA200236_HPP
# include <dgboost/python/detail/prefix.hpp>
# include <dgboost/static_assert.hpp>

namespace dgboost {} namespace boost = dgboost; namespace dgboost { namespace python { namespace converter { 

// The type of stored function pointers which actually do conversion
// by-value. The void* points to the object to be converted, and
// type-safety is preserved through runtime registration.
typedef PyObject* (*to_python_function_t)(void const*);

}}} // namespace dgboost::python::converter

#endif // TO_PYTHON_FUNCTION_TYPE_DWA200236_HPP
