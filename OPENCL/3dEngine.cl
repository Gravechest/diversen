kernel void add(global unsigned char *data,global char *map,global float *player ){
	int id = get_global_id(0)*4 ;
	float vz = cos((float)(id / 2048) / 128  + player[1]);
	float vx = cos((float)(id % 2048) / 2048 + player[0]);
	float vy = sin((float)(id % 2048) / 2048 + player[0]);
	float x  = 0;
	float y  = 0;
	float z  = 0;
	if(vx < 0 || vy < 0 || vz < 0){
		return;
	}
	for(int i = 0;i < 255;i++){
		float dx = (int)x + 1 - x;
		float dy = (int)y + 1 - y;
		float dz = (int)z + 1 - z;
		if(dx / vx < dy / vy && dx / vx < dz / vz){
			x += dx;
			y += vy * (dx / vx);
			z += vz * (dx / vx);
		}
		else if(dy / vy < dz / vz && dy / vy < dx / vx){
			x += vx * (dy / vy);
			y += dy;
			z += vz * (dy / vy);
		}
		else{
			x += vx * (dx / vx);
			y += vy * (dx / vx);
			z += dz;
		}
		if(x > 8 || y > 8 || z > 8){
			data[id] = i;
			return;
		}
		if(map[(int)x + ((int)y * 8) + ((int)z * 64)]){
			data[id+1] = 127;
			return;
		}
	} 
	
}
































