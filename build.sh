qmake untitled.pro -spec linux-g++ CONFIG+=qtquickcompiler
make -f Makefile qmake_all
bear make -j`nproc`
