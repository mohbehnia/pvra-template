#!/bin/sh -e
#                                               -*- Makefile -*-
#
# Copyright (C) 2011-2019 Intel Corporation
#
# SPDX-License-Identifier: BSD-3-Clause
#


test -d test_sgx || mkdir test_sgx
cd ./test_sgx
rm -f *


printf "\nINIT PVRA Attempt 5,525,910...\n"
../app/app --initPVRA --enclave-path `pwd`/../enclave/enclave.signed.so \
  --sealedState sealedState.bin \
  --quotefile quote.bin \
  --signature enckey.sig
  
echo "\nRunning Auditee to Extract PVRA_signing_key\n"
python3.7 ../auditee_extract.py

openssl dgst -sha256 -verify signingkey.pem -signature enckey.sig enckey.dat

exit

# hello post sleep mohammad
# look a bit into ECDH otherwise change encryptkey to RSA
# write client enc(AESKey) enc(CMD) -> requires cmd_formatter based on CMD.h 
# verify(signedFT) using RSA calls so thats why

# test_sgx/ias_report.json ready to be published to bulletin board

echo "\nVerifying Signature to Extract PVRA_encryption_key\n"
openssl dgst -sha256 -verify p256-pkey.pem -signature enckey.sig ekey.dat




printf "\nCOMMAND PVRA Attempt 2,425,129...\n"
../app/app --commandPVRA --enclave-path `pwd`/../enclave/enclave.signed.so \
  --sealedState sealedState.txt
  
#  --signedFT signedFT.txt \
#  --sealedState sealedState.txt \
#  --eCMD eCMD.txt \
#  --eKey eKey.txt