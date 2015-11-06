import os
from os.path import join

env = Environment()

## scons output colorizer
try:
    from colorizer import colorizer
    col = colorizer()
except:
    pass


root = Dir('#').abspath

vars = Variables('.scons.conf')

vars.Add('LIBPATH', '', '')
vars.Add('CPPPATH', '', '')

vars.Add('prefix', '', join(root, 'build', 'install'))
vars.Add('build_dir', '', join(root, 'build'))

vars.Add('debug', '', False)
vars.Update(env)


#Install/Build Dirs
for i in ['prefix', 'build_dir']:
    env[i]  = env.Dir(env[i]).abspath

#Build Flags
if(env['debug']):
    env['CCFLAGS'] = "-g -O0"
else:
    env['CCFLAGS'] = "-O3 -DNDEBUG"
env['CCFLAGS'] += " -std=c++11  -Wall -pedantic"


env['ENV']['TERM'] = os.environ['TERM']


Export('env')
SConscript('SConscript', variant_dir=env['build_dir'])
