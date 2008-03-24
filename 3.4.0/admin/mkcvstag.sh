OLDTAG=`cat admin/src_cvstag`

echo "Old CVSTAG is $OLDTAG"

if test -f configure.in; then
	echo "Requesting cvs log in the current directory"
else
	echo "This script must be called from the top source directory"
	exit -1
fi

cvs log > cvs.log

CVSTAG=`cat cvs.log | grep revision | grep -v total | wc -l`

echo $CVSTAG > admin/src_cvstag


echo "CVSTAG is $CVSTAG"