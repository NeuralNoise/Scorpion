{"filter":false,"title":"Filter.c","tooltip":"/scorpion/logservice/Filter.c","undoManager":{"mark":84,"position":84,"stack":[[{"start":{"row":22,"column":20},"end":{"row":32,"column":1},"action":"remove","lines":["","","void SetPriority(int priority)","{","  if(priority <= 2)","    PRIORITY = 2;","  else if(priority >= 7)","    PRIORITY = 7;","  else","    PRIORITY = priority;","}"],"id":131}],[{"start":{"row":24,"column":93},"end":{"row":24,"column":96},"action":"insert","lines":[", p"],"id":132}],[{"start":{"row":24,"column":95},"end":{"row":24,"column":96},"action":"insert","lines":["i"],"id":133}],[{"start":{"row":24,"column":96},"end":{"row":24,"column":97},"action":"insert","lines":["n"],"id":134}],[{"start":{"row":24,"column":97},"end":{"row":24,"column":98},"action":"insert","lines":["t"],"id":135}],[{"start":{"row":24,"column":98},"end":{"row":24,"column":99},"action":"insert","lines":[" "],"id":136}],[{"start":{"row":28,"column":18},"end":{"row":33,"column":15},"action":"remove","lines":["","         if(PRIORITY == 1){","             cout << \"warning: err processing log, priority not set.\" << endl;","             return stackmsg;","\t     }","         else {"],"id":137}],[{"start":{"row":32,"column":31},"end":{"row":33,"column":10},"action":"remove","lines":["","         }"],"id":138}],[{"start":{"row":29,"column":10},"end":{"row":29,"column":11},"action":"remove","lines":[" "],"id":139}],[{"start":{"row":29,"column":9},"end":{"row":29,"column":10},"action":"remove","lines":[" "],"id":140}],[{"start":{"row":30,"column":14},"end":{"row":30,"column":15},"action":"remove","lines":[" "],"id":141}],[{"start":{"row":30,"column":13},"end":{"row":30,"column":14},"action":"remove","lines":[" "],"id":142}],[{"start":{"row":31,"column":10},"end":{"row":31,"column":11},"action":"remove","lines":[" "],"id":143}],[{"start":{"row":31,"column":9},"end":{"row":31,"column":10},"action":"remove","lines":[" "],"id":144}],[{"start":{"row":32,"column":13},"end":{"row":32,"column":14},"action":"remove","lines":[" "],"id":145}],[{"start":{"row":32,"column":12},"end":{"row":32,"column":13},"action":"remove","lines":[" "],"id":146}],[{"start":{"row":29,"column":24},"end":{"row":29,"column":32},"action":"remove","lines":["PRIORITY"],"id":147},{"start":{"row":29,"column":24},"end":{"row":29,"column":25},"action":"insert","lines":["p"]}],[{"start":{"row":29,"column":26},"end":{"row":29,"column":35},"action":"remove","lines":["|| System"],"id":148}],[{"start":{"row":29,"column":25},"end":{"row":29,"column":26},"action":"remove","lines":[" "],"id":149}],[{"start":{"row":21,"column":17},"end":{"row":22,"column":20},"action":"remove","lines":["","bool System = false;"],"id":150}],[{"start":{"row":20,"column":0},"end":{"row":21,"column":17},"action":"remove","lines":["","int PRIORITY = 1;"],"id":151}],[{"start":{"row":19,"column":20},"end":{"row":20,"column":0},"action":"remove","lines":["",""],"id":152}],[{"start":{"row":27,"column":64},"end":{"row":27,"column":65},"action":"insert","lines":[","],"id":153}],[{"start":{"row":27,"column":65},"end":{"row":27,"column":66},"action":"insert","lines":[" "],"id":154}],[{"start":{"row":27,"column":66},"end":{"row":27,"column":67},"action":"insert","lines":["p"],"id":155}],[{"start":{"row":27,"column":66},"end":{"row":27,"column":67},"action":"remove","lines":["p"],"id":156}],[{"start":{"row":27,"column":65},"end":{"row":27,"column":66},"action":"remove","lines":[" "],"id":157}],[{"start":{"row":27,"column":64},"end":{"row":27,"column":65},"action":"remove","lines":[","],"id":158}],[{"start":{"row":24,"column":16},"end":{"row":24,"column":17},"action":"remove","lines":["0"],"id":159}],[{"start":{"row":24,"column":16},"end":{"row":24,"column":17},"action":"insert","lines":["s"],"id":160}],[{"start":{"row":24,"column":17},"end":{"row":24,"column":18},"action":"insert","lines":["t"],"id":161}],[{"start":{"row":24,"column":16},"end":{"row":24,"column":18},"action":"remove","lines":["st"],"id":162},{"start":{"row":24,"column":16},"end":{"row":24,"column":24},"action":"insert","lines":["stackmsg"]}],[{"start":{"row":22,"column":1},"end":{"row":23,"column":0},"action":"insert","lines":["",""],"id":163},{"start":{"row":23,"column":0},"end":{"row":23,"column":4},"action":"insert","lines":["    "]}],[{"start":{"row":23,"column":4},"end":{"row":23,"column":5},"action":"insert","lines":["c"],"id":164}],[{"start":{"row":23,"column":5},"end":{"row":23,"column":6},"action":"insert","lines":["o"],"id":165}],[{"start":{"row":23,"column":6},"end":{"row":23,"column":7},"action":"insert","lines":["u"],"id":166}],[{"start":{"row":23,"column":7},"end":{"row":23,"column":8},"action":"insert","lines":["t"],"id":167}],[{"start":{"row":23,"column":8},"end":{"row":23,"column":9},"action":"insert","lines":[" "],"id":168}],[{"start":{"row":23,"column":9},"end":{"row":23,"column":10},"action":"insert","lines":["<"],"id":169}],[{"start":{"row":23,"column":10},"end":{"row":23,"column":11},"action":"insert","lines":["<"],"id":170}],[{"start":{"row":23,"column":11},"end":{"row":23,"column":12},"action":"insert","lines":[" "],"id":171}],[{"start":{"row":23,"column":12},"end":{"row":23,"column":14},"action":"insert","lines":["\"\""],"id":172}],[{"start":{"row":23,"column":12},"end":{"row":23,"column":14},"action":"remove","lines":["\"\""],"id":173}],[{"start":{"row":23,"column":12},"end":{"row":23,"column":13},"action":"insert","lines":["p"],"id":174}],[{"start":{"row":23,"column":13},"end":{"row":23,"column":14},"action":"insert","lines":["r"],"id":175}],[{"start":{"row":23,"column":14},"end":{"row":23,"column":15},"action":"insert","lines":["i"],"id":176}],[{"start":{"row":23,"column":15},"end":{"row":23,"column":16},"action":"insert","lines":["o"],"id":177}],[{"start":{"row":23,"column":16},"end":{"row":23,"column":17},"action":"insert","lines":["r"],"id":178}],[{"start":{"row":23,"column":12},"end":{"row":23,"column":17},"action":"remove","lines":["prior"],"id":179},{"start":{"row":23,"column":12},"end":{"row":23,"column":20},"action":"insert","lines":["priority"]}],[{"start":{"row":23,"column":20},"end":{"row":23,"column":21},"action":"insert","lines":[" "],"id":180}],[{"start":{"row":23,"column":21},"end":{"row":23,"column":22},"action":"insert","lines":["<"],"id":181}],[{"start":{"row":23,"column":22},"end":{"row":23,"column":23},"action":"insert","lines":["<"],"id":182}],[{"start":{"row":23,"column":23},"end":{"row":23,"column":24},"action":"insert","lines":[" "],"id":183}],[{"start":{"row":23,"column":24},"end":{"row":23,"column":26},"action":"insert","lines":["\"\""],"id":184}],[{"start":{"row":23,"column":25},"end":{"row":23,"column":26},"action":"insert","lines":[":"],"id":185}],[{"start":{"row":23,"column":27},"end":{"row":23,"column":28},"action":"insert","lines":[" "],"id":186}],[{"start":{"row":23,"column":28},"end":{"row":23,"column":29},"action":"insert","lines":["<"],"id":187}],[{"start":{"row":23,"column":29},"end":{"row":23,"column":30},"action":"insert","lines":["<"],"id":188}],[{"start":{"row":23,"column":30},"end":{"row":23,"column":31},"action":"insert","lines":[" "],"id":189}],[{"start":{"row":23,"column":31},"end":{"row":23,"column":32},"action":"insert","lines":["p"],"id":190}],[{"start":{"row":23,"column":32},"end":{"row":23,"column":33},"action":"insert","lines":[" "],"id":191}],[{"start":{"row":23,"column":33},"end":{"row":23,"column":34},"action":"insert","lines":["<"],"id":192}],[{"start":{"row":23,"column":34},"end":{"row":23,"column":35},"action":"insert","lines":["<"],"id":193}],[{"start":{"row":23,"column":35},"end":{"row":23,"column":36},"action":"insert","lines":[" "],"id":194}],[{"start":{"row":23,"column":36},"end":{"row":23,"column":38},"action":"insert","lines":["\"\""],"id":195}],[{"start":{"row":23,"column":37},"end":{"row":23,"column":38},"action":"insert","lines":[":"],"id":196}],[{"start":{"row":23,"column":39},"end":{"row":23,"column":40},"action":"insert","lines":[" "],"id":197}],[{"start":{"row":23,"column":40},"end":{"row":23,"column":41},"action":"insert","lines":["<"],"id":198}],[{"start":{"row":23,"column":41},"end":{"row":23,"column":42},"action":"insert","lines":["<"],"id":199}],[{"start":{"row":23,"column":42},"end":{"row":23,"column":43},"action":"insert","lines":[" "],"id":200}],[{"start":{"row":23,"column":43},"end":{"row":23,"column":44},"action":"insert","lines":["l"],"id":201}],[{"start":{"row":23,"column":44},"end":{"row":23,"column":45},"action":"insert","lines":["o"],"id":202}],[{"start":{"row":23,"column":45},"end":{"row":23,"column":46},"action":"insert","lines":["g"],"id":203}],[{"start":{"row":23,"column":46},"end":{"row":23,"column":47},"action":"insert","lines":[" "],"id":204}],[{"start":{"row":23,"column":47},"end":{"row":23,"column":48},"action":"insert","lines":["<"],"id":205}],[{"start":{"row":23,"column":48},"end":{"row":23,"column":49},"action":"insert","lines":["<"],"id":206}],[{"start":{"row":23,"column":49},"end":{"row":23,"column":50},"action":"insert","lines":[" "],"id":207}],[{"start":{"row":23,"column":50},"end":{"row":23,"column":51},"action":"insert","lines":["e"],"id":208}],[{"start":{"row":23,"column":51},"end":{"row":23,"column":52},"action":"insert","lines":["m"],"id":209}],[{"start":{"row":23,"column":51},"end":{"row":23,"column":52},"action":"remove","lines":["m"],"id":210}],[{"start":{"row":23,"column":51},"end":{"row":23,"column":52},"action":"insert","lines":["n"],"id":211}],[{"start":{"row":23,"column":52},"end":{"row":23,"column":53},"action":"insert","lines":["d"],"id":212}],[{"start":{"row":23,"column":53},"end":{"row":23,"column":54},"action":"insert","lines":["l"],"id":213}],[{"start":{"row":23,"column":54},"end":{"row":23,"column":55},"action":"insert","lines":[";"],"id":214}],[{"start":{"row":22,"column":1},"end":{"row":23,"column":55},"action":"remove","lines":["","    cout << priority << \":\" << p << \":\" << log << endl;"],"id":215}]]},"ace":{"folds":[],"scrolltop":242.22222863891994,"scrollleft":0,"selection":{"start":{"row":22,"column":1},"end":{"row":22,"column":1},"isBackwards":false},"options":{"guessTabSize":true,"useWrapMode":false,"wrapToView":true},"firstLineState":{"row":13,"state":"comment","mode":"ace/mode/c_cpp"}},"timestamp":1442174920943,"hash":"901e75cbf54ba7cbe3caf16c3d153525939030b9"}