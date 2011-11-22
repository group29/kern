/*
#include <array.h>
#include "userfile.h"


//Dasmit, Ishaan
struct array* createfiletabe(){
	return array_create();


}


void destoryfiletable(struct array* a){
	for(int i=0;i < a->num;i++){
		if(a->v != NULL){	 
			kfree(a->v[i]->vp);
			kfree(a->v[i]->filename);
			kfree(a->v[i]);		
			array_destroy(a);
		}
		else{
			i++;
		}
	}
}



int getIndex(struct array *a,struct filedescriptor* guy){
	for (int x = 0; x < a->num; x++)
	{			
		if(array_getguy(a, x)==NULL){
			return i;
			break;
		}
	}
	return -1;
}

int addID (struct array *a, struct filedescriptor* guy) {

	int index = getIndex(a, guy);
	if (index != -1){
		array_setguy(a, guy, index);
		return index;
	}
	else {
		array_add(a,guy);
		return (a->num - 1); 
	}


}
/*
filedescriptor* getFD (struct array *a, int fileid)
{
return (filedescriptor *)array_getguy(a,fileid);

}*/
	


