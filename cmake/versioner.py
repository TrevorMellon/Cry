#!/bin/python3

import re
import io
import sys
import os

class versioner:
    def __init__(self,root):
        self.version_major = 0
        self.version_minor = 0
        self.version_build = 0
        self.root = root        

    def read_version_file(self):
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

    def update_version(self):
        fo = open(self.root + "/VERSION", "w")
        build = int(self.version_build)
        build = build + 1
        self.version_build = str(build)

        vs = self.version_major + "." + self.version_minor + "." + self.version_build

        fo.writelines(vs)
        
        fo.close()        

        self.update_cmake_ver()
        

    def update_cmake_ver(self):
        vs = self.version_major + "." + self.version_minor + "." + self.version_build

        fo = open(self.root + "/cmake/versioner.cmake", "w")
        major = "SET ( VERSION_MAJOR " + self.version_major + ")\r\n"
        minor = "SET ( VERSION_MINOR " + self.version_minor + ")\r\n"
        build = "SET ( VERSION_BUILD " + self.version_build + ")\r\n"
        verstr = "SET ( VERSION \"" + vs +"\")\r\n"

        vstr = major + minor + build + verstr

        fo.writelines(vstr)
        fo.close()

    def create_header(self, headerpath, header, project):
        if not os.path.exists(headerpath):
            os.makedirs(headerpath)
        filestr = headerpath + "/" + header
        uproject = project.lower()
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
        
        h = "#pragma once\r\n\r\n"        
        h += "#include <stdint.h>\r\n\r\n"

        c = "constexpr const uint32_t "
        major = c + uproject + "_version_major = " + self.version_major + ";\r\n"
        minor = c + uproject + "_version_minor = " + self.version_minor + ";\r\n"
        build = c + uproject + "_version_build = " + self.version_build + ";\r\n"
        verstr = "constexpr const char* " + uproject + "_version_string = \"" + vs + "\";\r\n" 

        vstr = h + major + minor + build + verstr

        fo.writelines(vstr)
        fo.close()

        self.update_cmake_ver()
        

c = len(sys.argv)

if c == 2:
    r = sys.argv[1]
    v = versioner(r)
    v.read_version_file()
    v.update_version()
elif c == 5:
    root = sys.argv[1]    
    pp = sys.argv[2]
    f = sys.argv[3]
    project = sys.argv[4]
    v = versioner(root)
    v.read_version_file()
    v.create_header(pp, f, project)
else:
    print("Incorrect number of variables\nargv.count: " + str(c))
