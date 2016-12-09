
#include "cpl_vsi.h"
#include "cpl_conv.h"
#include "cpl_string.h"

struct deminfo {
	double W, E, S, N;
	double xres,yres;
	int xsize,ysize;
    float *alti;
};

int printTIN(double *v1,double *v2,double *v3,double *v4){
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
	//printf("mtllib dem.mtl\n");
	//printf("g demgroup\n");
	//printf("usemtl vegetation\n");
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
	double txres=1.0/(d0->xsize-1);
	double tyres=1.0/(d0->ysize-1);
	int dnum = xsize*ysize;
	
	
	//top vertex
	for(int i=0;i<ysize;i++){
		for(int j=0;j<xsize;j++){
			printf("v %f %f %f\n",W+j*xres,N-i*yres,d0->alti[i*xsize+j]);
		}
	}
	//bottom vertex
	for(int i=0;i<ysize;i++){
		for(int j=0;j<xsize;j++){
			printf("v %f %f %f\n",W+j*xres,N-i*yres,base_elevation);
		}
	}
	//texture
	for(int i=0;i<ysize;i++){
		for(int j=0;j<xsize;j++){
			printf("vt %f %f \n",j*txres,1.0-i*tyres);
		}
	}
	
	//Top,Bottom face
	for(int i=0;i<ysize-1;i++){
		for(int j=0;j<xsize-1;j++){
			printf("f %d/%d %d/%d %d/%d\n",j+1+xsize*i,j+1+xsize*i,j+1+xsize*(i+1),j+1+xsize*(i+1),j+2+xsize*i,j+2+xsize*i);
			printf("f %d/%d %d/%d %d/%d\n",j+1+xsize*(i+1),j+1+xsize*(i+1),j+2+xsize*(i+1),j+2+xsize*(i+1),j+2+xsize*i,j+2+xsize*i);
			//printf("f %d %d %d %d\n",dnum+j+1+xsize*i,dnum+j+1+xsize*(i+1),dnum+j+2+xsize*(i+1),dnum+j+2+xsize*i);
			printf("f %d/%d %d/%d %d/%d %d/%d\n",dnum+j+1+xsize*i,1,dnum+j+2+xsize*i,1,dnum+j+2+xsize*(i+1),1,dnum+j+1+xsize*(i+1),1);
			
		}
	}
	
	//N,S face
	for(int i=0;i<xsize-1;i++){
		printf("f %d %d %d %d\n",i+2,dnum+i+2,dnum+i+1,i+1);
		printf("f %d %d %d %d\n",xsize*(ysize-1)+i+1,dnum+xsize*(ysize-1)+i+1,dnum+xsize*(ysize-1)+i+2,xsize*(ysize-1)+i+2);
		
	}
	//W,E face
	for(int j=0;j<ysize-1;j++){	
		printf("f %d %d %d %d\n",xsize*j+1,dnum+xsize*j+1,dnum+xsize*(j+1)+1,xsize*(j+1)+1);
		printf("f %d %d %d %d\n",xsize*(j+2),dnum+xsize*(j+2),dnum+xsize*(j+1),xsize*(j+1));
		
	   
	}				
	
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
