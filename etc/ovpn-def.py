def plugin_up(envp, device, tun_mtu, link_mtu, a, b, c):
        print dir()
        return True

def plugin_down(envp, device, tun_mtu, link_mtu, a, b, c):
        print dir()
        return True

def route_up(envp):
       print dir()
       return True

def ipchange(envp, ip, port):
        print dir()
        return True

def tls_verify(envp, depth, subject):
        print dir()
        return True

def auth_user_pass_verify(envp):
        print dir()
        return True

def client_connect(envp):
        print dir()
        return True

def client_disconnect(envp):
        print dir()
        return True

def learn_address(envp, action, addr, cn=''):
        print dir()
        return True

def tls_final(envp):
        print dir()
        return True

def enable_pf(envp):
        print dir()
        return True
