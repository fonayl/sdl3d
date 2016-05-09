/*
 * 3d.h
 *
 *  Created on: Mar 10, 2013
 *      Author: luc
 */
#ifndef _3D_H_
#define _3D_H_

#define MAX_VERTICES 256
#define MAX_POLYGONS 256
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define MAG 20

#define SWAP(A, B) {struct tempStruct { char C[sizeof(A)];} swap_tmp;\
    swap_tmp = *( struct tempStruct*) &A;\
    *( struct tempStruct*) &A = *( struct tempStruct*) &B;\
    *( struct tempStruct*) &B = swap_tmp;}

#define TICK_INTERVAL 30

SDL_Surface *screen;

typedef struct {
	float x,y,z;
	//Uint32 color;
} vertex_type;

typedef struct {
	int x,y;
	//Uint32 color;
} vertex2d_type;

typedef struct {
	int *vertex;
	int vertex_size;
	Uint32 color;
} surface_type;

typedef struct {
	int a,b,c;
	surface_type *surface;
} polygon_type;

typedef struct {
	vertex_type *vertex;
	int vertex_size;
	vertex_type *vertexrotated;
	vertex2d_type *vertex2d;
	polygon_type *polygon;
	int polygon_size;
	surface_type* surfaces;
	int surface_size;
} obj_type, *obj_type_ptr;

//void rotate3d(vertex_type *vertex, vertex_type *vertex_rotated, float alpha, float beta, float gamma);
//void drawtriangle(SDL_Surface *s, vertex2d_type *vertex1, vertex2d_type *vertex2, vertex2d_type *vertex3);

#endif
