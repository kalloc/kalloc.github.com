#include "var.h"
/*
unsigned hash(char *s)
{
 unsigned hashval;

 for(hashval=0;*s!='\0';s++)
 hashval=*s+31*hashval;

 return hashval % HASHSIZE;
}
*/
unsigned hash (char *namearg)
{
  const unsigned char *name = (const unsigned char *) namearg;
  unsigned h = 5381;
  unsigned char ch;

  while ((ch = *name++) != '\0')
    h = (h << 5) + h + ch;
  return h  % 1024;
}




struct nlist *lookup(struct nlist *hashtab[],char *s)
{

 struct nlist *np;
 
 for(np=hashtab[hash(s)];np != NULL;np = np->next)
	if(strcmp(s,np->name) == 0)
		return np;
 return NULL;
}


int install(struct nlist *hashtab[],char *name,char *value)
{


 struct nlist *np;
 unsigned hashval;
 np =lookup(hashtab,name);
 if(np == NULL) {
	np= (struct nlist *) malloc(sizeof(*np));
	if(np == NULL || (np->name = strdup(name)) == NULL)
		 return 0;

	 hashval=hash(name);
	 np->next=hashtab[hashval];
	 hashtab[hashval]=np;
 } 

 np->value = strdup(value);

 return 1;

}


void print_hashtab(struct nlist *hashtab)
{
// int i;
}

