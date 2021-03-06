// Create a test mesh and material
var v = new Vec3(0.5,0.5,0.5);
var mat = new Material(v);
var m = new Mesh("Assets/Meshs/Cube.obj",mat);

var tex = new Texture("Assets/Textures/Plaster.jpg")

mat.setAlbedo(new Vec3(1,1,1));
mat.setAlbedoMap(tex);
mat.setAO(new Vec3(0.01,0.01,0.01));

var mat2 = new Material(v);
mat2.setAlbedo(new Vec3(8,8,8));
mat2.setAO(new Vec3(0.1,0.1,0.1));
mat2.setRoughness(0.2);
mat2.setMetallic(0.2);

var m2 = new Mesh("Assets/Meshs/t.obj",mat2);

// Array to hold our placed blocks
var placedBlocks = [];

// Create an empty Texture. This will be used as alphamap and texture for all plain ui elements
var empty = new Texture(false);

// Load the Texture for the croshair. 
var crosshairTexture = new Texture("Assets/Textures/ch.jpg");

// Keep track of the item we are holding
var selectedItem = 0;

// Keep track if the placing button was pressed last frane
var placePressedLastFrame = false;
var deletePressedLastFrame = false;

// Define the foreground color for the UI as a Vec3. This is done so it can be sinply changed for all elements at once. For Colors the X component of the Vector equals Red, Y equals Green and Z equals Blue.
var fgCol = new Vec3(0.5,0.5,0.9);

// Describe all UI elements that should be rendered.
var b1 = new UIElement(new Vec3(50,0,0), new Vec3(-200,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);
var b2 = new UIElement(new Vec3(50,0,0), new Vec3(-140,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);
var b3 = new UIElement(new Vec3(50,0,0), new Vec3(-80,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);
var b4 = new UIElement(new Vec3(50,0,0), new Vec3(-20,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);
var b5 = new UIElement(new Vec3(50,0,0), new Vec3( 40,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);
var b6 = new UIElement(new Vec3(50,0,0), new Vec3(100,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);
var b6 = new UIElement(new Vec3(50,0,0), new Vec3(160,10,0),new Vec3(0,0,0), new Vec3(40,40,0), fgCol,new Vec3(0,0,0), 0.5, 0,10,empty,empty);

// Describe the UI element for the selection marker under the item tray
var selected = new UIElement(new Vec3(50,0,0), new Vec3(40,4,0),new Vec3(0,0,0), new Vec3(40,3,0), new Vec3(0.2,0.2,1),new Vec3(0,0,0), 0.5, 0,10,empty,empty);

// Describe the UI element for the background of the item tray. Background blurr is set to 2.156 because it looks nicer when the numbers aren't whole
var cont = new UIElement(new Vec3(50,0,0), new Vec3(-210,0,0),new Vec3(0,0,0), new Vec3(420,60,0), new Vec3(1,1,1),new Vec3(0,0,0), 0.2, 2.156,1,empty,empty);

// Describe the UI element for the croshair. It uses the previous defined texture 'crosshairTexture' as an alpha map 
var ch = new UIElement(new Vec3(50,50,0), new Vec3(-10,-10,0),new Vec3(0,0,0), new Vec3(20,20,0), new Vec3(1,1,1),new Vec3(0,0,0), 0.4, 0,1,empty,crosshairTexture);

// Move the selection marker to its starting position
updateHud();

var j = 0;
var z = 0;
var k = 0;

function PlaceFeeeeest(){
    if(j < -15){
        j = 0;
        z += 2;
    }
    if(z > 15){
        z = 0;
        k += 2;
    }
    var as = new Asset(new Vec3(z,k,j),new Vec3(0.8,2,1.5),0); 
    j -= 2;
    m2.attachto(as);
}

// called once per frame. Only used for input in this example
function OnTick(){
    
    // Retrive the users scroll input
    var scroll = input.getScroll();
    
    // Get the Y value of the scroll input (Y is the normal mousewheel, X is used for horizontal scrolling).
    var scY = -scroll.getY();

    // run updateSelection if the player scrolled. Determine the scroll direction as argument
    if(scY != 0){
        updateSelection(scY > 0);
    } 

    if(input.getKey(70)){
        for (i = 0; i < 2; i++) { 
            PlaceFeeeeest();
        }
    }


    // place a block if Q key is pressed and has not been pressed last frame. This is done to ensure that holding the button down won't place more than one block.
    if(input.getKey(0)){
        if(!placePressedLastFrame){
            placeBlock();
        }
        placePressedLastFrame = true;
    }else{
        placePressedLastFrame = false;
    }

    if(input.getKey(1)){
        if(!deletePressedLastFrame){
            deleteBlock();
        }
        deletePressedLastFrame = true;
    }else{
        deletePressedLastFrame = false;
    }
}

// Function to update 'selectedItem' after scroll input. If 'increaseSection' is true the next item will be choosen, otherwise the previous
function updateSelection(increaseSection){

    // Increase or decrease 'selectedItem' by 1 depending on 'increaseSection'
    selectedItem = increaseSection?selectedItem + 1 : selectedItem - 1;

    // If the selectedItem is out of bounds switch it to the other extreme
    if( selectedItem > 6){
        selectedItem = 0
    } 
    if( selectedItem < 0){
        selectedItem = 6
    }

    // Update the HUD to display the changed selection
    updateHud();
}

// Function to update the position of the selection marker depending on 'selectedItem'
function updateHud(){
    // Calculate the X coordinate by adding an offset to the X position of the left most element
    var px = (-200) + (60 * selectedItem);

    // Set the position of the selection marker to 'px' pixel from left and 4 pixel from the bottom 
    selected.setPositionPx(new Vec3(px,4,0));
}

function deleteBlock(){
    var cam = game.getActiveCamera();
    var campos = cam.getPosition();
    var camdir = cam.getForeward();
    var raydir = camdir;
    // normailze the foreward vector of the camera to lenght 15
    raydir.normalize(15);

    // Add the campos to the vector to get a point exactly 15 units infront of the camera
    raydir.add(campos);

    // Raycast from the camera to this point
    var rc = game.RayCast(campos,raydir);
    if(rc != false){
        // If we hit something - get the data
        var hitAsset = rc.getHitAsset();

        if(isPlaced(hitAsset)){
            hitAsset.destroy();
        }
    }
}

// Place a block on the point we are looking at
function placeBlock(){
    // Get information about the Camera
    var cam = game.getActiveCamera();
    var campos = cam.getPosition();
    var camdir = cam.getForeward();
    var raydir = camdir;
    // normailze the foreward vector of the camera to lenght 15
    raydir.normalize(15);

    // Add the campos to the vector to get a point exactly 15 units infront of the camera
    raydir.add(campos);

    // Raycast from the camera to this point
    var rc = game.RayCast(campos,raydir);
    if(rc != false){
        // If we hit something - get the data
        var placedAsset;
        var hitPos = rc.getHitPosition();
        var hitNormal = rc.getHitNormal();
        var hitAsset = rc.getHitAsset();

        // make sure the normal contains only 0 and 1 values
        var normal = nn(hitNormal);

        // Check if we placed on an previously placed asset.
        if(isPlaced(hitAsset)){
            // Place the new asset next to the center of the hit asset.
            normal.scale(0.5);
            var ap = hitAsset.getPosition()
            normal.add(ap);
            placedAsset = new Asset(normal,new Vec3(0.25,0.25,0.25),0); 
        }else{
            // Place the new asset on the surface of the hit asset at our hit position
            normal.scale(0.25);
            normal.add(hitPos);
            placedAsset = new Asset(normal,new Vec3(0.25,0.25,0.25),0); 
        }
        // Attach the cube mesh to the asset
        m.attachto(placedAsset);
        // Add the asset to the list of placed assets
        placedBlocks.push(placedAsset);
    }
}

// Correct the normal to Contain only 0,1 values.
function nn(norm){
    if (norm.getX() < 0.5 && norm.getX() > -0.5) {
        norm.setX(0);
    } else {
        if (norm.getX() > 0.5)
            norm.setX(1.0);
        if (norm.getX() < 0.5)
            norm.setX(-1.0);
    }

    if (norm.getY() < 0.5 && norm.getY() > -0.5) {
        norm.setY(0);
    } else {
        if (norm.getY() > 0.5)
            norm.setY(1.0);
        if (norm.getY() < 0.5)
            norm.setY(-1.0);
    }

    if (norm.getZ() < 0.5 && norm.getZ() > -0.5) {
        norm.setZ(0);
    } else {
        if (norm.getZ() > 0.5)
            norm.setZ(1.0);
        if (norm.getZ() < 0.5)
            norm.setZ(-1.0);
    }
    return norm;
}

// Function to check if an Asset is contained int 'placedBlocks'. native Javascript Comparison is not possible due to limitations of chakra core.
function isPlaced(hitAsset){
    for (i = 0; i < placedBlocks.length; i++) { 
        if (placedBlocks[i].equals(hitAsset))
        return true;
    }

    return false;
}