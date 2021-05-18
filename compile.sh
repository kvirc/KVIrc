# bash script by roboirc to compile kvirc

echo compiling...

cmake -DCMAKE_INSTALL_PREFIX=./ -DLIB_SUFFIX=./
make
make install

echo executing...
./src/kvirc/kvirc

#sudo echo "/usr/local/lib" >> /etc/ld.so.conf
#ldconfig
