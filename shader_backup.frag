#version 330 core

const int MAX_SHAPES = 25;

uniform float runTime;
uniform sampler2D tex;

in vec3 direction;
in vec3 camPos;

uniform vec3 positions[MAX_SHAPES];
uniform vec3 sizes[MAX_SHAPES];

uniform int sphereIndices[MAX_SHAPES];
uniform int sphereCount = 0;

uniform int boxIndices[MAX_SHAPES];
uniform int boxCount = 0;

uniform int cylinderIndices[MAX_SHAPES];
uniform int cylinderCount = 0;

vec3 color, dir, diff, normal, p;
float surfDist, minDist, t;
int i, index;

const float epsilon = .0001;

float sphereCollision(vec3 pointPos, float pointRadius, vec3 center, vec3 size){
    return length((pointPos - center) / size) - 1.0 - pointRadius;
}

float boxCollision(vec3 pointPos, float pointRadius, vec3 center, vec3 size){
    diff = abs(pointPos - center) - size;
    return max(diff.x, max(diff.y, diff.z));
}

float cylinderCollision(vec3 pointPos, float pointRadius, vec3 center, vec3 size){
    diff = pointPos - center;
    return max(abs(diff.y) - size.y, length(diff.xz / size.xz) - 1 - pointRadius);
}

float coneCollision(vec3 pointPos, float pointRadius, vec3 center, vec3 size){
    diff = pointPos - center;
    return max(abs(diff.y) - size.y, 
               length(diff.xz / (size.xz * (diff.y / size.y))) - 1 - pointRadius);
}

vec4 scene(in vec3 p){
    minDist = 10000.0; 
    
    // p.x += cos(p.y + runTime);
    // p.y += cos(p.x + runTime);

    minDist = min(minDist, p.y - 0.2*sin(length(p.xz) * radians(360.0) - runTime * 10.0));
    
    for (i=0; i < sphereCount; i++){
        index = sphereIndices[i];
        minDist = min(minDist, sphereCollision(p, .1, positions[index], sizes[index]));
    }
    for (i=0; i < boxCount; i++){
        index = boxIndices[i];
        minDist = min(minDist, boxCollision(p, .1, positions[index], sizes[index]));
    }
    for (i=0; i < cylinderCount; i++){
        index = cylinderIndices[i];
        minDist = min(minDist, cylinderCollision(p, .1, positions[index], sizes[index]));
    }
    return vec4(1.0, 0.0, 0.0, minDist * .5);
}

vec3 estimateNormal(in vec3 p, in float d) { 
    return normalize(vec3(scene(vec3(p.x - epsilon, p.yz)).w,
                          scene(vec3(p.x, p.y - epsilon, p.z)).w,
                          scene(vec3(p.xy, p.z - epsilon)).w) - d);
}

vec3 trace(in vec3 o, in vec3 dir){
    for (int i=0; i < 50; i++)
    {   
        p = o + dir * t;
        surfDist = scene(p).w;
        t += surfDist * 2;
        if (surfDist < .01) {
            return estimateNormal(p, surfDist);
        }
    }
    return vec3(0.0);
}

void main()
{
    dir = normalize(direction);
    color = trace(camPos, dir);
    //color = trace(p, reflect(p, color));

    gl_FragColor = vec4(color * .5 + .5, 1.0);
}