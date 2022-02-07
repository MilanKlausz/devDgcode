#global variables
from os import path
join=path.join
import conf#project specific configuration

#system dir is one up from the modules dir:
sysdir = path.realpath(join(path.dirname(__file__),'..'))
fmwkdir = conf.framework_dir(sysdir)
#print('fmwkdir', fmwkdir, '\n')
valdir = conf.validation_dir(sysdir)
#print('valdir', valdir, '\n')
blddir = conf.build_dir()
blddir_indicator = conf.build_dir_indicator(blddir)
#print('blddir', blddir, '\n')
makefiledir=join(blddir,'makefiles')
extpkgdirs = conf.ext_package_dirs()
#print('extpkgdirs', extpkgdirs, '\n')
codedirs = conf.code_dirs(sysdir)
#print('codedirs', codedirs, '\n')
installdir = conf.install_dir()
installdir_indicator = conf.install_dir_indicator(installdir)
#print('installdir', installdir, '\n')
testdir = conf.test_dir()
#print('testdir', testdir, '\n')
projdir = conf.project_dir()
cmakedetectdir = join(sysdir,'cmakedetect')
incdirname='include'
libdirname='libs'

sysinc = join(sysdir,'include')
envcache = join(blddir,'env.cache')
varcache = join(blddir,'vars.cache')#dynamic user settings (for pkg filters and cmake flags)
systimestamp_cache=join(blddir,'systimestamp.cache')
setupfile_cache=join(blddir,'setup.sh')
lockfile=blddir+".lock"

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
    return join(blddir,'pc',_pkgname(pkg),*subpaths)

def makefile_pkg_cache_dir(pkg,*subpaths):
    return join('${BLD}','pc',_pkgname(pkg),*subpaths)

#where we link (or create dynamic pkgs):
pkgdirbase = join(blddir,'pkgs')
def pkg_dir(pkg,*subpaths): return join(pkgdirbase,_pkgname(pkg),*subpaths)
def makefile_pkg_dir(pkg,*subpaths): return join('${PKG}',_pkgname(pkg),*subpaths)

#sanity:
for d in [sysdir,blddir,*codedirs,installdir]:
    assert not ' ' in d, 'Spaces not allowed in directory names. Error with "%s"'%d 

# Package directory aliases #keep them lowercase
pkgdir_aliases = {
  "framework": fmwkdir,
  "validation": valdir,
  "projects": projdir
  }
