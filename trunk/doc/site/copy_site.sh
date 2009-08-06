#!/bin/sh

FILES="files/rss2btg files/*.ini files/fixcase"

# Copy to the http site.
scp -r *.html *.css *.txt wojci@shell.berlios.de:~/htdocs
scp -r $FILES wojci@shell.berlios.de:~/htdocs/files
scp -r manpages/*.html wojci@shell.berlios.de:~/htdocs/manpages

# Copy to the ftp site too.
scp -r *.html *.css *.txt wojci@shell.berlios.de:~/ftp/site
scp -r $FILES wojci@shell.berlios.de:~/ftp/site/files
scp -r manpages/*.html wojci@shell.berlios.de:~/ftp/site/manpages
