
function(declare_includes_as_system_headers cflags_varname)
  #Go through compilation flags and looks for include paths (-Ixxx):
  if ("x${${cflags_varname}}" STREQUAL x)
    return()
  endif()
  set(sysdirs "")
  string(REPLACE " " ";" tmp ${${cflags_varname}})
  list(REMOVE_ITEM tmp "-I/usr/include")#important to remove to avoid conflicts with other extdeps
  list(REMOVE_ITEM tmp "-isystem/usr/include")
  set(out "")
  foreach(cflag ${tmp})
    set(out "${out} ${cflag}")
    string(REGEX MATCH "^-I.*$" tmp2 "${cflag}")
    if (tmp2)
      string(REPLACE "-I" "" tmp3 "${tmp2}")
      get_filename_component(tmp4 "${tmp3}" ABSOLUTE)
      if (EXISTS "${tmp4}/")
        list(APPEND sysdirs "${tmp4}")
      endif()
    endif()
  endforeach()
  #Prune a bit
  list(REMOVE_DUPLICATES sysdirs)
  #Add -isystem flags:
  foreach(tmp ${sysdirs})
    set(out "${out} -isystem${tmp}")
  endforeach()
  set(${cflags_varname} "${out}" PARENT_SCOPE)
endfunction()

function(fixvar input resvar)
  set(out "")
  set(SKIPNEXT 0)
  foreach(var ${input})
    if (NOT SKIPNEXT)
      if ("${var}" STREQUAL "optimized")
        if (DEBUG)
          set(SKIPNEXT 1)
        else()
          set(SKIPNEXT 0)
        endif()
      elseif("${var}" STREQUAL "debug")
        if (DEBUG)
          set(SKIPNEXT 0)
        else()
          set(SKIPNEXT 1)
        endif()
      elseif(NOT SKIPNEXT)
        list(APPEND out "${var}")
        set(SKIPNEXT 0)
      else()
        set(SKIPNEXT 0)
      endif()
    else()
      set(SKIPNEXT 0)
    endif()
  endforeach()
  set(${resvar} "${out}" PARENT_SCOPE)
endfunction()

function(findpackage_liblist_to_flags libs libdirs resvar)
  fixvar("${libs}" libs)
  fixvar("${libdirs}" libdirs)
  set(out "")
  foreach(libdir ${libdirs})
    set(out "${out} -L${libdir}")
  endforeach()
  foreach(lib ${libs})
    set(out "${out} ${lib}")
  endforeach()
  set("${resvar}" "${out}" PARENT_SCOPE)
endfunction()
