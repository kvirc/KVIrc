#/bin/bash

#################################################################################################
# Helper functions

function usage()
{
	echo "Usage:"
	echo "      $0 [options] <source_tree_dir> <kvirc_version>"
	echo "Available options:"
	echo "   --gzip"
	echo "     Use gzip instead of bzip2"
	echo "   --bzip"
	echo "     Use bzip instead of gzip"
	echo "   --md5sum"
	echo "     Create md5sum of the package too"
	echo "   --help"
	echo "     Show this help"
}

#################################################################################################
# Gather commandline params

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
		--bzip)
				COMPRESSSWITCH="j"
				OUTPUTEXTENSION="bz2"
			;;
		--help)
				usage
				exit
			;;
		--md5sum)
				DOMD5SUM="yes"
			;;
		--*)
			echo "ERROR: unrecognized option $i"
			usage
			exit
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
	echo "ERROR: source tree directory missing"
	usage
	exit
fi

if [ -z "$KVIRCVERSION" ]; then
	echo "ERROR: KVIrc version missing"
	usage
	exit
fi

if [ ! -d $SOURCETREEDIR ]; then
	echo "ERROR: the source tree parameter doesn't identify a directory"
	usage
	exit
fi

#################################################################################################
# Setup vars

THISDIR=$(pwd)
TEMPDIR=/tmp
PKGSRCDIR=kvirc-"$KVIRCVERSION"
TEMPSRCDIR="$TEMPDIR/$PKGSRCDIR"
OUTPUTFILENAME="kvirc-${KVIRCVERSION}.tar.${OUTPUTEXTENSION}"
OUTPUTFILE="${THISDIR}/${OUTPUTFILENAME}"
MD5SUMOUTPUTFILENAME="${OUTPUTFILENAME}.md5"
MD5SUMOUTPUTFILE="${THISDIR}/${MD5SUMOUTPUTFILENAME}"

#################################################################################################
# Clear the temporary dir

if [ -d "$TEMPSRCDIR" ]; then
	echo "Removing stale target directory..."
	rm -rf "$TEMPSRCDIR"
fi

#################################################################################################
# Export git into the temporary dir

cd "$SOURCETREEDIR"
echo "Exporting git dir into ${TEMPSRCDIR}..."
# Ensure the prefix parameter has a slash at the end, or bad things could happen
git checkout-index -a -f --prefix="${TEMPSRCDIR}/"


#################################################################################################
# Figure out the git revision

echo "Determining git revision..."
REVISION=$(git log -1 --date=short "--pretty=%h")

echo "Revision is $REVISION"
echo $REVISION > "$TEMPSRCDIR/.gitrevision"

#################################################################################################
# Remove stuff that should not be there

echo "Removing unused stuff..."
rm -f "${TEMPSRCDIR}/dist/secret.enc"

#################################################################################################
# Make room for the output files

if [ -f "${OUTPUTFILE}" ]; then
	echo "Removing the existing output file..."
	rm -f "${OUTPUTFILE}"
fi

if [ "${DOMD5SUM}" = "yes" ]; then
	if [ -f "${MD5SUMOUTPUTFILE}" ]; then
		echo "Cleaning stale md5sum file..."
		rm -f "${MD5SUMOUTPUTFILE}"
	fi
fi

#################################################################################################
# Do compress

echo "Compressing sources into $OUTPUTFILE"

cd "$TEMPDIR"

TARPARAMS="${COMPRESSSWITCH}cf"
tar -$TARPARAMS "$OUTPUTFILE" "$PKGSRCDIR" || exit

#################################################################################################
# Do md5sum if requested

cd "$THISDIR"

if [ "${DOMD5SUM}" = "yes" ]; then
	cd "$THISDIR"
	md5sum "${OUTPUTFILENAME}" > "${MD5SUMOUTPUTFILE}"
fi

#################################################################################################
# Cleanup the exported Git branch

echo "Removing target directory..."
rm -rf "${TEMPSRCDIR}"

#################################################################################################
# Show the work done

echo "Here is the output I've generated"

cd "${THISDIR}"

ls -al "${OUTPUTFILENAME}"

if [ "${DOMD5SUM}" = "yes" ]; then
	ls -la "${MD5SUMOUTPUTFILENAME}"
fi


#################################################################################################
# Done :)

echo "Done. Have a nice day. :)"
