#include <stdio.h>
#include "dstype.h"
#include "dsxdr.h"

#include "dscuts.h"

#define PP printf(
extern int gDone;


void CutsInit(void) {
  printf("Initializing cuts.\n"); gDone=0;
}









void ErrTop(int x) {
  PP"Error number %d in the file %s.\n",__FILE__);
}
int TopArray(
  DS_DATASET_T *dsPtr, size_t colNum,int tentSubscript,
  int *off, size_t *dim, size_t *array_size_t, char **typeName) {
  /* see comment uu4 */
  if(!dsColumnTypeName(typeName,dsPtr,colNum))          ErrTop( 17);
  if(!dsColumnDimCount(dim,dsPtr,colNum))               ErrTop( 18);
  if(!dsColumnDimensions(array_size_t,dsPtr,colNum))    ErrTop( 19);
  if(*dim>0) { 
    *off=tentSubscript; if(*off<0||*off>=*array_size_t) ErrTop( 20); 
  } else *off=0;
  return TRUE;
}
