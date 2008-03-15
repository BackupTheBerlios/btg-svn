#!/bin/sh

# Generate a private key used by the btg daemon (PEM).
certtool --generate-privkey --outfile btgdaemon.key

# Generate a request (PEM).
certtool --template btg.template --generate-request --load-privkey btgdaemon.key --outfile btgdaemon.csr

# Generate a certificate (PEM).

certtool --generate-certificate --load-request btgdaemon.csr --outfile btgdaemon.crt \
--load-ca-certificate CA/ca.crt --load-ca-privkey CA/ca.key

# Remove the request used.
rm -f *.csr
