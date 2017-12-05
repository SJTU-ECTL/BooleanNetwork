/**
  @file

  @ingroup nanotrav

  @brief Functions to read in a boolean network.

  @author Fabio Somenzi

  @copyright@parblock
  Copyright (c) 1995-2015, Regents of the University of Colorado

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  Neither the name of the University of Colorado nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
  @endparblock

*/

#include "cudd_bnet.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

#define MAXLENGTH 131072

/*---------------------------------------------------------------------------*/
/* Stucture declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/* Type of comparison function for qsort. */
typedef int (*DD_QSFP)(const void *, const void *);

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

static	char	BuffLine[MAXLENGTH];
static	char	*CurPos;
static	int	newNameNumber = 0;

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/** \cond */

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static char * readString (FILE *fp);
static char ** readList (FILE *fp, int *n);
static void printList (char **list, int n);
static char ** bnetGenerateNewNames (st_table *hash, int n);
static int bnetSetLevel (BnetNetwork *net);
static int bnetLevelDFS (BnetNetwork *net, BnetNode *node);
static BnetNode ** bnetOrderRoots (BnetNetwork *net, int *nroots);
static int bnetLevelCompare (BnetNode **x, BnetNode **y);

/** \endcond */


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**
  @brief Reads boolean network from blif file.

  @details A very restricted subset of blif is supported. Specifically:
  <ul>
  <li> The only directives recognized are:
    <ul>
    <li> .model
    <li> .inputs
    <li> .outputs
    <li> .latch
    <li> .names
    <li> .exdc
    <li> .wire_load_slope
    <li> .end
    </ul>
  <li> Latches must have an initial values and no other parameters
       specified.
  <li> Lines must not exceed MAXLENGTH-1 characters, and individual names must
       not exceed 1023 characters.
  </ul>
  Caveat emptor: There may be other limitations as well. One should
  check the syntax of the blif file with some other tool before relying
  on this parser.

  @return a pointer to the network if successful; NULL otherwise.
  
  @sideeffect None

  @see Bnet_PrintNetwork Bnet_FreeNetwork

*/
BnetNetwork *
Bnet_ReadNetwork(
  FILE * fp /**< pointer to the blif file */,
  int  pr /**< verbosity level */)
{
    char *savestring;
    char **list;
    int i, j, n;
    BnetNetwork *net;
    BnetNode *newnode;
    BnetNode *lastnode = NULL;
    BnetTabline *newline;
    BnetTabline *lastline;
    char ***latches = NULL;
    int maxlatches = 0;
    int exdc = 0;
    BnetNode	*node;
    int	count;

    /* Allocate network object and initialize symbol table. */
    net = ALLOC(BnetNetwork,1);
    if (net == NULL) goto failure;
    memset((char *) net, 0, sizeof(BnetNetwork));
    net->hash = st_init_table((st_compare_t) strcmp, st_strhash);
    if (net->hash == NULL) goto failure;

    savestring = readString(fp);
    if (savestring == NULL) goto failure;
    net->nlatches = 0;
    while (strcmp(savestring, ".model") == 0 ||
	strcmp(savestring, ".inputs") == 0 ||
	strcmp(savestring, ".outputs") == 0 ||
	strcmp(savestring, ".latch") == 0 ||
	strcmp(savestring, ".wire_load_slope") == 0 ||
	strcmp(savestring, ".exdc") == 0 ||
	strcmp(savestring, ".names") == 0 || strcmp(savestring,".end") == 0) {
	if (strcmp(savestring, ".model") == 0) {
	    /* Read .model directive. */
	    FREE(savestring);
	    /* Read network name. */
	    savestring = readString(fp);
	    if (savestring == NULL) goto failure;
	    if (savestring[0] == '.') {
		net->name = ALLOC(char,  1);
		if (net->name == NULL) goto failure;
                net->name[0] = '\0';
	    } else {
		net->name = savestring;
	    }
	} else if (strcmp(savestring, ".inputs") == 0) {
	    /* Read .inputs directive. */
	    FREE(savestring);
	    /* Read input names. */
	    list = readList(fp,&n);
	    if (list == NULL) goto failure;
	    if (pr > 2) printList(list,n);
	    /* Expect at least one input. */
	    if (n < 1) {
		(void) fprintf(stdout,"Empty input list.\n");
		goto failure;
	    }
	    if (exdc) {
		for (i = 0; i < n; i++)
		    FREE(list[i]);
		FREE(list);
		savestring = readString(fp);
		if (savestring == NULL) goto failure;
		continue;
	    }
	    if (net->ninputs) {
		net->inputs = REALLOC(char *, net->inputs,
		    (net->ninputs + n) * sizeof(char *));
		for (i = 0; i < n; i++)
		    net->inputs[net->ninputs + i] = list[i];
	    }
	    else
		net->inputs = list;
	    /* Create a node for each primary input. */
	    for (i = 0; i < n; i++) {
		newnode = ALLOC(BnetNode,1);
		memset((char *) newnode, 0, sizeof(BnetNode));
		if (newnode == NULL) goto failure;
		newnode->name = list[i];
		newnode->inputs = NULL;
		newnode->type = BNET_INPUT_NODE;
		newnode->active = FALSE;
		newnode->nfo = 0;
		newnode->ninp = 0;
		newnode->f = NULL;
		newnode->polarity = 0;
		newnode->next = NULL;
		if (lastnode == NULL) {
		    net->nodes = newnode;
		} else {
		    lastnode->next = newnode;
		}
		lastnode = newnode;
	    }
	    net->npis += n;
	    net->ninputs += n;
	} else if (strcmp(savestring, ".outputs") == 0) {
	    /* Read .outputs directive. We do not create nodes for the primary
	    ** outputs, because the nodes will be created when the same names
	    ** appear as outputs of some gates.
	    */
	    FREE(savestring);
	    /* Read output names. */
	    list = readList(fp,&n);
	    if (list == NULL) goto failure;
	    if (pr > 2) printList(list,n);
	    if (n < 1) {
		(void) fprintf(stdout,"Empty .outputs list.\n");
		goto failure;
	    }
	    if (exdc) {
		for (i = 0; i < n; i++)
		    FREE(list[i]);
		FREE(list);
		savestring = readString(fp);
		if (savestring == NULL) goto failure;
		continue;
	    }
	    if (net->noutputs) {
		net->outputs = REALLOC(char *, net->outputs,
		    (net->noutputs + n) * sizeof(char *));
		for (i = 0; i < n; i++)
		    net->outputs[net->noutputs + i] = list[i];
	    } else {
		net->outputs = list;
	    }
	    net->npos += n;
	    net->noutputs += n;
	} else if (strcmp(savestring,".wire_load_slope") == 0) {
	    FREE(savestring);
	    savestring = readString(fp);
	    net->slope = savestring;
	} else if (strcmp(savestring,".latch") == 0) {
	    FREE(savestring);
	    newnode = ALLOC(BnetNode,1);
	    if (newnode == NULL) goto failure;
	    memset((char *) newnode, 0, sizeof(BnetNode));
	    newnode->type = BNET_PRESENT_STATE_NODE;
	    list = readList(fp,&n);
	    if (list == NULL) goto failure;
	    if (pr > 2) printList(list,n);
	    /* Expect three names. */
	    if (n != 3) {
		(void) fprintf(stdout,
			       ".latch not followed by three tokens.\n");
		goto failure;
	    }
	    newnode->name = list[1];
	    newnode->inputs = NULL;
	    newnode->ninp = 0;
	    newnode->f = NULL;
	    newnode->active = FALSE;
	    newnode->nfo = 0;
	    newnode->polarity = 0;
	    newnode->next = NULL;
	    if (lastnode == NULL) {
		net->nodes = newnode;
	    } else {
		lastnode->next = newnode;
	    }
	    lastnode = newnode;
	    /* Add next state variable to list. */
	    if (maxlatches == 0) {
		maxlatches = 20;
		latches = ALLOC(char **,maxlatches);
	    } else if (maxlatches <= net->nlatches) {
		maxlatches += 20;
		latches = REALLOC(char **,latches,maxlatches);
	    }
	    latches[net->nlatches] = list;
	    net->nlatches++;
	    savestring = readString(fp);
	    if (savestring == NULL) goto failure;
	} else if (strcmp(savestring,".names") == 0) {
	    FREE(savestring);
	    newnode = ALLOC(BnetNode,1);
	    memset((char *) newnode, 0, sizeof(BnetNode));
	    if (newnode == NULL) goto failure;
	    list = readList(fp,&n);
	    if (list == NULL) goto failure;
	    if (pr > 2) printList(list,n);
	    /* Expect at least one name (the node output). */
	    if (n < 1) {
		(void) fprintf(stdout,"Missing output name.\n");
		goto failure;
	    }
	    newnode->name = list[n-1];
	    newnode->inputs = list;
	    newnode->ninp = n-1;
	    newnode->active = FALSE;
	    newnode->nfo = 0;
	    newnode->polarity = 0;
	    if (newnode->ninp > 0) {
		newnode->type = BNET_INTERNAL_NODE;
		for (i = 0; i < net->noutputs; i++) {
		    if (strcmp(net->outputs[i], newnode->name) == 0) {
			newnode->type = BNET_OUTPUT_NODE;
			break;
		    }
		}
	    } else {
		newnode->type = BNET_CONSTANT_NODE;
	    }
	    newnode->next = NULL;
	    if (lastnode == NULL) {
		net->nodes = newnode;
	    } else {
		lastnode->next = newnode;
	    }
	    lastnode = newnode;
	    /* Read node function. */
	    newnode->f = NULL;
	    if (exdc) {
		newnode->exdc_flag = 1;
		node = net->nodes;
		while (node) {
		    if (node->type == BNET_OUTPUT_NODE &&
			strcmp(node->name, newnode->name) == 0) {
			node->exdc = newnode;
			break;
		    }
		    node = node->next;
		}
	    }
	    savestring = readString(fp);
	    if (savestring == NULL) goto failure;
	    lastline = NULL;
	    while (savestring[0] != '.') {
		/* Reading a table line. */
		newline = ALLOC(BnetTabline,1);
		if (newline == NULL) goto failure;
		newline->next = NULL;
		if (lastline == NULL) {
		    newnode->f = newline;
		} else {
		    lastline->next = newline;
		}
		lastline = newline;
		if (newnode->type == BNET_INTERNAL_NODE ||
		    newnode->type == BNET_OUTPUT_NODE) {
		    newline->values = savestring;
		    /* Read output 1 or 0. */
		    savestring = readString(fp);
		    if (savestring == NULL) goto failure;
		} else {
		    newline->values = NULL;
		}
		if (savestring[0] == '0') newnode->polarity = 1;
		FREE(savestring);
		savestring = readString(fp);
		if (savestring == NULL) goto failure;
	    }
	} else if (strcmp(savestring,".exdc") == 0) {
	    FREE(savestring);
	    exdc = 1;
	} else if (strcmp(savestring,".end") == 0) {
	    FREE(savestring);
	    break;
	}
	if ((!savestring) || savestring[0] != '.')
	    savestring = readString(fp);
	if (savestring == NULL) goto failure;
    }

    /* Put nodes in symbol table. */
    newnode = net->nodes;
    while (newnode != NULL) {
	int retval = st_insert(net->hash,newnode->name,(char *) newnode);
	if (retval == ST_OUT_OF_MEM) {
	    goto failure;
	} else if (retval == 1) {
	    printf("Error: Multiple drivers for node %s\n", newnode->name);
	    goto failure;
	} else {
	    if (pr > 2) printf("Inserted %s\n",newnode->name);
	}
	newnode = newnode->next;
    }

    if (latches) {
	net->latches = latches;

	count = 0;
	net->outputs = REALLOC(char *, net->outputs,
	    (net->noutputs + net->nlatches) * sizeof(char *));
	for (i = 0; i < net->nlatches; i++) {
	    for (j = 0; j < net->noutputs; j++) {
		if (strcmp(latches[i][0], net->outputs[j]) == 0)
		    break;
	    }
	    if (j < net->noutputs)
		continue;
	    savestring = ALLOC(char, strlen(latches[i][0]) + 1);
	    strcpy(savestring, latches[i][0]);
	    net->outputs[net->noutputs + count] = savestring;
	    count++;
	    if (st_lookup(net->hash, savestring, (void **) &node)) {
		if (node->type == BNET_INTERNAL_NODE) {
		    node->type = BNET_OUTPUT_NODE;
		}
	    }
	}
	net->noutputs += count;

	net->inputs = REALLOC(char *, net->inputs,
	    (net->ninputs + net->nlatches) * sizeof(char *));
	for (i = 0; i < net->nlatches; i++) {
	    savestring = ALLOC(char, strlen(latches[i][1]) + 1);
	    strcpy(savestring, latches[i][1]);
	    net->inputs[net->ninputs + i] = savestring;
	}
	net->ninputs += net->nlatches;
    }

    /* Compute fanout counts. For each node in the linked list, fetch
    ** all its fanins using the symbol table, and increment the fanout of
    ** each fanin.
    */
    newnode = net->nodes;
    while (newnode != NULL) {
	BnetNode *auxnd;
	for (i = 0; i < newnode->ninp; i++) {
	    if (!st_lookup(net->hash,newnode->inputs[i],(void **)&auxnd)) {
		(void) fprintf(stdout,"%s not driven\n", newnode->inputs[i]);
		goto failure;
	    }
	    auxnd->nfo++;
	}
	newnode = newnode->next;
    }

    if (!bnetSetLevel(net)) goto failure;

    return(net);

failure:
    /* Here we should clean up the mess. */
    (void) fprintf(stdout,"Error in reading network from file.\n");
    return(NULL);

} /* end of Bnet_ReadNetwork */


/**
  @brief Prints to stdout a boolean network created by Bnet_ReadNetwork.

  @details Uses the blif format; this way, one can verify the
  equivalence of the input and the output with, say, sis.

  @sideeffect None

  @see Bnet_ReadNetwork

*/
void
Bnet_PrintNetwork(
  BnetNetwork * net /**< boolean network */)
{
    BnetNode *nd;
    BnetTabline *tl;
    int i;

    if (net == NULL) return;

    (void) fprintf(stdout,".model %s\n", net->name);
    (void) fprintf(stdout,".inputs");
    printList(net->inputs,net->npis);
    (void) fprintf(stdout,".outputs");
    printList(net->outputs,net->npos);
    for (i = 0; i < net->nlatches; i++) {
	(void) fprintf(stdout,".latch");
	printList(net->latches[i],3);
    }
    nd = net->nodes;
    while (nd != NULL) {
	if (nd->type != BNET_INPUT_NODE && nd->type != BNET_PRESENT_STATE_NODE) {
	    (void) fprintf(stdout,".names");
	    for (i = 0; i < nd->ninp; i++) {
		(void) fprintf(stdout," %s",nd->inputs[i]);
	    }
	    (void) fprintf(stdout," %s\n",nd->name);
	    tl = nd->f;
	    while (tl != NULL) {
		if (tl->values != NULL) {
		    (void) fprintf(stdout,"%s %d\n",tl->values,
		    1 - nd->polarity);
		} else {
		    (void) fprintf(stdout,"%d\n", 1 - nd->polarity);
		}
		tl = tl->next;
	    }
	}
	nd = nd->next;
    }
    (void) fprintf(stdout,".end\n");

} /* end of Bnet_PrintNetwork */


/**
  @brief Frees a boolean network created by Bnet_ReadNetwork.

  @sideeffect None

  @see Bnet_ReadNetwork

*/
void
Bnet_FreeNetwork(
  BnetNetwork * net)
{
    BnetNode *node, *nextnode;
    BnetTabline *line, *nextline;
    int i;

    FREE(net->name);
    /* The input name strings are already pointed by the input nodes.
    ** Here we only need to free the latch names and the array that
    ** points to them.
    */
    for (i = 0; i < net->nlatches; i++) {
	FREE(net->inputs[net->npis + i]);
    }
    FREE(net->inputs);
    /* Free the output name strings and then the array pointing to them.  */
    for (i = 0; i < net->noutputs; i++) {
	FREE(net->outputs[i]);
    }
    FREE(net->outputs);

    for (i = 0; i < net->nlatches; i++) {
	FREE(net->latches[i][0]);
	FREE(net->latches[i][1]);
	FREE(net->latches[i][2]);
	FREE(net->latches[i]);
    }
    if (net->nlatches) FREE(net->latches);
    node = net->nodes;
    while (node != NULL) {
	nextnode = node->next;
	if (node->type != BNET_PRESENT_STATE_NODE)
	    FREE(node->name);
	for (i = 0; i < node->ninp; i++) {
	    FREE(node->inputs[i]);
	}
	if (node->inputs != NULL) {
	    FREE(node->inputs);
	}
	/* Free the function table. */
	line = node->f;
	while (line != NULL) {
	    nextline = line->next;
	    FREE(line->values);
	    FREE(line);
	    line = nextline;
	}
	FREE(node);
	node = nextnode;
    }
    st_free_table(net->hash);
    if (net->slope != NULL) FREE(net->slope);
    FREE(net);

} /* end of Bnet_FreeNetwork */


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/


/**
  @brief Reads a string from a file.

  @details The string can be MAXLENGTH-1 characters at
  most. readString allocates memory to hold the string.

  @return a pointer to the result string if successful. It returns
  NULL otherwise.

  @sideeffect None

  @see readList

*/
static char *
readString(
  FILE * fp /**< pointer to the file from which the string is read */)
{
    char *savestring;
    int length;

    while (!CurPos) {
	if (!fgets(BuffLine, MAXLENGTH, fp))
	    return(NULL);
	BuffLine[strlen(BuffLine) - 1] = '\0';
	CurPos = strtok(BuffLine, " \t");
	if (CurPos && CurPos[0] == '#') CurPos = (char *)NULL;
    }
    length = strlen(CurPos);
    savestring = ALLOC(char,length+1);
    if (savestring == NULL)
	return(NULL);
    strcpy(savestring,CurPos);
    CurPos = strtok(NULL, " \t");
    return(savestring);

} /* end of readString */


/**
  @brief Reads a list of strings from a line of a file.

  @details The strings are sequences of characters separated by spaces
  or tabs.  The total length of the list, white space included, must
  not exceed MAXLENGTH-1 characters.  readList allocates memory for
  the strings and creates an array of pointers to the individual
  lists. Only two pieces of memory are allocated by readList: One to
  hold all the strings, and one to hold the pointers to
  them. Therefore, when freeing the memory allocated by readList, only
  the pointer to the list of pointers, and the pointer to the
  beginning of the first string should be freed.

  @return the pointer to the list of pointers if successful; NULL
  otherwise.

  @sideeffect n is set to the number of strings in the list.

  @see readString printList

*/
static char **
readList(
  FILE * fp /**< pointer to the file from which the list is read */,
  int * n /**< on return, number of strings in the list */)
{
    char	*savestring;
    int		length;
    char	*stack[8192];
    char	**list;
    int		i, count = 0;

    while (CurPos) {
	if (strcmp(CurPos, "\\") == 0) {
	    CurPos = (char *)NULL;
	    while (!CurPos) {
		if (!fgets(BuffLine, MAXLENGTH, fp)) return(NULL);
		BuffLine[strlen(BuffLine) - 1] = '\0';
		CurPos = strtok(BuffLine, " \t");
	    }
	}
	length = strlen(CurPos);
	savestring = ALLOC(char,length+1);
	if (savestring == NULL) return(NULL);
	strcpy(savestring,CurPos);
	stack[count] = savestring;
	count++;
	CurPos = strtok(NULL, " \t");
    }
    list = ALLOC(char *, count);
    for (i = 0; i < count; i++)
	list[i] = stack[i];
    *n = count;
    return(list);

} /* end of readList */


/**
  @brief Prints a list of strings to the standard output.

  @details The list is in the format created by readList.

  @sideeffect None

  @see readList Bnet_PrintNetwork

*/
static void
printList(
  char ** list /**< list of pointers to strings */,
  int  n /**< length of the list */)
{
    int i;

    for (i = 0; i < n; i++) {
	(void) fprintf(stdout," %s",list[i]);
    }
    (void) fprintf(stdout,"\n");

} /* end of printList */


/**
  @brief Generates n names not currently in a symbol table.

  @details The pointer to the symbol table may be NULL, in which case
  no test is made. The names generated by the procedure are
  unique. So, if there is no possibility of conflict with pre-existing
  names, NULL can be passed for the hash table.

  @return an array of names if succesful; NULL otherwise.

  @sideeffect None

  @see 

*/
static char **
bnetGenerateNewNames(
  st_table * hash /* table of existing names (or NULL) */,
  int  n /* number of names to be generated */)
{
    char **list;
    char name[256];
    int i;

    if (n < 1) return(NULL);

    list = ALLOC(char *,n);
    if (list == NULL) return(NULL);
    for (i = 0; i < n; i++) {
	do {
	    sprintf(name, "var%d", newNameNumber);
	    newNameNumber++;
	} while (hash != NULL && st_is_member(hash,name));
	list[i] = util_strsav(name);
    }

    return(list);

} /* bnetGenerateNewNames */


/**
  @brief Writes blif for the truth table of an n-input xnor.

  @return 1 if successful; 0 otherwise.

  @sideeffect None

*/
#if 0
static int
bnetBlifXnorTable(
  FILE * fp /**< file pointer */,
  int  n /**< number of inputs */)
{
    int power;	/* 2 to the power n */
    int i,j,k;
    int nzeroes;
    int retval;
    char *line;

    line = ALLOC(char,n+1);
    if (line == NULL) return(0);
    line[n] = '\0';

    for (i = 0, power = 1; i < n; i++) {
	power *= 2;
    }

    for (i = 0; i < power; i++) {
	k = i;
	nzeroes = 0;
	for (j = 0; j < n; j++) {
	    if (k & 1) {
		line[j] = '1';
	    } else {
		line[j] = '0';
		nzeroes++;
	    }
	    k >>= 1;
	}
	if ((nzeroes & 1) == 0) {
	    retval = fprintf(fp,"%s 1\n",line);
	    if (retval == 0) return(0);
	}
    }
    return(1);

} /* end of bnetBlifXnorTable */
#endif


/**
  @brief Sets the level of each node.

  @return 1 if successful; 0 otherwise.

  @sideeffect Changes the level and visited fields of the nodes it
  visits.

  @see bnetLevelDFS

*/
static int
bnetSetLevel(
  BnetNetwork * net)
{
    BnetNode *node;

    /* Recursively visit nodes. This is pretty inefficient, because we
    ** visit all nodes in this loop, and most of them in the recursive
    ** calls to bnetLevelDFS. However, this approach guarantees that
    ** all nodes will be reached ven if there are dangling outputs. */
    node = net->nodes;
    while (node != NULL) {
	if (!bnetLevelDFS(net,node)) return(0);
	node = node->next;
    }

    /* Clear visited flags. */
    node = net->nodes;
    while (node != NULL) {
	node->visited = 0;
	node = node->next;
    }
    return(1);

} /* end of bnetSetLevel */


/**
  @brief Does a DFS from a node setting the level field.

  @return 1 if successful; 0 otherwise.

  @sideeffect Changes the level and visited fields of the nodes it
  visits.

  @see bnetSetLevel

*/
static int
bnetLevelDFS(
  BnetNetwork * net,
  BnetNode * node)
{
    int i;
    BnetNode *auxnd;

    if (node->visited == 1) {
	return(1);
    }

    node->visited = 1;

    /* Graphical sources have level 0.  This is the final value if the
    ** node has no fan-ins. Otherwise the successive loop will
    ** increase the level. */
    node->level = 0;
    for (i = 0; i < node->ninp; i++) {
	if (!st_lookup(net->hash, node->inputs[i], (void **) &auxnd)) {
	    return(0);
	}
	if (!bnetLevelDFS(net,auxnd)) {
	    return(0);
	}
	if (auxnd->level >= node->level) node->level = 1 + auxnd->level;
    }
    return(1);

} /* end of bnetLevelDFS */


/**
  @brief Orders network roots for variable ordering.

  @return an array with the ordered outputs and next state variables
  if successful; NULL otherwise.

  @sideeffect None

*/
static BnetNode **
bnetOrderRoots(
  BnetNetwork * net,
  int * nroots)
{
    int i, noutputs;
    BnetNode *node;
    BnetNode **nodes = NULL;

    /* Initialize data structures. */
    noutputs = net->noutputs;
    nodes = ALLOC(BnetNode *, noutputs);
    if (nodes == NULL) goto endgame;

    /* Find output names and levels. */
    for (i = 0; i < net->noutputs; i++) {
	if (!st_lookup(net->hash,net->outputs[i],(void **)&node)) {
	    goto endgame;
	}
	nodes[i] = node;
    }

    util_qsort(nodes, noutputs, sizeof(BnetNode *),
               (DD_QSFP)bnetLevelCompare);
    *nroots = noutputs;
    return(nodes);

endgame:
    if (nodes != NULL) FREE(nodes);
    return(NULL);

} /* end of bnetOrderRoots */


/**
  @brief Comparison function used by qsort.

  @details Used to order the variables according to the number of keys
  in the subtables.

  @return the difference in number of keys between the two variables
  being compared.

  @sideeffect None

*/
static int
bnetLevelCompare(
  BnetNode ** x,
  BnetNode ** y)
{
    return((*y)->level - (*x)->level);

} /* end of bnetLevelCompare */
