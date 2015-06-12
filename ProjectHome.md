Plug-in embedding Python to OpenVPN using V1 API. In fact it is only a bridge allowing developer to write OpenVPN plug-in in Python. How long it will takes you to write user authorization against some PostregSQL user table with this plug-in? ;-) Just import psycopg and do some select stuff in auth\_user\_pass\_verify function. Almost the same for LDAP.
Want to control something in firewall? Just import [pyopenbsd](http://github.com/cortesi/pyopenbsd) and you could manage PF without bothering with pointers and pointer to pointers a.s.o.

To see method's provided parameters try such a Python 'plug-in' module:
```
import sys
print ">>>\n%s\n>>>\n" % sys.path

def plugin_up(*args):
     print ">>>\n%s\n>>>\n" % str(args)
     return True

def plugin_down(*args):
     print ">>>\n%s\n>>>\n" % str(args)
     return True

def route_up(*args):
    print ">>>\n%s\n>>>\n" % str(args)
    return True

def ipchange(*args):
    print ">>>\n%s\n>>>\n" % str(args)
    return True

def tls_verify(*args):
    print ">>>\n%s\n>>>\n" % str(args)
    return True

def auth_user_pass_verify(*args):
    print ">>>\n%s\n>>>\n" % str(args)
    return True

def client_connect(*args):
    print ">>>\n%s\n>>>\n" % str(args)
    return True

def client_disconnect(*args):
    print ">>>\n%s\n>>>\n" % str(args)
    return True

def learn_address(*args):
    print ">>>\n%s\n>>>\n" % str(args)
    return True

def tls_final(*args):
    print ">>>\n%s\n>>>\n" % str(args)
    return True

def enable_pf(*args):
    print ">>>\n%s\n>>>\n" % str(args)
    return True

```

Parameters are in `args` tuple variable. Run OpenVPN with `verb 3` parameter and it will explain you everything. ;-)