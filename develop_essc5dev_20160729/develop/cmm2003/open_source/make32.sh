. set32.x

export OSName=`uname`

# ±‡“Îlibxml2
gzip -d libxml2-2.8.0.tar.gz
tar xvf libxml2-2.8.0.tar
gzip  libxml2-2.8.0.tar
cd libxml2-2.8.0/
if [ "$OSName" = "AIX" ] ; then
	export OBJECT_MODE=32 && CC="cc -q32" ./configure -prefix=$CMM2003OPENDIR
else
	CC="gcc -m32" ./configure -prefix=$CMM2003OPENDIR
fi
make
make install
cd ../
rm -rf libxml2-2.8.0/

# ±‡“Îlibcurl
gzip -d curl-7.25.0.tar.gz
tar xvf curl-7.25.0.tar
gzip  curl-7.25.0.tar
cd curl-7.25.0/
if [ "$OSName" = "AIX" ] ; then
	export OBJECT_MODE=32 && CC="cc -q32" ./configure --prefix=$CMM2003OPENDIR --without-ssl --disable-shared
else
	CC="gcc -m32" ./configure -prefix=$CMM2003OPENDIR
fi
make
make install
cd ../
rm -rf curl-7.25.0/

# ±‡“Îopenssl
gzip -d openssl-0.9.8r.tar.gz
tar xvf openssl-0.9.8r.tar
gzip openssl-0.9.8r.tar
cd openssl-0.9.8r
if [ "$OSName" = "AIX" ] ; then
	./Configure aix-cc --prefix=$CMM2003OPENDIR
else
	CC="gcc -m32" MACHINE=i686 ./config --prefix=$CMM2003OPENDIR shared zlib-dynamic enable-camellia
fi
make
make install
cd ../
rm -rf openssl-0.9.8r

# ±‡“Îlibiconv
gzip -d libiconv-1.14.tar.gz
tar xvf libiconv-1.14.tar
gzip libiconv-1.14.tar
cd libiconv-1.14
if [ "$OSName" = "AIX" ] ; then
	export OBJECT_MODE=32 && CC="cc -q32" ./configure -prefix=$CMM2003OPENDIR  --enable-static=yes
else
	CC="gcc -m32" ./configure -prefix=$CMM2003OPENDIR  --enable-static=yes
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
	export OBJECT_MODE=32 && CC="cc -q32" ./configure -prefix=$CMM2003OPENDIR
else
	CC="gcc -m32" ./configure -prefix=$CMM2003OPENDIR
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
	export OBJECT_MODE=32 && CC="cc -q32" ./configure -prefix=$CMM2003OPENDIR --with-libevent=$CMM2003OPENDIR
else
	CC="gcc -m32" ./configure -prefix=$CMM2003OPENDIR --with-libevent=$CMM2003OPENDIR
fi
make
make install
cd ../
rm -rf memcached-1.4.14

# ±‡“Îlibmemcached
gzip -d libmemcached-0.53.tar.gz
tar xvf libmemcached-0.53.tar
gzip libmemcached-0.53.tar
cd libmemcached-0.53
if [ "$OSName" = "AIX" ] ; then
	export OBJECT_MODE=32 && CC="cc -q32" && CXX="cc -q32";./configure -prefix=$CMM2003OPENDIR
else
	CC="gcc -m32" && CXX="g++ -m32" ./configure -prefix=$CMM2003OPENDIR
fi
make
make install
cd ../
rm -rf libmemcached-0.53


gzip -d zlib-1.2.8.tar.gz
tar xvf zlib-1.2.8.tar
gzip zlib-1.2.8.tar
cd zlib-1.2.8
if [ "$OSName" = "AIX" ] ; then
        export OBJECT_MODE=32 && CC="cc -q32" && CXX="cc -q32" && CFLAGS="-fPIC";./configure -prefix=$CMM2003OPENDIR
else 
        CFLAGS="-fPIC -m32" ./configure --prefix=$CMM2003OPENDIR
fi
make && make install
rm -rf zlib-1.2.8
