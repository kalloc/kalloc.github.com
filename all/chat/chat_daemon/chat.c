/*
 * TAILQ example program.
 */

#include <stdlib.h>
#include <stdio.h>
#include <search.h>
#include <qtl_tailq.h>

#define NAME_SIZE 30
#define MAXLINE 1024
#define MAX_MSG_SIZE 5
//á¤¥« «¨ áâàãªâàã á®®¡é¥­¨©




typedef struct
{
  int uid;
  time_t time;
  char name[NAME_SIZE + 1];
  char string[MAXLINE + 1];
} msgs_struct;


TQ_DEFINE (places)
     int id;
     int mid;
     char name[NAME_SIZE + 1];
     msgs_struct msgs[MAX_MSG_SIZE + 1];
TQ_DEFINE_END (places)
     void *places_root = NULL;
     static int place_max_id = 0;

     time_t now, user;

     static int mid = 0;
     static int uid = 1;


     void print_node (arg, order, level)
     void **arg;
     VISIT order;
     int level;
{
  if (order == preorder || order == leaf) {
    TQ_VAR (places) * node;
    node = (TQ_VAR (places) *) * arg;
    printf ("place ID:%i -  %s. last mid: %d. addr=0x%x\n", node->id,
	    node->name, node->mid, arg);
  }
}

int
id_compare (const void *pa, const void *pb)
{

  TQ_VAR (places) * left, *right;
  left = (TQ_VAR (places) *) pa;
  right = (TQ_VAR (places) *) pb;
  if (left->id < right->id)
    return -1;
  else if (left->id > right->id)
    return 1;
  else
    return 0;
}



int
name_compare (const void *pa, const void *pb)
{

  TQ_VAR (places) * left, *right;
  left = (TQ_VAR (places) *) pa;
  right = (TQ_VAR (places) *) pb;
  //printf("try to cmp %s and %s\n",left->name,right->name);
  return (strcmp (left->name, right->name));
}





int
create_place (char *name)
{

  TQ_VAR (places) * place, query;
  strncpy (query.name, name, NAME_SIZE);

  if (tfind (&query, &places_root, name_compare)) {
    //printf("place %s, already exists\n",name);
    return 0;
  }
  else {


    place_max_id++;
    printf ("place %s, added\n", name);
    place = TQ_MALLOC (places);
    bzero (place, sizeof (TQ_VAR (places)));
    strncpy (place->name, name, NAME_SIZE);
    place->id = place_max_id;
    place->mid = 0;
    tsearch (place, &places_root, name_compare);
    TQ_INSERT (places, place);
    return place->id;
  }
  return 0;

}

TQ_VAR (places) * find_place_by_name (char *name)
{

  TQ_VAR (places) ** place, query;
  strncpy (query.name, name, NAME_SIZE);

  if ((place = tfind (&query, &places_root, name_compare))) {
    //printf("place %s, found, ret ptr 0%x\n",name,*place);
    return *place;
  }
  else {
    //printf("place %s, not found\n",name);
  }
  return 0;
}

TQ_VAR (places) * find_place_by_id (int pid)
{

  TQ_VAR (places) ** place, query;
  query.id = pid;
  if ((place = tfind (&query, &places_root, id_compare))) {
    //printf("place %s, found, ret ptr 0%x\n",(*place)->name,*place);
    return *place;
  }
  else {
    //printf("place ID:%i, not found\n",pid);
  }
  return 0;
}


int
chat_add_msg (char *placename, char *name, char *text)
{

  TQ_VAR (places) * place;
  msgs_struct *msg;
  if ((place = find_place_by_name (placename)) == 0) {
    return 0;
  }
  if (MAX_MSG_SIZE < place->mid)
    place->mid = 0;
  msg = &place->msgs[place->mid++];
  strncpy (msg->name, name, NAME_SIZE);
  strncpy (msg->string, text, MAXLINE);
  msg->uid = uid;
  msg->time = ++now;
  printf ("add msg to %s from:%s, msg:%s\n", place->name, msg->name,
	  msg->string);
  return (1);
}


int
chat_get_msg (char *placename)
{

  TQ_VAR (places) * place;
  msgs_struct *msg;
  if ((place = find_place_by_name (placename)) == 0) {
    return 0;
  }

  printf ("try to get message list\n");
  int stop;
// 
  for (stop = mid; place->msgs[stop].time > user;) {
    msg = &place->msgs[stop];
    printf ("%s->%s:%s\n", place->name, msg->name, msg->string);
    user = msg->time;
    if (stop >= MAX_MSG_SIZE)
      stop = 0;
    else
      ++stop;
  }
  mid = stop;
  return (1);
}


int
main (int argc, char **argv)
{

  TQ_INIT (places);

  user = now = time (NULL);
//      user+=1;
  create_place ("ÃËÀÂÍÛÉ ×ÀÒ");
  create_place ("ÂÒÎÐÎÉ ×ÀÒ");
  create_place ("ÒÐÅÒÈÉ ×ÀÒ");
  create_place ("MAIN");
  create_place ("FALEN");

  chat_add_msg ("ÒÐÅÒÈÉ ×ÀÒ", "n1ckname", "hi!!");
  chat_add_msg ("ÒÐÅÒÈÉ ×ÀÒ", "n1ckname", "1 kto-tut?");
  chat_add_msg ("ÒÐÅÒÈÉ ×ÀÒ", "n1ckname", "2 kto-tut?");
  chat_add_msg ("ÒÐÅÒÈÉ ×ÀÒ", "rootz", "1 wtf?");
  chat_add_msg ("ÒÐÅÒÈÉ ×ÀÒ", "rootz", "2 wtf?");
  chat_add_msg ("MEIN", "rootz", "3 wtf?");
  chat_get_msg ("MEIN");
  chat_add_msg ("MEIN", "rootz", "5 wtf?");
  chat_add_msg ("MEIN", "rootz", "6 wtf?");
  chat_get_msg ("MEIN");

  printf ("zzz:%d %s 0x%x\n", find_place_by_name ("ÒÐÅÒÈÉ ×ÀÒ")->id,
	  find_place_by_name ("ÒÐÅÒÈÉ ×ÀÒ")->name,
	  find_place_by_id (find_place_by_name ("ÒÐÅÒÈÉ ×ÀÒ")->id)->name);
//      printf("zzz:%d %s 0x%x\n",find_place_by_name("MEIN")->id,find_place_by_name("MEIN")->name,find_place_by_name("MEIN"));

  twalk ((void *) places_root, print_node);

/*
	for(end=0;end<MAX_MSG_SIZE;end++) {
	 printf("msgs[%d] = 0x%x\n",end,msgs[end]);
	}
*/

  exit (0);
}
