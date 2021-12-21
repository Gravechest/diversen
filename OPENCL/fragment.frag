#version 460 core

out vec4 FragColor;

uniform sampler3D map;

void main(){
    float vx = sin(gl_FragCoord.x / 500.0 + 0.0) * cos(gl_FragCoord.y / 500.0 + 0.0);
    float vy = cos(gl_FragCoord.x / 500.0 + 0.0) * cos(gl_FragCoord.y / 500.0 + 0.0);
    float vz = sin(gl_FragCoord.y / 500.0 + 0.0);
    float x = 2.0;
    float y = 2.0;
    float z = 0.0;
    float dx,dy,dz;
    for(int i = 0;i < 256;i++){
        if(vx > 0.0){
            dx = float(int(x)) + 1.0 - x;
        }
        else{
            dx = float(int(x)) - x;
            if(dx == 0.0){
                dx = -1.0;
            }
        }
        if(vy > 0.0){
            dy = float(int(y)) + 1.0 - y;
        }
        else{
            dy = float(int(y)) - y;
            if(dy == 0.0){
                dy = -1.0;
            }
        }
        if(vz > 0.0){
            dz = float(int(z)) + 1.0 - z;
        }
        else{
            dz = float(int(z)) - z;
            if(dz == 0.0){
                dz = -1.0;
            }
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
        if(x < 0.0 || x > 8.0){
            FragColor.r = 1.0;
            break;
        }
        if(y < 0.0 || y > 8.0){
            FragColor.g = 1.0;
            break;
        }
        if(z < 0.0 || z > 8.0){
            FragColor.b = 1.0;
            break;
        }
		vec4 testmap = texelFetch(map,ivec3(x,y,z),0);
        if(testmap[0] == 1){
            FragColor.r = x - float(int(x));
            FragColor.g = y - float(int(y));
            FragColor.b = z - float(int(z));
            break;
        }
    }
}
