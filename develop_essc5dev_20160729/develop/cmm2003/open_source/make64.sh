. set64.x

export OSName=`uname`

# ±‡“Îlibxml2
gzip -d libxml2-2.9.1.tar.gz
tar xvf libxml2-2.9.1.tar
gzip  libxml2-2.9.1.tar
cd libxml2-2.9.1/
if [ "$OSName" = "AIX" ] ; then
	export OBJECT_MODE=64 && CC="cc -q64" ./configure -prefix=$CMM2003OPENDIR
else
	CC="gcc -m64" ./configure -prefix=$CMM2003OPENDIR
fi
make
make install
cd ../
rm -rf libxml2-2.9.1/

# ±‡“Îlibcurl
gzip -d curl-7.25.0.tar.gz
tar xvf curl-7.25.0.tar
gzip  curl-7.25.0.tar
cd curl-7.25.0/
if [ "$OSName" = "AIX" ] ; then
	export OBJECT_MODE=64 && CC="cc -q64" ./configure --prefix=$CMM2003OPENDIR --without-ssl --disable-shared
else
	CC="gcc -m64" ./configure -prefix=$CMM2003OPENDIR
fi
make
make install
cd ../
rm -rf curl-7.25.0/

# ±‡“Îopenssl
gzip -d openssl-1.0.1g.tar.gz
tar xvf openssl-1.0.1g.tar
gzip openssl-1.0.1g.tar
cd openssl-1.0.1g
if [ "$OSName" = "AIX" ] ; then
	./Configure aix64-cc --prefix=$CMM2003OPENDIR
else
	./config --prefix=$CMM2003OPENDIR shared zlib-dynamic enable-camellia
fi
make
make install
cd ../
rm -rf openssl-1.0.1g

# ±‡“Îlibiconv
gzip -d libiconv-1.14.tar.gz
tar xvf libiconv-1.14.tar
gzip libiconv-1.14.tar
cd libiconv-1.14
if [ "$OSName" = "AIX" ] ; then
	export OBJECT_MODE=64 && CC="cc -q64" ./configure -prefix=$CMM2003OPENDIR  --enable-static=yes
else
	CC="gcc -m64" ./configure -prefix=$CMM2003OPENDIR --enable-static=yes
fi
make
make install
cd ../
rm -rf libiconv-1.14

# ±‡“Îlibevent
gzip -d libevent-2.0.20-stable.tar.gz
tar xvf libevent-2.0.20-stable.tar
gzip libevent-2.0.20-stable.tar
cd libevent-2.0.20-stable
if [ "$OSName" = "AIX" ] ; then
	export OBJECT_MODE=64 && CC="cc -q64" ./configure -prefix=$CMM2003OPENDIR
else
	CC="gcc -m64" ./configure -prefix=$CMM2003OPENDIR
fi
make
make install
cd ../
rm -rf libevent-2.0.20-stable

# ±‡“Îmemcached
gzip -d memcached-1.4.14.tar.gz
tar xvf memcached-1.4.14.tar
gzip memcached-1.4.14.tar
cd memcached-1.4.14
if [ "$OSName" = "AIX" ] ; then
	export OBJECT_MODE=64 && CC="cc -q64" ./configure -prefix=$CMM2003OPENDIR --with-libevent=$CMM2003OPENDIR
else
	CC="gcc -m64" ./configure -prefix=$CMM2003OPENDIR --with-libevent=$CMM2003OPENDIR
fi
make
make install
cd ../
rm -rf memcached-1.4.14

# ±‡“Îlibmemcached
gzip -d libmemcached-1.0.17.tar.gz
tar xvf libmemcached-1.0.17.tar
gzip libmemcached-1.0.17.tar
cd libmemcached-1.0.17
if [ "$OSName" = "AIX" ] ; then
	export OBJECT_MODE=64 && CC="cc -q64" && CXX="cc -q64";./configure -prefix=$CMM2003OPENDIR
else
	CC="gcc -m64" && CXX="g++ -m64" ./configure -prefix=$CMM2003OPENDIR
fi
make
make install
cd ../
rm -rf libmemcached-1.0.17

#gzip -d zlib-1.2.8.tar.gz  
#tar xvf zlib-1.2.8.tar
#gzip zlib-1.2.8.tar
#cd zlib-1.2.8
#if [ "$OSName" = "AIX" ] ; then
#	export OBJECT_MODE=64 && CC="cc -q64" && CXX="cc -q64" && CFLAGS="-fPIC";./configure -prefix=$CMM2003OPENDIR
#else
#	CFLAGS="-fPIC -m64" ./configure --prefix=$CMM2003OPENDIR 
#fi
#make && make install
#rm -rf zlib-1.2.8
