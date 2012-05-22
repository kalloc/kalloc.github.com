#include <stdio.h>
#include <qtp.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <iconv.h>

#include "idb.h"
#include "foto_index.h"
#define BLOCK 1024
#define FALSE 0
#define TRUE 1

char dir[BLOCK];
char main_dir[BLOCK];
char title[BLOCK];
extern char **environ;

void print_env ();

void
CookieSet ()
{
  char *cname;
  char *cvalue;
  /* Must set cookies BEFORE calling 
     cgiHeaderContentType */
  cname = strdup ("Teest");
  cvalue = strdup ("TVaaaluue");
  if (strlen (cname)) {
    /* Cookie lives for one day (or until 
       browser chooses to get rid of it, which 
       may be immediately), and applies only to 
       this script on this site. */
    cgiHeaderCookieSetString (cname, cvalue, 86400, cgiScriptName,
			      cgiServerName);
    cgiFormString ("cname", cname, sizeof (cname));
    cgiFormString ("cvalue", cvalue, sizeof (cvalue));

  }
}


FILE *cachef;			// ■═╘╚ ╙МХ═ (qtp -fcachef - ГБ╝║К ╛╔Е═╜╗╖╛ ╜═Г═╚ Ю═║╝Б═БЛ)

void
error_404 ()
{

  char query[BLOCK];

  cgiHeaderStatus (404, "Not Found");
  strncpy (query, (char *) getenv ("REQUEST_URI"), BLOCK - 1);
  printf ("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
	  "<html><head>\n"
	  "<title>404 Not Found</title>\n"
	  "</head><body>\n" "<h1>Not Found</h1>\n" "<p>The requested URL ");
  cgiHtmlEscape (query);
  printf (" was not found on this server.</p>\n"
	  "<hr>\n"
	  "<address>%s Server at %s Port %s</address>\n"
	  "</body></html>\n", cgiServerSoftware, cgiServerName,
	  cgiServerPort);
//              print_env();

}

void
foto_index_header_do ()
{

  foto_index_header (title);

}

void
recursive_read (char *path)
{

  DIR *dirp;
  struct dirent *dp;
  dirp = opendir (path);
  struct stat *buf;
  char fullpath[BLOCK];
  while ((dp = readdir (dirp))) {
    buf = malloc (sizeof (*buf));
    bzero (buf, sizeof (*buf));
    bzero (&fullpath, BLOCK);
    if (strcmp (dp->d_name, "..") && strcmp (dp->d_name, ".")) {
      snprintf (fullpath, BLOCK - 1, "%s/%s", path, dp->d_name);
      stat (fullpath, buf);
      if (S_ISDIR (buf->st_mode)) {
	printf ("dir: %s\n", fullpath);
	//recursive_read(fullpath);
      }
      else if (S_ISREG (buf->st_mode)) {
	printf ("file: %s\n", fullpath);
      }
    }
    free (buf);
  }
  closedir (dirp);


}

void
print_env ()
{

  char **p = environ;
  char *ENV_NAME;
  char *ENV_VALUE;
  while (*p != NULL) {
    ENV_VALUE = strstr (*p, "=");
    ENV_VALUE[0] = '\0';
    ENV_VALUE += 1;
    ENV_NAME = *p;
    printf ("<b>%s</b>:%s<br/>\n", ENV_NAME, ENV_VALUE);
    *p++;
  }


}

void
dir_read (char *path)
{

  DIR *dirp;
  struct dirent *dp;
  struct stat buf;
  char fullpath[BLOCK];
  char tn_fullpath[BLOCK];

  bzero (&buf, sizeof (struct stat));

  stat (path, &buf);
  if (S_ISDIR (buf.st_mode)) {

    dirp = opendir (path);

    while ((dp = readdir (dirp))) {
      bzero (&buf, sizeof (struct stat));
      bzero (&fullpath, BLOCK);
      bzero (&tn_fullpath, BLOCK);
      if (strcmp (dp->d_name, "..") && strcmp (dp->d_name, ".")) {
	snprintf (fullpath, BLOCK - 1, "%s/%s", path, dp->d_name);
	stat (fullpath, &buf);
	if (S_ISDIR (buf.st_mode)) {
	  snprintf (fullpath, BLOCK - 1, "%s/%s/", path, dp->d_name);
	  replace (fullpath, main_dir, "/foto");
	  replace (fullpath, "//", "/", 1);

	  snprintf (tn_fullpath, BLOCK - 1, "%s/%s/tn_%s.jpg", path,
		    dp->d_name, dp->d_name);
	  replace (tn_fullpath, "//", "/", 1);

	  bzero (&buf, sizeof (struct stat));
	  stat (tn_fullpath, &buf);
	  if (S_ISREG (buf.st_mode)) {
	    replace (tn_fullpath, main_dir, "/foto_content");
	    foto_index_body (fullpath, dp->d_name, tn_fullpath);
	  }
	  else {
	    foto_index_body (fullpath, dp->d_name, "");
	  }

	}
	else if (S_ISREG (buf.st_mode) && !strstr (dp->d_name, "tn_")
		 && strstr (dp->d_name, ".jpg")) {
	  snprintf (tn_fullpath, BLOCK - 1, "%s/tn_%s", path, dp->d_name);
	  bzero (&buf, sizeof (struct stat));
	  stat (tn_fullpath, &buf);
	  if (S_ISREG (buf.st_mode)) {
	    replace (fullpath, main_dir, "/foto_view");
	    replace (fullpath, "//", "/", 1);
	    replace (tn_fullpath, main_dir, "/foto_content");
	    replace (tn_fullpath, "//", "/", 1);
	    foto_index_body (fullpath, dp->d_name, tn_fullpath);

	  }
	}
      }
      else if (!strcmp (dp->d_name, "..") && strcmp (path, main_dir)) {
	strncpy (fullpath, 0,
		 strlen (path) - (int) (strrchr (path, '/') - path) - 1);
	//printf(fullpath);

	foto_index_body ("..", "parent", "");

      }
    }
    closedir (dirp);

  }


}



void
foto_index_body_do ()
{

  //print_env();
  dir_read (dir);
//spath,sizeof(spath)-1,                                                
}


void
foto_view_header_do ()
{
  foto_view_header (title);
}


void
foto_view_body_do ()
{

  replace (dir, main_dir, "/foto_content");
  replace (dir, "//", "/");
  char dirname[BLOCK];
  bzero (&dirname, BLOCK);
  strncpy (dirname, strrchr (dir, '/') + 1,
	   strlen (dir) - (int) (strrchr (dir, '/') - dir) - 5);

  foto_view_body (dir, dirname, dir);
}

void
printGB (char *path)
{

  struct stat buf;
  bzero (&buf, sizeof (struct stat));
  char str[1024];
  stat (path, &buf);
  if (S_ISREG (buf.st_mode)) {
    FILE *fd;
    fd = fopen (path, "r");
    printf
      ("document.getElementById('commentus').innerHTML='<hr style=\"border:1px solid #D4D1C6;width:200px\">'");
    while (!feof (fd) && fd) {
      bzero (&str, 1024);
      fgets (str, BLOCK, fd);
      replace (str, "\n", "<br>");
      replace (str, "\r", "");
      if (strlen (str) > 0)
	printf
	  ("+'%s<br><hr style=\"border:1px solid #D4D1C6;width:200px\">'",
	   str);
    }
    fclose (fd);
    printf ("+'';\n");
  }
  else {

    printf
      ("document.getElementById('commentus').innerHTML='(<span style=\"color:green;font-weight:bold\">ЙНЛЛЕМРНБ МЕР</span>)';\n");


  }


}

void
convert (char *str)
{


  iconv_t cd;
  size_t k, f, t;
  int se;
  const char *in = str;
  char buf[BLOCK];
  char *out = buf;

  cd = iconv_open ("cp1251", "utf-8");
  if (cd == (iconv_t) (-1))
    err (1, "iconv_open");
  f = strlen (str);
  t = sizeof buf;
  memset (&buf, 0, sizeof buf);
  k = iconv (cd, &in, &f, &out, &t);
  iconv_close (cd);
  bzero (str, BLOCK);
  strcpy (str, buf);


}

void
addGB (char *path, char *username, char *message)
{


  if (strlen (username) == 0) {

    printf ("alert('ОПЕДЯРЮБЭРЕЯЭ ВРНКХ');");


  }
  else if (strlen (message) == 0) {

    printf ("alert('МЮОХЬХРЕ ЯКНБЕВЙН, СДХБХРЕ ЛЕМЪ');");

  }
  else {
    replace (message, "\n", "<br>");
    replace (message, "\r", "");
    replace (username, "\n", "");
    replace (username, "\r", "");

    char str[BLOCK];
    char ip[BLOCK];
    bzero (&str, BLOCK);
    bzero (&ip, BLOCK);

    strncpy (ip, cgiRemoteAddr,
	     (int) (strrchr (cgiRemoteAddr, '.') - cgiRemoteAddr));
    rehack (message);
    rehack (username);
    snprintf (str, BLOCK - 1, "<b>%s</b><sup>%s.xxx</sup>:<br> %s\n",
	      username, ip, message);
    bzero (&ip, BLOCK);

    char *pstr;
    pstr = malloc (BLOCK);
    bzero (pstr, BLOCK);
    strcpy (pstr, str);
    convert (pstr);
    FILE *fd;
    fd = fopen (path, "a");
    fputs (pstr, fd);
    fclose (fd);
    printf
      ("document.getElementById('gb_name').value='';document.getElementById('gb_text').value='';");
    free (pstr);
    printGB (path);


  }

}


int checksafe(char *dir) {

    for(;*dir != 0;dir++) {
	if(dir[0] == '.' && dir[1] == '.') return FALSE;
    }

    return TRUE;

}
int
cgiInit ()
{
  qcm_contentlength_set (500000, 65535);
  qcm_timeout (0);
  // Устанавливаем размеры внутренних буферов
  // Внутренний - 500000, временный - 65535
  qcm_contentlength_on ();	// Включаем поддержку content-length
  qcm_dir ("/tmp/cache");	// Выставляем директорию для кэша
  qcm_init ();

  return 0;
}

int
cgiMain ()
{




  bzero (&main_dir, BLOCK - 1);
  strcpy (main_dir, "/www/default/foto_content");
  char query[BLOCK];




  //cgiWriteEnvironment("capcgi.dat");

  bzero (&dir, BLOCK - 1);
  bzero (&title, BLOCK - 1);


  if (strstr (cgiQueryString, "ajax")) {

    char *ptr = strstr (cgiQueryString, "&dir");
    char dirQ[BLOCK];
    char username[BLOCK];
    char message[BLOCK];
    char *ptr2;
    bzero (&dirQ, BLOCK);
    bzero (&username, BLOCK);
    bzero (&message, BLOCK);
    ptr += 5;
    ptr2 = strstr (ptr, "&");


    if (ptr2 == NULL) {
      strcpy (dirQ, ptr);
    }
    else if (ptr2 - ptr < BLOCK) {
      strncpy (dirQ, ptr, ptr2 - ptr);
    }
    else {
      error_404 (); return 0;
    }

    if(!checksafe(query)) { error_404(); return 0;}

    bzero (&query, BLOCK - 1);

    if (!cgiFormString ("call", query, BLOCK - 1)
	&& !strcmp (query, "print")) {

      struct stat buf;
      bzero (&buf, sizeof (struct stat));
      char path[BLOCK];
      bzero (&path, BLOCK);
      snprintf (path, BLOCK - 1, "%s/%s", main_dir, dirQ);
      stat (path, &buf);
      if (S_ISREG (buf.st_mode) || !strcmp (dirQ, "")) {
	printf ("Content-Type: text/plain; charset=windows-1251\n");
	printf ("A-Powered-Via: QTP 1.4.4\n\n");
	strcpy (dir, path);
	snprintf (path, BLOCK - 1, "%s/%s.txt", main_dir, dirQ);
	printGB (path);
	//print_env();
      }
      else {
	error_404 (); return 0;
      }
    }
    else if (!cgiFormString ("call", query, BLOCK - 1)
	     && !strcmp (query, "add")
	     && !cgiFormString ("gb_name", username, BLOCK - 1)
	     && !cgiFormString ("gb_text", message, BLOCK - 1)) {

      struct stat buf;
      bzero (&buf, sizeof (struct stat));
      char path[BLOCK];
      bzero (&path, BLOCK);
      snprintf (path, BLOCK - 1, "%s/%s", main_dir, dirQ);
      stat (path, &buf);
      if (S_ISREG (buf.st_mode) || !strcmp (dirQ, "")) {
	printf ("Content-Type: text/plain; charset=windows-1251\n");
	printf ("A-Powered-Via: QTP 1.4.4\n\n");
	strcpy (dir, path);
	snprintf (path, BLOCK - 1, "%s/%s.txt", main_dir, dirQ);

	addGB (path, username, message);
      }
      else {
	error_404 (); return 0;
      }


    }
    else {
      error_404 (); return 0;
    }

  }
  else {

    printf ("Content-Type: text/html; charset=windows-1251\n");
    printf ("A-Powered-Via: QTP 1.4.4\n");

    if (!cgiFormString ("action", query, BLOCK - 1)
	&& !strcmp (query, "view")
	&& !cgiFormString ("dir", query, BLOCK - 1) && strcmp (query, "")) {

      if(!checksafe(query)) { error_404(); return 0;}

      strcpy (title, "view");
      struct stat buf;
      bzero (&buf, sizeof (struct stat));

      char path[BLOCK];
      bzero (&path, BLOCK);
      snprintf (path, BLOCK - 1, "%s/%s", main_dir, query);

      qcm_checker ("foto_view");
      qcm_set (NULL);
      qcm_set ("dir");
      cachef = qcm_open ("foto_view");
      if (!cachef)
	return 0;		// Сработал кэш, ничего делать больше не надо

      printf ("create:now\n");

      stat (path, &buf);
      if (S_ISREG (buf.st_mode)) {
	strcpy (dir, path);
	foto_view ();
      }
      else {
	error_404 (); return 0;
      }

      qcm_close (&cachef);
      qcm_checker ("foto_view");
      qcm_clear ();

    }
    else {


      struct stat buf;
      bzero (&buf, sizeof (struct stat));

      cgiFormString ("dir", query, BLOCK - 1);

      if(!checksafe(query)) { error_404(); return 0;}

      qcm_checker ("foto_dir_checker");
      qcm_set (NULL);
      qcm_set ("dir");
      cachef = qcm_open ("foto_dir");
      if (!cachef)
	return 0;		// Сработал кэш, ничего делать больше не надо
      printf ("create:now\n");
//      strcpy(query,"2006");
      if (!strcmp (query, "")) {
	strcpy (title, "main");
	strcpy (dir, main_dir);
	foto_index ();
      } else {
	int qLen = strlen (query);
	if (query[qLen - 1] != '/') {
	  query[qLen] = '/';
	  query[qLen + 1] = '\0';
	}

	strcpy (title, query);
	char path[BLOCK];
	bzero (&path, BLOCK);
	snprintf (path, BLOCK - 1, "%s/%s", main_dir, query);
	stat (path, &buf);
	if (S_ISDIR (buf.st_mode)) {
	  strcpy (dir, path);
	  foto_index ();
	}
	else {
	  error_404 (); return 0;
	}
      }

      qcm_close (&cachef);
      qcm_clear ();


    }
  }
//      CookieSet();
/*	IDB pIDB = idb_open("/tmp/file.idb");
	char *apoweredby;
	if((apoweredby=idb_get(pIDB,"apoweredby")) == NULL) {
		idb_put(pIDB,"apoweredby","QTP 1.4.4 QL");
	}
	idb_close(pIDB);*/

  return 0;
}
