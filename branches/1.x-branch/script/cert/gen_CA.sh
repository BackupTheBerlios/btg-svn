#!/bin/sh

mkdir CA
chmod 0770 CA
cd CA

# Create a private key (PEM).
certtool --generate-privkey --outfile ca.key

# Create a certificate. (PEM)
certtool --generate-self-signed --load-privkey ca.key --outfile ca.crt
