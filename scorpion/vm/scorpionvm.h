#ifndef SCORPION_VM
#define SCORPION_VM

 #include <string>
 #include "exception.h"
 #include "scorpion_env.h"
 #include "security.h"
 #include "../clib/arraylist.h"

 using namespace std;
 
  #define nullptr ((void *)0)

  #define HEAP_STANDARD 1000000  // 1,000 kb
  #define vm_status_normal 0
  #define vm_status_no_run 1
  #define vm_status_if_ignore 2
  
  #define SVM_OK 0
  
  class ScorpionEnv;
  class XSO;
  
  class ScorpionVmState {
      
      public:
         unsigned long k;
         long i, m, src, methodcount, 
                  sp, ifcount, ifdepth, 
                  exc;
         bool initializing;
         int status;
         ListAdapter<double> bytestream; // holds the image bytes to be processed
         int DestroyScorpionVM(ScorpionVmState* vmstates, int N)
         {
			 if(vmstates == nullptr || N==0)
			    return -1;
			    
             if(N==1)
             {       
                  if(vmstates->bytestream.size() != 0)
                    vmstates->bytestream.clear();
                  
                  free( vmstates );
                  return SVM_OK;
             }
             
             return -1;
         }
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
    int llevel;
  };
  
  class ScorpionEnv;
  
  
  struct initdata;
  extern initdata idata;
  
  extern option options;
  extern void Init_CreateScorpionVM(ScorpionVmState* vm, ScorpionEnv* env, XSO* xso, const char** args, int ags_t);
  extern int Init_StartScorpionVM();
  extern void Init_ShutdownScorpionVM();

#endif // SCORPION_VM
