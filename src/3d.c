#include "SDL/SDL.h"
#include <math.h>
#include "3d.h"

#define focusdistance 400
#define ycenter 00
int xcenter=0;
int zcenter=1000;

float normalize(float x, float y, float z) {
	return (1.0/sqrt(x*x+y*y+z*z));
}
void cross_product(vertex_type *vertex1, vertex_type *vertex2, vertex_type *vertex3,float *x, float *y, float *z) {
	float dx1 = (vertex1->x-vertex3->x);
	float dy1 = (vertex1->y-vertex3->y);
	float dz1 = (vertex1->z-vertex3->z);
	float dx2 = (vertex2->x-vertex3->x);
	float dy2 = (vertex2->y-vertex3->y);
	float dz2 = (vertex2->z-vertex3->z);
	*x = dy1*dz2 - dz1*dy2;
	*y = dx1*dz2 - dz1*dx2;
	*z = dx1*dy2 - dy1*dx2;
}
void cylinder (obj_type *obj, int sides) {

	float g;
	float x,y,z;

	obj->vertex=malloc(sizeof(vertex_type)*sides*2);
	obj->vertexrotated=malloc(sizeof(vertex_type)*sides*2);
	obj->vertex2d=malloc(sizeof(vertex2d_type)*sides*2);
	//obj->polygon=malloc(sizeof(polygon_type)*sides*4-2);
	obj->surface_size=sides + 2;
	obj->surfaces=malloc(sizeof(surface_type)*obj->surface_size);

	vertex_type *v = obj->vertex;

	int i=0;
	for (g=0;g<360;g+=(360.0/sides)) {
		x = MAG * cos(g*M_PI/180);
		y = MAG * sin(g*M_PI/180);
		z = (float)MAG;
		fprintf(stderr,"%f,%f,%f,255,\n",x,y,z);
		fprintf(stderr,"%f,%f,%f,255,\n",x,y,-z);
		v->x = x;v->y=y;v->z=z;v++;
		v->x = x;v->y=y;v->z=-z;v++;
		i+=2;
	}
	obj->vertex_size = i;

	int h;
	surface_type *s = obj->surfaces;
	for (h=0;h<sides;h++) {
		s->vertex_size = 4;
		s->vertex = malloc(sizeof(int)*s->vertex_size);
		s->vertex[0] = h*2;
		s->vertex[1] = h*2+1;
		s->vertex[2] = (h*2+3)%i;
		s->vertex[3] = (h*2+2)%i;
		s++;
	}

	s->vertex_size = sides;
	s->vertex = malloc(sizeof(int)*s->vertex_size);
	s->vertex[0] = 0;
	for (h=1;h<sides;h++) s->vertex[h] = h*2;
	s++;

	s->vertex_size = sides;
	s->vertex = malloc(sizeof(int)*s->vertex_size);
	s->vertex[0] = 1;
	for (h=1;h<sides;h++) s->vertex[h] = (sides-h)*2+1;

/*
	polygon_type *p = obj->polygon;
	int h;
	for (h=0;h<sides;h++) {
		fprintf(stderr,"%d,%d,%d,\n",h*2,h*2+1,(h*2+2)%(sides*2));
		fprintf(stderr,"%d,%d,%d,\n",(h*2+2)%(sides*2),(h*2+1)%(sides*2),(h*2+3)%(sides*2));
		p->a = h*2; p->b = h*2+1; p->c = (h*2+2)%(sides*2); p++;
		p->a = (h*2+2)%(sides*2); p->b = h*2+1; p->c = (h*2+3)%(sides*2); p++;
	}
	for (h=1;h<sides-1;h++) {
		fprintf(stderr,"%d,%d,%d,\n",0,h*2,(h*2+2)%(sides*2));
		fprintf(stderr,"%d,%d,%d,\n",1,(h*2+3)%(sides*2),h*2+1);
		p->a = 0; p->b = h*2; p->c = (h*2+2)%(sides*2); p++;
		p->a = 1; p->b = (h*2+3)%(sides*2); p->c = (h*2+1)%(sides*2); p++;
	}
*/

}

void torus(obj_type *obj,int sides, int sections) {
	obj->vertex_size = sides * sections;
	obj->vertex = malloc(sizeof(vertex_type)*obj->vertex_size);
	obj->vertexrotated = malloc(sizeof(vertex_type)*obj->vertex_size);
	obj->vertex2d = malloc(sizeof(vertex2d_type)*obj->vertex_size);

	obj->surface_size = sides*sections;
	obj->surfaces = malloc(sizeof(surface_type)*obj->surface_size);

//	obj->polygon_size = sides*sections*2;
//	obj->polygon = malloc(sizeof(polygon_type)*obj->polygon_size);

	int c1,c2;
	float xbase,ybase,zbase;
	vertex_type *v = obj->vertex;
	for (c1=0;c1<sides;c1++) {
		xbase = cos(c1*M_PI*2/sides);
		ybase = sin(c1*M_PI*2/sides);
		zbase = 0;
		for (c2=0;c2<sections;c2++,v++) {
			v->x = xbase*(MAG + MAG * cos(c2*M_PI*2/sections)/3);
			v->y = ybase*(MAG + MAG * cos(c2*M_PI*2/sections)/3);
			v->z = MAG*sin(c2*M_PI*2/sections)/3;
		}
	}
	surface_type *s = obj->surfaces;
	for (c1=0;c1<sides;c1++) {
		for (c2=0;c2<sections;c2++,s++) {
			s->vertex_size = 4;
			s->vertex = malloc(sizeof(int)*4);
			s->vertex[0] = c1*sections + c2;
			s->vertex[3] = c1*sections + (c2+1)%sections;
			s->vertex[2] = ((c1+1)%sides)*sections + (c2+1)%sections;
			s->vertex[1] = ((c1+1)%sides)*sections + c2;

		}
	}

}

void generate_polygons(obj_type *obj) {
	int h;
	int j;
	int poly_size = 0;
	surface_type *s;
	polygon_type *p;
	int *v;

	s = obj->surfaces;
	for (h=0;h<obj->surface_size;h++) {
		poly_size += s->vertex_size-2;
		s++;
	}

	obj->polygon = malloc(sizeof(polygon_type)*poly_size);
	obj->polygon_size = poly_size;

	s = obj->surfaces;
	p = obj->polygon;
	for (h=0;h<obj->surface_size;h++) {
		v = s->vertex;
		for (j=0;j<s->vertex_size-2;j++) {
			p->a = v[0];
			p->b = v[j+1];
			p->c = v[j+2];
			p->surface = s;
			p++;
		}
		s++;
	}
}

/*
void torusknot(obj_type *obj) {
	obj->vertex=malloc(sizeof(vertex_type)*361);
	obj->vertexrotated=malloc(sizeof(vertex_type)*361);
	obj->vertex2d=malloc(sizeof(vertex2d_type)*361);
	obj->polygon=malloc(sizeof(polygon_type)*360);

	vertex_type *v = obj->vertex;
	polygon_type *poly = obj->polygon;
	int i,j;
	float x,y,z,r,p,q;
	q = 8; p = 3;
	j=0;
	for (i=0; i<360;i++) {
		r = cos(q*i*M_PI/180);
		x = MAG*r*cos(p*i*M_PI/180);
		y = MAG*r*sin(p*i*M_PI/180);
		z = MAG*-sin(q*i*M_PI/180);
		v->x = x;
		v->y = y;
		v->z = z;
		fprintf(stderr,"%f %f %f\n",x,y,z);
		v++;
		poly->a = (j++)%360;
		poly->b = (j)%360;
		poly->c = (j+1)%360;
		fprintf(stderr,"%d %d %d\n",poly->a,poly->b,poly->c);
		poly++;
	}
	v->x = 0;
	v->y = 0;
	v->z = 0;
}

*/

Uint32 TimeLeft() {
    static Uint32 next_time = 0;
    Uint32 now;

    now = SDL_GetTicks();
    if ( next_time <= now ) {
        next_time = now+TICK_INTERVAL;
        return(0);
    }
    return(next_time-now);
}
int setupSDL() {
	SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);
	screen = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,0,SDL_SWSURFACE);
	return screen == NULL;
}
void drawpixel(SDL_Surface *screen, int x, int y) {
	Uint32 color = SDL_MapRGB(screen->format, 255,255,255);
    if ( SDL_MUSTLOCK(screen) ) {
        if ( SDL_LockSurface(screen) < 0 ) {
            //return 1;
        }
    }
    //py=(n>>5)+(n<<8)/(600+(y-(n>>1)));
    //px=(n>>1)+((x-(n>>1))<<8)/(400+(y-(n>>1)));

    if (x>=0&&x<SCREEN_WIDTH&&y>=0&&y<SCREEN_HEIGHT) {
		//bufp = ;// + py*screen->pitch + px*4;
		*(Uint32*)((Uint8 *)screen->pixels + y*screen->pitch + (x<<2)) = color;
    }
    if ( SDL_MUSTLOCK(screen) ) {
        SDL_UnlockSurface(screen);
    }
}

void project(obj_type *obj) {

	int counter;

	float x;
	float y;
	float z;

	vertex_type *vertexrotated = obj->vertexrotated;
	vertex2d_type *vertex2d = obj->vertex2d;

	for (counter=0; counter< obj->vertex_size; counter++) {
		x = vertexrotated[counter].x;
		y = vertexrotated[counter].y;
		z = vertexrotated[counter].z;

		if (focusdistance+zcenter>0) {
			vertex2d[counter].x = (SCREEN_WIDTH>>1) +(focusdistance*(xcenter-x))/(focusdistance+zcenter+z);
			vertex2d[counter].y = (SCREEN_HEIGHT>>1) + (focusdistance*(ycenter-y))/(focusdistance+zcenter+z);
//			vertex2d[counter].color = vertexrotated[counter].color;
		} else {
			vertex2d[counter].x = 100000000;
			vertex2d[counter].y = 100000000;
		}
	}
}
void drawtriangle(SDL_Surface *s, vertex2d_type *vertex1, vertex2d_type *vertex2, vertex2d_type *vertex3, int color) {
	//Uint32 color = SDL_MapRGB(screen->format, 255,255,255);
	int xcurrent1,xcurrent2,xcur;
	int ycurrent;

	if (vertex1->y>vertex2->y) SWAP(vertex1,vertex2);
	if (vertex2->y>vertex3->y) SWAP(vertex2,vertex3);
	if (vertex1->y>vertex2->y) SWAP(vertex1,vertex2);

//	fprintf(stderr,"%d %d %d\n",(int)vertex1->y,(int)vertex2->y,(int)vertex3->y);

	int xtop = ((int)vertex1->x)<<16;
	int ytop = (int)vertex1->y;
	int xmid = ((int)vertex2->x)<<16;
	int ymid = (int)vertex2->y;
	int xbot = ((int)vertex3->x)<<16;
	int ybot = (int)vertex3->y;

	int xtopbotdelta = xbot-xtop;
	int ytopbotdelta = ybot-ytop;
	xtopbotdelta /= (ytopbotdelta?ytopbotdelta:1<<16);

	int xtopmiddelta = xmid-xtop;
	int ytopmiddelta = ymid-ytop;
	xtopmiddelta /= (ytopmiddelta?ytopmiddelta:1<<16);

	int xmidbotdelta = xbot-xmid;
	int ymidbotdelta = ybot-ymid;
	xmidbotdelta /= (ymidbotdelta?ymidbotdelta:1<<16);

	xcurrent1 = (ytop==ymid?xmid:xtop);
	xcurrent2 = xtop;

	int xleft,xright;

	color = color + (color/4)*256+(color/8)*65536;


	for (ycurrent=ytop;ycurrent<=ybot;ycurrent++) {
		if (ycurrent>=0&&ycurrent<SCREEN_HEIGHT) {
			//fprintf(stderr,"%d %d  %d %d \n",xcurrent1>>16,ycurrent,xcurrent2>>16,ycurrent);
			xleft = xcurrent1>>16; xright = xcurrent2>>16;
			if (xleft>xright) SWAP(xleft, xright);
			if (xleft<0) xleft=0;
			if (xleft>=SCREEN_WIDTH) xleft=SCREEN_WIDTH;
			if (xright<0) xright=-1;
			if (xright>=SCREEN_WIDTH) xright=SCREEN_WIDTH-1;

			for (xcur=xleft;xcur<=xright;xcur++)
				*(Uint32*)((Uint8 *)s->pixels + ycurrent*s->pitch + ((xcur)<<2)) = (Uint32)color;
//			} else {
//				for (xcur=xcurrent1>>16;xcur<=xcurrent2>>16;xcur++)
//					*(Uint32*)((Uint8 *)s->pixels + ycurrent*s->pitch + ((xcur)<<2)) = color;
//
//			}

	//		drawpixel(s,xcurrent1>>16,ycurrent);
	//		drawpixel(s,xcurrent2>>16,ycurrent);
		}
		xcurrent1+=(ycurrent<ymid?xtopmiddelta:xmidbotdelta);
		xcurrent2+=xtopbotdelta;



	}


}

void rotate3d(obj_type *obj, float alpha, float beta, float gamma) {

	float cosa,cosb,cosc,sina,sinb,sinc;
	cosa = cos(alpha); sina = sin(alpha);
	cosb = cos(beta); sinb = sin(beta);
	cosc = cos(gamma); sinc = sin(gamma);

	int counter;
	float x, y, z;
	float xt, yt, zt;
	vertex_type *vertex = obj->vertex;
	vertex_type *vertexrotated = obj->vertexrotated;

	for (counter=0; counter<obj->vertex_size; counter++) {
		x = vertex[counter].x*40;
		y = vertex[counter].y*40;
		z = vertex[counter].z*40;

		if (alpha>0) {
			yt = y*cosa-z*sina;
			zt = y*sina+z*cosa;
			y = yt; z = zt;
		}
		if (beta>0) {
			xt = x*cosb-z*sinb;
			zt = x*sinb+z*cosb;
			x = xt; z = zt;
		}
		if (gamma>0) {
			xt = x*cosc-y*sinc;
			yt = x*sinc+y*cosc;
			x = xt; y = yt;
		}
		vertexrotated[counter].x = x;
		vertexrotated[counter].y = y;
		vertexrotated[counter].z = z;
	}

	int counter2;
	surface_type *s = obj->surfaces;
//	fprintf(stderr,"%d\n",obj->surface_size);
	for (counter=0;counter<obj->surface_size;counter++) {
		cross_product(&(obj->vertexrotated[s->vertex[2]]), &(obj->vertexrotated[s->vertex[1]]),&(obj->vertexrotated[s->vertex[0]]),&x,&y,&z);
		z *= normalize(x,y,z);
		z = sin(z*M_PI/2);
		z *= z*z*z*255;
	//	fprintf(stderr,"%f ",z);
		//if (z>0)
			s->color = z; //else s->color =0;

		s++;
	}

}

void drawpolygons(SDL_Surface *screen, obj_type *obj) {
	int counter;
	int dxab, dxac, dxbc, dyab, dyac, dybc;
	float x,y,z;

	polygon_type *polygon = obj->polygon;
	vertex2d_type *vertex2d = obj->vertex2d;

	for (counter=0;counter<obj->polygon_size;counter++) {
		dxab = vertex2d[polygon[counter].b].x - vertex2d[polygon[counter].a].x+1;
		dyab = vertex2d[polygon[counter].b].y - vertex2d[polygon[counter].a].y+1;
		dxac = vertex2d[polygon[counter].c].x - vertex2d[polygon[counter].a].x+1;
		dyac = vertex2d[polygon[counter].c].y - vertex2d[polygon[counter].a].y+1;
		//cross_product(&(obj->vertex[polygon[counter].a]),&(obj->vertex[polygon[counter].b]),&(obj->vertex[polygon[counter].c]),&x,&y,&z);
		//fprintf(stderr,"(%d,%d) (%d,%d) (%d,%d)\n",vertex2d[polygon[counter].a].x,vertex2d[polygon[counter].a].y,vertex2d[polygon[counter].b].x,vertex2d[polygon[counter].b].y,vertex2d[polygon[counter].c].x,vertex2d[polygon[counter].c].y);
//		dxbc = vertex2d[polygon[counter].c].x - vertex2d[polygon[counter].b].x;
//		dybc = vertex2d[polygon[counter].c].y - vertex2d[polygon[counter].b].y;

		if (dxab * dyac < dxac * dyab)
			//fprintf(stderr,"Drawing..\n");
			drawtriangle(screen, &vertex2d[polygon[counter].a],&vertex2d[polygon[counter].b],&vertex2d[polygon[counter].c],polygon[counter].surface->color);
		//}
	}
}

int main(int argc, char* args[]) {
	SDL_Event event;
	if (setupSDL()) {
	    fprintf(stderr,"Unable to setup SDL: %s\n",SDL_GetError());
	    exit(1);
	}
//	obj_type *obj;
//	obj = malloc(sizeof(obj_type));
//	cylinder(obj,50);
//	//torusknot(obj);
//	generate_polygons(obj);

	obj_type *obj;
	obj = malloc(sizeof(obj_type));
	torus(obj,32,16);
	generate_polygons(obj);


	if (obj->polygon == NULL) {
		fprintf(stderr,"Forgot to generate polygons!\n");
		exit(1);
	}
	float a;
	int i=0;
	for ( a=0.001;a<10;a+=0.005) {
		SDL_FillRect(screen,NULL,0x0);

//		drawtriangle(screen,&v1,&v2,&v3);

		//if (i%20<12) {
			rotate3d(obj,a*0.1,a*1.2,a*1.4);
			project(obj);

			drawpolygons(screen,obj);
//		} else {
//			rotate3d(obj2,a*0.2,a*0.3,a*0.9);
//			project(obj2);
//
//			drawpolygons(screen,obj2);
//
//		}


		SDL_UpdateRect(screen, 0,0,SCREEN_WIDTH,SCREEN_HEIGHT);

		SDL_Delay(TimeLeft());
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					exit(0);
			}
		}
		i++;
	}

	while(1)
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				exit(0);
		}
	}

	return 0;
}
