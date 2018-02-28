var v = new Vec3(1,2,3);
var mat = new Material(v);
var a = new Asset(new Vec3(2,2,2),new Vec3(0.5,0.5,0.5),0); 
var m = new Mesh("Assets/Meshs/Cube.obj");

m.attachto(a);


function OnTick(){
    var b = new Asset(new Vec3(2,2,2),new Vec3(0.5,0.5,0.5),0); 
}
