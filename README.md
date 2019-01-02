# Elementaryengine
Elementaryengine is a super light weight yet high performance game engine made for fully dynamic worlds containing many copy's of a limited amount of meshes. Physics are handled using the Bullet3 Physics library.
It supports Javascript and can load scripts dynamically using Cakra Core by Microsoft. This engine is still pre-alpha and has a long way to go.
The main features missing at the moment are networking, animations and open world support. 
Other features may be incomplete. Advanced features like AI or custom shaders will never be part of this Engine. Which doesn't mean its impossible to do, but you will have to add those features yourself. This is part of keeping the engine bloat free, which also is why I don't plan to add a GUI and leveldesigner at the moment.
## But Why?
This project is intended to bring game development closer to gamers and creatives. To gamify game development. These days developing games is a huge time and money investment, wich leads to a high output of low risk games and a low amount of games with new ideas. This projects goal is to give creative people a way to easily implement their ideas and test them fast. Therefore its needed to have some drawbacks unfortunaly. To achive this the level designer will be a gamemode, running on a server to have an easy way for teams to work on a common project, but this will limit the possible details in graphics. If the level/world designer is simple it will have minecraft-like visuals, wich are not suited for a lot of games. On the other hand if the designer is to complex, it will become hard to work with and ultimatliy defeat the purpose of this project. JavaScript is choosen as the main scripting language to enable a lot of people to write game code and distribute it at runtime. Assets are stored in .obj format so any major 3D software can export to it. Level/world-design, asset creation and menagement, and coding will allways be stricly seperated, to avoid conflicts in the team. The goal is to stream levels, assets and code from the server to the clients on runtime, somewhat like GMod, to enable easy distribution.
## I do not recommend using this engine right now. Its far from finished and will never be as user friendly as unity, cryengine or unreal. I only recommend using it when your needs are out of reach for those engines and you want something very customizable or want to  learn on a deeper level.
Another goal for this engine is to be friendly to people that are not experienced in c++ and graphics APIs. That's why it uses JavaScript as an interface to be reachable for many people. You can still code in c++ of course

## Screenshots
physically based rendering
![screenshot2](https://github.com/JanNitschke/Elementaryengine/blob/master/Screenshots/PBRDemo1.jpg?raw=true)
volumetric lighting
![screenshot2](https://github.com/JanNitschke/Elementaryengine/blob/master/Screenshots/Demo01.png?raw=true)

## If you want to use my work commercially please get in contact with me for licensing at:
jan.nitschke@gmail.com 

[![](https://codescene.io/projects/2920/status.svg) Get more details at **codescene.io**.](https://codescene.io/projects/2920/jobs/latest-successful/results)
