#ifndef IRC_PARSE
#define IRC_PARSE

#define NOT_VALID_TAG 	-2
#define NOT_VALID 	-1

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

struct tag {
	char *tname;
	char *value;
};

struct tag_list {
	struct tag *list;
	int size;	
};

struct source {
	char *name;
	char *user;
	char *host;
};

struct param {
	char **params;
	char *data;
	char *last;
};

struct irc_msg {
	struct tag_list  tlist;
	struct source 	 source;
	struct param  	 param;
	char 		*raw_msg;
};

/*
 * Will parse an tag string until '\0'.
 * This function expects and string
 * that starts with '@' and ends with '\0'.
 */
extern int tag_parse(struct tag_list*, char *);

/*
 * Will parse source string until '\0'.
 *
 * This function expects the string to start
 * with ':'
 *
 * if the source only has the host,
 * name & user & host will be the same value.
 */
extern int source_parse(struct source *, char *);

/*
 * Will parse params and command until '\0'.
 *
 * command will always be the first
 * in the params array.
 *
 * If encounter ':' everything after will be the 
 * las param.
 */
extern int param_parse(struct param *, char *);

extern char *
search_tag_name(const char *, struct tag_list *);

extern struct irc_msg *
irc_msg_alloc(void);

extern void
irc_msg_free(struct irc_msg *);

extern struct irc_msg *
irc_parse_message(char *);

extern int
irc_parse(struct irc_msg *,char *);

extern char *_rtrim(char *);
extern char *_ltrim(char *);
extern char *_trim(char *);
#endif
