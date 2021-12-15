kernel void add(global unsigned char *data,global char *map,global unsigned  short *prop,int count){
	for(int i = 0;i < count;i++){
		char r = prop[i*3+2] >> 0 & 15;
		char g = prop[i*3+2] >> 4 & 15;
		char b = prop[i*3+2] >> 8 & 15;
		int id = get_global_id(0);
		float x  = (float)prop[i*3+0];
		float y  = (float)prop[i*3+1];
		float vx = cos((float)id / 1.1330 );
		float vy = sin((float)id / 1.1330 );
		while(x > 0 && y > 0 && x < 1024 && y < 1024){
			x += vx;
			y += vy;
			int val = (((int)x * 1024) + (int)y) * 4;
			if(data[val] < 240){
				data[val]+=b;
			}
			else{
				data[val] = 255;
			}
			if(data[val+1] < 240){
				data[val+1]+=g;
			}
			else{
				data[val+1] = 255;
			}
			if(data[val+2] < 240){
				data[val+2]+=r;
			}
			else{
				data[val+2] = 255;
			}
		}
	}
}






