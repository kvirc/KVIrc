# bash script by roboirc to compile kvirc

cmake -DCMAKE_INSTALL_PREFIX=./ -DLIB_SUFFIX=./
make
make install


#sudo echo "/usr/local/lib" >> /etc/ld.so.conf
#ldconfig
