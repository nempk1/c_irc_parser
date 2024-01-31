#include "irc_parse.h"

char *_ltrim(char *s)
{
    	while(isspace(*s))
	    	s++;
    	return s;
}

char *_rtrim(char *s)
{
	char* back = s + strlen(s);

	/* Loop backwards until 
	 * non space character
	 */
    	while(isspace(*--back));

	/* New 'end' set */
	*(back+1) = '\0';
	return s;
}

char *_trim(char *s)
{
    	return _rtrim(_ltrim(s));
}

int tag_parse(struct tag_list* tags, char *buffer)
{
	int counter = 0;
	volatile int tag_count 	= 0;
	struct tag *tptr;

	char *run = buffer;
	while(*run++ != '\0') {
		if(*run == ';')
			tag_count++;	
	}

	tags->list = calloc(tag_count+1, sizeof(struct tag));
	tags->size = tag_count;	
	tptr = &tags->list[tag_count];
	tptr = NULL;

	buffer++;
	do {
		char *tmp = strsep(&buffer, ";");		
		char *res = strsep(&tmp, "=");

		(tags->list + counter)->tname = res;

		if(!tmp) 
			(tags->list + counter)->value = NULL;
		else
			(tags->list + counter)->value = tmp;

		counter++;

	}while(counter < tag_count &&
			buffer != NULL);	
	return 0;
}

int source_parse(struct source *src, char *buffer)
{
	char *tmp;

	if(*buffer != ':')
		return NOT_VALID;

	buffer++;

	if(!strpbrk(buffer, "!@")) {
		src->name = buffer;
		src->user = buffer;
		src->host = buffer;
		return 1;
	} 

	tmp = strsep(&buffer, "!");
	src->name = tmp;
	tmp = strsep(&buffer, "@");
	src->user = tmp;
	src->host = buffer;
	return 0;
}

int param_parse(struct param *prm, char *buffer)
{
	char **tmp = NULL;
	char *p = strsep(&buffer, ":");
	int count = 0;

	char *run = p;
	
	_rtrim(run);

	
	while(*run++ != '\0') {
		if(*run == ' ') {
			count+=2;
		}
	}

	prm->params = calloc(count, sizeof(char*));
	if(!prm->params) {
		errno = ENOMEM;
		return -1;
	}
	prm->params[count-1] = NULL;

	int count2 = 0;
	tmp = prm->params;
	while(p != NULL) {
		tmp[count2] = strsep(&p, " ");
		count2++;	
	}
	tmp[count2-1] = NULL;

	if(!buffer) {
		prm->last = tmp[count2-1];

	} else {
		prm->last = buffer;
	}
	return 0;
}

struct irc_msg *
irc_msg_alloc()
{
	struct irc_msg *res = malloc(sizeof(*res));
	if(!res) {
		errno = ENOMEM;
		return NULL;
	}

	res->raw_msg = NULL;
	res->param.params = NULL;
	res->tlist.list = NULL;

	return res;
}

void irc_msg_free(struct irc_msg *ptr)
{
	free(ptr->param.params);
	free(ptr->tlist.list);
	free(ptr->raw_msg);
	free(ptr);
}

struct irc_msg *
irc_parse_message(char *r_msg)
{
	char *tmp = NULL;
	struct irc_msg *res = irc_msg_alloc();
	if(res == NULL)
		return NULL;
	res->raw_msg = r_msg;

	if(*r_msg == '@') {
		tmp = strsep(&r_msg, " ");
		tag_parse(&res->tlist, tmp);
	}
	if(*r_msg == ':') {
		tmp = strsep(&r_msg, " ");
		source_parse(&res->source, tmp);
	}

	param_parse(&res->param, r_msg);

	return res;
}
int
irc_parse(struct irc_msg *res, char *r_msg)
{
	char *tmp = NULL;
	if(res == NULL)
		return -1;
	res->raw_msg = r_msg;

	if(*r_msg == '@') {
		tmp = strsep(&r_msg, " ");
		tag_parse(&res->tlist, tmp);
	}
	if(*r_msg == ':') {
		tmp = strsep(&r_msg, " ");
		source_parse(&res->source, tmp);
	}

	param_parse(&res->param, r_msg);

	return 0;
}

char *search_tag_name(const char *tag_name, struct tag_list *t_list)
{
	struct tag *tag_ptr = t_list->list;		
	int i = 0; 

	/* Loop until not more tag's found
	 * or end of tag_list.
	 */
	while(tag_ptr != NULL && i < t_list->size) {
		
		if(!strcmp(tag_name, tag_ptr->tname)) {
			return tag_ptr->value;
		}

		/* increment tag's list index 
		 * and size counter.*/
		tag_ptr++; i++;
	}
	return NULL;
}
