 #include "../zlib.h"
 #include <sstream>
 #include <iostream>
 
 using namespace std;

 Zlib zlib;

// TODO: Add terminal color lib to clib/
int main(){
 
    Zlib::AUTO_CLEAN=(true);
    zlib.Compress("Main.c", "Main.zb");
    zlib.Decompress("file", "file.txt");
    zlib.Compress_Buffer2File("Thiiiiiis isssss a teeeeesssssst messageeeeeeeee", "test.zb");
    
    stringstream __outbuf__;
    zlib.Compress_Buffer2Buffer("Thiiiiiis isssss a teeeeesssssst messageeeeeeeee", __outbuf__, ZLIB_LAST_SEGMENT);
    
    stringstream __dec_buf__;
    zlib.Decompress_File2Buffer("test.zb", __dec_buf__);
   // cout << "file: test.zb \n\n" << __dec_buf__.str();
   
    stringstream buf;
    zlib.Decompress_Buffer2Buffer(__outbuf__.str(), buf);
    cout << "decompressd data \n\n" << __dec_buf__.str();
}



