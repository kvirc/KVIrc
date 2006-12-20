#!/bin/bash

#echo "this file is useless: use make messages-update"



#exit 0

find ../../ -maxdepth 10 -name *.cpp > files
find ../../ -maxdepth 10 -name *.h >> files
xgettext -o kvirc.pot -ki18n -ktr2i18n -kI18N_NOOP -k__tr -k__tr_no_lookup -k__tr2qs -k__tr2wc -k__tr2ws -k__tr_ctx -k__tr2ts_ctx -f files
rm files

langs=`ls *.po`
old=_old
for lang  in $langs
do
  echo "moving $lang to $lang$old..."
  mv $lang $lang$old
  echo "done!"
  echo "merging new strings..."
  msgmerge -o $lang $lang$old kvirc.pot
  echo "done!"
  echo "deleting old file $lang$old..-"
  rm $lang$old
  echo "done!"
done
