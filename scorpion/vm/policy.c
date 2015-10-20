  #include "permission.h"
  #include "Globals.h"
  #include "../logservice/alog.h"
  #include "policy.h"
  #include <stdio.h>
  #include <iostream>
  #include <sstream>
  #include <string>
  using namespace std;

       bool Policy::policyRequested(string policy){
           for(int i = 0; i < psize; i++){
               if(policies[i] == policy)
                   return true;
           }
           return false;
       }
       void Policy::preparePolicy(){
           Exception::trace.addproto("vm.internal.system.preparePolicyEnforcment", "Policy", 1);
           psize = gSvm.appmanager.controller.getPermissionSize();
           policies = new string[psize];
           pcount = 0;
       }
       void Policy::usingPolicy(string policy){
           if(gSvm.appolicy.policyRequested(policy))
              return;
              
          Exception::trace.addproto("vm.internal.system.usingPolicy", "Policy", 1);
           
           Permission newPolicy;
           newPolicy.setPermission(policy);
           //cout << gSvm.appmanager.controller.valueAt(0).getPermission() << endl;
           
           if(!gSvm.appmanager.checkPermission(newPolicy)){
              stringstream ss;
              ss << "the policy " << policy << " could not be found.";
              Exception(ss.str(), "PolicyNotFoundException");
           }
           
           if(!gSvm.appmanager.hasPermission(newPolicy)){
              stringstream ss;
              ss << "Application requires permission " << policy << ". Have you declared this in your Dev script?";
              Exception(ss.str(), "SecurityException");
           }
           
           policies[pcount++] = policy;
       }
