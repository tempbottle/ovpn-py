def plugin_up(envp, device, tun_mtu, link_mtu, a, b, c):
    print dir()
    return True

def plugin_down(envp, device, tun_mtu, link_mtu, a, b, c):
    print dir()
    return True

def route_up(envp):
    print envp
    return True

def ipchange(envp, ip, port):
    print "Connection from: %s:%s" % (ip, port)
    return True

def tls_verify(envp, depth, subject):
    print "Certificate %s: %s" % (depth, subject)
    return True

def auth_user_pass_verify(envp):
    print "Username: %s" % envp['username']
    print "Password: %s" % envp['password']
    return True

def client_connect(envp):
    print envp
    return True

def client_disconnect(envp):
    print envp
    return True

def learn_address(envp, action, addr, cn=''):
    if action == 'add':
        print "Adding address %s for CN %s" % (addr, cn)
    elif action == 'modify':
        print "Adding address %s for CN %s" % (addr, cn)
    else:
        print "Deleting address %s" % addr
    return True

def tls_final(envp):
    print envp
    return True

def enable_pf(envp):
    print envp
    return True
