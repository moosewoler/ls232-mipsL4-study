#include "chief_ipc.h"

#define SERIAL_TID ((l4_threadid_t) (dword_t) 0x1002000000060001LL)

/************* Chief IPC Tester **************************************\
* Tests that proper reporting of redirection occurs.                  *
* Starts a child task in a copy of its own address space.             *
\*********************************************************************/ 

#define CHIEF_IPC_CHILD_STACK_SIZE 512
dword_t chief_IPC_test_child_stack[CHIEF_IPC_CHILD_STACK_SIZE];
#define ChiefIPCTestSP ((dword_t) \
  &(chief_IPC_test_child_stack[CHIEF_IPC_CHILD_STACK_SIZE]))
l4_ipc_reg_msg_t chief_IPC_array;
l4_threadid_t chief_IPC_parent, chief_IPC_child, chief_IPC_grandchild;
l4_threadid_t chief_IPC_dest, chief_IPC_src;
int chief_IPC_child_seq_match;



int chief_IPC_array_test(int *seq, l4_ipc_reg_msg_t *msg) {
    int seq_matches = 1;
    int s = * seq;

    seq_matches = ((*msg).reg[0] == s++) && seq_matches;
    seq_matches = ((*msg).reg[1] == s++) && seq_matches;
    seq_matches = ((*msg).reg[2] == s++) && seq_matches;
    seq_matches = ((*msg).reg[3] == s++) && seq_matches;
    seq_matches = ((*msg).reg[4] == s++) && seq_matches;
    seq_matches = ((*msg).reg[5] == s++) && seq_matches;
    seq_matches = ((*msg).reg[6] == s++) && seq_matches;
    seq_matches = ((*msg).reg[7] == s++) && seq_matches;

    *seq = s;
    return(seq_matches);
}


int chief_IPC_tester_report(l4_threadid_t dest,
			    l4_threadid_t reported_dest,
			    l4_threadid_t src,
			    l4_threadid_t reported_src,
			    int *sequence,
			    l4_ipc_reg_msg_t *msg) {
    l4_msgdope_t result;
    l4_ipc_reg_msg_t pmsg;
    int success;

    if(thread_equal(reported_dest, l4_myself())) {
	l4_send_task_short(SERIAL_TID, &result,
			   "FAIL: Redirect does not give real destination");
	success = 0;
    } else if(thread_equal(dest, reported_dest)) {
	l4_send_task_short(SERIAL_TID, &result,
			   "PASS: Redirect gives real destination");
	success = 1;
    } else {
	l4_send_task_short_f(SERIAL_TID, &pmsg, &result,
			     "FAIL: Bad destination thread id:0x%lx",
			     reported_dest.ID);
	success = 0;
    }
    
    if (thread_equal(src, reported_src)) {
	l4_send_task_short(SERIAL_TID, &result,
			   " (Correct source id)\n");
    } else {
	l4_send_task_short(SERIAL_TID, &result,
			   "\nFAIL: Incorrect source id\n");
	success = 0;
    }
    if (chief_IPC_array_test(sequence, msg)) {
	return(success);
    } else {
	l4_send_task_short(SERIAL_TID, &result,
			   "FAIL: Message garbled\n");
	return(0);
    }
}


void chief_IPC_array_set(int *num, l4_ipc_reg_msg_t *msg) {
    (* msg).reg[0] = (* num)++;
    (* msg).reg[1] = (* num)++;
    (* msg).reg[2] = (* num)++;
    (* msg).reg[3] = (* num)++;
    (* msg).reg[4] = (* num)++;
    (* msg).reg[5] = (* num)++;
    (* msg).reg[6] = (* num)++;
    (* msg).reg[7] = (* num)++;
}

void chief_IPC_test_grandchild() {while(1);}

l4_ipc_reg_msg_t chief_child_msg;

void chief_IPC_test_child() {
    l4_threadid_t dest = SIGMA0_TID;
    l4_threadid_t fake_src;
    l4_msgdope_t result;
    int sequence = 1;
    dword_t child_stack[4];
    l4_snd_fpage_t *fp;
    dword_t fault_addr;


    /*Create child task*/
    chief_IPC_grandchild = l4_myself(); chief_IPC_grandchild.id.task ++;
    chief_IPC_grandchild = l4_task_new(
	chief_IPC_grandchild, l4_myself().ID,
	(dword_t) &child_stack[3], (dword_t) chief_IPC_test_grandchild,
	l4_myself(), l4_myself());
    /*Map pages to child task*/
    l4_mips_ipc_receive(chief_IPC_grandchild, L4_IPC_SHORT_MSG,
			&chief_child_msg, L4_IPC_TIMEOUT(0,0,1,5,0,0),
			&result);
    while (result.msgdope & L4_IPC_RETIMEOUT) {
	fault_addr =  (chief_child_msg.reg[0] & (~(dword_t) 3));
	fp = (l4_snd_fpage_t *) &chief_child_msg.reg[0];
	fp[0].snd_base = fault_addr;
	fp[0].fpage = l4_fpage(fault_addr, 12, 1, 0);
	fp[1].fpage.fpage = 0;
	fp[1].snd_base = *((dword_t *) fault_addr);/*<----- Don't copy this*/
	assert(0 == l4_mips_ipc_send(chief_IPC_grandchild, 
				     L4_IPC_SHORT_FPAGE, &chief_child_msg,
				     L4_IPC_NEVER, &result));
	l4_mips_ipc_receive(chief_IPC_grandchild, L4_IPC_SHORT_MSG,
			    &chief_child_msg, L4_IPC_TIMEOUT(0,0,1,5,0,0),
			    &result);
    }
    assert(! L4_IPC_ERROR(result));


    /******** Send message two times *******/
    chief_IPC_array_set(&sequence, &chief_child_msg);
    l4_mips_ipc_send(dest, L4_IPC_SHORT_MSG,
		      &chief_child_msg, L4_IPC_NEVER, &result);
    chief_IPC_array_set(&sequence, &chief_child_msg);
    l4_mips_ipc_send(dest, L4_IPC_SHORT_MSG,
		     &chief_child_msg, L4_IPC_NEVER, &result);

    /***** Send and deceive with higher nest value (preserving direction)*/
    chief_IPC_array_set(&sequence, &chief_child_msg);
    l4_mips_ipc_send_deceiving(dest, chief_IPC_grandchild,
			       L4_IPC_SHORT_MSG, &chief_child_msg,
			       L4_IPC_NEVER, &result);
    /***** Send and deceive with lower nest value (not preserving direction)*/
    l4_id_nearest(SIGMA0_TID, &fake_src);
    chief_IPC_array_set(&sequence, &chief_child_msg);
    l4_mips_ipc_send_deceiving(dest, fake_src, L4_IPC_SHORT_MSG,
		     &chief_child_msg, L4_IPC_NEVER, &result);

    /******** Send deceive to test closed receive bug ***********/
    /*** Sender IPCs first ****/
    chief_IPC_array_set(&sequence, &chief_child_msg);
    l4_mips_ipc_send_deceiving(SIGMA0_TID, chief_IPC_grandchild,
			       L4_IPC_SHORT_MSG, &chief_child_msg,
			       L4_IPC_NEVER, &result);
    /*** Receiver IPCs first ***/
    l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0), &result); /*1 sec*/
    chief_IPC_array_set(&sequence, &chief_child_msg);
    l4_mips_ipc_send_deceiving(SIGMA0_TID, chief_IPC_grandchild,
			       L4_IPC_SHORT_MSG, &chief_child_msg,
			       L4_IPC_NEVER, &result);

    /******** Send three chief messages *******/
    /* Chief send */
    chief_IPC_array_set(&sequence, &chief_child_msg);
    assert(0 == l4_mips_ipc_chief_send(dest, chief_IPC_grandchild,
				       L4_IPC_SHORT_MSG, &chief_child_msg,
				       L4_IPC_NEVER, &result));


    /* Wait until parent clears communication registers and then ipc
       chief_call*/
    while(~ thread_equal(chief_IPC_dest, L4_NIL_ID) && 
	  ~ thread_equal(chief_IPC_src, L4_NIL_ID) && 
	  chief_IPC_child_seq_match != 2) {
	l4_thread_switch(L4_NIL_ID);
    }
    chief_IPC_array_set(&sequence, &chief_child_msg);
    assert(0 == l4_mips_ipc_chief_call(dest, chief_IPC_grandchild,
				       L4_IPC_SHORT_MSG, &chief_child_msg,
				       &chief_IPC_dest,
				       L4_IPC_SHORT_MSG, &chief_child_msg,
				       L4_IPC_NEVER, &result));
    chief_IPC_src = SIGMA0_TID;
    chief_IPC_child_seq_match =
	chief_IPC_array_test(&sequence, &chief_child_msg);
    
    
    /* Wait until parent clears communication registers and then ipc
       chief_reply_and_wait. */
    while(~ thread_equal(chief_IPC_dest, L4_NIL_ID) && 
	  ~ thread_equal(chief_IPC_src, L4_NIL_ID) && 
	  chief_IPC_child_seq_match != 2) {
	l4_thread_switch(L4_NIL_ID);
    }
    chief_IPC_array_set(&sequence, &chief_child_msg);
    assert(0 == l4_mips_ipc_chief_reply_and_wait(
	SIGMA0_TID, chief_IPC_grandchild, L4_IPC_SHORT_MSG, &chief_child_msg,
	&chief_IPC_src, &chief_IPC_dest, L4_IPC_SHORT_MSG, &chief_child_msg,
	L4_IPC_NEVER, &result));
    chief_IPC_child_seq_match =
	chief_IPC_array_test(&sequence, &chief_child_msg);
    /*assert(0);*/
    while(1);
    assert(0);
}



















#define chief_IPC_test_macro(ipc_op, output, msg, result) \
    l4_send_task_short(SERIAL_TID, &result, output); \
    l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,7,0,0), &result); \
    assert(0 == ipc_op); \
    l4_send_task_short_f(SERIAL_TID, &pmsg, &result, \
			 "Message from 0x%lx to 0x%lx\n", \
			  src.ID, real_dest.ID);

void chief_IPC_test(int childtask, l4_threadid_t pager_thread)
{
    l4_ipc_reg_msg_t msg, pmsg;
    l4_msgdope_t result;
    l4_threadid_t src, real_dest;
    l4_snd_fpage_t *fp;
    int passed_all = 1; /*True if all tests passed*/
    int sequence = 1;

    fp = (l4_snd_fpage_t *) &msg.reg[0];
    chief_IPC_parent = l4_myself();

    l4_send_task_short(SERIAL_TID, &result,
         "\nCHIEF IPC TEST: Tests that redirected messages are received");
    l4_send_task_short(SERIAL_TID, &result,
	 " with a valid\n         destination attached. Assumes that ");
    l4_send_task_short(SERIAL_TID, &result,
	 "the functions l4_mips_ipc_send\n         and ");
    l4_send_task_short(SERIAL_TID, &result,
	 "l4_mips_ipc_send_deceiving work.\n");
    l4_send_task_short(SERIAL_TID, &result,
	 "         (chief_reply_and_wait not fully tested)\n");

/**********Create child***************************************/

    l4_send_task_short(SERIAL_TID, &result,
		       "Initialize Chief IPC Tester Child:\n");
    chief_IPC_child.ID = 0; chief_IPC_child.id.task = childtask;
    chief_IPC_child = l4_task_new(chief_IPC_child, l4_myself().ID,
			  ChiefIPCTestSP, (dword_t) chief_IPC_test_child,
				  pager_thread, l4_myself());
    l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,6,0,0), &result); /*0.25 sec*/
    l4_send_task_short_f(SERIAL_TID, &pmsg, &result,
			 "TID:0x%lx,IP:0x%lx,SP:0x%lx\n",
			 chief_IPC_child.ID, (dword_t) chief_IPC_test_child,
			 ChiefIPCTestSP);

/********** Test chief_wait **************************************/

    chief_IPC_test_macro(
	l4_mips_ipc_chief_wait(&src, &real_dest,
			       L4_IPC_SHORT_MSG, &msg, 
			       L4_IPC_NEVER, &result),
	"l4_mips_ipc_chief_wait(): ",
	msg, result);
    passed_all = chief_IPC_tester_report(SIGMA0_TID, real_dest,
					 chief_IPC_child, src,
					 &sequence, &msg) && passed_all;

/********* Test chief_receive ***********************************/
    chief_IPC_test_macro(
	l4_mips_ipc_chief_receive(chief_IPC_child, &real_dest,
				  L4_IPC_SHORT_MSG, &msg,
				  L4_IPC_NEVER, &result),
	"l4_mips_ipc_chief_receive(): ",
	msg, result);
    passed_all = chief_IPC_tester_report(SIGMA0_TID, real_dest,
					 chief_IPC_child, src,
					 &sequence, &msg) && passed_all;
    

/********* Test deceiving send from child ********************/
    l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0), &result); /*1 sec*/
    chief_IPC_test_macro(
	l4_mips_ipc_chief_wait(&src, &real_dest,
			       L4_IPC_SHORT_MSG, &msg, 
			       L4_IPC_NEVER, &result),
	"Child direction-preserving deceiving send: ",
	msg, result);
    passed_all = chief_IPC_tester_report(SIGMA0_TID, real_dest,
					 chief_IPC_grandchild, src,
					 &sequence, &msg) && passed_all;


    chief_IPC_test_macro(
	l4_mips_ipc_chief_wait(&src, &real_dest, L4_IPC_SHORT_MSG, &msg,
			       L4_IPC_NEVER, &result),
	"Not direction preserving: ",
	msg, result);
    passed_all = chief_IPC_tester_report(SIGMA0_TID, real_dest,
					 chief_IPC_child, src,
					 &sequence, &msg) && passed_all;

    /****** Test Closed Receive bug *******************************/
    l4_send_task_short(SERIAL_TID, &result,
		       "Closed receive bug (Receiving from a deceiving source)\n");
    l4_send_task_short(SERIAL_TID, &result,
		       "Sender IPCs before receiver:");
    l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0), &result); /*1 sec*/
    /*** Send first ***/
    l4_mips_ipc_chief_receive(chief_IPC_grandchild, &real_dest,
			      L4_IPC_SHORT_MSG, &msg,
			      L4_IPC_TIMEOUT(0,0,4,5,0,0), &result);
    if(result.msgdope & L4_IPC_RETIMEOUT) {
	l4_send_task_short(SERIAL_TID, &result, "FAILED\n");
	assert(0 ==
	   l4_mips_ipc_chief_wait(&src, &real_dest, L4_IPC_SHORT_MSG, &msg,
	   L4_IPC_NEVER, &result));
	passed_all = 0;
    } else {
	l4_send_task_short_f(SERIAL_TID, &pmsg, &result, \
			 "PASSED: Message from 0x%lx to 0x%lx\n", \
			  src.ID, real_dest.ID);
    }
    passed_all = chief_IPC_array_test(&sequence, &msg) && passed_all;
    /*** Receive first ***/
    l4_send_task_short(SERIAL_TID, &result,
		       "Receiver IPCs before sender:");
    l4_mips_ipc_chief_receive(chief_IPC_grandchild, &real_dest,
			      L4_IPC_SHORT_MSG, &msg,
			      L4_IPC_TIMEOUT(0,0,4,5,0,0), &result);
    if(result.msgdope & L4_IPC_RETIMEOUT) {
	l4_send_task_short(SERIAL_TID, &result, "FAILED\n");
	assert(0 ==
	   l4_mips_ipc_chief_wait(&src, &real_dest, L4_IPC_SHORT_MSG, &msg,
	   L4_IPC_NEVER, &result));
	passed_all = 0;
    } else {
	l4_send_task_short_f(SERIAL_TID, &pmsg, &result, \
			 "PASSED: Message from 0x%lx to 0x%lx\n", \
			  src.ID, real_dest.ID);
    }
    passed_all = chief_IPC_array_test(&sequence, &msg) && passed_all;

/********* Child send as chief tests *******************************/
    /*** chief send ************************************************/
    chief_IPC_test_macro(
	l4_mips_ipc_chief_wait(&src, &real_dest, L4_IPC_SHORT_MSG, &msg,
			       L4_IPC_NEVER, &result),
	"l4_mips_ipc_chief_send: ",
	msg, result);
    passed_all = chief_IPC_tester_report(SIGMA0_TID, real_dest,
					 chief_IPC_grandchild, src,
					 &sequence, &msg) && passed_all;

    
    /*** chief_call ************************************************/
    
    chief_IPC_dest = L4_NIL_ID; chief_IPC_src = L4_NIL_ID;
    chief_IPC_child_seq_match = 2;
    chief_IPC_test_macro(
	l4_mips_ipc_chief_wait(&src, &real_dest, L4_IPC_SHORT_MSG, &msg,
			       L4_IPC_NEVER, &result),
	"l4_mips_ipc_chief_call()[send]: ",
	msg, result);
    passed_all = chief_IPC_tester_report(SIGMA0_TID, real_dest,
					 chief_IPC_grandchild, src,
					 &sequence, &msg) && passed_all;
    l4_send_task_short(SERIAL_TID, &result,
		       "l4_mips_ipc_chief_call()[receive]:\n");
    chief_IPC_array_set(&sequence, &msg);
    l4_mips_ipc_chief_send(chief_IPC_child, SIGMA0_TID,
			   L4_IPC_SHORT_MSG, &msg,
			   L4_IPC_NEVER, &result);
    l4_send_task_short_f(SERIAL_TID, &pmsg, &result,
			 "Message from 0x%lx to 0x%lx\n", 
			 chief_IPC_src.ID, chief_IPC_dest.ID);
    if (chief_IPC_child_seq_match) {
	sequence -= 8; chief_IPC_array_set(&sequence, &msg); sequence -= 8;
	/* This is a horrible hack, it sets up the message to look right for
	   the test, "chief_IPC_child_seq_match" which gives the result of
	   the test carried out by the child. */
	passed_all = chief_IPC_tester_report(chief_IPC_child, chief_IPC_dest,
					     SIGMA0_TID, chief_IPC_src,
					     &sequence, &msg) && passed_all;
    } else {
	sequence -= 8; msg.reg[0] = 0;
	passed_all = chief_IPC_tester_report(chief_IPC_child, chief_IPC_dest,
					     SIGMA0_TID, chief_IPC_src,
					     &sequence, &msg) && passed_all;
    }

    /*** chief reply_and_wait **************************************/
    
    /* Reset communication variables and wait*/
    chief_IPC_dest = L4_NIL_ID; chief_IPC_src = L4_NIL_ID;
    chief_IPC_child_seq_match = 2;
    chief_IPC_test_macro(
	l4_mips_ipc_chief_wait(&src, &real_dest, L4_IPC_SHORT_MSG,&msg,
			       L4_IPC_NEVER, &result),
	"l4_mips_ipc_chief_reply_and_wait()[reply]: ",
	msg, result);
    /*Test reply*/
    passed_all = chief_IPC_tester_report(SIGMA0_TID, real_dest,
					 chief_IPC_grandchild, src,
					 &sequence, &msg) && passed_all;
    /* Send back message to waiting child */
    chief_IPC_array_set(&sequence, &msg);
    l4_send_task_short(SERIAL_TID, &result,
		       "l4_mips_ipc_chief_reply_and_wait()[wait]:\n");
    assert(0 == l4_mips_ipc_chief_send(chief_IPC_child, SIGMA0_TID,
				       L4_IPC_SHORT_MSG, &msg,
				       L4_IPC_NEVER, &result));
    while(thread_equal(chief_IPC_dest, L4_NIL_ID) || 
	  thread_equal(chief_IPC_src, L4_NIL_ID) || 
	  chief_IPC_child_seq_match == 2) {
	l4_thread_switch(L4_NIL_ID);
    }

    l4_send_task_short_f(SERIAL_TID, &pmsg, &result, \
			 "Message from 0x%lx to 0x%lx\n", \
			 chief_IPC_src.ID, chief_IPC_dest.ID);

    if (chief_IPC_child_seq_match) {
	sequence -= 8; chief_IPC_array_set(&sequence, &msg); sequence -= 8;
	/* This is a horrible hack, it sets up the message to look right for
	   the test, "chief_IPC_child_seq_match" gives the result of the test
	   carried out by the child. */
	passed_all = chief_IPC_tester_report(chief_IPC_child,
					     chief_IPC_dest,
					     SIGMA0_TID, chief_IPC_src,
					     &sequence, &msg) && passed_all;
    } else {
	sequence -= 8; msg.reg[0] = 0;
	passed_all = chief_IPC_tester_report(chief_IPC_child,
					     chief_IPC_dest,
					     SIGMA0_TID, chief_IPC_src,
					     &sequence, &msg) && passed_all;
    }

/********* End test ******************************************/
    if(passed_all) {
	l4_send_task_short(SERIAL_TID, &result,
			   "Chief IPC Test PASSED\n");
    } else {
	l4_send_task_short(SERIAL_TID, &result,
			   "Chief IPC Test FAILED\n");
    }
    /***** Kill Child *****/
    l4_task_new(chief_IPC_child, l4_myself().ID, 0, 0, L4_NIL_ID,
		l4_myself());
}
