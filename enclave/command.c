/*
 * Copyright (C) 2019 Intel Corporation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "enclavestate.h"
#include "command.h"

#include "enclave.h"
#include <enclave_t.h>

#include <sgx_quote.h>
#include <sgx_tcrypto.h>
#include <sgx_tseal.h>
#include <sgx_utils.h>


#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/bignum.h>
#include <mbedtls/pk.h>
#include <mbedtls/rsa.h>





/**
 * This function generates a key pair and then seals the private key.
 *
 * @param pubkey                 Output parameter for public key.
 * @param pubkey_size            Input parameter for size of public key.
 * @param sealedprivkey          Output parameter for sealed private key.
 * @param sealedprivkey_size     Input parameter for size of sealed private key.
 *
 * @return                       SGX_SUCCESS (Error code = 0x0000) on success,
 * some sgx_status_t value upon failure.
 */

#define BUFLEN 2048
#define KEY_SIZE 2048
#define MBED_TLS_KEY_SIZE 2049
#define EXPONENT 65537

#define AESGCM_128_KEY_SIZE 16
#define AESGCM_128_MAC_SIZE 16
#define AESGCM_128_IV_SIZE 12
#define EXPECTEDINPUT 16



struct cResponse statusUpdate(struct ES *enclave_state, struct cInputs *CI)
{
    struct cResponse ret;

    if(CI->uid > NUM_USERS-1) {
        char *m = "[statusUpdate]: ERROR invalid userID";
        printf("%s\n", m);
        memcpy(ret.message, m, strlen(m));
        ret.error = 1;
        return ret;
    }

    if(enclave_state->appdata.num_tests[CI->uid] == NUM_TESTS) {
        char *m = "[statusUpdate]: ERROR full test_history";
        printf("%s\n", m);
        memcpy(ret.message, m, strlen(m));
        ret.error = 2;
        return ret;
    }

    if((CI->test_result != 0) && (CI->test_result != 1))
    {
        char *m = "[statusUpdate]: ERROR invalid test_result";
        printf("%s [%d]\n", m, CI->test_result);
        memcpy(ret.message, m, strlen(m));
        ret.error = 3;
        return ret;
    }

    ret.error = 0;
    char *m = "[statusUpdate]: SAVED test_result";
    printf("%s\n", m);
    memcpy(ret.message, m, strlen(m));
    enclave_state->appdata.test_history[CI->uid][enclave_state->appdata.num_tests[CI->uid]] = CI->test_result;
    enclave_state->appdata.num_tests[CI->uid]++;
    enclave_state->appdata.query_counter[CI->uid]++;
    return ret;
}

struct cResponse statusQuery(struct ES *enclave_state, struct cInputs *CI)
{
    struct cResponse ret;

    enclave_state->appdata.query_counter[CI->uid]++;

    if(CI->uid > NUM_USERS-1) {
        char *m = "[statusQuery]: ERROR invalid userID";
        printf("%s\n", m);
        memcpy(ret.message, m, strlen(m));
        ret.error = 1;
        return ret;
    }

    if(enclave_state->appdata.num_tests[CI->uid] < 2) {
        char *m = "[statusQuery]: ERROR insufficient testing";
        printf("%s\n", m);
        memcpy(ret.message, m, strlen(m)+1);
        ret.error = 2;
        return ret;
    }

    ret.error = 0;
    if ( (enclave_state->appdata.test_history[CI->uid][enclave_state->appdata.num_tests[CI->uid]-1] == 0) &&
            (enclave_state->appdata.test_history[CI->uid][enclave_state->appdata.num_tests[CI->uid]-2] == 0) ) {
        ret.access = true;
        char *m = "[statusQuery]: ACCESS GRANTED";
        printf("%s\n", m);
        memcpy(ret.message, m, strlen(m));
    }
    else {
        ret.access = false;
        char *m = "[statusQuery]: ACCESS DENIED";
        printf("%s\n", m);
        memcpy(ret.message, m, strlen(m));
    }

    return ret;
}




int init(struct cResponse (*functions[NUM_COMMANDS])(struct ES*, struct cInputs*)) 
{
    //printf(": Initialized Application Ecalls\n");
    (functions[0]) = &statusUpdate;
    (functions[1]) = &statusQuery;
    return 0;
}


char *format_cResponse(struct cResponse cRet) {
    char cRstring[2] = "0";
    return cRstring;
}