import os
env = Environment(ENV = {'PATH' : os.environ['PATH']})
env.ParseConfig('python-config --cflags')
env.ParseConfig('python-config --ldflags')
env['RPATH'] = '/usr/local/lib'
env['LIBPATH'] = '/usr/local/lib'
#print env.Dump()
env.Decider('timestamp-newer')
libobj = env.SharedObject('ovpn-py.c')
env.Depends(libobj, 'openvpn-plugin.h')
env.SharedLibrary('ovpn-py', libobj)