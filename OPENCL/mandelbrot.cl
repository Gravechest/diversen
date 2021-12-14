kernel void add(global int *val){
	int id = get_global_id(0);
	float x = ((float)(id / 256) - 120) / 128;
	float y = ((float)(id % 256) - 120) / 128;
	float vx = 0;
	float vy = 0;
	int itt = 0;
	while(vx + vy < 512 && itt < 512){
		float xtemp = vx*vx-vy*vy+x;
		vy = 2*vx*vy+y;
		vx = xtemp;
		itt++;
	}
	val[id] = itt;
}
