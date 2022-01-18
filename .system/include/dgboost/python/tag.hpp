// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef TAG_DWA2002720_HPP
# define TAG_DWA2002720_HPP

# include <dgboost/python/detail/prefix.hpp>

namespace dgboost {} namespace boost = dgboost; namespace dgboost { namespace python { 

// used only to prevent argument-dependent lookup from finding the
// wrong function in some cases. Cheaper than qualification.
enum tag_t { tag };

}} // namespace dgboost::python

#endif // TAG_DWA2002720_HPP
