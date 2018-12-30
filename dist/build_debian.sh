#!/bin/bash

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
SVNGITBZR="~git-"
VERSION='4:5.0.0'
VERSION1='5.0.0'
TMPFILE=$(mktemp)
TMPGPG=$(mktemp)
DIST_PPA="xenial bionic cosmic"
PPANAME=kvirc

dchppa_pkg(){
NEW_VER=$(dpkg-parsechangelog | awk '/^Version: / {print $2}')
cd "$BUILDDIR/${PKG_NAME}-${VERSION1}${SVNGITBZR}${dat}/" || exit 1
cp -f debian/changelog "${TMPFILE}"
for i in ${DIST_PPA}
do
cp -f "${TMPFILE}" debian/changelog
dch -b --force-distribution --distribution "$i" -v "${NEW_VER}ppa1~${i}1" \
  "Automated backport upload; no source changes."
echo "$SOURCEUP" | grep -q YES && debuild --no-lintian -p"gpg --passphrase-file $TMPGPG --batch --no-use-agent" -S -sa
echo "$SOURCEUP" | grep -q YES || debuild --no-lintian -p"gpg --passphrase-file $TMPGPG --batch --no-use-agent" -S -sd
SOURCEUP=YES
done
unset SOURCEUP
for i in ${DIST_PPA}
do
dput "${PPANAME}" "../${PKG_NAME}_*${i}1_source.changes"
sleep 10
done
cp -f "${TMPFILE}" debian/changelog
}

dputcf(){
cat > ~/.dput.cf << EOF
[kvirc]
fqdn = ppa.launchpad.net
method = ftp
incoming = ~kvirc/kvirc/ubuntu/
login = anonymous
allow_unsigned_uploads = 0

EOF
}

tmpgpg(){
cat > "$TMPGPG" << EOF
$GPGPASS
EOF
}
gpgkey(){
openssl enc -d -aes-256-cbc -in "${DIR}/secret.enc" -out "${DIR}/secret.gpg" -k "${AESPASS}"
gpg --import "${DIR}/public.gpg"
gpg --allow-secret-key-import --import "${DIR}/secret.gpg"
}

test -d "$BUILDDIR" && rm -rf "${BUILDDIR}"
mkdir -p "$BUILDDIR"
cd "$GITDIR" || exit 1
abbrevcommit=$(git log -1 --abbrev-commit | grep -i "^commit" | awk '{print $2}')
numcommit=$(git log | grep -c "^Date:")
dat="${numcommit}-${abbrevcommit}"
#dat=$(git describe --dirty)
branch=$(git branch | grep "\*" | sed 's/\* //g')
commit=$(git log -1 | grep -i "^commit" | awk '{print $2}')
datct=$(git log -n 1 --format=%ct)

test -z "${dat}" && dat="git-99999-$(git describe --always)"
test -z "${branch}" && branch="travis_debian"

if [ ! -z "$PPA" ]
then
    branch="${TRAVIS_BRANCH}"
fi

tar -cpf  "${BUILDDIR}/${PKG_NAME}_${VERSION1}${SVNGITBZR}${dat}.orig.tar" --exclude ".git" --exclude "dist" .
cd "${BUILDDIR}" || exit 1
xz -9 "${PKG_NAME}_${VERSION1}${SVNGITBZR}${dat}.orig.tar"
mkdir "${PKG_NAME}-${VERSION1}${SVNGITBZR}${dat}"
tar -xpJf "${PKG_NAME}_${VERSION1}${SVNGITBZR}${dat}.orig.tar.xz" -C "${PKG_NAME}-${VERSION1}${SVNGITBZR}${dat}"
cd "$DIR" || exit 1
cp -r debian "$BUILDDIR/${PKG_NAME}-${VERSION1}${SVNGITBZR}${dat}/debian"
cd "$BUILDDIR/${PKG_NAME}-${VERSION1}${SVNGITBZR}${dat}/" || exit 1
dch -v "${VERSION}${SVNGITBZR}${dat}-1" -D "unstable" --force-distribution "Compile"
dch -a "Branch: $branch"
dch -a "Commit: $commit"
dch -a "Date: $datct"

if [ -z "$PPA" ]
then
    if [ ! -z "${TRAVIS_BRANCH}" ]
    then
        sed -i '/^ cmake (>= 3.1),/d' debian/control
        debuild --no-lintian --preserve-env --prepend-path="${PATH}" -us -uc 
    else
        debuild --no-lintian -us -uc 
    fi
else
    gpgkey
    tmpgpg
    test -f ~/.dput.cf || dputcf
    dchppa_pkg
fi


