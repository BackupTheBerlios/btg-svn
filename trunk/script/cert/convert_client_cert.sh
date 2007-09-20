#!/bin/sh

# Convert a client certificate so it can be used by the php UI.

# The generated client certificate is in combined PEM format.
# This contains BOTH certificate and private key.

openssl x509 -out btgclient.crt.pem -in btgclient.crt -outform pem && \
cat btgclient.key > btgclient.pem && cat btgclient.crt.pem >> btgclient.pem && rm btgclient.crt.pem && \
echo "Created btgclient.pem."