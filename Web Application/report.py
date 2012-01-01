from django.http import HttpResponse
from django.http import HttpResponseRedirect
from django.shortcuts import render_to_response
from django.template import RequestContext
from django.template import loader
import os

LOG_PATH = "/home/zozanh/env/djcode/thesis/static/data/"

def finish(path):
    """
    Check out whether the scanning is finished
    """
    if os.path.isfile(path + "fin.log"):
        return True
    return False

def render_report(request, sha):
    """
    Render report views 
    """
    path = LOG_PATH
    if sha:
        if finish(path + sha + '/'):
                c = RequestContext(request, {'sha': sha+'/'})
                return render_to_response('report.html', c)
        else:
            c = RequestContext(request, {'sha': ""})
            return HttpResponseRedirect("/")
    else:
        c = RequestContext(request, {'sha': ""})
        return render_to_response('report.html', c)
