#!/bin/bash          
# This is our bash script for muilding the Scorpion virtual machine

# G++ will be our loveley compiler
CC="g++"
OBJ="scorpionc"
FILE_FORM=".o"

# bash make info
S_VERSION="SCORPIONC_1_0"


# We want our warnings to become errors
#C_FLAGS="-W -Wall -ansi -pedantic -Wformat-nonliteral -Wcast-align
# -Wpointer-arith -Wmissing-declarations -Winline -Wundef
# -Wcast-qual -Wshadow -Wconversion -Wwrite-strings
# -Wno-conversion -ffloat-store -g -m32 -Os -s"
# C_FLAGS="-W -Wall -g -m32"
C_FLAGS="-Os -s"

# Compile first source tree
SOURCE_TREE_1="*.c"
SOURCE_TREE_2="clib/*.c"
SOURCE_TREE_3="clib/*/*.c"

$CC $C_FLAGS $SOURCE_TREE_1 $SOURCE_TREE_2 $SOURCE_TREE_3 -o $OBJ

# Set build file to /sbin and clean dir 
sudo cp $OBJ /sbin
#rm $OBJ
