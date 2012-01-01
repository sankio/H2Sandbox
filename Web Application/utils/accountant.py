# Given report files from logger, these function will generate the 
# accounting data for the report page


from symbol import except_clause
import xml.dom.minidom
from xml.dom.minidom import Node

old_path = "/home/zozanh/prethesis"
source_path = "/home/zozanh/env/djcode/thesis/static/data/"

def count_all(sha):
    """
    Counting the number of functions recorded
    
    """
    if sha:
        path = source_path + sha + "/"
    else:
        path = source_path
    process_c = 0
    file_c = 0
    reg_c = 0
    network_c = 0
    
    try:
        file_doc = xml.dom.minidom.parse(path + "/file.xml")
        file_c += file_doc.getElementsByTagName("function").length
    except xml.parsers.expat.ExpatError:
        print "Error handle the file.xml: Not well-formed"
    try:
        process_doc = xml.dom.minidom.parse(path + "process.xml")
        process_c += process_doc.getElementsByTagName("dll").length
    except xml.parsers.expat.ExpatError:
        print "Error handle the process.xml: Not well-formed"
    try:
        reg_doc = xml.dom.minidom.parse(path + "reg.xml")
        reg_c += reg_doc.getElementsByTagName("function").length
    except xml.parsers.expat.ExpatError:
        print "Error handle the reg.xml: Not well-formed"
    try:
        network_doc = xml.dom.minidom.parse(path + "network.xml")
        network_c += network_doc.getElementsByTagName("function").length        
    except xml.parsers.expat.ExpatError:
        print "Error handle the network.xml: Not well-formed"
    text = "var pFields = ['tab', 'dll', 'file', 'registry', 'network'];\nvar pData = [{tab: 'Module', dll: %s},{tab: 'Process & File',  file: %s},{tab: 'Registry', registry: %s},{tab: 'Network', network: %s}];"

    summary = open(path + "summary-data.js", 'w')
    summary.write(text % (process_c, file_c, reg_c, network_c))
    summary.close()
    

def count_file(sha):
    """
    Counting the number of file functions recorded
    
    """
    if sha:
        path = source_path + sha + "/"
    else:
        path = source_path
    try:
        file = {}
        file_doc = xml.dom.minidom.parse(path + "file.xml")
        for node in file_doc.getElementsByTagName("function"):
            call = node.getAttribute("call")
            if call in file:
                file[call] += 1
            else:
                file[call] = 1

        file_data = open(path + "file-data.js", 'w')
        file_data.write("var fFields = ['name', 'call', 'views'];\nvar fData = [")
        for tex in file:
            s = "{name:'%s', call: %s, views:0},\n" % (tex, file[tex])
            file_data.write(s)
        file_data.write("];")
        file_data.close()
    except xml.parsers.expat.ExpatError:
        print "Error handle the file.xml for statistic data: Not well-formed"
        file_data = open(path + "file-data.js", 'w')
        file_data.write("var fFields = ['name', 'call', 'views'];\nvar fData = [];")
        file_data.close()

def count_network(sha):
    """
    Counting the number of network functions recorded
    
    """
    if sha:
        path = source_path + sha + "/"
    else:
        path = source_path
    try:
        network = {}
        network_doc = xml.dom.minidom.parse(path + "network.xml")

        for node in network_doc.getElementsByTagName("function"):
            call = node.getAttribute("call")
            if call in network:
                network[call] += 1
            else:
                network[call] = 1
        network_data = open(path + "network-data.js", 'w')
        network_data.write("var nFields = ['name', 'call', 'views'];\nvar nData = [")
        for tex in network:
            s = "{name:'%s', call: %s, views:0},\n" % (tex, network[tex])
            network_data.write(s)
        network_data.write("];")
        network_data.close()
    except xml.parsers.expat.ExpatError:
        print "Error handle the network.xml for statistic data: Not well-formed"
        network_data = open(path + "network-data.js", 'w')
        network_data.write("var nFields = ['name', 'call', 'views'];\nvar nData = [];")
        network_data.close()

def count_reg(sha):
    """
    Counting the number of registry functions recorded
    
    """
    if sha:
        path = source_path + sha + "/"
    else:
        path = source_path
    try:
        reg = {}
        reg_doc = xml.dom.minidom.parse(path + "reg.xml")
        for node in reg_doc.getElementsByTagName("function"):
            call = node.getAttribute("call")
            if call in reg:
                reg[call] += 1
            else:
                reg[call] = 1
        reg_data = open(path + "reg-data.js", 'w')
        reg_data.write("var rFields = ['name', 'call', 'views'];\nvar rData = [")
        for tex in reg:
            s = "{name:'%s', call: %s, views:0},\n" % (tex, reg[tex])
            reg_data.write(s)
        reg_data.write("];")
        reg_data.close()
    except xml.parsers.expat.ExpatError:
        print "Error handle the reg.xml for statistic data: Not well-formed"
        network_data = open(path + "reg-data.js", 'w')
        network_data.write("var rFields = ['name', 'call', 'views'];\nvar rData = [];")
        network_data.close()



def try_recover(src, des):
    """
    Trying to cleanup the invalid character in a report file
    
    """
    f1 = open(src, 'r')    
    s = f1.read()
    f1.close()

    f2 = open(des, 'w')
    parte = r"[^A-Za-z 0-9 \n \.,\?'\"\"!@#\$%\^&\*\(\)-_=\+;:<>\/\\\|\}\{\[\]`~]*"
    import re
    print re.sub(parte, "", s)
    f2.write(re.sub(parte, "", s))
    f2.close()

def clean_all(sha):
    """
    Clean the invalid character of the report files
    
    """
    try_recover(source_path + sha +"/file.xml", source_path + sha +"/file.xml")
    try_recover(source_path + sha +"/reg.xml", source_path + sha +"/reg.xml")
    try_recover(source_path + sha +"/network.xml", source_path + sha +"/network.xml")


def do_job(sha):
    """
    Main function for generating the report
    
    """
    clean_all(sha)
    count_file(sha)
    count_network(sha)
    count_reg(sha)
    count_all(sha)

if __name__ == "__main__":
    pass
#    Testing the accounting function
#    try_recover()
#    sha = "bc6697b0f1a90259c560054a596488730ce0c4a1"
#    try_recover(source_path + sha +"/file.xml", source_path + sha +"/file.xml")
#    count_file(sha)
#    count_network(sha)
#    count_reg(sha)
#    count_all(sha)