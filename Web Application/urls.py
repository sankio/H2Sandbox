from django.conf.urls.defaults import patterns, include, url
from front import *
from report import *
# Uncomment the next two lines to enable the admin:
# from django.contrib import admin
# admin.autodiscover()

urlpatterns = patterns('',
    # Examples:
    # url(r'^$', 'thesis.views.home', name='home'),
    # url(r'^thesis/', include('thesis.foo.urls')),
    url(r'^charts.swf$', return_chart),
    url(r'(?P<sha>\w{0,40})/(?P<name>\w{0,80}).xml$', return_xml),
    url(r'^$', upload_file),
    url(r'^report/(?P<sha>\w{0,40})', render_report),
    
#    url(r'reg.xml$', return_reg),
#    #url(r'file.xml$', return_file),
#    url(r'process.xml$', return_process),
    # Uncomment the admin/doc line below to enable admin documentation:
    # url(r'^admin/doc/', include('django.contrib.admindocs.urls')),

    # Uncomment the next line to enable the admin:
    # url(r'^admin/', include(admin.site.urls)),
)
