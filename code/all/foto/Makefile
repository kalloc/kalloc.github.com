default:
		/usr/local/bin/qtp  -fcachef -s templates/foto_index.qtp -o foto_index
		/usr/local/bin/qtp  -fcachef -s templates/foto_view.qtp -o foto_view
		gcc main_foto_index.c foto_index.c foto_view.c utils.c idb.c -lqtp -lfcgi  -o foto.cgi