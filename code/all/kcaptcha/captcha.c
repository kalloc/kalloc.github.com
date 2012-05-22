/**
    ainmarh.lab fcgi captcha
    
    собирать
    $gcc captcha.c  -lqtp -lfcgi -lgd -lm -O2 -o cap.fcgi
    поместить fonts в директорию куда будет установлен cap.fcgi

**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>
#include <gd.h>
#include <stdio.h>
#include <stdlib.h>
#include <qtp.h>
#include <qtl_tailq.h>

#define height 60
#define width 120
#define NOSPACE 0
#define fluctuation_amplitude 5
#define HEAD "Expires: Mon, 26 Jul 1997 05:00:00 GMT\r\n" \
	     "Cache-Control: no-store, no-cache, must-revalidate\r\n" \
	     "Pragma: no-cache\r\n" \
	     "A-Powered-Via:QTP 1.4.4\n"


#define Header cgiWriteStr(HEAD"Content-Type:image/png\n\n")
#define HeaderD cgiWriteStr(HEAD"Content-Type:text/plain\n\n")
#define mt_rand(s1,s2) (s1 + rand() % (s2+1-s1))
#define min(s1,s2) s1 < s2 ? s1 : s2


//CGI
#define cgiprintf(...)          printf(__VA_ARGS__)
#define cgiWriteEnd(str)        cgiTop();fwrite(str,sizeof(str)-1,1,stdout);cgiDown();
#define cgiWrite(str,len)       fwrite(str,len,1,stdout)
#define cgiWriteStr(str)        fwrite(str,sizeof(str)-1,1,stdout)



#define ALP "0123456789abcdefghijklmnopqrstuvwxyz"
gdImagePtr im,im2;
gdSink mySink;

#define output(im)     Header;gdImagePngToSink(im, &mySink)



FILE *out ;
char *alp = ALP;
char *countPtr,*codePtr,*timePtr;
char query[65];

int alp_len=sizeof(ALP)-1;
int len=0,indx=0,i=0,rgb=0,count=0;

float opacity,left,color,sy,sx,py,px,x,y,center,black,white,shift;    

float rand1,rand2,rand3,rand4,rand5,rand6,rand7,rand8,rand9,rand10,color_x,color_y,color_xy,foreground,background,newred,newgreen,newblue,frsx,frsy,frsx1,frsy1,newcolor,newcolor0;


struct metric_st {int start;int end;};
struct metric_st metric;

struct fonts_st {
    char name[100];
    char *buf;
     int size;
     int y;
     int x;
    struct metric_st metrics[sizeof(ALP)-1];
    gdImagePtr *im;
} *fonts[1024];







int  imagecolorat(gdImagePtr im,  int x, int y) {

	if (gdImageTrueColor(im)) {
		if (im->tpixels && gdImageBoundsSafe(im, x, y)) {
			return gdImageTrueColorPixel(im, x, y);
		} 
	} else {
		if (im->pixels && gdImageBoundsSafe(im,x, y)) {
			return (im->pixels[x][y]);
		} 
	}
	return 0;
}


void readfont(char *filefont) {
    FILE *fp;
    gdImagePtr im;
    struct stat buffer;
    stat(filefont,&buffer);
    if(!(fp=fopen(filefont,"rb")))  return;
    fonts[indx]=malloc(sizeof(fonts));
    fonts[indx]->buf=malloc(buffer.st_size);
    fread(fonts[indx]->buf,buffer.st_size,1,fp);
    fonts[indx]->size=buffer.st_size;
    if((strrchr(filefont,'.')) != NULL)
        filefont[strrchr(filefont,'.')-filefont]=0;
    snprintf(fonts[indx]->name,99,"%s",filefont);
    fclose(fp);

    im=gdImageCreateFromPngPtr(fonts[indx]->size,(void *)fonts[indx]->buf);
    gdImageAlphaBlending(im,1);
    fonts[indx]->im=(gdImagePtr *)im;
    fonts[indx]->x=gdImageSX(im);
    fonts[indx]->y=gdImageSY(im)-1;

    char sym;
    int cnt,read_sym=0;
    char trans;

    for(cnt=0,sym=0;(cnt<fonts[indx]->x) &&  (sym < alp_len);cnt++) {

	trans= ((imagecolorat(im,cnt,0) >>  24) == 127);
	if(!read_sym && !trans) {
	
	    fonts[indx]->metrics[(int)alp[(int)sym]].start=cnt;
	    read_sym=1;
	}
	if(read_sym && trans) {
	    fonts[indx]->metrics[(int)alp[(int)sym]].end=cnt;
	    read_sym=0;
	    sym++;
	}
    }
    if(sym == alp_len)
    indx++;


}

void loadfont(char *path) {

    int i, n;
    struct dirent ** entry;
    chdir(path);
    n = scandir("./", &entry,(void *)NULL, alphasort);
    for (i = 0; i < n; i++) {
        if(strstr(entry[i]->d_name,".png"))
	    readfont(entry[i]->d_name);
    }

    chdir("../");

}

static int stdioSink(void *context, char *buffer, int len)
{
  return fwrite(buffer, 1, len, (FILE *) context);
}






void draw_text(char *text)  {


    srand(time(NULL)*rand());

    float fc0,fc1,fc2,bc0,bc1,bc2;
    fc0=(float)mt_rand(0,100);
    fc1=(float)mt_rand(0,100);
    fc2=(float)mt_rand(0,100);
    bc0=(float)mt_rand(200,255);
    bc1=(float)mt_rand(200,255);
    bc2=(float)mt_rand(200,255);
    
    struct fonts_st *font;
    font=fonts[(int)mt_rand(0,indx)];

    len=strlen(text);


    //printf("FontName:%s\r\n",font->name);
//    printf("Code:%s\r\n",text);
//    while(1) {

        im = gdImageCreateTrueColor(width,height);
        gdImageAlphaBlending(im,1);
        white = gdImageColorAllocate(im, 255, 255, 255);  
        black = gdImageColorAllocate(im, 0, 0, 0);  
        gdImageFilledRectangle(im, 0,0, width-1, height-1, white);


    // draw text
    x=1;
    
        //printf("draw cycle\n");

    for(i=0;i<len;i++){
         metric=font->metrics[(int)text[i]];
//         printf("ch:%c %d->%d\r\n",text[i],metric.start,metric.end);
	 
         y=(float)mt_rand(-fluctuation_amplitude, fluctuation_amplitude)+(height-font->y)/2+2;
	    if(NOSPACE) {
                shift=0;
                if(i>0){
                    shift=1000;
                    for(sy=7;sy<font->y-20;sy+=1){
                     for(sx=metric.start-1;sx<metric.end;sx+=1){
                                    rgb=imagecolorat((gdImagePtr)font->im, sx, sy);
                                    opacity=rgb>>24;
                            if(opacity<127){
                                    left=sx-metric.start+x;
                                    py=sy+y;
                                    if(py>height) break;
                                    for(px=min(left,width-1);px>left-12 && px>=0;px-=1){
                                            color=imagecolorat(im, px, py) & 0xff;
                                            if(color+opacity<190){
                                                    if(shift>left-px){
                                                         shift=left-px;
                                                    }
                                                    break;
                                            }
                                    }
                                    break;
                            }
                         }
                        }
                        if(shift==1000){
                         shift= (float)mt_rand(4,6);
                        }

            	    }    
                 }else{
                    shift=1;
                 }
                 gdImageCopy(im,(gdImagePtr)font->im,x-shift,y,metric.start,1,metric.end-metric.start,font->y);
                 x+=metric.end-metric.start-shift;
        }
//		 if(x<width-10) break;
//    }
	         center=x/2;

    im2=gdImageCreateTrueColor(width, height);
    foreground=gdImageColorAllocate(im2, fc0, fc1, fc2);
    background=gdImageColorAllocate(im2, bc0, bc1, bc2);
    gdImageFilledRectangle(im2, 0, height, width-1, height+12, foreground);
												
														

// periods
                rand1=(float)mt_rand(750000,1200000)/10000000;
                rand2=(float)mt_rand(750000,1200000)/10000000;
                rand3=(float)mt_rand(750000,1200000)/10000000;
                rand4=(float)mt_rand(750000,1200000)/10000000;
// phases
                rand5=(float)mt_rand(0,3141592)/500000;
                rand6=(float)mt_rand(0,3141592)/500000;
                rand7=(float)mt_rand(0,3141592)/500000;
                rand8=(float)mt_rand(0,3141592)/500000;
// amplitudes
                rand9=(float)mt_rand(330,420)/110;
                rand10=(float)mt_rand(330,450)/110;
		

                for(x=0;x<width;x++){
                        for(y=0;y<height;y++){
                                sx=x+(sin(x*rand1+rand5)+sin(y*rand3+rand6))*rand9-width/2+center+1;
                                sy=y+(sin(x*rand2+rand7)+sin(y*rand4+rand8))*rand10;

                                if(sx<0 || sy<0 || sx>=width-1 || sy>=height-1){
                                        color=255;
                                        color_x=255;
                                        color_y=255;
                                        color_xy=255;
                                }else{
                                        color=imagecolorat(im, sx, sy) & 0xFF;
                                        color_x=imagecolorat(im, sx+1, sy) & 0xFF;
                                        color_y=imagecolorat(im, sx, sy+1) & 0xFF;
                                        color_xy=imagecolorat(im, sx+1, sy+1) & 0xFF;
                                }

                                if(color==0 && color_x==0 && color_y==0 && color_xy==0){
                                        newred=fc0;
                                        newgreen=fc1;
                                        newblue=fc2;
                                }else if(color==255 && color_x==255 && color_y==255 && color_xy==255){
                                        newred=bc0;
                                        newgreen=bc1;
                                        newblue=bc2;
                                }else{
                                        frsx=sx-floor(sx);
                                        frsy=sy-floor(sy);
                                        frsx1=1-frsx;
                                        frsy1=1-frsy;

                                        newcolor=(
                                                color*frsx1*frsy1+
                                                color_x*frsx*frsy1+
                                                color_y*frsx1*frsy+
                                                color_xy*frsx*frsy);

                                        if(newcolor>255) newcolor=255;
                                        newcolor=newcolor/255;
                                        newcolor0=1-newcolor;

                                        newred=newcolor0*fc0+newcolor*bc0;
                                        newgreen=newcolor0*fc1+newcolor*bc1;
                                        newblue=newcolor0*fc2+newcolor*bc2;
                        }
                        gdImageSetPixel(im2, x, y, gdImageColorAllocate(im2, newred, newgreen, newblue));
                }
        }


    output(im2);
    gdImageDestroy(im2);
    gdImageDestroy(im);



}





int cgiInit () {

	qsm_init("/tmp/","myproject",1200);
        loadfont("./fonts/");

	if(indx>0) indx--;
	mySink.context = (void *) stdout;
	mySink.sink = (void*)stdioSink;
	return 0;

}


void set_multiple() {

	if((countPtr=qsm_get_param("Ccount")) != NULL && (count=atoi(countPtr)) < 3 &&  (timePtr=qsm_get_param("Ctime")) != NULL && ((atoi(timePtr))+60 > time(NULL)) )  {
		count++;
		snprintf(query,64,"%d",count);
		qsm_set_param("Ccount",query);
		codePtr=qsm_get_param("Ccode");

	} else  {

		qsm_set_param("Ccount","1");
		snprintf(query,64,"%d",(int)time(NULL));
		qsm_set_param("Ctime",query);

		for(len=0;len<6;len++) {
		    query[len]=alp[mt_rand(0,alp_len-1)];
		}		
		query[len]=0;
		qsm_set_param("Ccode",query);

		codePtr=query;

	}



}


void set_one() {

	for(len=0;len<6;len++) {
	    query[len]=alp[mt_rand(0,alp_len-1)];
	}		
	query[len]=0;
	qsm_set_param("Ccode",query);
	codePtr=query;

}




int cgiMain () {
	if(indx==0) {
    		HeaderD;
		cgiWriteStr("dir ./fonts/ not found or dir empty");
		exit(0);
	}


        srand(time(NULL)*rand());
	qsm_init_session();
	set_one();
        draw_text(codePtr);

        return 0;

}

