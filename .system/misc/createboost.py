#!/usr/bin/env python3

fake=False

#warm up:

import os,sys,glob
from pipes import quote as shquote

assert len(sys.argv)==2,"Please specify location of unpacked boost source tar-ball"
boost_src = sys.argv[1]
dgdir = os.getenv('DGCODE_DIR')
assert dgdir
sys.path = [os.path.join(dgdir,'.system/mods')] + sys.path
tmpdir = os.path.join(dgdir,'.bld/tmpboost')
import utils
inctarget = os.path.join(dgdir,'.system/include/dgboost')
utils.rm_rf(inctarget)
utils.mkdir_p(inctarget)
if not fake:
    utils.rm_rf(tmpdir)
utils.mkdir_p(tmpdir)

header_mods = "python"#TODO REDUCE AS MUCH AS POSSIBLE...

#Testing on gcc 4.9.2/Fedora21 shows that headers in the following projects are
#not actually included despite being selected by bcp. Thus, we agressively ignore them:
block_header_mods = 'align atomic chrono date_time dynamic_bitset fusion intrusive mpi program_options proto random ratio regex spirit test thread timer xpressive io'
block_subdirs = ['no_ctps','msvc60','msvc70','mwcw','bcc51','dmc','bcc','bcc_pre590','no_ttp','numpy']
block_huge_files = ['vector50.hpp','vector100.hpp','vector150.hpp','vector200.hpp']
#time to work
if not fake:
    ec = utils.system("cd %s && ./bootstrap.sh && ./b2 tools/bcp"%(shquote(boost_src))
                      + "&& ./dist/bin/bcp --namespace=dgboost --namespace-alias --unix-lines "
                      + header_mods + " " + shquote(tmpdir))
else:
    ec = 0

assert ec==0

def get_files_recursively(path,extensions,blocklist=None):
    for fbn in os.listdir(path):
        if fbn in block_huge_files:
            print("IGNORING HUGE FILE",fbn)
            continue#some huge files we probably don't need anyway
        f = os.path.join(path,fbn)
        if os.path.isdir(f):
            if fbn in block_subdirs:
                print("Blocking subdir",fbn)
                continue
            if blocklist and fbn in blocklist:
                print(block_header_mods)
                print("Blocking subdir",fbn)
                continue
            for ff in get_files_recursively(f,extensions):
                yield ff
        elif os.path.splitext(f)[1] in extensions:
            yield f

def copycodefile(srcf,trgf):
    utils.mkdir_p(os.path.dirname(trgf))
    of = open(trgf,'w')
    infile = open(srcf)
    for l in infile:
        if 'include' in l and '"zlib.h"' in l:
            l=l.replace('"zlib.h"','"MiniZLib/zlib.h"')
        if 'boost/' in l:
            l = l.replace('"boost/','"dgboost/')
            l = l.replace('(boost/','(dgboost/')
            l = l.replace('<boost/','<dgboost/')
        of.write(l)
    of.close()
    infile.close()

hdrsrc=os.path.join(tmpdir,'boost')
for srcf in sorted(get_files_recursively(hdrsrc,['.hpp','.ipp','.h'],block_header_mods.split())):
    trgf = os.path.join(inctarget,os.path.relpath(srcf,hdrsrc))
    copycodefile(srcf,trgf)

#build select libs (specified here with inter-dependencies, None means it will be embedded in the library of the DGBoost package):

lib_mods={'python':None}
#'iostreams':['MiniZLib']}

libs_embedded_in_core=['python','system','filesystem']

extra_cflags="-Wno-shadow -Wno-error -Wno-extra -Wno-all -Wno-pedantic"
for lm in sorted(lib_mods.keys()):
    pkgdeps = lib_mods[lm]
    if pkgdeps is not None:
        pkgdir = os.path.join(dgdir,'packages/Framework/External/CoreLibs/Boost%s'%lm.capitalize())
        utils.rm_rf(pkgdir)
        pkgsrc = os.path.join(pkgdir,'libsrc')
        utils.mkdir_p(pkgsrc)
        fh=open(os.path.join(pkgdir,'pkg.info'),'w')
        deps=''
        if lib_mods[lm]:
            deps='USEPKG '+' '.join(sorted(lib_mods[lm]))+' '
        fh.write('package(%sEXTRA_COMPILE_FLAGS %s)\n'%(deps,extra_cflags)
                 +'#Autogenerated package providing library for the boost-%s module\n'%lm)
        fh.close()
    else:
        pkgsrc=os.path.join(dgdir,'packages/Framework/External/DGBoost/libsrc')
        for oldfile in glob.glob(os.path.join(pkgsrc,'dgboost*')):
            os.remove(oldfile)
    pkgsrcsrc = os.path.join(tmpdir,'libs',lm,'src')
    for f in  get_files_recursively(pkgsrcsrc,['.cpp','.hpp','.ipp','.h']):
        if lm=='iostreams' and f.endswith('/bzip2.cpp'):
            continue
        newf = os.path.relpath(f,pkgsrcsrc).replace(os.sep,'__').replace('.cpp','.cc').replace('.hpp','.hh').replace('.ipp','.icc')
        if newf.endswith('.h'):
            newf += 'h'
        if pkgdeps is None:
            newf=os.path.join(os.path.dirname(newf),'dgboost%s_'%lm+os.path.basename(newf))
        copycodefile(f,os.path.join(pkgsrc,newf))

#copy over needed parts by hand

##     13 algorithm/string.hpp /// Just starts_with and ends_with
##      1 cstdint.hpp // by Core_Types.hh
##      9 filesystem.hpp // mainly exists
##      1 iostreams/filter/gzip.hpp //asciireader... we could live without it eventually...
##      1 iostreams/filtering_streambuf.hpp //asciireader we could live without it eventually...
##      1 mpl/vector.hpp
##      1 python/enum.hpp
##      3 python.hpp
##      1 python/operators.hpp
##      1 shared_ptr.hpp
##      1 static_assert.hpp


#Those packages must compile without: -Wshadow -pedantic -Wextra -Werror -Wall

#investigate precompiled headers! At least some of them, like dgboost/python.hpp
#  Must use same compilation flags!
#  Debug with: -Winvalid-pch
#  .pch to .hgignore/.gitignore
#
# But perhaps we should simply instead precompile our own headers (then Core/Python.hh will take care of business). Would be nice if we could keep them in separate dirs though so they won't show up under packages/ (perhaps we have to symlink headers...).... I guess we can, if we keep stuff like:
#
# .bld/pc/Utils/pch/Utils/ParametersBase.hh.gch
#
# And then keep the include symlinks to these files.
#
# This should only be for libinc/ headers of course!
#
# And it should for sure be optional, so we disable it if needed on obscure systems (or to keep disk space reqs. low).
# NB: Can't precompile c-headers and then include from c++ (perhaps).

print (" DONE... probably... note that for boost 1.71, we had to copy the file .system/include/dgboost/graph/detail/empty_header.hpp from somewhere online (it contains nothing except the include guard), so perhaps we should restore it to the previous version if compilation fails")
