
layout(location = 0) out vec4 FragColor;

const int OBJECT_COUNT = 3;

const int MAX_SHAPES = 25;
const float EPSILON = .0001;

uniform float runTime;
uniform sampler2D tex;
uniform float startingDist;

in vec3 direction;
in vec3 camPos;

vec3 color, dir, diff, normal, lightNormal, p, o;
float surfDist, t, value;
int i, index;

float Bailout=5.0;
float Power=8.0;
int Iterations = 4;

float lastDist = 10000.0;

void addObject(in float d, out float minDist, in int i, float distances[OBJECT_COUNT]){
	// minDist = min(minDist, d);

	if (d > distances[i]) {
		distances[i] = -1.0f;
	}
	else {
		distances[i] = d;
		minDist = min(minDist, d);
	}
}

vec4 scene(in vec3 p, float distances[OBJECT_COUNT]){
    float minDist = 100.0;

	minDist = min(fPlane(p, vec3(0, 1, 0), 0), minDist);

	// float crazyValue = sin(p.y + runTime) * 2 + .4;
	//p.y *= crazyValue;
	// p.z += crazyValue * .2;

	// p.z = abs(p.z) - 5;
	// if (p.z < p.x || p.x > 20.0){
	// 	p.xz = p.zx;
	// }

	float shape1 = fSphere(p-vec3(0, 2, 0), 1.0);
	float shape2 = fBox(p-vec3(0, 2, 0), vec3(1.0, 0.5, 1.0));
	// float shape2 = fPlane(p, vec3(0, 1, 0), 0);

	if (lastDist > 0){
		float dist = shape1 + (shape2-shape1) * (cos(runTime) * .5 + .5);
		if (dist < lastDist){
			lastDist = dist;
			minDist = min(minDist, dist);
		}
		else{
			lastDist = -1.0f;
		}
	}

	if(p.y < 25.0){
		pMod1(p.x, 10.0);

		pMirror(p.z, 5.0);
		pMod1(p.z, 20.0);
		
		float lastY = p.y;
		p.y += p.z * .7;
		float roof = fBoxCheap(p-vec3(0, 10.5, -2.5), vec3(6, .5, 5));
		p.y = lastY;
		float box = fOpUnionChamfer(fBoxCheap(p-vec3(0, 5, 0), vec3(5, 5, .4)),
									roof, .1);

		p.x = abs(p.x);
		float sphere = fCylinder((p-vec3(-.7, 5, 0)).xzy, 2.5, 1.1);
		float doorway = fOpUnionChamfer(fBoxCheap(p-vec3(-.7, 2.5, 0), vec3(3, 2.5, 1)), 
										sphere, .1);

		float wall = fOpIntersectionChamfer(box, -doorway, .1);
		minDist = min(wall, minDist);
	}
	
    return vec4(1.0, 0.0, 0.0, minDist);
}

vec3 estimateNormal(in vec3 p, in float d, float distances[OBJECT_COUNT]) {
	lastDist = 1000.0f;
	float x = scene(vec3(p.x - EPSILON, p.yz), distances).w;
	lastDist = 1000.0f;
	float y = scene(vec3(p.x, p.y - EPSILON, p.z), distances).w;
	lastDist = 1000.0f;
	float z = scene(vec3(p.xy, p.z - EPSILON), distances).w;
    return normalize(vec3(x, y, z) - d);
}

bool trace(in vec3 dir){
	float distances[OBJECT_COUNT] = {10000.0, 10000.0, 10000.0};
    for (int i=0; i < 200; i++){
        p = o + dir * t;
        surfDist = scene(p, distances).w;
        t += surfDist;
		if (t > 50.0){
			return false;
		}
        else if (surfDist < .001){
			color = vec3(t / 50.0, 1.0, 1.0);
			normal = estimateNormal(p, surfDist, distances);
			return true;
        }
    }
    return false;
}

void main(){
    dir = normalize(direction);
    bool hit;
    o = camPos;
	t = 1;
    trace(dir);
    // gl_FragColor = vec4(dot(normal, dir) * normal * .5 + .5, 1.0);
    // gl_FragColor = vec4(normal, 1.0);
    FragColor = vec4((dot(normal, dir) * color) * (1 - t / 50.0), 1.0);
}