// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <dgboost/python/converter/arg_to_python_base.hpp>
#include <dgboost/python/errors.hpp>
#include <dgboost/python/converter/registrations.hpp>
#include <dgboost/python/handle.hpp>
#include <dgboost/python/refcount.hpp>

namespace dgboost {} namespace boost = dgboost; namespace dgboost { namespace python { namespace converter { 

namespace detail
{
  arg_to_python_base::arg_to_python_base(
      void const volatile* source, registration const& converters)
# if !defined(BOOST_MSVC) || BOOST_MSVC <= 1300 || _MSC_FULL_VER > 13102179
      : handle<>
# else
      : m_ptr
# endif
         (converters.to_python(source))
  {
  }
}

}}} // namespace dgboost::python::converter
