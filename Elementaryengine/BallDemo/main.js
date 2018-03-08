console.log("UI Element test");

// definition: new Texture("path",notEmpty)
var cht = new Texture("Assets/Textures/ch.jpg",true);
var empty = new Texture("",false);

var fgCol = new Vec3(0.5,0.6,1);

// definition: new UIElement(positionIn%, posistionInPX, sizein%, sizeInPx, foregroundColor, backgroundColor, opacity, backgroundBlurr, zindex, texture, alphamap)var b1 = new UIElement(new Vec3(50,0,0), new Vec3(-180,10,0),new Vec3(0,0,0), new Vec3(40,40,0), new Vec3(1,0.2,0.2),new Vec3(0,0,0), 0.5, 0,10,empty,empty);
var b1 = new UIElement(new Vec3(50,0,0), new Vec3(-200,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);
var b2 = new UIElement(new Vec3(50,0,0), new Vec3(-140,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);
var b3 = new UIElement(new Vec3(50,0,0), new Vec3(-80,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);
var b4 = new UIElement(new Vec3(50,0,0), new Vec3(-20,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);
var b5 = new UIElement(new Vec3(50,0,0), new Vec3( 40,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);
var b6 = new UIElement(new Vec3(50,0,0), new Vec3(100,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);
var b6 = new UIElement(new Vec3(50,0,0), new Vec3(160,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);

var selected = new UIElement(new Vec3(50,0,0), new Vec3(40,4,0),new Vec3(0,0,0), new Vec3(40,3,0), new Vec3(0.2,0.2,1),new Vec3(0,0,0), 0.5, 0,10,empty,empty);

var cont = new UIElement(new Vec3(50,0,0), new Vec3(-210,0,0),new Vec3(0,0,0), new Vec3(420,60,0), new Vec3(1,1,1),new Vec3(0,0,0), 0.2, 2.156,1,empty,empty);
var ch = new UIElement(new Vec3(50,50,0), new Vec3(-10,-10,0),new Vec3(0,0,0), new Vec3(20,20,0), new Vec3(1,1,1),new Vec3(0,0,0), 0.4, 0,1,empty,cht);