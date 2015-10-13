{"filter":false,"title":"security.c","tooltip":"/scorpion/vm/security.c","undoManager":{"mark":100,"position":100,"stack":[[{"start":{"row":2,"column":23},"end":{"row":2,"column":24},"action":"insert","lines":["n"],"id":101}],[{"start":{"row":2,"column":24},"end":{"row":2,"column":25},"action":"insert","lines":["a"],"id":102}],[{"start":{"row":2,"column":25},"end":{"row":2,"column":26},"action":"insert","lines":["g"],"id":103}],[{"start":{"row":2,"column":26},"end":{"row":2,"column":27},"action":"insert","lines":["e"],"id":104}],[{"start":{"row":2,"column":27},"end":{"row":2,"column":28},"action":"insert","lines":["r"],"id":105}],[{"start":{"row":2,"column":27},"end":{"row":2,"column":28},"action":"remove","lines":["r"],"id":106}],[{"start":{"row":2,"column":26},"end":{"row":2,"column":27},"action":"remove","lines":["e"],"id":107}],[{"start":{"row":2,"column":25},"end":{"row":2,"column":26},"action":"remove","lines":["g"],"id":108}],[{"start":{"row":2,"column":24},"end":{"row":2,"column":25},"action":"remove","lines":["a"],"id":109}],[{"start":{"row":2,"column":23},"end":{"row":2,"column":24},"action":"remove","lines":["n"],"id":110}],[{"start":{"row":2,"column":22},"end":{"row":2,"column":23},"action":"remove","lines":["a"],"id":111}],[{"start":{"row":2,"column":21},"end":{"row":2,"column":22},"action":"remove","lines":["M"],"id":112}],[{"start":{"row":2,"column":20},"end":{"row":2,"column":21},"action":"remove","lines":["m"],"id":113}],[{"start":{"row":2,"column":19},"end":{"row":2,"column":20},"action":"remove","lines":["e"],"id":114}],[{"start":{"row":2,"column":18},"end":{"row":2,"column":19},"action":"remove","lines":["t"],"id":115}],[{"start":{"row":2,"column":17},"end":{"row":2,"column":18},"action":"remove","lines":["s"],"id":116}],[{"start":{"row":2,"column":16},"end":{"row":2,"column":17},"action":"remove","lines":["y"],"id":117}],[{"start":{"row":2,"column":15},"end":{"row":2,"column":16},"action":"remove","lines":["S"],"id":118}],[{"start":{"row":2,"column":14},"end":{"row":2,"column":15},"action":"remove","lines":[" "],"id":119}],[{"start":{"row":2,"column":14},"end":{"row":2,"column":15},"action":"insert","lines":[" "],"id":120}],[{"start":{"row":2,"column":15},"end":{"row":2,"column":16},"action":"insert","lines":["S"],"id":121}],[{"start":{"row":2,"column":16},"end":{"row":2,"column":17},"action":"insert","lines":["e"],"id":122}],[{"start":{"row":2,"column":15},"end":{"row":2,"column":17},"action":"remove","lines":["Se"],"id":123},{"start":{"row":2,"column":15},"end":{"row":2,"column":30},"action":"insert","lines":["SecurityManager"]}],[{"start":{"row":2,"column":30},"end":{"row":2,"column":31},"action":"insert","lines":[" "],"id":124}],[{"start":{"row":2,"column":15},"end":{"row":2,"column":16},"action":"remove","lines":["S"],"id":125}],[{"start":{"row":2,"column":15},"end":{"row":2,"column":16},"action":"insert","lines":["S"],"id":126}],[{"start":{"row":2,"column":30},"end":{"row":2,"column":31},"action":"remove","lines":[" "],"id":127}],[{"start":{"row":2,"column":30},"end":{"row":2,"column":31},"action":"insert","lines":[":"],"id":128}],[{"start":{"row":2,"column":31},"end":{"row":2,"column":32},"action":"insert","lines":[":"],"id":129}],[{"start":{"row":7,"column":15},"end":{"row":7,"column":32},"action":"insert","lines":["SecurityManager::"],"id":130}],[{"start":{"row":16,"column":21},"end":{"row":16,"column":38},"action":"insert","lines":["SecurityManager::"],"id":131}],[{"start":{"row":26,"column":15},"end":{"row":26,"column":32},"action":"insert","lines":["SecurityManager::"],"id":132}],[{"start":{"row":33,"column":15},"end":{"row":33,"column":32},"action":"insert","lines":["SecurityManager::"],"id":133}],[{"start":{"row":35,"column":11},"end":{"row":36,"column":0},"action":"insert","lines":["",""],"id":134},{"start":{"row":36,"column":0},"end":{"row":36,"column":10},"action":"insert","lines":["          "]}],[{"start":{"row":36,"column":10},"end":{"row":37,"column":0},"action":"insert","lines":["",""],"id":135},{"start":{"row":37,"column":0},"end":{"row":37,"column":10},"action":"insert","lines":["          "]}],[{"start":{"row":37,"column":10},"end":{"row":66,"column":8},"action":"insert","lines":["void setupAccesPermissions(){","           access_size_t = 4;","           permissionList = new string[access_size_t];","           ","           permissionList[0] = \"scorpion.security.permission.READ_INTERNAL_STORAGE\";","           permissionList[1] = \"scorpion.security.permission.WRITE_INTERNAL_STORAGE\";","           permissionList[2] = \"scorpion.security.permission.GPIO\";","           permissionList[3] = \"scorpion.security.permission.REBOOT\";","       }","       bool permissionExists(Permission perm){","           for(int i = 0; i < access_size_t; i++){","               if(perm.getPermission() == permissionList[i])","                 return true;","           }","          return false;","       }","       int getPermissionSize(){","           return access_size_t;","       }","       Permission valueAt(int idx){","           ","           Permission p;","           p.setPermission(\"null\");","           if(idx < access_size_t){","               p.setPermission(permissionList[idx]);","               return p;","           }","           else","             return p;","       }"],"id":136}],[{"start":{"row":66,"column":8},"end":{"row":67,"column":0},"action":"insert","lines":["",""],"id":137},{"start":{"row":67,"column":0},"end":{"row":67,"column":7},"action":"insert","lines":["       "]}],[{"start":{"row":67,"column":7},"end":{"row":68,"column":0},"action":"insert","lines":["",""],"id":138},{"start":{"row":68,"column":0},"end":{"row":68,"column":7},"action":"insert","lines":["       "]}],[{"start":{"row":68,"column":7},"end":{"row":69,"column":0},"action":"insert","lines":["",""],"id":139},{"start":{"row":69,"column":0},"end":{"row":69,"column":7},"action":"insert","lines":["       "]}],[{"start":{"row":37,"column":9},"end":{"row":37,"column":10},"action":"remove","lines":[" "],"id":140}],[{"start":{"row":37,"column":8},"end":{"row":37,"column":9},"action":"remove","lines":[" "],"id":141}],[{"start":{"row":37,"column":13},"end":{"row":37,"column":14},"action":"insert","lines":["A"],"id":142}],[{"start":{"row":37,"column":14},"end":{"row":37,"column":15},"action":"insert","lines":["c"],"id":143}],[{"start":{"row":37,"column":15},"end":{"row":37,"column":16},"action":"insert","lines":["c"],"id":144}],[{"start":{"row":37,"column":13},"end":{"row":37,"column":16},"action":"remove","lines":["Acc"],"id":145},{"start":{"row":37,"column":13},"end":{"row":37,"column":29},"action":"insert","lines":["AccessController"]}],[{"start":{"row":37,"column":29},"end":{"row":37,"column":30},"action":"insert","lines":[":"],"id":146}],[{"start":{"row":37,"column":30},"end":{"row":37,"column":31},"action":"insert","lines":[":"],"id":147}],[{"start":{"row":46,"column":12},"end":{"row":46,"column":30},"action":"insert","lines":["AccessController::"],"id":148}],[{"start":{"row":53,"column":11},"end":{"row":53,"column":29},"action":"insert","lines":["AccessController::"],"id":149}],[{"start":{"row":56,"column":18},"end":{"row":56,"column":36},"action":"insert","lines":["AccessController::"],"id":150}],[{"start":{"row":0,"column":0},"end":{"row":7,"column":24},"action":"insert","lines":[" #include \"permission.h\"","    #include \"Globals.h\"","    #include \"../logservice/alog.h\"","    #include <stdio.h>","    #include <iostream>","    #include <sstream>","    #include <string>","    using namespace std;"],"id":151}],[{"start":{"row":0,"column":0},"end":{"row":0,"column":1},"action":"insert","lines":[" "],"id":152}],[{"start":{"row":0,"column":1},"end":{"row":0,"column":2},"action":"insert","lines":[" "],"id":153}],[{"start":{"row":0,"column":2},"end":{"row":0,"column":3},"action":"insert","lines":[" "],"id":154}],[{"start":{"row":41,"column":21},"end":{"row":41,"column":37},"action":"remove","lines":["gSvm.appmanager."],"id":155}],[{"start":{"row":54,"column":50},"end":{"row":55,"column":0},"action":"insert","lines":["",""],"id":157},{"start":{"row":55,"column":0},"end":{"row":55,"column":15},"action":"insert","lines":["               "]}],[{"start":{"row":55,"column":15},"end":{"row":55,"column":16},"action":"insert","lines":["c"],"id":158}],[{"start":{"row":55,"column":16},"end":{"row":55,"column":17},"action":"insert","lines":["o"],"id":159}],[{"start":{"row":55,"column":17},"end":{"row":55,"column":18},"action":"insert","lines":["y"],"id":160}],[{"start":{"row":55,"column":18},"end":{"row":55,"column":19},"action":"insert","lines":["t"],"id":161}],[{"start":{"row":55,"column":18},"end":{"row":55,"column":19},"action":"remove","lines":["t"],"id":162}],[{"start":{"row":55,"column":17},"end":{"row":55,"column":18},"action":"remove","lines":["y"],"id":163}],[{"start":{"row":55,"column":17},"end":{"row":55,"column":18},"action":"insert","lines":["u"],"id":164}],[{"start":{"row":55,"column":18},"end":{"row":55,"column":19},"action":"insert","lines":["t"],"id":165}],[{"start":{"row":55,"column":19},"end":{"row":55,"column":20},"action":"insert","lines":[" "],"id":166}],[{"start":{"row":55,"column":20},"end":{"row":55,"column":21},"action":"insert","lines":["<"],"id":167}],[{"start":{"row":55,"column":21},"end":{"row":55,"column":22},"action":"insert","lines":["<"],"id":168}],[{"start":{"row":55,"column":22},"end":{"row":55,"column":23},"action":"insert","lines":[" "],"id":169}],[{"start":{"row":55,"column":23},"end":{"row":55,"column":43},"action":"insert","lines":["perm.getPermission()"],"id":170}],[{"start":{"row":55,"column":43},"end":{"row":55,"column":44},"action":"insert","lines":[" "],"id":171}],[{"start":{"row":55,"column":44},"end":{"row":55,"column":45},"action":"insert","lines":["<"],"id":172}],[{"start":{"row":55,"column":45},"end":{"row":55,"column":46},"action":"insert","lines":["<"],"id":173}],[{"start":{"row":55,"column":46},"end":{"row":55,"column":47},"action":"insert","lines":[" "],"id":174}],[{"start":{"row":55,"column":47},"end":{"row":55,"column":49},"action":"insert","lines":["\"\""],"id":175}],[{"start":{"row":55,"column":48},"end":{"row":55,"column":49},"action":"insert","lines":[" "],"id":176}],[{"start":{"row":55,"column":49},"end":{"row":55,"column":50},"action":"insert","lines":[":"],"id":177}],[{"start":{"row":55,"column":50},"end":{"row":55,"column":51},"action":"insert","lines":[":"],"id":178}],[{"start":{"row":55,"column":51},"end":{"row":55,"column":52},"action":"insert","lines":[" "],"id":179}],[{"start":{"row":55,"column":53},"end":{"row":55,"column":54},"action":"insert","lines":[" "],"id":180}],[{"start":{"row":55,"column":54},"end":{"row":55,"column":71},"action":"insert","lines":["permissionList[i]"],"id":181}],[{"start":{"row":55,"column":53},"end":{"row":55,"column":54},"action":"insert","lines":[" "],"id":182}],[{"start":{"row":55,"column":54},"end":{"row":55,"column":55},"action":"insert","lines":["<"],"id":183}],[{"start":{"row":55,"column":55},"end":{"row":55,"column":56},"action":"insert","lines":["<"],"id":184}],[{"start":{"row":55,"column":74},"end":{"row":55,"column":75},"action":"insert","lines":[";"],"id":185}],[{"start":{"row":55,"column":74},"end":{"row":55,"column":75},"action":"insert","lines":[" "],"id":186}],[{"start":{"row":55,"column":75},"end":{"row":55,"column":76},"action":"insert","lines":["<"],"id":187}],[{"start":{"row":55,"column":76},"end":{"row":55,"column":77},"action":"insert","lines":["<"],"id":188}],[{"start":{"row":55,"column":77},"end":{"row":55,"column":78},"action":"insert","lines":[" "],"id":189}],[{"start":{"row":55,"column":78},"end":{"row":55,"column":79},"action":"insert","lines":["e"],"id":190}],[{"start":{"row":55,"column":79},"end":{"row":55,"column":80},"action":"insert","lines":["n"],"id":191}],[{"start":{"row":55,"column":80},"end":{"row":55,"column":81},"action":"insert","lines":["l"],"id":192}],[{"start":{"row":55,"column":80},"end":{"row":55,"column":81},"action":"insert","lines":["d"],"id":193}],[{"start":{"row":48,"column":41},"end":{"row":48,"column":49},"action":"remove","lines":["security"],"id":194}],[{"start":{"row":48,"column":40},"end":{"row":48,"column":41},"action":"remove","lines":["."],"id":195}],[{"start":{"row":49,"column":41},"end":{"row":49,"column":49},"action":"remove","lines":["security"],"id":196}],[{"start":{"row":49,"column":40},"end":{"row":49,"column":41},"action":"remove","lines":["."],"id":197}],[{"start":{"row":50,"column":41},"end":{"row":50,"column":49},"action":"remove","lines":["security"],"id":198}],[{"start":{"row":50,"column":40},"end":{"row":50,"column":41},"action":"remove","lines":["."],"id":199}],[{"start":{"row":51,"column":41},"end":{"row":51,"column":49},"action":"remove","lines":["security"],"id":200}],[{"start":{"row":51,"column":40},"end":{"row":51,"column":41},"action":"remove","lines":["."],"id":201}],[{"start":{"row":54,"column":50},"end":{"row":55,"column":83},"action":"remove","lines":["","               cout << perm.getPermission() << \" :: \" << permissionList[i] << endl;"],"id":202}]]},"ace":{"folds":[],"scrolltop":836.6667671203613,"scrollleft":0,"selection":{"start":{"row":78,"column":10},"end":{"row":78,"column":10},"isBackwards":false},"options":{"guessTabSize":true,"useWrapMode":false,"wrapToView":true},"firstLineState":{"row":39,"state":"start","mode":"ace/mode/c_cpp"}},"timestamp":1442978468476,"hash":"64c54325721ab7831c8da7673615ca30a27d3559"}