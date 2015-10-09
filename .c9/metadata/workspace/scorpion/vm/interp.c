{"filter":false,"title":"interp.c","tooltip":"/scorpion/vm/interp.c","undoManager":{"mark":100,"position":100,"stack":[[{"start":{"row":263,"column":18},"end":{"row":263,"column":19},"action":"remove","lines":["m"],"id":4314}],[{"start":{"row":263,"column":17},"end":{"row":263,"column":18},"action":"remove","lines":["v"],"id":4315}],[{"start":{"row":263,"column":16},"end":{"row":263,"column":17},"action":"remove","lines":["S"],"id":4316}],[{"start":{"row":263,"column":15},"end":{"row":263,"column":16},"action":"remove","lines":["g"],"id":4317}],[{"start":{"row":263,"column":70},"end":{"row":263,"column":98},"action":"insert","lines":["gSvm.vm.vStaticRegs[VREG_SP]"],"id":4318}],[{"start":{"row":263,"column":98},"end":{"row":263,"column":99},"action":"insert","lines":[","],"id":4319}],[{"start":{"row":263,"column":99},"end":{"row":263,"column":100},"action":"insert","lines":[" "],"id":4320}],[{"start":{"row":263,"column":115},"end":{"row":263,"column":116},"action":"insert","lines":[" "],"id":4321}],[{"start":{"row":263,"column":116},"end":{"row":263,"column":118},"action":"insert","lines":["\"\""],"id":4322}],[{"start":{"row":263,"column":115},"end":{"row":263,"column":116},"action":"insert","lines":[","],"id":4323}],[{"start":{"row":263,"column":100},"end":{"row":263,"column":115},"action":"remove","lines":["arguments.byte1"],"id":4324},{"start":{"row":263,"column":100},"end":{"row":263,"column":188},"action":"insert","lines":["svmBlockFromAddr(gSvm.env->getBlockTable(), STACK_BLOCK, gSvm.vm.vStaticRegs[VREG_SP]--)"]}],[{"start":{"row":263,"column":186},"end":{"row":263,"column":187},"action":"remove","lines":["-"],"id":4325}],[{"start":{"row":263,"column":185},"end":{"row":263,"column":186},"action":"remove","lines":["-"],"id":4326}],[{"start":{"row":263,"column":100},"end":{"row":264,"column":0},"action":"insert","lines":["",""],"id":4327},{"start":{"row":264,"column":0},"end":{"row":264,"column":15},"action":"insert","lines":["               "]}],[{"start":{"row":264,"column":15},"end":{"row":264,"column":16},"action":"insert","lines":[" "],"id":4328}],[{"start":{"row":264,"column":16},"end":{"row":264,"column":17},"action":"insert","lines":[" "],"id":4329}],[{"start":{"row":264,"column":17},"end":{"row":264,"column":18},"action":"insert","lines":[" "],"id":4330}],[{"start":{"row":264,"column":18},"end":{"row":264,"column":19},"action":"insert","lines":[" "],"id":4331}],[{"start":{"row":264,"column":19},"end":{"row":264,"column":20},"action":"insert","lines":[" "],"id":4332}],[{"start":{"row":262,"column":12},"end":{"row":262,"column":13},"action":"insert","lines":["/"],"id":4333}],[{"start":{"row":262,"column":13},"end":{"row":262,"column":14},"action":"insert","lines":["/"],"id":4334}],[{"start":{"row":263,"column":23},"end":{"row":263,"column":24},"action":"remove","lines":["t"],"id":4335}],[{"start":{"row":263,"column":23},"end":{"row":263,"column":24},"action":"insert","lines":["T"],"id":4336}],[{"start":{"row":264,"column":64},"end":{"row":264,"column":75},"action":"remove","lines":["STACK_BLOCK"],"id":4337},{"start":{"row":264,"column":64},"end":{"row":264,"column":65},"action":"insert","lines":["d"]}],[{"start":{"row":264,"column":65},"end":{"row":264,"column":66},"action":"insert","lines":["a"],"id":4338}],[{"start":{"row":264,"column":65},"end":{"row":264,"column":66},"action":"remove","lines":["a"],"id":4339}],[{"start":{"row":264,"column":64},"end":{"row":264,"column":65},"action":"remove","lines":["d"],"id":4340}],[{"start":{"row":264,"column":63},"end":{"row":264,"column":64},"action":"remove","lines":[" "],"id":4341}],[{"start":{"row":264,"column":63},"end":{"row":264,"column":64},"action":"insert","lines":[" "],"id":4342}],[{"start":{"row":264,"column":64},"end":{"row":264,"column":65},"action":"insert","lines":["D"],"id":4343}],[{"start":{"row":264,"column":65},"end":{"row":264,"column":66},"action":"insert","lines":["A"],"id":4344}],[{"start":{"row":264,"column":64},"end":{"row":264,"column":66},"action":"remove","lines":["DA"],"id":4345},{"start":{"row":264,"column":64},"end":{"row":264,"column":74},"action":"insert","lines":["DATA_BLOCK"]}],[{"start":{"row":264,"column":76},"end":{"row":264,"column":104},"action":"remove","lines":["gSvm.vm.vStaticRegs[VREG_SP]"],"id":4346},{"start":{"row":264,"column":76},"end":{"row":264,"column":77},"action":"insert","lines":["a"]}],[{"start":{"row":264,"column":77},"end":{"row":264,"column":78},"action":"insert","lines":["r"],"id":4347}],[{"start":{"row":264,"column":76},"end":{"row":264,"column":78},"action":"remove","lines":["ar"],"id":4348},{"start":{"row":264,"column":76},"end":{"row":264,"column":85},"action":"insert","lines":["arguments"]}],[{"start":{"row":264,"column":85},"end":{"row":264,"column":86},"action":"insert","lines":["."],"id":4349}],[{"start":{"row":264,"column":86},"end":{"row":264,"column":87},"action":"insert","lines":["b"],"id":4350}],[{"start":{"row":264,"column":87},"end":{"row":264,"column":88},"action":"insert","lines":["y"],"id":4351}],[{"start":{"row":264,"column":86},"end":{"row":264,"column":88},"action":"remove","lines":["by"],"id":4352},{"start":{"row":264,"column":86},"end":{"row":264,"column":91},"action":"insert","lines":["byte1"]}],[{"start":{"row":198,"column":0},"end":{"row":199,"column":22},"action":"remove","lines":["","    cout << i << endl;"],"id":4353}],[{"start":{"row":263,"column":20},"end":{"row":263,"column":21},"action":"insert","lines":[" "],"id":4354}],[{"start":{"row":263,"column":21},"end":{"row":263,"column":22},"action":"insert","lines":[" "],"id":4355}],[{"start":{"row":187,"column":37},"end":{"row":192,"column":34},"action":"remove","lines":["","  gSvm.vm.vStaticRegs[VREG_SP] = -1; // we start the stack ptr below the stack","  gSvm.vm.flags[VFLAG_MTHDC] = 0;","  gSvm.vm.flags[VFLAG_IFC] = 0;","  gSvm.vm.flags[VFLAG_IF_IGNORE] = 0;","  gSvm.vm.flags[VFLAG_IGNORE] = 0;"],"id":4356}],[{"start":{"row":106,"column":2},"end":{"row":106,"column":3},"action":"insert","lines":["_"],"id":4357}],[{"start":{"row":106,"column":3},"end":{"row":106,"column":4},"action":"insert","lines":["d"],"id":4358}],[{"start":{"row":331,"column":58},"end":{"row":331,"column":59},"action":"insert","lines":[" "],"id":4359}],[{"start":{"row":331,"column":58},"end":{"row":331,"column":59},"action":"remove","lines":[" "],"id":4360}],[{"start":{"row":331,"column":43},"end":{"row":331,"column":44},"action":"insert","lines":["("],"id":4361}],[{"start":{"row":331,"column":44},"end":{"row":331,"column":45},"action":"insert","lines":["l"],"id":4362}],[{"start":{"row":331,"column":45},"end":{"row":331,"column":46},"action":"insert","lines":["o"],"id":4363}],[{"start":{"row":331,"column":46},"end":{"row":331,"column":47},"action":"insert","lines":["n"],"id":4364}],[{"start":{"row":331,"column":47},"end":{"row":331,"column":48},"action":"insert","lines":["g"],"id":4365}],[{"start":{"row":331,"column":48},"end":{"row":331,"column":49},"action":"insert","lines":[")"],"id":4366}],[{"start":{"row":331,"column":49},"end":{"row":331,"column":50},"action":"insert","lines":[" "],"id":4367}],[{"start":{"row":338,"column":43},"end":{"row":338,"column":44},"action":"insert","lines":["("],"id":4368}],[{"start":{"row":338,"column":44},"end":{"row":338,"column":45},"action":"insert","lines":["l"],"id":4369}],[{"start":{"row":338,"column":45},"end":{"row":338,"column":46},"action":"insert","lines":["o"],"id":4370}],[{"start":{"row":338,"column":46},"end":{"row":338,"column":47},"action":"insert","lines":["n"],"id":4371}],[{"start":{"row":338,"column":47},"end":{"row":338,"column":48},"action":"insert","lines":["g"],"id":4372}],[{"start":{"row":338,"column":48},"end":{"row":338,"column":49},"action":"insert","lines":[")"],"id":4373}],[{"start":{"row":338,"column":49},"end":{"row":338,"column":50},"action":"insert","lines":[" "],"id":4374}],[{"start":{"row":250,"column":13},"end":{"row":251,"column":0},"action":"insert","lines":["",""],"id":4375},{"start":{"row":251,"column":0},"end":{"row":251,"column":16},"action":"insert","lines":["                "]}],[{"start":{"row":251,"column":16},"end":{"row":251,"column":17},"action":"insert","lines":["c"],"id":4376}],[{"start":{"row":251,"column":17},"end":{"row":251,"column":18},"action":"insert","lines":["o"],"id":4377}],[{"start":{"row":251,"column":18},"end":{"row":251,"column":19},"action":"insert","lines":["u"],"id":4378}],[{"start":{"row":251,"column":19},"end":{"row":251,"column":20},"action":"insert","lines":["t"],"id":4379}],[{"start":{"row":251,"column":20},"end":{"row":251,"column":21},"action":"insert","lines":[" "],"id":4380}],[{"start":{"row":251,"column":21},"end":{"row":251,"column":22},"action":"insert","lines":["<"],"id":4381}],[{"start":{"row":251,"column":22},"end":{"row":251,"column":23},"action":"insert","lines":["<"],"id":4382}],[{"start":{"row":251,"column":23},"end":{"row":251,"column":24},"action":"insert","lines":[" "],"id":4383}],[{"start":{"row":251,"column":24},"end":{"row":251,"column":25},"action":"insert","lines":[":"],"id":4384}],[{"start":{"row":251,"column":24},"end":{"row":251,"column":25},"action":"remove","lines":[":"],"id":4385}],[{"start":{"row":251,"column":24},"end":{"row":251,"column":26},"action":"insert","lines":["\"\""],"id":4386}],[{"start":{"row":251,"column":25},"end":{"row":251,"column":26},"action":"insert","lines":["p"],"id":4387}],[{"start":{"row":251,"column":26},"end":{"row":251,"column":27},"action":"insert","lines":["u"],"id":4388}],[{"start":{"row":251,"column":27},"end":{"row":251,"column":28},"action":"insert","lines":["s"],"id":4389}],[{"start":{"row":251,"column":28},"end":{"row":251,"column":29},"action":"insert","lines":["h"],"id":4390}],[{"start":{"row":251,"column":29},"end":{"row":251,"column":30},"action":"insert","lines":["i"],"id":4391}],[{"start":{"row":251,"column":30},"end":{"row":251,"column":31},"action":"insert","lines":["n"],"id":4392}],[{"start":{"row":251,"column":31},"end":{"row":251,"column":32},"action":"insert","lines":["g"],"id":4393}],[{"start":{"row":251,"column":32},"end":{"row":251,"column":33},"action":"insert","lines":[" "],"id":4394}],[{"start":{"row":251,"column":34},"end":{"row":251,"column":35},"action":"insert","lines":[" "],"id":4395}],[{"start":{"row":251,"column":35},"end":{"row":251,"column":36},"action":"insert","lines":["<"],"id":4396}],[{"start":{"row":251,"column":36},"end":{"row":251,"column":37},"action":"insert","lines":["<"],"id":4397}],[{"start":{"row":251,"column":37},"end":{"row":251,"column":38},"action":"insert","lines":[" "],"id":4398}],[{"start":{"row":250,"column":13},"end":{"row":251,"column":38},"action":"remove","lines":["","                cout << \"pushing \" << "],"id":4399}],[{"start":{"row":255,"column":15},"end":{"row":256,"column":38},"action":"insert","lines":["","                cout << \"pushing \" << "],"id":4400}],[{"start":{"row":256,"column":38},"end":{"row":256,"column":110},"action":"insert","lines":["svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1)"],"id":4401}],[{"start":{"row":256,"column":110},"end":{"row":256,"column":111},"action":"insert","lines":[";"],"id":4402}],[{"start":{"row":255,"column":15},"end":{"row":256,"column":111},"action":"remove","lines":["","                cout << \"pushing \" << svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1);"],"id":4403}],[{"start":{"row":113,"column":16},"end":{"row":113,"column":20},"action":"remove","lines":["long"],"id":4404},{"start":{"row":113,"column":16},"end":{"row":113,"column":19},"action":"insert","lines":["int"]}],[{"start":{"row":115,"column":16},"end":{"row":115,"column":20},"action":"remove","lines":["long"],"id":4405},{"start":{"row":115,"column":16},"end":{"row":115,"column":19},"action":"insert","lines":["int"]}],[{"start":{"row":117,"column":16},"end":{"row":117,"column":20},"action":"remove","lines":["long"],"id":4406},{"start":{"row":117,"column":16},"end":{"row":117,"column":19},"action":"insert","lines":["int"]}],[{"start":{"row":119,"column":16},"end":{"row":119,"column":20},"action":"remove","lines":["long"],"id":4407},{"start":{"row":119,"column":16},"end":{"row":119,"column":19},"action":"insert","lines":["int"]}],[{"start":{"row":121,"column":16},"end":{"row":121,"column":20},"action":"remove","lines":["long"],"id":4408},{"start":{"row":121,"column":16},"end":{"row":121,"column":19},"action":"insert","lines":["int"]}],[{"start":{"row":123,"column":16},"end":{"row":123,"column":20},"action":"remove","lines":["long"],"id":4409},{"start":{"row":123,"column":16},"end":{"row":123,"column":19},"action":"insert","lines":["int"]}],[{"start":{"row":125,"column":16},"end":{"row":125,"column":20},"action":"remove","lines":["long"],"id":4410},{"start":{"row":125,"column":16},"end":{"row":125,"column":19},"action":"insert","lines":["int"]}],[{"start":{"row":127,"column":16},"end":{"row":127,"column":20},"action":"remove","lines":["long"],"id":4411},{"start":{"row":127,"column":16},"end":{"row":127,"column":19},"action":"insert","lines":["int"]}],[{"start":{"row":129,"column":16},"end":{"row":129,"column":20},"action":"remove","lines":["long"],"id":4412},{"start":{"row":129,"column":16},"end":{"row":129,"column":19},"action":"insert","lines":["int"]}],[{"start":{"row":131,"column":16},"end":{"row":131,"column":20},"action":"remove","lines":["long"],"id":4413},{"start":{"row":131,"column":16},"end":{"row":131,"column":19},"action":"insert","lines":["int"]}],[{"start":{"row":255,"column":15},"end":{"row":256,"column":66},"action":"remove","lines":["","            //   cout << \"adding \" << arguments.byte1 << \"to \" << "],"id":4426}]]},"ace":{"folds":[],"scrolltop":3911.111214720175,"scrollleft":0,"selection":{"start":{"row":255,"column":15},"end":{"row":255,"column":15},"isBackwards":false},"options":{"guessTabSize":true,"useWrapMode":false,"wrapToView":true},"firstLineState":{"row":41,"state":"start","mode":"ace/mode/c_cpp"}},"timestamp":1444336195044,"hash":"8f40b1ef8360ae186ff2f28ac2d42296c34d9141"}