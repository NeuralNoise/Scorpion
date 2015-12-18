#ifndef SCORPION_VM
#define SCORPION_VM

 #include <string>
 #include "exception.h"
 #include "memory/vm_env.h"
 #include "../logservice/alog.h"
 #include "security.h"
 #include "../clib/arraylist.h"

 using namespace std;
 
  #define nullptr ((void *)0)

  #define HEAP_STANDARD 1000000  // 1,000 kb
  #define vm_status_normal 0
  #define vm_status_no_run 1
  #define vm_status_if_ignore 2
  
  #define SVM_OK 0
  
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
			    
		     int shutdown_response = SVM_OK;
             if(N==1)
             {       
                  if(vmstates->bytestream.size() != 0)
                    vmstates->bytestream.clear();
                  
                  if(vmstates->status != vm_status_normal)
                  {
					stringstream ss;
					ss << "The Scorpion virtual machine is attempting to shutdown with abnormal status code (" << vmstates->status << ").";
					alog.ALOGV(ss.str());
					shutdown_response = -1;
				  }
                  
                  free( vmstates );
                  return shutdown_response;
             }
             
             return shutdown_response;
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
  
  struct initdata;
  extern initdata idata;
  
  extern option options;
  extern void Init_CreateScorpionVM(ScorpionVmState* vm, ScorpionVM::memory::environment::scorpion_env* env, 
           XSO* xso, const char** args, int ags_t);
  extern int Init_StartScorpionVM();
  extern void Init_ShutdownScorpionVM();

#endif // SCORPION_VM
