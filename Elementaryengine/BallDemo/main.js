var cht = new Texture("Assets/Textures/ch.jpg");
var empty = new Texture("");


var v = new Vec3(0.5,0.5,0.5);
console.log("UI Element test");

var mat = new Material(v);
var a = new Asset(new Vec3(2,2,2),new Vec3(0.5,0.5,0.5),0); 
var m = new Mesh("Assets/Meshs/Cube.obj",mat);
m.attachto(a);


var selectedItem = 0;

var fgCol = new Vec3(0.5,0.5,0.8);
// definition: new UIElement(positionIn%, posistionInPX, sizein%, sizeInPx, foregroundColor, backgroundColor, opacity, backgroundBlurr, zindex, texture, alphamap)var b1 = new UIElement(new Vec3(50,0,0), new Vec3(-180,10,0),new Vec3(0,0,0), new Vec3(40,40,0), new Vec3(1,0.2,0.2),new Vec3(0,0,0), 0.5, 0,10,empty,empty);
var b1 = new UIElement(new Vec3(50,0,0), new Vec3(-200,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);
var b2 = new UIElement(new Vec3(50,0,0), new Vec3(-140,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);
var b3 = new UIElement(new Vec3(50,0,0), new Vec3(-80,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);
var b4 = new UIElement(new Vec3(50,0,0), new Vec3(-20,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);
var b5 = new UIElement(new Vec3(50,0,0), new Vec3( 40,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);
var b6 = new UIElement(new Vec3(50,0,0), new Vec3(100,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);
var b6 = new UIElement(new Vec3(50,0,0), new Vec3(160,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);

var uicubes = [b1,b2,b3,b4,b5,b6];

var selected = new UIElement(new Vec3(50,0,0), new Vec3(40,4,0),new Vec3(0,0,0), new Vec3(40,3,0), new Vec3(0.2,0.2,1),new Vec3(0,0,0), 0.5, 0,10,empty,empty);

var cont = new UIElement(new Vec3(50,0,0), new Vec3(-210,0,0),new Vec3(0,0,0), new Vec3(420,60,0), new Vec3(1,1,1),new Vec3(0,0,0), 0.2, 2.156,1,empty,empty);
var ch = new UIElement(new Vec3(50,50,0), new Vec3(-10,-10,0),new Vec3(0,0,0), new Vec3(20,20,0), new Vec3(1,1,1),new Vec3(0,0,0), 0.4, 0,1,empty,cht);


selected.setPositionPx(new Vec3(200,200,0));

updateHud();

function OnTick(){
    var scroll = input.getScroll();
    var scY = -scroll.getY();

    if(scY > 0){
        selectedItem += 1;
        if( selectedItem > 6){
            selectedItem = 0
        } 
        updateHud();
    } else if(scY < 0){
        selectedItem -= 1
        if( selectedItem < 0){
            selectedItem = 6
        }
        updateHud();
    }
}

function updateHud(){
    var px = (-200) + (60 * selectedItem);
    selected.setPositionPx(new Vec3(px,4,0));
}