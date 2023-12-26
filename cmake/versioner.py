#!/usr/bin/python3

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
        t = line.split(".")        
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
        major = "set ( VERSION_MAJOR " + self.version_major + ")\r\n"
        minor = "set ( VERSION_MINOR " + self.version_minor + ")\r\n"
        build = "set ( VERSION_BUILD " + self.version_build + ")\r\n"
        verstr = "set ( VERSION \"" + vs +"\")\r\n\r\n"

        vstr = major + minor + build + verstr

        fo.writelines(vstr)
        fo.close()

    def add_header(self):
        h=u"/*\r\n"
        h+=u" *\r\n" 
        h+=u" *\r\n" 
        h+=u" * ⠀⠈⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠿⠿⠛⠛⠛⠋⢉⣉⣀⡤\r\n"
        h+=u"⠀*  ⠰⡄⠹⣿⣿⣿⣿⠿⠛⠛⠛⠉⠉⠉⠀⠀⢠⣤⣤⣴⣶⣶⣿⣿⣿⣿⡿⠁\r\n"
        h+=u"⠀*  ⠀⠘⣦⠈⢿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⠛⠿⣿⣿⣿⣿⡟⠀\r\n"
        h+=u"⠀*  ⠀⠀⠘⢷⡄⠹⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⣿⡟⠀\r\n"
        h+=u"⠀*  ⠀⠀⠀⠈⢿⣦⠈⢿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣸⣿⣿⠏⠀\r\n"
        h+=u"⠀*  ⠀⠀⠀⠀⠈⢻⣷⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⣿⠏⠀\r\n"
        h+=u"⠀*  ⠀⠀⠀⠀⠀⠀⢻⣿⣦⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⣿⠃\r\n"
        h+=u"⠀*  ⠀⠀⠀⠀⠀⠀⠀⠻⣿⣷⡄⠀⠀⠀⢀⣠⡞⠀⣾⡿⠃\r\n"
        h+=u"⠀*  ⠀⠀⠀⠀⠀⠀⠀⠀⠹⣿⣿⣦⣠⣴⣿⣿⠁⣸⡿⠁\r\n"
        h+=u"⠀*  ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣿⣿⣿⣿⣿⠇⢰⡟⠁\r\n"
        h+=u"⠀*  ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⣿⣿⡟⢀⡟\r\n"
        h+=u"⠀*  ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⡿⠁⠎\r\n"
        h+=u"⠀*  ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠃\r\n"
        h+=u" *\r\n"
        h+=u" *    Cry - Cryptor\r\n"
        h+=u" *    An ignifi Project © 2023\r\n"
        h+=u" *\r\n"
        h+=u" *    Licence : BSD 3-Clause License\r\n"
        h+=u" *    see file: LICENCE\r\n"
        h+=u" *\r\n"
        h+=u" *    Contact: cry.2023.gitlab.x9j3f22@ignifi.eu\r\n"
        h+=u" *\r\n"
        h+=u" */\r\n\r\n\r\n"
        return h.encode('utf-8', 'ignore')

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
        fo = open(filestr, "w+", encoding="utf-8")

        header = self.add_header()
        
        h = "#pragma once\r\n\r\n"        
        h += "#include <stdint.h>\r\n\r\n"

        c = "constexpr const uint32_t "
        major = c + uproject + "_version_major = " + self.version_major + ";\r\n"
        minor = c + uproject + "_version_minor = " + self.version_minor + ";\r\n"
        build = c + uproject + "_version_build = " + self.version_build + ";\r\n"
        verstr = "constexpr const char* " + uproject + "_version_string = \"" + vs + "\";\r\n" 

        vstr = header.decode("utf-8") + h + major + minor + build + verstr

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
