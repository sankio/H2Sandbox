import sys
import time

from logger import clean_log
from accountant import do_job
from logger import getlog
from symbol import except_clause
import vboxapi
from vboxapi import VirtualBoxManager


__author__ = "zozanh"
__date__ = "$Apr 19, 2011 12:16:16 PM$"

USERNAME = "h2sandbox"
PASSWORD = "dynamix"

def progressBar(ctx, p, wait=1000):
    """
    Checking the progress of the API calling
    
    """
    try:
        while not p.completed:
            pass
        return 1
    except KeyboardInterrupt:
        print "Interrupted."
        return 0

def connect(ctx, args):
    """
    Connect to the VirtualBox server
    
    """
    if (len(args) > 4):
        return 0

    if ctx['vb'] is not None:
        return 0

    if (len(args) > 1):
        url = args[0]
    else:
        url = None

    if (len(args) > 2):
        user = args[1]
    else:
        user = ""

    if (len(args) > 3):
        passwd = args[2]
    else:
        passwd = ""
    
    ctx['wsinfo'] = [url, user, passwd]
    vbox = ctx['global'].platform.connect(url, user, passwd)
    ctx['vb'] = vbox
    print "Initialized connection to VirtualBox version %s" % (vbox.version)
    ctx['perf'] = ctx['global'].getPerfCollector(ctx['vb'])
    return 0

def disconnect(ctx, args):
    """
    Disconnect from the VirtualBox server
    
    """
    if (len(args) != 1):
        return 0

    if ctx['vb'] is None:
        print "Not connected yet."
        return 0

    try:
        ctx['global'].platform.disconnect()
        print "disconnected"
    except:
        print "failed"
        ctx['vb'] = None
        raise
    ctx['vb'] = None
    return 0

def reconnect(ctx, args):
    """
    Reconnect to the VirtualBox server
    
    """
    if ctx['wsinfo'] is None:
        print "Never connected..."
        return 0

    try:
        ctx['global'].platform.disconnect()
    except:
        pass

    [url, user, passwd] = ctx['wsinfo']
    ctx['vb'] = ctx['global'].platform.connect(url, user, passwd)
    print "Running VirtualBox version %s" % (ctx['vb'].version)
    return 0

def getMachines(ctx, invalidate=False, simple=False):
    """
    Get the virtual machine list from the virtualbox server
    
    """
    if ctx['vb'] is not None:
        if ctx['_machlist'] is None or invalidate:
            ctx['_machlist'] = ctx['global'].getArray(ctx['vb'], 'machines')
        if simple:
            return ctx['_machlistsimple']
        else:
            return ctx['_machlist']
    else:
        return []

def machineByName(ctx, name):
    """
    Search for the virtual machine in the virtualbox server by given name
    
    """
    mach = None
    for m in getMachines(ctx, False, False):
        if m.name == name:
            mach = m
            break
        mid = str(m.id)
        if mid[0] == '{':
            mid = mid[1:-1]
        if mid == id:
            mach = m
            break
    return mach
    
def execInGuest(ctx, session, args, env, user, passwd, tmo):
    """
    Execute a program in a virtual machine by given name in args[0]
    
    """
    if len(args) < 1:
        return    
    env = ["CD="]
    console = session.getConsole()
    guest = console.guest
    # shall contain program name as argv[0]
    gargs = args
    print "executing %s with args %s as %s" % (args[0], gargs, user)
    (progress, pid) = guest.executeProcess(args[0], 0, gargs, env, user, passwd, tmo)
    print "executed with pid %d" % (pid)
    time.sleep(68)


def pauseGuest(ctx, session):
    """
    Pausing a virtual machine by given session obj
    
    """
    console = session.getConsole()
    p = console.saveState()
    p.waitForCompletion(-1)
    ctx['global'].waitForEvents(0)
    print "Done Pause"
    return 0

def restoreSnapshot(ctx, session, uuid, name):
    """
    Restore a virtual machine to a snapshot specified by name
    
    """
    vb = ctx["vb"]
    vb.openSession(session, uuid)
    if (name == None):
        name = "prebuild"
    machine = session.getMachine()
    snap = machine.findSnapshot(name)
    console = session.getConsole()
    p = console.restoreSnapshot(snap)
    p.waitForCompletion(-1)
    ctx['global'].waitForEvents(0)
    print "Done restore"
    return 0

def startVm(ctx, mach, type, f, sha):
    """
    Start a virtual machien and run the analysis process
    
    """
    mgr = ctx['mgr']
    vb = ctx['vb']
    session = mgr.perf = ctx['perf']
    session = mgr.getSessionObject(vb)
    uuid = mach.id
    from datetime import datetime
    start_time = datetime.now()
    end_time = datetime.now()
    try:
        snap = mach.currentSnapshot
        if snap.name != "testing":
            raise Exception        
        progress = vb.openRemoteSession(session, uuid, type, "")
        with open('/home/zozanh/sharedmal/target.exe', 'wb+') as destination:
            for chunk in f.chunks():
                destination.write(chunk)
        clean_log()
        if progressBar(ctx, progress, 100) and int(progress.resultCode) == 0:

                execInGuest(ctx, session, ["C:\\WINDOWS\\system32\\PrototypeI.exe"], None, "zozanh", "12345", -1)
                pauseGuest(ctx, session)
                end_time = datetime.now()
                time.sleep(1)
                getlog({"SHA1": sha, "End Time": end_time, "Start Time": start_time, "File Name": f.name, "File Size": f.size}, sha)
                do_job(sha)
                file = open("/home/zozanh/env/djcode/thesis/static/data/" + sha + '/fin.log', "wr+")
                file.close()
                restoreSnapshot(ctx, session, uuid, "testing")
                time.sleep(1)
                session.close()
        else:
            pass
    except Exception:
        return False
    return True
    
def calling(f, sha):
    """
    Main interface of the module, expected to be called from outside
    to execute the analysis process
    
    """
    style = "WEBSERVICE"
    myManager = VirtualBoxManager(style, None)
    if (not f or not sha):
        file = open("/home/zozanh/Desktop/fin.log", "wr+")
        file.close()
    ctx = {'global' : myManager,
        'mgr':myManager.mgr,
        'vb': None,
        'const':myManager.constants,
        'remote':myManager.remote,
        'type':myManager.type,
        '_machlist': None,
        }
    connect(ctx, ["http://localhost:18083/", USERNAME, PASSWORD])
    oMachine = machineByName(ctx, "prethesis")
    if startVm(ctx, oMachine, "gui", f, sha):
        disconnect(ctx, ("h2sandbox",))
        return True
    else:
        disconnect(ctx, ("h2sandbox",))
        return False

if __name__ == "__main__":
#   Testing the functionality of the module
    print "Hello World"
    style = "WEBSERVICE"
    myManager = VirtualBoxManager(style, None)
    ctx = {'global':myManager,
        'mgr':myManager.mgr,
        'vb': None,
        'const':myManager.constants,
        'remote':myManager.remote,
        'type':myManager.type,
        '_machlist': None,
    }
    connect(ctx, ["http://localhost:18083/", "test", "test"])
    oMachine = machineByName(ctx, "prethesis")
    startVm(ctx, oMachine, "gui")




