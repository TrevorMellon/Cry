#!/bin/python3

import re
import io
import sys
import os

class Versioner:
    def __init__(self,root):
        self.version_major = 0
        self.version_minor = 0
        self.version_build = 0
        self.root = root        

    def readVersionFile(self):
        fo = open(self.root + "/VERSION", "r")
        line = fo.readline()
        x = re.findall("([0-9]+)\\.([0-9]+)\\.([0-9]+)", line)
        t = x[0]
        self.version_major = t[0]
        self.version_minor = t[1]
        self.version_build = t[2]
        
        print("Major: " + self.version_major)
        print("Minor: " + self.version_minor)
        print("Build: " + self.version_build)

        fo.close()

    def updateVersion(self):
        fo = open(self.root + "/VERSION", "w")
        build = int(self.version_build)
        build = build + 1
        self.version_build = str(build)

        vs = self.version_major + "." + self.version_minor + "." + self.version_build

        fo.writelines(vs)
        
        fo.close()        

        fo = open(self.root + "/cmake/versioner.cmake", "w")
        maj = "SET ( VERSION_MAJOR " + self.version_major + ")\r\n"
        min = "SET ( VERSION_MINOR " + self.version_minor + ")\r\n"
        build = "SET ( VERSION_BUILD " + self.version_build + ")\r\n"
        verstr = "SET ( VERSION \"" + vs +"\")\r\n"

        vstr = maj + min + build + verstr

        fo.writelines(vstr)
        fo.close()

    def createHeader(self, headerpath, header, project):
        if not os.path.exists(headerpath):
            os.makedirs(headerpath)
        filestr = headerpath + "/" + header
        uproject = project.upper()
        print (self.root + "/VERSION")
        
        fo = open(self.root + "/VERSION", "w")
        build = int(self.version_build)
        build = build + 1
        self.version_build = str(build)

        vs = self.version_major + "." + self.version_minor + "." + self.version_build

        fo.writelines(vs)

        fo.close()
        print ("header: " + header)
        fo = open(filestr, "w+")
        
        h = "#ifndef " + uproject + "_VERSION_H\r\n"
        h = h + "#define " + uproject + "_VERSION_H\r\n\r\n"

        maj = "#define " + uproject + "_VERSION_MAJOR " + self.version_major + ")\r\n"
        min = "#define " + uproject + "_VERSION_MINOR " + self.version_minor + ")\r\n"
        build = "#define " + uproject + "_VERSION_BUILD " + self.version_build + ")\r\n"
        verstr = "#define " + uproject + "_VERSION_STRING \"" + vs +"\")\r\n\r\n"

        f = "#endif //" + uproject + "_VERSION_H\r\n"

        vstr = h + maj + min + build + verstr + f

        fo.writelines(vstr)
        fo.close()
        

c = len(sys.argv)

if c == 2:
    r = sys.argv[1]
    v = Versioner(r)
    v.readVersionFile()
    v.updateVersion()
elif c == 5:
    root = sys.argv[1]    
    pp = sys.argv[2]
    f = sys.argv[3]
    project = sys.argv[4]
    v = Versioner(root)
    v.readVersionFile()
    v.createHeader(pp, f, project)
else:
    print("Incorrect number of variables\nargv.count: " + str(c))
