
#============== CONSTANTS DECLARATIONS - CHANGE AS NESCCESSARY =================
#NOTE: This script assumes the following:
#  1. Envar MONGO_ROOT is set
#  2. Envar ASSIMP_ROOT is set
#  3. Envar BOOST_LIBRARYDIR is set
#  4. Envar REPOBOUNCER_ROOT is set
#  5. Envar GLC_ROOT is set
#  6. 3drepogui.exe is compiled (through QT Creator or other means)

#github repository
githubRepo = "https://github.com/3drepo/3drepogui"

#Release tag (Also used to name the folder)
releaseTag = "v1.0a"

#name
projName = "3drepogui"
#where 3drepogui is
guiPath = "C:\\Users\\Carmen\\3D Repo\\Repo\\build-3drepogui-Desktop_Qt_5_5_1_MSVC2013_64bit-Release\\release\\"
bouncerdll = "3drepobouncer_1_0a.dll"


#Specification required for qmake
qmakeSpec = "win32-msvc2013"

#Path to jom
jomPath = "C:\\Qt5.5\\Tools\\QtCreator\\bin\\jom.exe"

#Name of GLC dll
glcdll = "GLC_lib3.dll"

#Name of mongo dll
mongodll = "mongoclient.dll"

#Name of assimp dll
assimpdll = "assimp-vc120-mt.dll"

#Names of boost dlls
boostdlls = ["boost_chrono-vc120-mt-1_58.dll", "boost_date_time-vc120-mt-1_58.dll", "boost_filesystem-vc120-mt-1_58.dll", "boost_log_setup-vc120-mt-1_58.dll","boost_log-vc120-mt-1_58.dll","boost_program_options-vc120-mt-1_58.dll","boost_regex-vc120-mt-1_58.dll","boost_system-vc120-mt-1_58.dll","boost_thread-vc120-mt-1_58.dll"]
#===============================================================================

import os

rootDir = os.getcwd() + "\\" + releaseTag
installDir = rootDir

#Create output directory
os.system("mkdir \"" + rootDir + "\"")
os.system("mkdir \"" +installDir +"\\bin\"")

#Get everything needed to run QT project
#windeployqt doesn't like quotes in the path... copy to a scratch and use relative path.
os.system("mkdir scratch")
os.system("copy  \""+ guiPath +"\" scratch")
os.system("windeployqt scratch")

#Copy exe
os.system("xcopy /e scratch\\* \"" + installDir + "\\bin\"")
os.system("del /s \"" + installDir + "\\bin\\*.cpp")
os.system("del /s \"" + installDir + "\\bin\\*.obj")

os.system("rmdir scratch /S /Q")

#Copy libraries
fpMongodll = os.environ["MONGO_ROOT"] + "\\lib\\" + mongodll
fpAssimpdll = os.environ["ASSIMP_ROOT"] + "\\lib\\" + assimpdll
fpGLCdll = os.environ["GLC_ROOT"] + "\\lib\\Release\\" + glcdll

os.system("copy \"" + fpMongodll + "\" \"" + installDir + "\\bin\"")
os.system("copy \"" + fpAssimpdll + "\" \"" + installDir + "\\bin\"")
os.system("copy \"" + fpGLCdll + "\" \"" + installDir + "\\bin\"")

boostLib = os.environ["BOOST_LIBRARYDIR"]
for boostDll in boostdlls:
	os.system("copy \"" + boostLib + "\\" + boostDll + "\" \"" + installDir + "\\bin\"")

os.system("copy \"" + os.environ["REPOBOUNCER_ROOT"] + "\\lib\\" + bouncerdll + "\" \"" + installDir + "\\bin\"")

#copy README and licensing info
os.system("copy \"" + rootDir + "\\..\\README.txt\" \"" + installDir + "\\\"")
os.system("mkdir \"" + installDir +"\\license\"")
os.system("copy \"" + rootDir + "\\..\\license\\*\" \"" + installDir + "\\license\"")

