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


### 0.0 INITIALIZE FRAMEWORK COMPONENTS ###

### SCS INIT ###
#curl https://127.0.0.1:8000/app/scs/request -X POST --cacert service_cert.pem --cert user0_cert.pem --key user0_privk.pem -H "Content-Type: application/json" --data-binary '{"id": "4", "init": "0000000000000000000000000000000000000000000000000000000000000000"}'

### BulletinBoard INIT ###



### 1.0 INITIALIZE PVRA ENCLAVE ###

printf "\n[biPVRA] INITPVRA LAUNCH\n"
../app/app --initPVRA --enclave-path `pwd`/../enclave/enclave.signed.so \
  --sealedState sealedState.bin \
  --quotefile quote.bin \
  --signature enckey.sig
  
echo "\n[biPVRA] Running Auditee to Extract PVRA_signing_key\n"

#source ~/.venvs/auditee/bin/activate
python3.7 ../auditee_extract.py
#ias_report.json ready to be posted to BulletinBoard




### 2.0 RUNNING PVRA APPLICATION ###

openssl dgst -sha256 -verify signingkey.pem -signature enckey.sig enckey.dat





printf "[bcPVRA] Client generating AES session key\n"
#pre-generated AES key for debug
cp /home/azureuser/mbehnia/pvra-template/scratch/aes128gcm.pem .

printf "[bcPVRA] Client encrypting AES session key\n"
openssl rsautl -encrypt -pubin -inkey enckey.dat -in aes128gcm.pem > eAESkey.bin
cp /home/azureuser/mbehnia/pvra-template/debug/aes/encrypt_command .
printf "[bcPVRA] Client encrypting command\n"
./encrypt_command {1,{0},0,0} > /dev/null
#cp /home/azureuser/mbehnia/pvra-template/scratch/eCMD.bin .



printf "[bcPVRA] Client->Host eCMD+eAESkey sent\n"




printf "[bcPVRA] Host requesting new signedFT\n"
#gen FT = Hash(FTold || Hash(sealedstate || eCMD))
#curl https://127.0.0.1:8000/app/scs/update -X POST --cacert service_cert.pem --cert user0_cert.pem --key user0_privk.pem -H "Content-Type: application/json" --data-binary '{"id": "4", "commit": "a547891be9ed742869b0cdac2644c0ba676ec14da845fb8ab072eea7bc221ca0"}'
#sample CCF signature for debug
cp /home/azureuser/mbehnia/pvra-template/scratch/signedFT.bin .



printf "\n[bcPVRA] Host COMMANDPVRA LAUNCH\n"
../app/app --commandPVRA --enclave-path `pwd`/../enclave/enclave.signed.so \
  --sealedState sealedState.bin \
  --signedFT signedFT.bin \
  --eCMD eCMD.bin \
  --eAESkey eAESkey.bin \
  --cResponse cResponse.txt \
  --cRsig cResponse.sig \
  --sealedOut sealedOut.bin


printf "[bcPVRA] Host->Client cResponse sent\n"
#python3.7 ../extract_verify.py
openssl dgst -sha256 -verify signingkey.pem -signature cResponse.sig cResponse.txt



#mv sealedOut.bin sealedState.bin

exit