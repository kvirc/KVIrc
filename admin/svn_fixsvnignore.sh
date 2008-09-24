export BASEDIR=$(pwd)

for a in $(find ./ -name ".svnignore"); do
	DADIR=$(echo $a | sed -e 's/[a-z\.-]*[^/]$//g')
	echo $BASEDIR/$DADIR
	cd $BASEDIR/$DADIR
	svn propset svn:ignore -F .svnignore ./
done


