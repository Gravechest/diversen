float absf(float v){
	return v > 0 ? v : -v;
}

inline void OutOfBounce(int id,global unsigned char *data,float x,float y,float z){
	if((int)z + (int)x + (int)y  & 1){
		data[id] = 255;
		data[id+1] = 255;
		data[id+2] = 255;

	}
}


kernel void add(global unsigned char *data,global char *map,global float *player,global int *prop){
	int id = get_global_id(0)*4;
	float vz = sin((float)(id / prop[0]) / (prop[0] / 4 / player[6]) + player[1]);
	float vx = cos((float)(id % prop[0]) / (prop[0] / 4 / player[5]) + player[0]) * cos((float)(id / prop[0]) / (prop[0] / 4 / player[6]) + player[1]);
	float vy = sin((float)(id % prop[0]) / (prop[0] / 4 / player[5]) + player[0]) * cos((float)(id / prop[0]) / (prop[0] / 4 / player[6]) + player[1]);
	float x  = player[2];
	float y  = player[3];
	float z  = player[4];
	float dx,dy,dz;
	int pr = prop[1];
	if(vx > 0){
		if(vy > 0){
			if(vz > 0){
				for(int i = 0;i < prop[2];i++){
					dx = (int)x + 1 - x;
					dy = (int)y + 1 - y;
					dz = (int)z + 1 - z;
					if(dx / vx < dy / vy){
						if(dx / vx < dz / vz){
							x += dx;
							y += vy * dx / vx;
							z += vz * dx / vx;
						}
						else{
							x += vx * dz / vz;
							y += vy * dz / vz;
							z += dz;
						}
					}
					else if(dy / vy < dz / vz){
						x += vx * dy / vy;
						y += dy;
						z += vz * dy / vy;
					}
					else{
						x += vx * dz / vz;
						y += vy * dz / vz;
						z += dz;
					}
					if(x < 0 || y < 0 || z < 0 || x > pr || y > pr || z > pr){	
						OutOfBounce(id,data,x,y,z);	
						return;
					}
					switch(map[(int)x + (int)y * pr + (int)z * pr * pr]){
					case 1:
						if(x - (int)x == 0){
							data[id] = 160;
						}
						else if(y - (int)y == 0){
							data[id+1] = 160;
						}
						else{
							data[id+2] = 160;
						}
						return;
					case 2:
						data[id] = 255;
						return;
					}
				}
				return;
			}
			else{
				for(int i = 0;i < prop[2];i++){
					dx = (int)x + 1 - x;
					dy = (int)y + 1 - y;
					dz = (int)z - z;
					if(!dz){
						dz = -1;
					}
					if(dx / vx < dy / vy){
						if(dx / vx < dz / vz){
							x += dx;
							y += vy * dx / vx;
							z += vz * dx / vx;
						}
						else{
							x += vx * dz / vz;
							y += vy * dz / vz;
							z += dz;
						}
					}
					else if(dy / vy < dz / vz){
						x += vx * dy / vy;
						y += dy;
						z += vz * dy / vy;
					}
					else{
						x += vx * dz / vz;
						y += vy * dz / vz;
						z += dz;
					}
					if(x < 0 || y < 0 || z < 0.00001 || x > pr || y > pr || z > pr){
						OutOfBounce(id,data,x,y,z);	
						return;
					}
					switch(map[(int)x + (int)y * prop[1] + (int)(z - 0.00001) * prop[1] * prop[1]]){
					case 1:
						if(x - (int)x == 0){
							data[id] = 160;
						}
						else if(y - (int)y == 0){
							data[id+1] = 160;
						}
						else{
							data[id+2] = 160;
						}
						return;
					case 2:
						data[id] = 255;
						return;
					}
				}
				return;
			}
		}
		else{
			if(vz > 0){
					for(int i = 0;i < prop[2];i++){
					dx = (int)x + 1 - x;
					dy = (int)y - y;
					if(!dy){
						dy = -1;
					}
					dz = (int)z + 1 - z;
					if(dx / vx < dy / vy){
						if(dx / vx < dz / vz){
							x += dx;
							y += vy * dx / vx;
							z += vz * dx / vx;
						}
						else{
							x += vx * dz / vz;
							y += vy * dz / vz;
							z += dz;
						}
					}
					else if(dy / vy < dz / vz){
						x += vx * dy / vy;
						y += dy;
						z += vz * dy / vy;
					}
					else{
						x += vx * dz / vz;
						y += vy * dz / vz;
						z += dz;
					}
					if(x < 0 || y < 0.00001 || z < 0 || x > pr || y > pr || z > pr){
						OutOfBounce(id,data,x,y,z);	
						return;
					}
					switch(map[(int)x + (int)(y - 0.00001) * prop[1] + (int)z * prop[1] * prop[1]]){
					case 1:
						if(x - (int)x == 0){
							data[id] = 160;
						}
						else if(y - (int)y == 0){
							data[id+1] = 160;
						}
						else{
							data[id+2] = 160;
						}
						return;
					case 2:
						data[id] = 255;
						return;
					}
				}
				return;
			}
			else{
				for(int i = 0;i < prop[2];i++){
					dx = (int)x + 1 - x;
					dy = (int)y - y;
					if(!dy){
						dy = -1;
					}
					dz = (int)z - z;
					if(!dz){
						dz = -1;
					}
					if(dx / vx < dy / vy){
						if(dx / vx < dz / vz){
							x += dx;
							y += vy * dx / vx;
							z += vz * dx / vx;
						}
						else{
							x += vx * dz / vz;
							y += vy * dz / vz;
							z += dz;
						}
					}
					else if(dy / vy < dz / vz){
						x += vx * dy / vy;
						y += dy;
						z += vz * dy / vy;
					}
					else{
						x += vx * dz / vz;
						y += vy * dz / vz;
						z += dz;
					} 
					if(x < 0 || y < 0.00001 || z < 0.00001 || x > pr || y > pr || z > pr){
						OutOfBounce(id,data,x,y,z);	
						return;
					}
					switch(map[(int)x + (int)(y - 0.000001 ) * prop[1] + (int)(z - 0.000001) * prop[1] * prop[1]]){
					case 1:
						if(x - (int)x == 0){
							data[id] = 160;
						}
						else if(y - (int)y == 0){
							data[id+1] = 160;
						}
						else{
							data[id+2] = 160;
						}
						return;
					case 2:
						data[id] = 255;
						return;
					}
				}
				return;
			}
		}
	}
	else{
		if(vy > 0){
			if(vz > 0){
				for(int i = 0;i < prop[2];i++){
					dx = (int)x - x;
					if(!dx){
						dx = -1;
					}
					dy = (int)y + 1 - y;
					dz = (int)z + 1 - z;
					if(dx / vx < dy / vy){
						if(dx / vx < dz / vz){
							x += dx;
							y += vy * dx / vx;
							z += vz * dx / vx;
						}
						else{
							x += vx * dz / vz;
							y += vy * dz / vz;
							z += dz;
						}
					}
					else if(dy / vy < dz / vz){
						x += vx * dy / vy;
						y += dy;
						z += vz * dy / vy;
					}
					else{
						x += vx * dz / vz;
						y += vy * dz / vz;
						z += dz;
					}
					if(x < 0.00001 || y < 0 || z < 0 || x > pr || y > pr || z > pr){
						OutOfBounce(id,data,x,y,z);	
						return;
					}
					switch(map[(int)(x - 0.000001) + (int)y * prop[1] + (int)z * prop[1] * prop[1]]){
					case 1:
						if(x - (int)x == 0){
							data[id] = 160;
						}
						else if(y - (int)y == 0){
							data[id+1] = 160;
						}
						else{
							data[id+2] = 160;
						}
						return;
					case 2:
						data[id] = 255;
						return;
					}
				}
				return;
			}
			else{
				for(int i = 0;i < prop[2];i++){
					dx = (int)x - x;
					if(!dx){
						dx = -1;
					}
					dy = (int)y + 1 - y;
					dz = (int)z - z;
					if(!dz){
						dz = -1;
					}
					if(dx / vx < dy / vy){
						if(dx / vx < dz / vz){
							x += dx;
							y += vy * dx / vx;
							z += vz * dx / vx;
						}
						else{
							x += vx * dz / vz;
							y += vy * dz / vz;
							z += dz;
						}
					}
					else if(dy / vy < dz / vz){
						x += vx * dy / vy;
						y += dy;
						z += vz * dy / vy;
					}
					else{
						x += vx * dz / vz;
						y += vy * dz / vz;
						z += dz;
					}
					if(x < 0.00001 || y < 0 || z < 0.00001 || x > pr || y > pr || z > pr){
						OutOfBounce(id,data,x,y,z);	
						return;
					}
					switch(map[(int)(x - 0.000001) + (int)y * prop[1] + (int)(z - 0.000001) * prop[1] * prop[1]]){
					case 1:
						if(x - (int)x == 0){
							data[id] = 160;
						}
						else if(y - (int)y == 0){
							data[id+1] = 160;
						}
						else{
							data[id+2] = 160;
						}
						return;
					case 2:
						data[id] = 255;
						return;
					}
				}
				return;
			}
		}
		else{
			if(vz > 0){
				for(int i = 0;i < prop[2];i++){
					dx = (int)x - x;
					if(!dx){
						dx = -1;
					}
					dy = (int)y - y;
					if(!dy){
						dy = -1;
					}
					dz = (int)z + 1 - z;
					if(dx / vx < dy / vy){
						if(dx / vx < dz / vz){
							x += dx;
							y += vy * dx / vx;
							z += vz * dx / vx;
						}
						else{
							x += vx * dz / vz;
							y += vy * dz / vz;
							z += dz;
						}
					}
					else if(dy / vy < dz / vz){
						x += vx * dy / vy;
						y += dy;
						z += vz * dy / vy;
					}
					else{
						x += vx * dz / vz;
						y += vy * dz / vz;
						z += dz;
					}
					if(x < 0.00001 || y < 0.00001 || z < 0 || x > pr || y > pr || z > pr){
						OutOfBounce(id,data,x,y,z);	
						return;
					}
					switch(map[(int)(x - 0.000001) + (int)(y - 0.000001) * prop[1] + (int)z * prop[1] * prop[1]]){
					case 1:
						if(x - (int)x == 0){
							data[id] = 160;
						}
						else if(y - (int)y == 0){
							data[id+1] = 160;
						}
						else{
							data[id+2] = 160;
						}
						return;
					case 2:
						data[id] = 255;
						return;
					}
				}
				return;
			}
			else{
				for(int i = 0;i < prop[2];i++){
					dz = (int)z - z;
					if(!dz){
						dz = -1;
					}
					dx = (int)x - x;
					if(!dx){
						dx = -1;
					}
					dy = (int)y - y;
					if(!dy){
						dy = -1;
					}
					if(dx / vx < dy / vy){
						if(dx / vx < dz / vz){
							x += dx;
							y += vy * dx / vx;
							z += vz * dx / vx;
						}
						else{
							x += vx * dz / vz;
							y += vy * dz / vz;
							z += dz;
						}
					}
					else if(dy / vy < dz / vz){
						x += vx * dy / vy;
						y += dy;
						z += vz * dy / vy;
					}
					else{
						x += vx * dz / vz;
						y += vy * dz / vz;
						z += dz;
					}
					if(x < 0.00001 || y < 0.00001 || z < 0.00001 || x > pr || y > pr || z > pr){
						OutOfBounce(id,data,x,y,z);
						return;
					}
					switch(map[(int)(x - 0.000001) + (int)(y - 0.000001) * prop[1] + (int)(z - 0.000001) * prop[1] * prop[1]]){
					case 1:
						if(x - (int)x == 0){
							data[id] = 160;
						}
						else if(y - (int)y == 0){
							data[id+1] = 160;
						}
						else{
							data[id+2] = 160;
						}
						return;
					case 2:
						data[id] = 255;
						return;
					}
				}
				return;
			}
		}
	}
}































