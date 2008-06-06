#!/bin/sh

# Generate a private key used by the btg daemon (PEM).
certtool --generate-privkey --outfile btgclient.key

# Generate a request (PEM).
certtool --template btg.template --generate-request --load-privkey btgclient.key --outfile btgclient.csr

# Generate a certificate (PEM).

certtool --generate-certificate --load-request btgclient.csr --outfile btgclient.crt \
--load-ca-certificate CA/ca.crt --load-ca-privkey CA/ca.key

# Remove the request used.
rm -f *.csr

