#!/usr/bin/python

import sys
import os

def fatalError(message):
    print message
    sys.exit()

numArguments = len(sys.argv)

if numArguments < 4:
    fatalError("Usage: " + sys.argv[0] + " <prod/dev> <guiversion> <bouncerversion>")

release_type = sys.argv[1]
version      = sys.argv[2]
bversion     = sys.argv[3]

production   = (release_type == "prod")
branch       = "release" if production else "master"

os.system("cd ../..")

code = os.system("git checkout " + branch)

if code:
    fatalError("git checkout failed")

if code:
    fatalError("git force add failed")


os.system("sed -i 's/BOUNCER_VERS = [^ ]*/BOUNCER_VERS = " + bversion.replace('.', '_') + "/' dependencies.pri")

os.system("sed -i 's/QCoreApplication::setApplicationVersion(\"[^ ]*/QCoreApplication::setApplicationVersion(\"" + version + "\");/' src/main.cpp")

os.system("sed -i 's/EXPECTED_BOUNCER_VERSION \"[^ ]*/EXPECTED_BOUNCER_VERSION \"" + bversion + "\"/' src/repo/gui/repo_gui.h")


os.system("git add dependencies.pri src/main.cpp src/repo/gui/repo_gui.h")
os.system("git clean -f -d")

os.system("git commit -m \"Version " + version + "\"")

os.system("git push origin :refs/tags/" + version)
os.system("git tag -fa " + version + " -m \" Version " + version + " \"")

if production:
    os.system("git push origin :refs/tags/latest")
    os.system("git tag -fa latest -m \"Update latest\"")
else:
    os.system("git push origin :refs/tags/dev_latest")
    os.system("git tag -fa dev_latest -m \"Update latest\"")

os.system("git push origin --tags")
os.system("git push")

