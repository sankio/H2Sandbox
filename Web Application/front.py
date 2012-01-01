import thread
import threading
from django import forms
from django.http import HttpResponse
from django.http import HttpResponseRedirect
from django.shortcuts import render_to_response
from django.template import RequestContext
from django.template import loader
from utils.accountant import do_job
from utils.logger import clean_log
from utils.logger import getlog
from utils.wvbox import calling
import os
import time

LOG_PATH = "/home/zozanh/env/djcode/thesis/static/data/"

class UploadFileForm(forms.Form):
    """
    Main upload form for the application
    """
    file  = forms.FileField()

class MyModel():
    _busy = False

    @classmethod
    def lock(cls):
        cls._busy = True
        
    @classmethod
    def unlock(cls):
        cls._busy = False

    def is_busy(self):
        return self._busy

def run_thread(threadname, f, sha):
    while not calling(f,sha):
        time.sleep(5)
    file = open(LOG_PATH + sha + '/fin.log', "wr+")
    file.close()
    thread.interrupt_main()

class BossThread(threading.Thread):

    def __init__(self, f, sha):
        threading.Thread.__init__(self)
        self.f = f
        self.sha = sha

    def run(self):
        while not calling(self.f, self.sha):
            time.sleep(5)
        


def handle_uploaded_file(f):
    """
    Handling the file extracted from upload form
    
    """
    path = LOG_PATH
    import hashlib
    sha = ""
    sha = hashlib.sha1(f.read()).hexdigest()
    if sha and os.path.isdir(path + sha):
        return sha
    else:
        os.makedirs(path + sha)     
        #thread.start_new_thread(run_thread, ("Thread1", f, sha))
        my_thread = BossThread(f, sha)
        my_thread.start()
        my_thread.join()
        return sha

def upload_file(request):
    """
    Handling the upload form from main page
    
    """
    if request.method == 'POST':
        form = UploadFileForm(request.POST, request.FILES)
        if form.is_valid():
            sha = handle_uploaded_file(request.FILES['file'])
            #c = RequestContext(request, {'form': form})
            if sha:
            	return HttpResponseRedirect("/report/" + sha)
            #return HttpResponseRedirect('/success/url/')
        else:
            c = RequestContext(request, {'form': form})
            return render_to_response('index.html', c)
    else:
        form = UploadFileForm()
        c = RequestContext(request, {'form': form})
        return render_to_response('index.html', c)

def return_xml(request, sha, name):
    """
    Obstacle method, delete in the future
    """
    file = open(LOG_PATH + sha + '/' + name + ".xml", "r+")
    xml_data = file.read()
    file.close()
    return HttpResponse(xml_data, mimetype="text/xml")

def return_file(request):
    """
    Obstacle method, delete in the future
    """
    if request.is_ajax():
        file = open(LOG_PATH + "file.xml", "r+")
        xml_data = file.read()
        print xml_data
        file.close()
        return HttpResponse(xml_data, mimetype="text/xml")
    else:
        return HttpResponse(status=400)

def return_reg(request):
    """
    Obstacle method, delete in the future
    """
    if request.is_ajax():
        file = open(LOG_PATH + "reg.xml", "r+")
        xml_data = file.read()
        file.close()
        return HttpResponse(xml_data, mimetype="text/xml")
    else:
        return HttpResponse(status=400)


def return_chart(request):
    """
    Obstacle method, delete in the future
    """
    file = open("/home/zozanh/env/djcode/thesis/static/ext-3.3/resources/charts.swf", "rb")
    chart_data = file.read()
    file.close()
    return HttpResponse(chart_data, status=404, mimetype="application/x-shockwave-flash")
