
#include "SceneBase.h"

// http://glsl.heroku.com/e#4681.0
static const char* scene1shader = SHADER(
 /* mods by dist, shrunk slightly by @danbri*/
 uniform float time;
 uniform vec3 val;
 uniform vec2 resolution;
 void main(void) {
     vec2 uPos = ( gl_FragCoord.xy / resolution.xy );//normalize wrt y axis
     uPos -= .5;
     vec3 color = vec3(0.1);
     float vertColor = 0.0;
     for( float i = 1.; i < 10.; ++i ) {
         float t = i + 2.0;
         uPos.y += sin( 1.4 + time*10.0 + uPos.x*(t)*40.0*val.z + (val.z* t * t * 0.3) ) * 0.20;
         float fTemp = abs(0.5 / uPos.y / 50.0) * sqrt(val.x);
         vertColor += fTemp;
         color += vec3( fTemp*(7.0-t)/7.0, fTemp*t/10.0, pow(fTemp,0.9)*1.5 );
     }
     gl_FragColor = vec4(color, 1.0);
 }
 );

// http://glsl.heroku.com/e#4536.0
static const char* scene2shader = SHADER(
uniform vec3 val;
uniform float time;
uniform vec2 resolution;
vec3 roy(vec3 v, float x)
{
    return vec3(cos(x)*v.x - sin(x)*v.z,v.y,sin(x)*v.x + cos(x)*v.z);
}
                                         
vec3 rox(vec3 v, float x)
{
    return vec3(v.x,v.y*cos(x) - v.z*sin(x),v.y*sin(x) + v.z*cos(x));
}
float fdtun(vec3 rd, vec3 ro, float r)
{
	float a = dot(rd.xy,rd.xy);
  	float b = dot(ro.xy,rd.xy);
	float d = (b*b)-(10.0*a*(dot(ro.xy,ro.xy)+(r*r)));
  	return (-b+sqrt(abs(d)))/(3.75*a);
}
                                         
vec2 tunuv(vec3 pos){
 return vec2(pos.z,(atan(pos.y, pos.x))/0.31830988618379);
}

vec3 checkerCol(vec2 loc, vec3 col)
{
	return mix(col, vec3(0.0005), mod(step(fract(loc.x), 0.75) + step(fract(loc.y), 0.25),7.5));
}
                                         
vec3 lcheckcol(vec2 loc, vec3 col)
{
	return checkerCol(loc*7.5,col)*checkerCol(loc*1.75,col);	
}
 void main( void ) {
     vec3 dif = vec3(0.15,0.25,0.15);
     vec3 scoll = vec3(0.75,0.25,0.75);
     vec3 scolr = vec3(0.75,0.75,0.5);
     vec2 uv = (gl_FragCoord.xy/resolution.xy);
     vec3 ro = vec3(0.0,0.0,time*1.75);
     vec3 dir = normalize( vec3( -1.0 + 2.0*vec2(uv.x - .2, uv.y)* vec2(resolution.x/resolution.y, 1.0), -1.33 ) );
     float ry = time*0.3;
     
     dir = roy(rox(dir,time*0.5),time*-1.5);
     vec3 lro = ro-dif;
     vec3 rro = ro+dif;
     
     const float r = 1.0;
     float ld = fdtun(dir,lro,r);
     float rd = fdtun(dir,rro,r);
     vec2 luv = tunuv(ro + ld*dir);
     vec2 ruv = tunuv(ro + rd*dir);
     vec3 coll = lcheckcol(luv*.25,scoll)*(20.0/exp(sqrt(ld)));
     vec3 colr = lcheckcol(ruv*.5,scolr)*(20.0/exp(sqrt(rd)));
     gl_FragColor = vec4(sqrt(val.x)* sqrt(mix(coll + colr, coll * colr, 5.0+sin(time * 0.5))),1.0);
 }
);

// http://glsl.heroku.com/e#4288.0
static const char* scene3shader = SHADER(
 uniform vec3 val;
 uniform float time;
 uniform vec2 resolution;
 
 mat3 genRotMat(float a0,float x,float y,float z){
     float a=a0*3.1415926535897932384626433832795/180.0;
     return mat3(
                 1.0+(1.0-cos(a))*(x*x-1.0),
                 -z*sin(a)+(1.0-cos(a))*x*y,
                 y*sin(a)+(1.0-cos(a))*x*z,
                 z*sin(a)+(1.0-cos(a))*x*y,
                 1.0+(1.0-cos(a))*(y*y-1.0),
                 -x*sin(a)+(1.0-cos(a))*y*z,
                 -y*sin(a)+(1.0-cos(a))*x*z,
                 x*sin(a)+(1.0-cos(a))*y*z,
                 1.0+(1.0-cos(a))*(z*z-1.0)
                 );
 }
 
 float cubeDist(vec3 p){
     return max(abs(p.x),max(abs(p.y),abs(p.z)));
 }
 
 void main(){
     float spread=5.5;
     float total=0.0;
     float delta=0.01;
     float cameraZ=-1.75;
     float nearZ=-1.0;
     float farZ=1.0;
     float gs=0.0;
     int iter=0;
     vec3 col=vec3(0.0,0.0,0.0);
     vec3 ray=vec3(0.0,0.0,0.0);
     mat3 rot=genRotMat(sin(time/4.13+(2.0*sqrt(val.z)))*360.0,1.0,0.0,0.0);
     rot=rot*genRotMat(sin(time/4.64)*360.0,0.0,1.0,0.0);
     rot=rot*genRotMat(sin(time/4.24)*360.0,0.0,0.0,1.0);
     vec2 p=vec2(0.0,0.0);
     p.x=gl_FragCoord.x/resolution.y-0.5*resolution.x/resolution.y;
     p.y=gl_FragCoord.y/resolution.y-0.5;
     ray.xy+=p.xy*spread*(nearZ-cameraZ);
     vec3 rayDir=vec3(spread*p.xy*delta,delta);
     vec3 tempDir=rayDir*rot;
     vec3 norm;
     ray.z=nearZ;
     bool refracted=false;
     for(int i=0;i<250;i++){
         vec3 temp;
         vec3 tempc;
         float val;
         temp=ray.xyz*rot;
         tempc=temp;
         float thres=0.5;
         if(tempc.x<0.0)tempc.x=abs(tempc.x);
         if(tempc.x<thres)tempc.x=0.0;
         else tempc.x=1.0/tempc.x*sin(time);
         if(tempc.y<0.0)tempc.y=abs(tempc.y);
         if(tempc.y<thres)tempc.y=0.0;
         else tempc.y=1.0/tempc.y*sin(time*0.842);
         if(tempc.z<0.0)tempc.z=abs(tempc.z);
         if(tempc.z<thres)tempc.z=0.0;
         else tempc.z=1.0/tempc.z*sin(time*1.132);
         val=cubeDist(temp);
         if(val<thres && !refracted){
             rayDir=vec3(0.5*spread*p.xy*delta,delta);
             refracted=true;
         }
         if(val<0.0)val=abs(val);
         if(val<thres)val=0.0;
         else val=1.0/val;
         col.x+=(val+tempc.x)/2.0;
         col.y+=(val+tempc.y)/2.0;
         col.z+=(val+tempc.z)/2.0;
         ray+=rayDir;
         iter++;
         if(ray.z>=farZ)break;
     }
     col.x=col.x/float(iter);
     col.y=col.y/float(iter);
     col.z=col.z/float(iter);
     gl_FragColor=vec4(col,1.0);
 }
);

// http://glsl.heroku.com/e#5001.0
static const char* scene4shader = SHADER(
uniform vec2 resolution;
uniform float time;

/*Object A (tunnel)*/
float oa(vec3 q)
{
    return cos(q.x)+cos(q.y*0.5)+cos(1.2*q.z)+fract(q.y*3.)*.05;
}
/*Scene*/
float o(vec3 q)
{
    return min(oa(q),1.);
}

/*Get Normal*/
vec3 gn(vec3 q)
{
    vec3 f=vec3(.01,0,0);
    return normalize(vec3(o(q+f.xyy),o(q+f.yxy),o(q+f.yyx)));
}

/*MainLoop*/
void main(void)
{
    vec2 p = -1.0 + 2.0 * gl_FragCoord.xy / resolution.xy;
    p.x *= resolution.x/resolution.y;
    
    vec4 c=vec4(1.0);
    vec3 org=vec3(sin(time)*.5,cos(time*.5*10.0)*.25+.25,time*10.0);
    vec3 dir=normalize(vec3(p.x*1.6,p.y,1.0));
    vec3 q=org;
    vec3 pp;
    float d=.0;
    
    /*First raymarching*/
    for(int i=0;i<64;i++)
    {
        d=o(q);
        q+=d*dir;
    }
    pp=q;
    float f=length(q-org)*0.02;
    
    /*Second raymarching (reflection)*/
    dir=reflect(dir,gn(q));
    q+=dir;
    for(int i=0;i<64;i++)
    {
        d=o(q);
        q+=d*dir;
    }
    c=max(dot(gn(q),vec3(.1,.1,.0)),.0)+vec4(.3,cos(time*.5)*.5+.5,sin(time*.5)*.5+.5,1.)*min(length(q-org)*.04,1.);
    
    //Final Color
    vec4 fcolor = ((c+vec4(f))+(1.-min(pp.y+1.9,1.))*vec4(1.,.8,.7,1.))*min(time*.5,1.);
    gl_FragColor=vec4(fcolor.xyz,1.0);
}
);

#define SCENEDEFINE(classname,shadersrc) class classname : public SceneBase \
{\
public:\
	classname() {} \
	void Init() \
	{ \
        m_sd = new ShaderDrawer(shadersrc); \
	} \
	void Draw(double time, const float* syncval) \
	{ \
        m_sd->Bind(); \
        m_sd->Uniform2f("resolution", m_resolution[0], m_resolution[1]); \
        m_sd->Uniform1f("time", static_cast<float>(time)); \
        m_sd->Uniform3f("val", syncval[0],syncval[1],syncval[2]); \
        m_sd->Draw(); \
        m_sd->Unbind(); \
	} \
private: \
    ShaderDrawer* m_sd; \
};

SCENEDEFINE(Scene1, scene3shader);
SCENEDEFINE(Scene2, scene4shader);
SCENEDEFINE(Scene3, scene1shader);
SCENEDEFINE(Scene4, scene2shader);


