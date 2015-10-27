// Example program
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "clib/filestream.h" 
#include <unistd.h>
#include <sstream>

using namespace std;

#define prodflav "SDK\r"

char getch(){
    system("stty raw");
    int i;
    i = getchar();
    system("stty cooked");
    
    return i;
}

struct info_cfg {
   string *products;
   int product_size_t;
   
   string productflav;
   float version;
   string company;
};

void splitproducts(string data, info_cfg &icfg)
{
    int count =0;
    stringstream str;
    icfg.product_size_t = 0;
    
    for(int i = 0; i < data.size(); i++){
        if(data.at(i) == ':')
           icfg.product_size_t++;
    }
    icfg.products = new string[++icfg.product_size_t];
    
    for(int i = 0; i < data.size(); i++){
        if(data.at(i) == ':' || !(i + 1 < data.size())){
            if(str.str() != ""){
                icfg.products[count++] = str.str();
                str.str("");
            }
        }
        else
          str << data.at(i);
    }
}

void parsecontent(string content, info_cfg &icfg){
    
    stringstream tag, value;
    for(int i = 0; i < content.size(); i++){
        tag.str("");
        value.str("");
        for(int i2 = i; i < content.size(); i++){
            if(content.at(i) == '=' || content.at(i) == '\n' || content.at(i) == '#')
              break;
            else
              tag << content.at(i);
        }
        
        if(content.at(i) == '\n')
              continue;
        else if(content.at(i) == '=')
              i++;
        else if(content.at(i) == '#'){
            for(int i2 = i; i < content.size(); i++){
                if(content.at(i) == '\n')
                   break;
            }
            continue;
        }
              
        for(int i2 = i; i < content.size(); i++){
            if(content.at(i) == '\n' || !(i + 1 < content.size()) || content.at(i) == '#'){
              if(content.at(i) == '#')
                 i--;
              break;
            }
            else
              value << content.at(i);
        }
        
        if(tag.str() == "products")
           splitproducts(value.str(), icfg);
        else if(tag.str() == "productFlavor")
           icfg.productflav = value.str();
        else if(tag.str() == "version")
            icfg.version = atof(value.str().c_str());
        else if(tag.str() == "company")
            icfg.company = value.str();
    }
}

int err(string folder){
    stringstream ss;
    ss.str("");
    ss << "rm -r /usr/share/scorpion/sdk/" << folder;
    system(ss.str().c_str());
    return 1;
}

/* UNIX install script */
int main()
{
    cout << "###################################\n";
    cout << "   Scorpion Install Script (UNIX)\n";
    cout << "###################################\n\n";
    cout << "Product installer: Scorpion Development Kit(SDK)\n";
    
    
    stringstream folder, sstream, dir;
    string package;
    int exit_val;
    char answer;
    
    cout << "> Type the linux distribution package you would like to install: ";
    cin >> package;
    
    if(FileStream::exists(package.c_str())){
        if(FileStream::endswith(".tar.gz", package)){
            
            cout << "> Warning: this will overide any currently installed Scorpion software. Ok to install(Y/n)? ";
            cin >>answer;
            
            if(answer != 'Y' && answer != 'y')
            {
                cout << "Exiting installer.\n";
                return 0;
            }
            
            cout << "> Extracting files...\n";
            
            for(int i = 0; i < package.size() - 7; i++)
                folder << package.at(i);
                
            sstream << "mkdir -p /usr/share/scorpion/sdk/" << folder.str();
            exit_val = system(sstream.str().c_str());
            if(exit_val != 0){
                cout << "> install:  error: could not create instillation path. Do you have root?\n";
                return 1;
            }
            
            sstream.str("");
            sstream << "tar -zxvf " << package << " -C /usr/share/scorpion/sdk/" << folder.str();
            
            exit_val = system(sstream.str().c_str());
            if(exit_val != 0){
                cout << "> install:  error: could not properly extract the file.\n";
                return err(folder.str());
            }
            
            cout << "> Setting up SDK directories...\n";
            system("mkdir -p /usr/share/scorpion/libs /usr/share/scorpion/vm/log");
            
            dir << "/usr/share/scorpion/sdk/" << folder.str();

            chdir(dir.str().c_str());
            
            if(!FileStream::exists("info.cfg")){
                cout << "install:  error: could not find 'info.cfg' file. Is this a Scorpion distribution package?\n";
                return err(folder.str());
            }
            
            info_cfg cfg, icfg;
            parsecontent(FileStream::getfile("info.cfg"), cfg);
           if(cfg.productflav.compare(prodflav) != 0){
                cout << " install:  error: the disribution package you are trying to install does not look like a SDK distribution, exiting.\n";
                return err(folder.str());
            }
            
            // TODO: rebuild .tar.gz file
            cout << "> Scanning for previously installed Scorpion software...\n";
            chdir("/usr/share/scorpion/");
            
            if(FileStream::exists("info.cfg")){
                parsecontent(FileStream::getfile("info.cfg"), icfg);
                
                if(cfg.version == icfg.version){
                    cout << "> It looks like you are trying to install an already existing version of Scorpion. Continue to install(Y/n)? ";
                    cin >>answer;
            
                    if(answer != 'Y' && answer != 'y')
                    {
                        cout << "Exiting installer.\n";
                        return (1 - err(folder.str()));
                    }
                }
                
                bool uninstall = false;
                chdir("/sbin/");
                for(int i = 0; i < icfg.product_size_t; i++){
                    if(FileStream::exists(icfg.products[i].c_str()))
                      uninstall = true;
                }
                chdir("/usr/share/scorpion/");
                
                if(uninstall)
                {
                   cout << "Currently installed Scorpion software was detected:\n";
                   cout << "productFlavor=" << icfg.productflav << endl;
                   cout << "version=" << icfg.version << endl;
                   cout << "owner=" << icfg.company << endl;
                   cout << "> Uninstalling...\n";
                   
                   system("rm info.cfg");
                   for(int i = 0; i < icfg.product_size_t; i++){
                      stringstream ss;
                      ss << "rm /sbin/" <<icfg.products[i];
                      if(FileStream::exists(icfg.products[i].c_str()))
                        system(ss.str().c_str());
                   }
                }
                else
                    cout << "install:  warning: a config file was found but no software was detected.\n";
            }
            else {
               bool uninstall = false;
                chdir("/sbin/");
                for(int i = 0; i < cfg.product_size_t; i++){
                    if(FileStream::exists(cfg.products[i].c_str()))
                      uninstall = true;
                }
                
                
                if(uninstall)
                {
                   cout << "install:  warning: a config file was not found but Scorpion software was detected.\n";
                   cout << "> Uninstalling...\n";
                   
                   for(int i = 0; i < cfg.product_size_t; i++){
                      stringstream ss;
                      ss << "rm " <<cfg.products[i];
                      if(FileStream::exists(cfg.products[i].c_str()))
                        system(ss.str().c_str());
                   }
                }
                else
                   cout << "install:  info: no software found.\n";
            }
            
            chdir(dir.str().c_str());
            cout << "> Extracting Binaries and libraries...\n";
            sstream.str("");
            sstream << "tar -zxvf " << package << " -C " << dir.str();
            
            exit_val = system(sstream.str().c_str());
            if(exit_val != 0){
                cout << "install:  error: could not properly extract the file.\n";
                return err(folder.str());
            }
            
            cout << "install:  info: check out examples/ for examples of the the Scorpion programming language.\n";
            
            // TODO: nano into the release notes finally
            
            cout << "> Installing Software...\n";
            chdir("bin/");
            
            // dynamically install all products
            bool err = false;
            for(int i = 0; i < cfg.product_size_t; i++){
                stringstream ss;
                ss << "chmod u+x " << cfg.products[i];
                if(system(ss.str().c_str()) != 0)
                   err = true;
            }
            
            if(err)
            {
                cout << "install:  err: the software was not installed properly, exiting.\n";
                return 1;
            }
            
            for(int i = 0; i < cfg.product_size_t; i++){
                stringstream ss;
                ss << "cp " << cfg.products[i] << " /sbin/";
                system(ss.str().c_str());
            }
            chdir("../");
            system("cp -a libs/. /usr/share/scorpion/libs/");
            system("cp info.cfg /usr/share/scorpion");
            
            cout << "> Scorpion was successfully installed!\n";
            cout << "productFlavor=" << cfg.productflav << endl;
            cout << "version=" << cfg.version << endl;
            cout << "owner=" << cfg.company << endl << endl;
            
            system("nano release.txt");
            
        }
        else
          cout << "install:  error: distribution package must be a '.tar.gz' file.\n";
    }
    else
      cout << "install:  error: '" << package << "' is no such file.\n";
    return 0;
}

