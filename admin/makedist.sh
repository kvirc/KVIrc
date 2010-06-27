#/bin/bash

function usage()
{
	echo "Usage:"
	echo "      $0 [options] <source_tree_dir> <kvirc_version>"
	echo "Available options:"
	echo "   --gzip"
	echo "     Use gzip instead of bzip2"
	echo "   --md5sum"
	echo "     Create md5sum of the package too"
	echo "   --help"
	echo "     Show this help"
}


SOURCETREEDIR=""
KVIRCVERSION=""
COMPRESSSWITCH="j"
OUTPUTEXTENSION="bz2"
DOMD5SUM="no"

for i in $*; do
	case $i in
		--gzip)
				COMPRESSSWITCH="z"
				OUTPUTEXTENSION="gz"
			;;
		--help)
				usage
				exit
			;;
		--md5sum)
				DOMD5SUM="yes"
			;;
		*)
			if test -z "$SOURCETREEDIR"; then
				SOURCETREEDIR=$i
			else
				KVIRCVERSION=$i
			fi
			;;
	esac
done

if [ -z "$SOURCETREEDIR" ]; then
	echo "ERROR: Source tree directory missing"
	usage
	exit
fi

if [ -z "$KVIRCVERSION" ]; then
	echo "ERROR: Kvirc version missing"
	usage
	exit
fi

if [ ! -d $SOURCETREEDIR ]; then
	echo "ERROR: The source tree parameter doesn't identify a directory"
	usage
	exit
fi

THISDIR=$(pwd)
TEMPDIR=/tmp
PKGSRCDIR=kvirc-"$KVIRCVERSION"
TEMPSRCDIR="$TEMPDIR/$PKGSRCDIR"
OUTPUTFILENAME="kvirc-${KVIRCVERSION}.tar.${OUTPUTEXTENSION}"
OUTPUTFILE="${THISDIR}/${OUTPUTFILENAME}"

if [ -d "$TEMPSRCDIR" ]; then
	echo "Removing stale target directory..."
	rm -rf "$TEMPSRCDIR"
fi

echo "Exporting svn dir into ${TEMPSRCDIR}..."
svn export "${SOURCETREEDIR}" "${TEMPSRCDIR}"

echo "Determining svn revision..."
cd "$SOURCETREEDIR"
# If we update svn here, we have a different revision than we exported above
#svn update
REVISION=$(svnversion -n .)

echo "Revision is $REVISION"
echo $REVISION > "$TEMPSRCDIR/.svnrevision"

if [ -f "$OUTPUTFILE" ]; then
	echo "Cleaning the target package file path..."
	rm -f "$OUTPUTFILE"
fi

if [ "${DOMD5SUM}" = "yes" ]; then
	if [ -f "${OUTPUTFILE}.md5sum" ]; then
		echo "Cleaning stale md5sum file..."
		rm -f "${OUTPUTFILE}.md5sum"
	fi
fi

echo "Compressing sources into $OUTPUTFILE"

cd "$TEMPDIR"

TARPARAMS="${COMPRESSSWITCH}cf"
tar -$TARPARAMS "$OUTPUTFILE" "$PKGSRCDIR" || exit

cd "$THISDIR"

if [ "${DOMD5SUM}" = "yes" ]; then
	cd "$THISDIR"
	md5sum "${OUTPUTFILENAME}" > "${OUTPUTFILENAME}.md5sum"
fi

echo "Removing target directory..."
rm -rf "${TEMPSRCDIR}"


echo "Here is the output I've generated"

cd "$THISDIR"

ls -al "${OUTPUTFILENAME}"

if [ "${DOMD5SUM}" = "yes" ]; then
	ls -la "${OUTPUTFILENAME}.md5sum"
fi


echo "Done. Have a nice day :)"

