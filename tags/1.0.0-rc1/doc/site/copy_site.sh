#!/bin/sh

# Copy to the http site.
scp -r *.html *.css *.txt wojci@shell.berlios.de:~/htdocs
scp -r files/*.ini wojci@shell.berlios.de:~/htdocs/files
scp -r manpages/*.html wojci@shell.berlios.de:~/htdocs/manpages

# Copy to the ftp site too.
scp -r *.html *.css *.txt wojci@shell.berlios.de:~/ftp/site
scp -r files/*.ini wojci@shell.berlios.de:~/ftp/site/files
scp -r manpages/*.html wojci@shell.berlios.de:~/ftp/site/manpages