#!/bin/sh

# simple install script for Scorpion
VERSION_=1.0

# Prompt for a user for distribution package...
echo "Please enter the distribution package you want to install:"
read PACKAGE

if [ -s ${PACKAGE} ]; then
   mkdir -p "/usr/share/scorpion/sdk"
   if [ $? != 0 ]; then 
      exit
   fi
   
   printf "extracting files...\n"
   tar -zxvf $PACKAGE -C "/usr/share/scorpion/sdk"
   if [ $? != 0 ]; then 
      exit
   fi
   
   printf "Setting up SDK directories...\n"
   mkdir -p "/usr/share/scorpion/{libs,vm/log}"
   cd "/usr/share/scorpion/sdk/sdk0.1.0_0/"
   
   printf "Extracting Binaries and libraries...\n"
   tar -zxvf sdk0.1.0_0.tar.gz
  
   printf "Installing Software...\n"
   cd bin/
   chmod u+x scorpion sar
   sudo cp scorpion sar /sbin/
   cd ../
   sudo cp -a libs/. /usr/share/scorpion/libs/
else 
   echo "install:  error: '${PACKAGE}' is no such file."
fi
