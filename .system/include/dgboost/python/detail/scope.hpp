// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef SCOPE_DWA2002927_HPP
# define SCOPE_DWA2002927_HPP

# include <dgboost/python/detail/config.hpp>

namespace dgboost {} namespace boost = dgboost; namespace dgboost { namespace python { namespace detail {

void BOOST_PYTHON_DECL scope_setattr_doc(char const* name, object const& obj, char const* doc);

}}} // namespace dgboost::python::detail

#endif // SCOPE_DWA2002927_HPP
