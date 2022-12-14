#include <stdbool.h>

#define INITSEALSIZE 7092

#ifndef COMMAND_H
#define COMMAND_H

#define NUM_COMMANDS 2
#define NUM_USERS 2
#define NUM_TESTS 100

#define INIT_NUM_USERS 2
#define INIT_NUM_TESTS 100

typedef struct {
    unsigned char data[64];
} secp256k1_pubkey;


struct cInputs
{
	int uid;
	int test_result;
};


struct cType 
{
	int tid;
};


struct private_command {
	struct cType CT;
	struct cInputs CI;
	int seqNo;
};


struct clientCommand
{
	secp256k1_pubkey user_pubkey;
	struct private_command eCMD;
};





struct cResponse
{
	bool access;
	int error;
	char message[100];

};

/*
struct AD
{
	char test_history[NUM_USERS*NUM_TESTS];
	int num_tests[NUM_USERS];
	int query_counter[NUM_USERS];
};*/



struct AD
{
	char *test_history;
	int *num_tests;
	int *query_counter;
};




#endif
