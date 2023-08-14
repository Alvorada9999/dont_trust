rm -R build
mkdir build
find . -type f -name \*.h -exec cp \{\} ./build/ \; 
find . -type f -name \*.c -exec cp \{\} ./build/ \; 
