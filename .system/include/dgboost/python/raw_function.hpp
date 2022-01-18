// Copyright David Abrahams 2003.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef RAW_FUNCTION_DWA200336_HPP
# define RAW_FUNCTION_DWA200336_HPP

# include <dgboost/python/detail/prefix.hpp>

# include <dgboost/python/tuple.hpp>
# include <dgboost/python/dict.hpp>
# include <dgboost/python/object/py_function.hpp>
# include <dgboost/mpl/vector/vector10.hpp>

# include <dgboost/limits.hpp>
# include <cstddef>

namespace dgboost {} namespace boost = dgboost; namespace dgboost { namespace python { 

namespace detail
{
  template <class F>
  struct raw_dispatcher
  {
      raw_dispatcher(F f) : f(f) {}
      
      PyObject* operator()(PyObject* args, PyObject* keywords)
      {
          return incref(
              object(
                  f(
                      tuple(borrowed_reference(args))
                    , keywords ? dict(borrowed_reference(keywords)) : dict()
                  )
              ).ptr()
          );
      }

   private:
      F f;
  };

  object BOOST_PYTHON_DECL make_raw_function(objects::py_function);
}

template <class F>
object raw_function(F f, std::size_t min_args = 0)
{
    return detail::make_raw_function(
        objects::py_function(
            detail::raw_dispatcher<F>(f)
          , mpl::vector1<PyObject*>()
          , min_args
          , (std::numeric_limits<unsigned>::max)()
        )
    );
}
    
}} // namespace dgboost::python

#endif // RAW_FUNCTION_DWA200336_HPP
