#!/bin/bash

# Generate directory
mkdir certificates

# Generate keys
openssl genpkey -algorithm RSA -out certificates/authority.pem -pkeyopt rsa_keygen_bits:2048
openssl rsa -pubout -in certificates/authority.pem -out certificates/workers.pem