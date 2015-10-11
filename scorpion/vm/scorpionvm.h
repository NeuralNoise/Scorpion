#ifndef SCORPION_VM
#define SCORPION_VM

 #include <string>
 #include "exception.h"
 #include "../vm/scorpion_env.h"
 #include "security.h"

 using namespace std;

  #define HEAP_STANDARD 1000000  // 1,000 kb
  #define STAT_REGS_NUM 10
  #define VM_FLAG_SIZE 6

  // register Identifiers
  #define VREG_PC 0     // ---------------- Static Regs --------------------
  #define VREG_SP 1   // stack pointer
  #define VREG_SRC 2
  #define VREG_EXC 3

  #define VFLAG_IGNORE 0 // ---------------- Static flags -------------------
  #define VFLAG_MTHDC  1 // method count (scope)
  #define VFLAG_IFC 2
  #define VFLAG_IF_IGNORE 3
  #define VFLAG_LBL_IGNORE 4
  #define VFLAG_NO 4 // do not run
  #define VFLAG_IF_DEPTH 5

  class ScorpionEnv;
  class XSO;

  // TODO: setup entire Virtual machine (static constants, helper functions, flags &status identifiers)
  class ScorpionVM {
      long SREG_SIZE;
         
      public:
         long *vStaticRegs;
         bool initializing;
         int status;
         int loop, func, _try;
         long *flags; // ignore, mthdc, if_block, if_ignore
         int setRegs(long sz){
             vStaticRegs = new (nothrow) long[sz];
             if(vStaticRegs == nullptr)
                return -1; 

             return 0;
         }
         long getSRegSize(){
            return SREG_SIZE;
         }
         // Make heap processor namespace
  };
  
  struct option {
    bool Assert;
    double  minHeap;
    double  maxHeap;
    bool Debug;
    string XSOF;
    string SARF;
    string *ags;
    int ags_t;
    bool out;
  };
  
  class ScorpionEnv;
  
  
  struct initdata;
  extern initdata idata;
  
  extern option options;
  extern void Init_CreateScorpionVM(ScorpionVM vm, ScorpionEnv* env, XSO* xso, const char** args, int ags_t);
  extern int Init_StartScorpionVM();
  extern void Init_ShutdownScorpionVM();

#endif // SCORPION_VM
