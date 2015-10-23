 #include "../zlib.h"
 #include "../zdecompress.h"
 #include "../zcompress.h"
 #include "../zinclude.h"

 Zip zip;

// TODO: Intergrate zlib library with class constr methods
int main(){
    zip.Compress("Test.txt", "Output.zip");
}



