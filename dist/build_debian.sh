#/bin/sh

test -z "$DEBFULLNAME" && export DEBFULLNAME="Alexander Pozdnyakov"
test -z "$DEBEMAIL" && export DEBEMAIL="kvircbot@gmail.com"

#export DEBEMAIL="kvircbot@gmail.com"
#export GPGPASS=secret
#export AESPASS=secret

bin_debuild=$(which debuild 2>/dev/null)

while getopts "p" Option
do
    case $Option in
        p) PPA=1 ;;
    esac
done
shift $((OPTIND - 1))

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
VERSION='4:4.9.2'
VERSION1='4.9.2'
TMPFILE=$(mktemp)
TMPGPG=$(mktemp)
DIST_PPA="trusty wily xenial yakkety"
PPANAME=kvirc

dchppa_pkg(){
NEW_VER=$(dpkg-parsechangelog | awk '/^Version: / {print $2}')
cd $BUILDDIR/${PKG_NAME}-${VERSION1}${SVNGITBZR}${dat}/
cp -f debian/changelog ${TMPFILE}
for i in ${DIST_PPA}
do
cp -f ${TMPFILE} debian/changelog
dch -b --force-distribution --distribution "$i" -v "${NEW_VER}ppa1~${i}1" \
  "Automated backport upload; no source changes."
[ -z $(echo $SOURCEUP | grep YES) ] && debuild --no-lintian -p"gpg --passphrase-file $TMPGPG --batch --no-use-agent" -S -sa
[ -z $(echo $SOURCEUP | grep YES) ] || debuild --no-lintian -p"gpg --passphrase-file $TMPGPG --batch --no-use-agent" -S -sd
SOURCEUP=YES
done
unset SOURCEUP
for i in ${DIST_PPA}
do
dput ${PPANAME} ../${PKG_NAME}_*${i}1_source.changes
sleep 10
done
cp -f ${TMPFILE} debian/changelog
}

dputcf(){
cat > ~/.dput.cf << EOF
[kvirc]
fqdn = ppa.launchpad.net
method = ftp
incoming = ~kvirc/kvirc/ubuntu/
login = anonymous
allow_unsigned_uploads = 0

[kvirc-qt5.5]
fqdn = ppa.launchpad.net
method = ftp
incoming = ~kvirc/kvirc-qt5.5/ubuntu/
login = anonymous
allow_unsigned_uploads = 0

[kvirc-dbg]
fqdn = ppa.launchpad.net
method = ftp
incoming = ~kvirc/kvirc-dbg/ubuntu/
login = anonymous
allow_unsigned_uploads = 0

EOF
}

tmpgpg(){
cat > $TMPGPG << EOF
$GPGPASS
EOF
}
gpgkey(){
openssl enc -d -aes-256-cbc -in ${DIR}/secret.enc -out ${DIR}/secret.gpg -k ${AESPASS}
gpg --import ${DIR}/public.gpg
gpg --allow-secret-key-import --import ${DIR}/secret.gpg
}

test -d $BUILDDIR && rm -rf ${BUILDDIR}
mkdir -p $BUILDDIR
cd $GITDIR
dat=$(git describe --dirty)
branch=$(git branch | grep "\*" | sed 's/\* //g')
commit=$(git log -1 | grep -i "^commit" | awk '{print $2}')
datct=$(git log -n 1 --format=%ct)

test -z "${dat}" && dat="git-9999-$(git describe --always)"
test -z "${branch}" && branch="travis_debian"

if [ ! -z "$PPA" ]
then
    branch="${TRAVIS_BRANCH}"
fi

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

if [ -z "$PPA" ]
then
    debuild --no-lintian -us -uc
else
    gpgkey
    tmpgpg
    test -f ~/.dput.cf || dputcf
    sed "s/Release/Debug/g" -i debian/rules
    dchppa_pkg
    rm -f ../*ppa*
    DIST_PPA="trusty wily"
    PPANAME=kvirc-qt5.5
    dchppa_pkg
    #DIST_PPA="trusty wily xenial"
    #rm -f ../*ppa*
    #PPANAME=kvirc-dbg
    #dch -a "DCMAKE_BUILD_TYPE=Debug"
    #cat >> debian/rules << EOF
#override_dh_strip:
#
#EOF
#    dchppa_pkg
fi


