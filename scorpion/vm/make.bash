#!/bin/bash          
# This is our bash script for muilding the Scorpion virtual machine

# G++ will be our loveley compiler
CC="g++"
OBJ="scorpion"
FILE_FORM=".o"

# bash make info
S_VERSION="SCORPION_1_0"


# We want our warnings to become errors
#C_FLAGS="-W -Wall -ansi -pedantic -Wformat-nonliteral -Wcast-align
# -Wpointer-arith -Wmissing-declarations -Winline -Wundef
# -Wcast-qual -Wshadow -Wconversion -Wwrite-strings
# -Wno-conversion -ffloat-store -g -m32 -Os -s"
# C_FLAGS="-W -Wall -g -m32"
C_FLAGS="-W -Wall -Os -m32 -s"

# Compile first source tree
SOURCE_TREE_1="*.c"

$CC $C_FLAGS $SOURCE_TREE_1 -o $OBJ

# Set build file to /sbin and clean dir 
sudo cp $OBJ /sbin
#rm $OBJ
