import subprocess
import sys
import os

PackagesNeeded=['requests',  'PyQT5', 'pyserial']

# we first check to see if we are not windows
if os.name != 'nt':
	print('This script only supports Windows, packages needed are listed before for manual installation')
	print(' '.join(PackagesNeeded))
	sys.exit(1)

# we programmatically find the python path
pythonPath = sys.executable
pythonDir = os.path.split(pythonPath)[0]
# and then find the pip path
pipPath = os.path.join(pythonDir,'Scripts','pip3')

for package in PackagesNeeded:
	print('Installing '+package)
	subprocess.run([pipPath, 'install','--upgrade',package])