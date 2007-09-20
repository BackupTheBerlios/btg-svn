#!/bin/sh

# Copy to the http site.
scp -r *.html *.css wojci@shell.berlios.de:~/htdocs
scp -r files/*.ini wojci@shell.berlios.de:~/htdocs/files
scp -r files/*.txt wojci@shell.berlios.de:~/htdocs/files

# Copy to the ftp site too.
scp -r *.html *.css wojci@shell.berlios.de:~/ftp/site
scp -r files/*.ini wojci@shell.berlios.de:~/ftp/site/files
scp -r files/*.txt wojci@shell.berlios.de:~/ftp/site/files
