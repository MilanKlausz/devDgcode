#global variables
from os import path
join=path.join
import conf#project specific configuration
from pathlib import Path

#system dir is one up from the modules dir:
sysdir = path.realpath(join(path.dirname(__file__),'..'))
#sysdir = Path(__file__).resolve().parent
fmwkdir = conf.framework_dir(sysdir)
valdir = conf.validation_dir(sysdir)
blddir = conf.build_dir()
blddir_indicator = conf.build_dir_indicator(blddir)
makefiledir = blddir.joinpath('makefiles')
pkgpathdirs = conf.pkg_path_dirs()
extpkgdirs = conf.ext_package_dirs()
codedirs = conf.code_dirs(sysdir)
installdir = conf.install_dir()
installdir_indicator = conf.install_dir_indicator(installdir)
testdir = conf.test_dir()
projdir = conf.project_dir()
cmakedetectdir = join(sysdir,'cmakedetect')
incdirname='include'
libdirname='libs'

sysinc = join(sysdir,'include')
envcache = blddir.joinpath('env.cache')
varcache = blddir.joinpath('vars.cache')#dynamic user settings (for pkg filters and cmake flags)
systimestamp_cache=blddir.joinpath('systimestamp.cache')
setupfile_cache=blddir.joinpath('setup.sh')
lockfile=blddir.joinpath(".lock")

def makefile_instdir(*subpaths):
    if not subpaths:
        return "${INST}"
    subpaths=join(*subpaths)
    if subpaths[0]=='/':
        subpaths=path.relpath(subpaths,installdir)
    return join("${INST}",subpaths)

def makefile_blddir(*subpaths):
    if not subpaths:
        return "${BLD}"
    subpaths=join(*subpaths)
    if subpaths[0]=='/':
        subpaths=path.relpath(subpaths,blddir)
    return join("${BLD}",subpaths)

def _pkgname(pkg):
    return pkg.name if hasattr(pkg,'name') else pkg

def pkg_cache_dir(pkg,*subpaths):
    return blddir.joinpath('pc',_pkgname(pkg),*subpaths)

def makefile_pkg_cache_dir(pkg,*subpaths):
    return join('${BLD}','pc',_pkgname(pkg),*subpaths)

#where we link (or create dynamic pkgs):
pkgdirbase = blddir.joinpath('pkgs')
def pkg_dir(pkg,*subpaths): return join(pkgdirbase,_pkgname(pkg),*subpaths)
def makefile_pkg_dir(pkg,*subpaths): return join('${PKG}',_pkgname(pkg),*subpaths)

#sanity:
#for d in [sysdir,blddir,*codedirs,installdir]:
#    assert not ' ' in d, 'Spaces not allowed in directory names. Error with "%s"'%d 

# Package directory aliases #keep them lowercase
pkgdir_aliases = {
  "framework": fmwkdir,
  "validation": valdir,
  "projects": projdir
  }
