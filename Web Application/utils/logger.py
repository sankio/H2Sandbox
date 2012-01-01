# Function that take the report file from analysis process and finalize it for 
# future processing

sour_path = "/home/zozanh/prethesis/"
des_path = "/home/zozanh/env/djcode/thesis/static/data/"
    
def clean_log():
    """
    Get the report files and write closing tags to the end of them
    
    """
    sour = open(sour_path + 'process.xml', 'w')
    s = '<process>'
    #sour.write(s)
    sour.close()

    sour = open(sour_path + 'file.xml', 'w')
    s = '<functions>'
    sour.write(s)
    sour.close()

    sour = open(sour_path + 'reg.xml', 'w')
    s = '<registry>'
    sour.write(s)
    sour.close()

    sour = open(sour_path + 'network.xml', 'w')
    s = '<network>'
    sour.write(s)
    sour.close()

def transform(s):
    """
    Escape all special characters of XML with correctsponding code  
    
    """
    while s.find("<![CDATA[") != -1:
         start = s.find("<![CDATA[")
         end = s.find("]]>")
         process_string = s[start + 9:end]
         print process_string
         process_string = process_string.replace("&", "&amp;")
         process_string = process_string.replace("'", "&apos;")
         process_string = process_string.replace("\"", "&quot;")
         process_string = process_string.replace("<", "&lt;")
         process_string = process_string.replace(">", "&gt;")
         print process_string
         s = s[0:start] + process_string + s[end + 3:]
    return s

def getlog(process_detail, sha):
    """
    Copy all of the log file out of the virtual machine
     
    """

    if sha:
        des_path = des_path + sha + "/"
    sour = open(sour_path + "process.xml", 'r')
    destination = open(des_path + 'process.xml', 'w')
    s = '<process>\n'
    destination.write(s)
    for field in process_detail:
        destination.write('<detail name ="%s" text="%s"></detail>' % (field, process_detail[field]))
    s = '<dlls type="loaded" text="Loaded Modules">'
    destination.write(s)
    destination.write(sour.read() + "\n")
    destination.write("</dlls>\n</process>")
    sour.close()
    destination.close()

    sour = open(sour_path + 'file.xml', 'r')
    destination = open(des_path + 'file.xml', 'w')
    
    destination.write(sour.read() + "\n")
    destination.write("</functions>")
    sour.close()
    destination.close()   

    sour = open(sour_path + 'reg.xml', 'r')
    destination = open(des_path + 'reg.xml', 'w')
    destination.write(sour.read() + "\n")
    destination.write("</registry>")
    sour.close()
    destination.close()

    sour = open(sour_path + 'network.xml', 'r')
    destination = open(des_path + 'network.xml', 'w')
    s = transform(sour.read())
    destination.write(s + "\n")
    
    destination.write("</network>")
    sour.close()
    destination.close()

if __name__ == "__main__":
    pass
#    getlog({},"1a2d99391cc4712df43e9e8bac7f45e56ecdd9ed")
