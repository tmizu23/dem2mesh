#include "cpl_vsi.h"
#include "cpl_conv.h"
#include "cpl_string.h"

struct deminfo {
  char mesh[10];
  char optstring[100];
  double W, E, S, N;
  int lowx, lowy, highx, highy, startx, starty;
  float *alti;
};

char *cut (const char *str, const char *ss, const char *es, char *buff)
{
  char *wk, *s, *e, *c;

  wk = strdup (str);
  if (wk == NULL) {
	free (wk);
	return NULL;
  }
  s = strstr (wk, ss);
  if (s == NULL) {
	free (wk);
	return NULL;
  }
  c = s + strlen (ss);
  e = strstr (c, es);
  if (e == NULL) {
	free (wk);
	return NULL;
  }
  *e = '\0';
  strcpy (buff, c);
  free (wk);
  return buff;
}
int printTIN(double *v1,double *v2,double *v3){
	double nx,ny,nz;
	double v12x,v12y,v12z;
	double v23x,v23y,v23z;
	double length;
	
			v12x=v2[0]-v1[0];
			v12y=v2[1]-v1[1];
			v12z=v2[2]-v1[2];
			v23x=v3[0]-v2[0];
			v23y=v3[1]-v2[1];
			v23z=v3[2]-v2[2];
			nx=v12y*v23z-v12z*v23y;
			ny=v12z*v23x-v12x*v23z;
			nz=v12x*v23y-v12y*v23x;
			length = pow(nx*nx+ny*ny+nz*nz,0.5);
			nx /= length;
			ny /= length;
			nz /= length;
			printf("facet normal %f %f %f\n",nx,ny,nz);
			printf("outer loop\n");
			printf("vertex %f %f %f\n",v1[0],v1[1],v1[2]);
			printf("vertex %f %f %f\n",v2[0],v2[1],v2[2]);
			printf("vertex %f %f %f\n",v3[0],v3[1],v3[2]);
			printf("endloop\n");
			printf("endfacet\n");
}

int makeTIN(struct deminfo *d0,double base_elevation)
{
	printf("solid terrain\n");

	double v1[3];
	double v2[3];
	double v3[3];
	double v4[3];
	double N=7500;
	double S=0;
	double W=0;
	double E=11250;
	double resx=10.0;
	double resy=10.0;
	int dsn_xsize = 1125;
	int dsn_ysize = 750;
	
	for(int i=0;i<dsn_ysize-1;i++){
		for(int j=0;j<dsn_xsize-1;j++){
			v1[0]=W+j*resx;
			v1[1]=N-i*resy;
			v1[2]=d0->alti[i*dsn_xsize+j];
			v2[0]=v1[0];
			v2[1]=v1[1]-resy;
			v2[2]=d0->alti[(i+1)*dsn_xsize+j];
			v3[0]=v1[0]+resx;
			v3[1]=v2[1];
			v3[2]=d0->alti[(i+1)*dsn_xsize+j+1];
			v4[0]=v3[0];
			v4[1]=v1[1];
			v4[2]=d0->alti[i*dsn_xsize+j+1];
			printTIN(v1,v2,v4);
			printTIN(v2,v3,v4);
			
		}
	}
	//N face
	for(int i=0;i<dsn_xsize-1;i++){
		v1[0]=W+resx*(i+1);
		v1[1]=N;
	    v1[2]=d0->alti[i+1];
	    v2[0]=W+resx*(i+1);
	    v2[1]=N;
	    v2[2]=base_elevation;
	    v3[0]=W+resx*i;
	    v3[1]=N;
	    v3[2]=base_elevation;
	    v4[0]=W+resx*i;
	    v4[1]=N;
	    v4[2]=d0->alti[i];
	    printTIN(v1,v2,v4);
	    printTIN(v2,v3,v4);
	}
	//S face
	for(int i=0;i<dsn_xsize-1;i++){
	    v1[0]=W+resx*i;
	    v1[1]=S;
	    v1[2]=d0->alti[dsn_xsize*(dsn_ysize-1)+i];
	    v2[0]=W+resx*i;
	    v2[1]=S;
	    v2[2]=base_elevation;
	    v3[0]=W+resx*(i+1);
	    v3[1]=S;
	    v3[2]=base_elevation;
	    v4[0]=W+resx*(i+1);
	    v4[1]=S;
	    v4[2]=d0->alti[dsn_xsize*(dsn_ysize-1)+i+1];
	    printTIN(v1,v2,v4);
		printTIN(v2,v3,v4);
	}
	//W face
	for(int j=0;j<dsn_ysize-1;j++){	
	    v1[0]=W;
	    v1[1]=N-resy*j;
	    v1[2]=d0->alti[dsn_xsize*j];
	    v2[0]=W;
	    v2[1]=N-resy*j;
	    v2[2]=base_elevation;
	    v3[0]=W;
	    v3[1]=N-resy*(j+1);
	    v3[2]=base_elevation;
	    v4[0]=W;
	    v4[1]=N-resy*(j+1);
	    v4[2]=d0->alti[dsn_xsize*(j+1)];
	    printTIN(v1,v2,v4);
 	    printTIN(v2,v3,v4);
	}				
	//E face
	for(int j=0;j<dsn_ysize-1;j++){	
	    v1[0]=E;
	    v1[1]=N-resy*(j+1);
	    v1[2]=d0->alti[dsn_xsize*(j+2)-1];
	    v2[0]=E;
	    v2[1]=N-resy*(j+1);
	    v2[2]=base_elevation;
	    v3[0]=E;
	    v3[1]=N-resy*j;
	    v3[2]=base_elevation;
	    v4[0]=E;
	    v4[1]=N-resy*j;
	    v4[2]=d0->alti[dsn_xsize*(j+1)-1];
	    printTIN(v1,v2,v4);
		printTIN(v2,v3,v4);
	}
	//Bottom face
	for(int i=0;i<dsn_ysize-1;i++){
		for(int j=0;j<dsn_xsize-1;j++){
			v1[0]=W+j*resx;
			v1[1]=S+i*resy;
			v1[2]=base_elevation;
			v2[0]=W+j*resx;
			v2[1]=S+(i+1)*resy;
			v2[2]=base_elevation;
			v3[0]=v1[0]+resx;
			v3[1]=v2[1];
			v3[2]=base_elevation;
			v4[0]=v3[0];
			v4[1]=v1[1];
			v4[2]=base_elevation;
			printTIN(v1,v2,v4);
			printTIN(v2,v3,v4);
		}
	}
	
	printf("endsolid terrain\n");
	return 0;
}





int
main (int argc, char *argv[])
{

	struct deminfo d0;
	FILE *fp;
	char buf[256];
	char cbuf[256];
	int dnum, snum, k;
	int nodata;
	double height_scale;
	double base_elevation;
	
	if(argc<5){
		printf("##\nUSAGE:\n kiban2tin xmlfile nodata_elevation height_scale base_elevation > outfile.stl\n##\n");
		exit(1);
	}
	nodata = atoi(argv[2]);
	height_scale = atof(argv[3]);
	base_elevation = atof(argv[4]);
	
	d0.optstring[0] = '\0';

	fp = fopen (argv[1], "r");

	while (1) {
		fgets (buf, 256, fp);

		if (strstr (buf, "<mesh>") != NULL) {
			strcpy (d0.mesh, cut (buf, "<mesh>", "</mesh>", cbuf));
		} else if (strstr (buf, "<gml:timePosition>") != NULL) {
			strcpy (d0.optstring, cut (buf, "<gml:timePosition>", "</gml:timePosition>", cbuf));
		} else if (strstr (buf, "<gml:lowerCorner>") != NULL) {
			sscanf (cut (buf, "<gml:lowerCorner>", "</gml:lowerCorner>", cbuf), "%lf %lf", &d0.S, &d0.W);
		} else if (strstr (buf, "<gml:upperCorner>") != NULL) {
			sscanf (cut (buf, "<gml:upperCorner>", "</gml:upperCorner>", cbuf), "%lf %lf", &d0.N, &d0.E);
		} else if (strstr (buf, "<gml:low>") != NULL) {
			sscanf (cut (buf, "<gml:low>", "</gml:low>", cbuf), "%d %d", &d0.lowx, &d0.lowy);
		} else if (strstr (buf, "<gml:high>") != NULL) {
			sscanf (cut (buf, "<gml:high>", "</gml:high>", cbuf), "%d %d", &d0.highx, &d0.highy);;

		} else if (strstr (buf, "<gml:tupleList>") != NULL) {
			dnum = (d0.highx - d0.lowx + 1) * (d0.highy - d0.lowy + 1);
			d0.alti = (float *) malloc (sizeof (float) * (dnum + 1) );
	
			k = 0;
			fgets (buf, 256, fp);
			while (strstr (buf, ",") != NULL) {

				d0.alti[k++] = (float) atof (cut (buf, ",", "\n", cbuf))*2.0;
				fgets (buf, 256, fp);
			}
		} else if (strstr (buf, "<gml:startPoint>") != NULL) {
			sscanf (cut (buf, "<gml:startPoint>", "</gml:startPoint>", cbuf), "%d %d", &d0.startx, &d0.starty);
			snum = (d0.highx + 1) * d0.starty + d0.startx;
		}

		if (feof (fp)) {
			break;
		}
	}

	fclose (fp);

	//snum が0でない場合は、オフセットしたのち、altiを再取得。
	//無駄なアルゴリズムだけど、snumが0の場合は希なので、問題なし。

	if (snum != 0) {
		k = 0;
		while (k < snum) {
			d0.alti[k++] = -9999;
		}

		fp = fopen (argv[1], "r");
		while (1) {
			fgets (buf, 256, fp);
			if (strstr (buf, "<gml:tupleList>") != NULL) {
				fgets (buf, 256, fp);
				while (strstr (buf, ",") != NULL) {
					d0.alti[k++] = (float) atof (cut (buf, ",", "\n", cbuf))*height_scale;
					fgets (buf, 256, fp);
				}
				break;
			}
		}
	}
	while (k <= dnum) {
		d0.alti[k++] = -9999;
	}

	//データなしを0にする。
	if(nodata == 0){
		for(k=0;k<dnum;k++){
			if(d0.alti[k] == -9999) d0.alti[k] = 0;
		}
	}

	makeTIN (&d0,base_elevation);
	free (d0.alti);
	return 0;
}
