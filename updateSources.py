import os

srcDir='src'
headerDir='src'
formDir='forms'
resourcesDir='.'
fileName='3drepogui.pri'

def printHeader(file):
	
	file.write('#THIS IS AN AUTOMATICALLY GENERATED FILE - DO NOT OVERWRITE THE CONTENT!\n')
	file.write('#If you need to update the sources/headers/forms, run updateSources.py at project root level\n')
	file.write('#If you need to import an extra library or something clever, do it on the 3drepogui.pron')
	file.write('#If you really need to overwrite this file, be aware that it will be overwritten if updateSources.py is executed.\n\n\n')
	return

def writeAllFilesFound(dirName, files, subDirList, fileToWrite, ext):
	for fname in files:
		if fname.lower().endswith(ext):
			fileToWrite.write(' \\\n\t' + dirName.replace('\\','/') + '/'+fname)
	return


priFile = open(fileName, 'w')
priFile.truncate()

printHeader(priFile)

#Headers

priFile.write('HEADERS += ')
print('writing headers to ' + fileName + '...\n')
for dir, subDirList, fl in os.walk(headerDir):
	writeAllFilesFound(dir, fl, subDirList, priFile, '.h')

#Sources
priFile.write('\n\nSOURCES += ')
print('writing sources to ' + fileName + '...\n')
for dir, subDirList, fl in os.walk(srcDir):
	writeAllFilesFound(dir, fl, subDirList, priFile, '.cpp')


#Forms
priFile.write('\n\nFORMS += ')
print('writing forms to ' + fileName + '...\n')
for dir, subDirList, fl in os.walk(formDir):
	writeAllFilesFound(dir, fl, subDirList, priFile, '.ui')

#Resources
priFile.write('\n\nRESOURCES += ')
print('writing resources to ' + fileName + '...\n')
for dir, subDirList, fl in os.walk(resourcesDir):
	writeAllFilesFound(dir, fl, subDirList, priFile, '.qrc')

priFile.close()

print('done.');
