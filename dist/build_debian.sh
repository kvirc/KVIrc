#/bin/sh

test -z "$DEBFULLNAME" && export DEBFULLNAME="Alexander Pozdnyakov"
test -z "$DEBEMAIL" && export DEBEMAIL="almipo@mail.ru"
bin_debuild=$(which debuild 2>/dev/null)


if [ ! -x "$bin_debuild" ]
then
    echo "Please install debuild (package devscripts)."
    exit 1
fi

DIR=$(pwd)
BUILDDIR="${DIR}/build"
cd .. && GITDIR=$(pwd)
PKG_NAME=kvirc
SVNGITBZR="~"
VERSION='4:4.3.2'
VERSION1='4.3.2'

test -d $BUILDDIR && rm -rf ${BUILDDIR} 
mkdir -p $BUILDDIR

cd $GITDIR

dat=$(git describe --dirty)
branch=$(git branch | grep "\*" | sed 's/\* //g')
commit=$(git log -1 | grep -i "^commit" | awk '{print $2}')
datct=$(git log -n 1 --format=%ct)

tar -cpf  "${BUILDDIR}/${PKG_NAME}_${VERSION1}${SVNGITBZR}${dat}.orig.tar" --exclude ".git" --exclude "dist" . 
cd ${BUILDDIR}
xz -9 "${PKG_NAME}_${VERSION1}${SVNGITBZR}${dat}.orig.tar"
mkdir "${PKG_NAME}-${VERSION1}${SVNGITBZR}${dat}"
tar -xpJf "${PKG_NAME}_${VERSION1}${SVNGITBZR}${dat}.orig.tar.xz" -C "${PKG_NAME}-${VERSION1}${SVNGITBZR}${dat}"
cd $DIR
cp -r debian $BUILDDIR/${PKG_NAME}-${VERSION1}${SVNGITBZR}${dat}/debian
cd $BUILDDIR/${PKG_NAME}-${VERSION1}${SVNGITBZR}${dat}/
dch -v "${VERSION}${SVNGITBZR}${dat}-1" -D "unstable" --force-distribution "Compile"
dch -a "Branch: $branch"
dch -a "Commit: $commit"
dch -a "Date: $datct"

debuild
