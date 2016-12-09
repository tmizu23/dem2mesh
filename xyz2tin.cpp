
#include "cpl_vsi.h"
#include "cpl_conv.h"
#include "cpl_string.h"

struct deminfo {
	double W, E, S, N;
	double xres,yres;
	int xsize,ysize;
    float *alti;
};

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

int makeTIN(struct deminfo *d0, double base_elevation)
{
	printf("solid terrain\n");

	double v1[3];
	double v2[3];
	double v3[3];
	double v4[3];
	double N=d0->N;
	double S=d0->S;
	double W=d0->W;
	double E=d0->E;
	double xres=d0->xres;
	double yres=d0->yres;
	int xsize = d0->xsize;
	int ysize = d0->ysize;
	//Top face(elevation)
	for(int i=0;i<ysize-1;i++){
		for(int j=0;j<xsize-1;j++){
			v1[0]=W+j*xres;
			v1[1]=N-i*yres;
			v1[2]=d0->alti[i*xsize+j];
			v2[0]=v1[0];
			v2[1]=v1[1]-yres;
			v2[2]=d0->alti[(i+1)*xsize+j];
			v3[0]=v1[0]+xres;
			v3[1]=v2[1];
			v3[2]=d0->alti[(i+1)*xsize+j+1];
			v4[0]=v3[0];
			v4[1]=v1[1];
			v4[2]=d0->alti[i*xsize+j+1];
			printTIN(v1,v2,v4);
			printTIN(v2,v3,v4);
			
		}
	}
	//N face
	for(int i=0;i<xsize-1;i++){
		v1[0]=W+xres*(i+1);
		v1[1]=N;
	    v1[2]=d0->alti[i+1];
	    v2[0]=W+xres*(i+1);
	    v2[1]=N;
	    v2[2]=base_elevation;
	    v3[0]=W+xres*i;
	    v3[1]=N;
	    v3[2]=base_elevation;
	    v4[0]=W+xres*i;
	    v4[1]=N;
	    v4[2]=d0->alti[i];
	    printTIN(v1,v2,v4);
	    printTIN(v2,v3,v4);
	}
	//S face
	for(int i=0;i<xsize-1;i++){
	    v1[0]=W+xres*i;
	    v1[1]=S;
	    v1[2]=d0->alti[xsize*(ysize-1)+i];
	    v2[0]=W+xres*i;
	    v2[1]=S;
	    v2[2]=base_elevation;
	    v3[0]=W+xres*(i+1);
	    v3[1]=S;
	    v3[2]=base_elevation;
	    v4[0]=W+xres*(i+1);
	    v4[1]=S;
	    v4[2]=d0->alti[xsize*(ysize-1)+i+1];
	    printTIN(v1,v2,v4);
		printTIN(v2,v3,v4);
	}
	//W face
	for(int j=0;j<ysize-1;j++){	
	    v1[0]=W;
	    v1[1]=N-yres*j;
	    v1[2]=d0->alti[xsize*j];
	    v2[0]=W;
	    v2[1]=N-yres*j;
	    v2[2]=base_elevation;
	    v3[0]=W;
	    v3[1]=N-yres*(j+1);
	    v3[2]=base_elevation;
	    v4[0]=W;
	    v4[1]=N-yres*(j+1);
	    v4[2]=d0->alti[xsize*(j+1)];
	    printTIN(v1,v2,v4);
 	    printTIN(v2,v3,v4);
	}				
	//E face
	for(int j=0;j<ysize-1;j++){	
	    v1[0]=E;
	    v1[1]=N-yres*(j+1);
	    v1[2]=d0->alti[xsize*(j+2)-1];
	    v2[0]=E;
	    v2[1]=N-yres*(j+1);
	    v2[2]=base_elevation;
	    v3[0]=E;
	    v3[1]=N-yres*j;
	    v3[2]=base_elevation;
	    v4[0]=E;
	    v4[1]=N-yres*j;
	    v4[2]=d0->alti[xsize*(j+1)-1];
	    printTIN(v1,v2,v4);
		printTIN(v2,v3,v4);
	}
	//Bottom face
	for(int i=0;i<ysize-1;i++){
		for(int j=0;j<xsize-1;j++){
			v1[0]=W+j*xres;
			v1[1]=S+i*yres;
			v1[2]=base_elevation;
			v2[0]=W+j*xres;
			v2[1]=S+(i+1)*yres;
			v2[2]=base_elevation;
			v3[0]=v1[0]+xres;
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
	int dnum,k=0;
    double height_scale;
	double base_elevation;
	if(argc!=4 && argc!=8){
		printf("##\nUSAGE:\n xyz2tin xyzfile height_scale base_elevation [N] [S] [W] [E] > outfile.stl\n##\n");
		exit(1);
	}
	height_scale = atof(argv[2]);
	base_elevation = atof(argv[3]);
	fp = fopen (argv[1], "r");
	while (fgets (buf, 256, fp) != NULL) {
		dnum++;
	}
	rewind(fp);
	d0.alti = (float *) malloc (sizeof (float) * (dnum + 1) );
	d0.xsize = 0;
	char x[30],y[30],z[30];
	while (fgets (buf, 256, fp)!=NULL) {
		sscanf(buf,"%[^\t\r\n ] %[^\t\r\n ] %[^\t\r\n ]",x,y,z);
		d0.alti[k++] = (float)atof(z)*height_scale;
		if(k==1){
			d0.W=atof(x);
			d0.N=atof(y);
		}else if(k==dnum){
			d0.E=atof(x);d0.S=atof(y);
		}else if(d0.xsize==0 && atof(x)==d0.W){
			d0.xsize=k-1;
			d0.ysize=dnum/d0.xsize;
		}
		
	}
	if(argc==8){
		d0.N = atof(argv[4]);
		d0.S = atof(argv[5]);
		d0.W = atof(argv[6]);
		d0.E = atof(argv[7]);
	}
	d0.xres=(d0.E-d0.W)/(double)(d0.xsize-1);
	d0.yres=(d0.N-d0.S)/(double)(d0.ysize-1);
	//printf("%d,%d,%d,%f,%f,%f,%f,%f,%f\n",dnum,d0.xsize,d0.ysize,d0.N,d0.S,d0.W,d0.E,d0.xres,d0.yres);
	fclose (fp);
	makeTIN (&d0,base_elevation);
	free (d0.alti);
	return 0;
}
