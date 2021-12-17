float absf(float x){
	return x > 0 ? x : -x;
}

inline void fill(global unsigned char *data,int i,char r,char g,char b){
	if(data[i] < 240){
		data[i]+=b;
	}
	else{
		data[i] = 255;
	}
	if(data[i+1] < 240){
		data[i+1]+=g;
	}
	else{
		data[i+1] = 255;
	}
	if(data[i+2] < 240){
		data[i+2]+=r;
	}
	else{
		data[i+2] = 255;
	}
}

kernel void add(global unsigned char *data,global unsigned  char *map,global unsigned short *prop,int reso,int rayC){
	int id  = get_global_id(0);
	char r = prop[(id / rayC)*3+2] >> 0 & 15;
	char g = prop[(id / rayC)*3+2] >> 4 & 15;
	char b = prop[(id / rayC)*3+2] >> 8 & 15;
	float x  = (float)prop[(id / rayC) *3];
	float y  = (float)prop[(id / rayC) *3+1];
	float vx = cos((float)id / 1.1);
	float vy = sin((float)id / 1.1);
	for(;;){
		x += vx;
		y += vy;
		int val = (((int)x * reso) + (int)y) * 4;
		if(map[val >> 2]){
			switch(map[val >> 2]){
			case 1:
				return;
			case 2:{
                                float nmy = (y - (int)y) / (x - (int)x);
                    		float nmx = vy / vx;
			                 if(vx > 0 && vy > 0){
						if(nmy < nmx){
							if(map[val-reso]){
								vx = -vx;
							}
							else{
								vy = -vy;
							}
                    				}
                    				else{
							if(map[val-1]){
								vy = -vy;
							}
							else{
								vx = -vx;
							}
                    				}
					}
					else if(vx > 0 && vy < 0){
						if(nmy < nmx){
							if(map[val-1]){
								vy = -vy;
							}
							else{
								vx = -vx;
							}
                    				}
                    				else{
							if(map[val-reso]){
								vx = -vx;
							}
							else{
								vy = -vy;
							}
                    				}
					}
					else if(vx < 0 && vy > 0){
						if(nmy < nmx){
							if(map[val-1]){
								vy = -vy;
							}
							else{
								vx = -vx;
							}
                    				}
                    				else{
							if(map[val+reso]){
								vx = -vx;
							}
							else{
								vy = -vy;
							}
                    				}
					}
					else{
						if(nmy < nmx){
							if(map[val+reso]){
								vx = -vx;
							}
							else{
								vy = -vy;
							}
                    				}
                    				else{
							if(map[val-reso]){
								vy = -vy;
							}
							else{
								vx = -vx;
							}
                    				}
					}
					fill(data,val,r << 3,g << 3,b << 3);
					r >>= 1;
					g >>= 1;
					b >>= 1;
					if(!(r + g + b)){
						return;
					}
					continue;
				}
			}
		}
		fill(data,val,r,g,b);
	}
}






