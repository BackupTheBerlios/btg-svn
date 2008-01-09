#!/bin/sh

FILENAME=manpages.rst

echo "=============" > $FILENAME
echo "BTG Man Pages" >> $FILENAME
echo "=============" >> $FILENAME
echo >> $FILENAME

for index in `find ../.. -name \*.man`
do
  echo "Converting $index"
  echo " - \``basename $index`\`_" >> $FILENAME
  groff -t -e -a -mandoc -Tascii $index > files/`basename $index`.txt
done

echo >> $FILENAME

for index in `find ../.. -name \*.man`
do
  echo ".. _`basename $index`: files/`basename $index`.txt" >> $FILENAME
done

echo "" >> $FILENAME
echo ".. footer:: \`BTG home page\`_" >> $FILENAME
echo ".. _BTG home page: http://btg.berlios.de/" >> $FILENAME

