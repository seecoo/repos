filename=""
dirname=""
work_dir=`pwd`
#. $HOME/envFor64Bits/profileOf64Bits
. $HOME/develop/ktools/profileOf64Bits
for fullname in `find . -name "*.mk"|grep -v makeBins|grep -v test|grep -v tmp`
do
	if echo $fullname |grep -q ProductHouseWare ; then
		continue
	fi
        filename=`basename $fullname`
        dirname=`dirname $fullname`
        cd $dirname
        make -f $filename
        cd $work_dir
done

for fullname in `find . -name makefile|grep -v makeBins`
do
	if echo $fullname |grep -q ProductHouseWare ; then
		continue
	fi
        filename=`basename $fullname`
        dirname=`dirname $fullname`
        cd $dirname
        make
        cd $work_dir
done
#
