export PATH="$PATH:/cygdrive/c/bin:/F:\\Cygwin\\bin"

PROJECTDIR="/cygdrive/f/Devel/Pragma/kvirccvs/kvirc"
PACKAGEDIR="/cygdrive/f/Devel/Pragma/kvirccvs/kvirc/win32build/package"
DOCSDIR="/usr/local/share/kvirc/3.0.1.99/help/en"
MOSDIR="/usr/local/share/kvirc/3.0.1.99/locale/"
BASEDIR="/cygdrive/f/Devel/Pragma/kvirccvs/kvirc/win32build"

cd $PROJECTDIR

# Make the directories

echo "Preparing distribution"
echo "Making $PACKAGEDIR"

mkdir -p $PACKAGEDIR


##############################################################################

SUBDIRS="pics pics/coresmall doc doc/scriptexamples \
	doc/scriptexamples/mp3share \
	doc/scriptexamples/playing \
	doc/scriptexamples/antiidle \
	doc/scriptexamples/runmenu \
	modules \
	modules/caps \
	modules/caps/crypt \
	modules/caps/tool \
	modules/caps/serverimport \
	modules/caps/toolbar \
	themes \
	themes/default.hires \
	themes/default.lores \
	msgcolors \
	defscript \
	config \
	config/modules \
	help \
	help/en \
	license \
	locale"

for adir in $SUBDIRS; do
	echo "mkdir -p $PACKAGEDIR/$adir"
	mkdir -p $PACKAGEDIR/$adir
done

##############################################################################

docopy()
{
	echo "cp -f $1 $2"
	cp -f $1 $2 || exit 0
}



##############################################################################

docopy $PROJECTDIR/README $PACKAGEDIR/README.txt
docopy $PROJECTDIR/doc/scriptexamples/mp3share/mp3share_icon.png $PACKAGEDIR/doc/scriptexamples/mp3share/
docopy $PROJECTDIR/doc/scriptexamples/mp3share/mp3share.kvs $PACKAGEDIR/doc/scriptexamples/mp3share/
docopy $PROJECTDIR/doc/scriptexamples/playing/playing_icon.png $PACKAGEDIR/doc/scriptexamples/playing/
docopy $PROJECTDIR/doc/scriptexamples/playing/playing.kvs $PACKAGEDIR/doc/scriptexamples/playing/
docopy $PROJECTDIR/doc/scriptexamples/antiidle/antiidle.kvs $PACKAGEDIR/doc/scriptexamples/antiidle/
docopy $PROJECTDIR/doc/scriptexamples/runmenu/runmenu.kvs $PACKAGEDIR/doc/scriptexamples/runmenu/

DISTDOCFILES=$(ls $PROJECTDIR/doc/*.txt)
for afile in $DISTDOCFILES; do
	docopy $afile $PACKAGEDIR/doc
done

docopy $PROJECTDIR/INSTALL $PACKAGEDIR/doc/
docopy $PROJECTDIR/FAQ $PACKAGEDIR/doc/
docopy $PROJECTDIR/TODO $PACKAGEDIR/doc/

DATAFILES=`ls $PROJECTDIR/data/msgcolors/*.msgclr`

for afile in $DATAFILES; do
	docopy $afile $PACKAGEDIR/msgcolors/
done

DATAFILES=`ls $PROJECTDIR/data/deftheme/hires/*.kvc`

for afile in $DATAFILES; do
	docopy $afile $PACKAGEDIR/themes/default.hires/
done

DATAFILES=`ls $PROJECTDIR/data/deftheme/lores/*.kvc`

for afile in $DATAFILES; do
	docopy $afile $PACKAGEDIR/themes/default.lores/
done

CONFIGS=`ls $PROJECTDIR/data/config/*.kvc`

for afile in $CONFIGS; do
	docopy $afile $PACKAGEDIR/config/
done

docopy $PROJECTDIR/data/defscript/default.kvs $PACKAGEDIR/defscript/

rm -f $PACKAGEDIR/pics/*.png

PICFILES=$(find $PROJECTDIR/data/pics/coresmall/ -name "*.png")

for afile in $PICFILES; do
	docopy $afile $PACKAGEDIR/pics/coresmall/
done

PICFILES=$(find $PROJECTDIR/src/ -name "*.png")

for afile in $PICFILES; do
	docopy $afile $PACKAGEDIR/pics/
done

PICFILES=$(ls $PROJECTDIR/data/pics/*.png)

for afile in $PICFILES; do
	docopy $afile $PACKAGEDIR/pics/
done



DLLS=`ls $BASEDIR/*.dll`
for afile in $DLLS; do
	docopy $afile $PACKAGEDIR/
done


docopy $PROJECTDIR/doc/COPYING $PACKAGEDIR/license/

docopy $PROJECTDIR/src/modules/tip/libkvitip.kvc $PACKAGEDIR/config/modules/

docopy $PROJECTDIR/src/modules/logview/caps/logview $PACKAGEDIR/modules/caps/tool/
docopy $PROJECTDIR/src/modules/lamerizer/lamerizer $PACKAGEDIR/modules/caps/crypt/
docopy $PROJECTDIR/src/modules/rijndael/caps/rijndael $PACKAGEDIR/modules/caps/crypt/
docopy $PROJECTDIR/src/modules/filetransferwindow/caps/filetransferwindow $PACKAGEDIR/modules/caps/tool/
docopy $PROJECTDIR/src/modules/sharedfileswindow/caps/sharedfileswindow $PACKAGEDIR/modules/caps/tool/
docopy $PROJECTDIR/src/modules/mircimport/caps/mircimport $PACKAGEDIR/modules/caps/serverimport/
docopy $PROJECTDIR/src/modules/tb_options/caps/tb_options $PACKAGEDIR/modules/caps/toolbar/
docopy $PROJECTDIR/src/modules/tb_scripting/caps/tb_scripting $PACKAGEDIR/modules/caps/toolbar/
docopy $PROJECTDIR/src/modules/tb_winops/caps/tb_winops $PACKAGEDIR/modules/caps/toolbar/
docopy $PROJECTDIR/src/modules/url/caps/url $PACKAGEDIR/modules/caps/toolbar/

DOCS="$DOCSDIR/*.html"

for afile in $DOCS; do
	docopy $afile $PACKAGEDIR/help/en/
done

docopy $PROJECTDIR/data/helppics/helplogoleft.png $PACKAGEDIR/help/en/
docopy $PROJECTDIR/data/helppics/helplogoright.png $PACKAGEDIR/help/en/


MOS=`ls $MOSDIR/*.mo`

# echo "MOS is $MOS"

for afile in $MOS; do
	docopy $afile $PACKAGEDIR/locale/
done

echo "DONE!"
