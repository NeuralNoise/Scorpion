{"filter":false,"title":"build.c","tooltip":"/scorpion/build.c","undoManager":{"mark":17,"position":17,"stack":[[{"start":{"row":0,"column":0},"end":{"row":14,"column":1},"action":"insert","lines":["#include <stdio.h>","#include <iostream>","#include <string>","#include <sstream>","#include <stdlib.h>","using namespace std;","","int main()","{","  system(\"g++ -std=c++11 -o scorpionc *.cpp clib/*.cpp\");","  system(\"sudo cp scorpionc /bin\");","  system(\"scorpionc\");","  system(\"cd test/.dev/\");","  system(\"scorpionc --build settings.dev ../app/src/scorpion/application/HelloWorld.scorpion\");","}"],"id":1}],[{"start":{"row":9,"column":36},"end":{"row":9,"column":37},"action":"remove","lines":["c"],"id":2}],[{"start":{"row":9,"column":37},"end":{"row":9,"column":53},"action":"remove","lines":["*.cpp clib/*.cpp"],"id":3}],[{"start":{"row":9,"column":37},"end":{"row":9,"column":38},"action":"insert","lines":["s"],"id":4}],[{"start":{"row":9,"column":38},"end":{"row":9,"column":39},"action":"insert","lines":["c"],"id":5}],[{"start":{"row":9,"column":39},"end":{"row":9,"column":40},"action":"insert","lines":["o"],"id":6}],[{"start":{"row":9,"column":40},"end":{"row":9,"column":41},"action":"insert","lines":["r"],"id":7}],[{"start":{"row":9,"column":37},"end":{"row":9,"column":41},"action":"remove","lines":["scor"],"id":8},{"start":{"row":9,"column":37},"end":{"row":9,"column":47},"action":"insert","lines":["scorpionvm"]}],[{"start":{"row":9,"column":47},"end":{"row":9,"column":48},"action":"insert","lines":["/"],"id":9}],[{"start":{"row":9,"column":48},"end":{"row":9,"column":49},"action":"insert","lines":["M"],"id":10}],[{"start":{"row":9,"column":49},"end":{"row":9,"column":50},"action":"insert","lines":["a"],"id":11}],[{"start":{"row":9,"column":50},"end":{"row":9,"column":51},"action":"insert","lines":["i"],"id":12}],[{"start":{"row":9,"column":51},"end":{"row":9,"column":52},"action":"insert","lines":["n"],"id":13}],[{"start":{"row":9,"column":52},"end":{"row":9,"column":53},"action":"insert","lines":["."],"id":14}],[{"start":{"row":9,"column":53},"end":{"row":9,"column":54},"action":"insert","lines":["c"],"id":15}],[{"start":{"row":10,"column":26},"end":{"row":10,"column":27},"action":"remove","lines":["c"],"id":16}],[{"start":{"row":11,"column":18},"end":{"row":11,"column":19},"action":"remove","lines":["c"],"id":17}],[{"start":{"row":11,"column":21},"end":{"row":13,"column":95},"action":"remove","lines":["","  system(\"cd test/.dev/\");","  system(\"scorpionc --build settings.dev ../app/src/scorpion/application/HelloWorld.scorpion\");"],"id":18}]]},"ace":{"folds":[],"scrolltop":0,"scrollleft":0,"selection":{"start":{"row":11,"column":21},"end":{"row":11,"column":21},"isBackwards":false},"options":{"guessTabSize":true,"useWrapMode":false,"wrapToView":true},"firstLineState":0},"timestamp":1441992873000,"hash":"93b9c477012a71dac5ca86c0780d68940d0ff3af"}