#!/bin/sh

FILENAME=manpages.rst

echo "=============" > $FILENAME
echo "BTG Man Pages" >> $FILENAME
echo "=============" >> $FILENAME
echo >> $FILENAME

# Convert man pages to html.
for index in `find ../.. -name \*.man`
do
  echo "Converting $index"
  echo " - \``basename $index`\`_" >> $FILENAME
  groff -t -e -mandoc -Thtml $index > manpages/`basename $index`.html
done

echo >> $FILENAME

for index in `find ../.. -name \*.man`
do
  echo ".. _`basename $index`: manpages/`basename $index`.html" >> $FILENAME
done

echo "" >> $FILENAME
echo ".. footer:: \`BTG home page\`_" >> $FILENAME
echo ".. _BTG home page: http://btg.berlios.de/" >> $FILENAME

