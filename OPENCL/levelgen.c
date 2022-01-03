#include <main.h>
#include <intrin.h>

void levelgen(){
	for(int i = 0;i < properties->lvlSz * properties->lvlSz * properties->lvlSz;i++){
		if(_rdtsc() % 255 < 4){
			map[i] = 2;
		}
	}
}
