//
//  GameScene.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/10/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import SpriteKit
import UIKit
import CoreMotion

class MMWGameSceneView: SKScene, SKPhysicsContactDelegate {
    

    
    var backgroundNode : SKSpriteNode?
    var foregroundNode : SKSpriteNode?
    var spriteTest : SKSpriteNode?
//    var orbNode : SKSpriteNode?
    var impulseCount = 4
    var diagnosticText : SKLabelNode?
    
//    var gameView: UIView!
//    var level: Level!
//    private var data = GameData()
    //private var audioController: AudioController
    //stopwatch variables
    private var secondsLeft: Int = 0
    private var timer: NSTimer?
    
    let coreMotionManager = CMMotionManager()
    var xAxisAcceleration : CGFloat = 0.0
//    var tileLocationX : Int
//    var tileLocationY : Int
    
    let CollisionCategoryPlayer : UInt32 = 0x1 << 1
    let CollisionCategoryPowerUp : UInt32 = 0x1 << 2
    
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    
    override init(size: CGSize) {
        super.init(size: size)
        physicsWorld.contactDelegate = self
        physicsWorld.gravity = CGVectorMake(0.0, -4.0)
        backgroundColor = SKColor(red: 0.5, green: 0.0, blue: 0.0, alpha: 1.0)
        userInteractionEnabled = true
        
        // add BG
        backgroundNode = SKSpriteNode(imageNamed: "MarkMyWordBGiPadMod.jpg")
        backgroundNode!.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        backgroundNode!.position = CGPoint(x: size.width/2.0, y: 0.0)
//        // set scale for various platforms
//        backgroundNode?.xScale = 0.5
//        backgroundNode?.yScale = 0.5
        backgroundNode?.userInteractionEnabled = false
        addChild(backgroundNode!)
        
        foregroundNode = SKSpriteNode()
        addChild(foregroundNode!)
        
        // add sprite
        spriteTest = SKSpriteNode (imageNamed: "LetterTest90x90")
        spriteTest!.physicsBody = SKPhysicsBody(circleOfRadius: spriteTest!.size.width / 2)
        spriteTest!.physicsBody!.dynamic = false
        spriteTest!.anchorPoint = CGPoint(x: 0.5, y: 0.5)
        spriteTest!.position = CGPoint(x: self.size.width/2.0, y: 180.0)
        spriteTest!.physicsBody!.linearDamping = 1.0
        spriteTest!.physicsBody!.allowsRotation = false
        spriteTest!.physicsBody!.categoryBitMask = CollisionCategoryPlayer
        spriteTest!.physicsBody!.contactTestBitMask = CollisionCategoryPowerUp
        //spriteTest!.physicsBody!.collisionBitMask = 1
        foregroundNode!.addChild(spriteTest!)
        
        timerHUD(123)
        partialWordHUD("ssdx", isWord: false)
        tilesRemainingHUD(55)
        
        
        addTile("Q")
//
//        let tile = TileView(letter: "R", sideLength: 45.0)
//        tile.center = CGPointMake(300, 300)
//        tile.randomize()
//        //tile.dragDelegate = self
//        //4
//        gameView.addSubview(tile)



        
//        addOrbsToForeground()
    }
    
    func partialWordHUD (letters : String, isWord : Bool) {
        var partialWordHUD = SKLabelNode(fontNamed: FontHUDName)
        var isPartial : String
        if isWord {
            isPartial = ""
        }
        else {
            isPartial = "not"
        }
        partialWordHUD.text = "\(letters) is \(isPartial) a partial word"
        partialWordHUD.fontSize = FontHUDSize
        partialWordHUD.position = CGPointMake(size.width/2.0 - 357, 3)
        partialWordHUD.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 1)!
        addChild(partialWordHUD)
    }
    
    func timerHUD (timeAmount: Int) {
        var timeRemainingHUD = SKLabelNode(fontNamed: FontHUDName)
        timeRemainingHUD.text = "Time Remaining: \(timeAmount)"
        timeRemainingHUD.fontSize = FontHUDSize
        timeRemainingHUD.fontColor = FontHUDRed
        timeRemainingHUD.position = CGPointMake(size.width/2.0 + 177, 3)
        timeRemainingHUD.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 1)!
        addChild(timeRemainingHUD)
    }
    
    func tilesRemainingHUD (tilesLeft : Int){
        var tilesRemainingHUD = SKLabelNode(fontNamed: FontHUDName)
        tilesRemainingHUD.text = "Tiles Remaining: \(tilesLeft)"
        tilesRemainingHUD.fontSize = FontHUDSize
        tilesRemainingHUD.position = CGPointMake(size.width/2.0 - 23.0, 3.0)
        tilesRemainingHUD.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 1)!
        addChild(tilesRemainingHUD)
    }
    
    func addTile (letterTile : Character){
        var tile = SKSpriteNode(imageNamed: "avatarpic")
        tile.position = CGPointMake(155, 155)
        foregroundNode!.addChild(tile)
    }
    
    
    
//    func addOrbsToForeground () {
//        var orbNodePosition = CGPoint(x: spriteTest!.position.x, y: spriteTest!.position.y + 100)
//        var orbXShift : CGFloat = -1.0
//        for _ in 1...50 {
//            var orbNode = SKSpriteNode(imageNamed: "avatarpic")
//            if orbNodePosition.x - (orbNode.size.width * 2) <= 0 {
//                orbXShift = 1.0
//            }
//            if orbNodePosition.x + orbNode.size.width >= self.size.width {
//                orbXShift = -1.0
//            }
//            orbNodePosition.x += 40.0 * orbXShift
//            orbNodePosition.y += 120
//            orbNode.position = orbNodePosition
//            orbNode.physicsBody = SKPhysicsBody(circleOfRadius: orbNode.size.width / 2)
//            orbNode.physicsBody!.dynamic = false
//            
//            orbNode.physicsBody!.categoryBitMask = CollisionCategoryPowerUp
//            orbNode.physicsBody!.collisionBitMask = 0
//            orbNode.name = "POWER_UP_ORB"
//            foregroundNode!.addChild(orbNode)
//        }
//        
//        enumerateChildNodesWithName("POWER_UP_ORB", usingBlock: {
//            node, stop in
//            // do something with node or stop
//            println("Found node \(node)");
//
//            //stop.memory = true // stop iterating over each node found
//        })
//    }

    
    override func touchesBegan(touches: Set<NSObject>, withEvent event: UIEvent) {
        //spriteTest!.physicsBody!.applyImpulse(CGVectorMake(0.0, 40.0))
//        if !spriteTest!.physicsBody!.dynamic {
//            spriteTest!.physicsBody!.dynamic = true
//            self.coreMotionManager.accelerometerUpdateInterval = 0.3
//            self.coreMotionManager.startAccelerometerUpdatesToQueue(NSOperationQueue(), withHandler:  {
//                (data: CMAccelerometerData!, error: NSError!) in
//                if let constVar = error {
//                    println("Error in accelerometer")
//                }
//                else{
//                    self.xAxisAcceleration = CGFloat(data!.acceleration.x)
//                }
//            })
//        }
//        
//        if impulseCount > 0 {
//            spriteTest!.physicsBody!.applyImpulse(CGVectorMake(0.0, 40.0))
//            impulseCount--
//        }
        
        println("TOUCH: \(event.description) ")
        for touch: AnyObject in touches {
            var location = (touch as! UITouch).locationInView(self.view)
            // addTile(location)
            diagnosticText = SKLabelNode(text: "\(UITouch.locationInView(touch as! UITouch))")

            let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
            runAction(actionSound)
            //     .rotateByAngle(CGFloat(M_PI), duration:1)
            //sprite.runAction(SKAction.    .repeatActionForever(actionSound))
             
            //if UITouchPhase.Stationary. { println("STATIONARY") }
            //println("TOUCH! \(UITouch.locationInView(touch as! UITouch))")
            //println("preTOUCH! \(UITouch.previousLocationInView(touch as! UITouch))")
        }
        
//        // add orangeTile
//        var orangeTile : Tile
//        //orangeTile = Tile.makeTile(Tile())()
//        orangeTile = Tile()
//        orangeTile.anchorPoint = CGPoint(x: 0.5, y: 0.0)
//        orangeTile.position = CGPoint(x: size.width/2.0 + 25, y: 10.0)
//        self.addChild(orangeTile)
//        
//        // add orangeTile
//        var tetrisCell : TetrisCell
//        //orangeTile = Tile.makeTile(Tile())()
//        tetrisCell = TetrisCell(imageNamed: "HookAvatar80x80.jpg")
//        //orangeTile!.anchorPoint = CGPoint(x: 0.5, y: 0.0)
//        tetrisCell.position = CGPoint(x: size.width/2.0 + 55, y: 20.0)
//        addChild(tetrisCell)
//        
//        var tetrisCell2 : TetrisCell
//        var tex = SKTexture(imageNamed: "HookAvatar80x80.jpg")
//        var col = SKColor(red: 0.5, green: 0.1, blue: 0.2, alpha: 0.5)
//        var siz = CGSize(width: 166, height: 44)
//        tetrisCell2 = TetrisCell(texture: tex!, color: col, size: siz)
//        tetrisCell2.name = "TetrisCell2"
//        tetrisCell2.position = CGPoint(x: size.width/2.0, y: 25.0)
//        tetrisCell2.userInteractionEnabled = true
//        addChild(tetrisCell2)
//        
//        var tetrisCell3 : TetrisCell
//        tetrisCell3 = TetrisCell()
//        tetrisCell3.position = CGPoint(x: size.width/3.0, y: 45.0)
//        addChild(tetrisCell3)
    }
    
    
    func didBeginContact(contact: SKPhysicsContact) {
        var nodeB = contact.bodyB!.node!
        if nodeB.name == "POWER_UP_ORB" {
            impulseCount++
            nodeB.removeFromParent()
        }
        println("CONTACT! contactPoint:\(contact.contactPoint)")
    }
    
    
    func addTile (atPoint: CGPoint){   // add letter, name, colorize?,
        self.userInteractionEnabled = true
        
        var letter : Character = "Q"
        
        var tileNode = SKSpriteNode(imageNamed: "LetterTest90x90")
        
        tileNode.position.x = atPoint.x
        tileNode.position.y = (size.height - (atPoint.y))
        
        tileNode.name = "tile"
        
        tileNode.color = SKColor(red: 1.0, green: 0.2, blue: 0.2, alpha: 1.0)
        tileNode.colorBlendFactor = 1.0
        
        var letterLabel = SKLabelNode(fontNamed: FontHUDName)
        letterLabel.text = "Q"
        letterLabel.fontSize = 44 // FontHUDSize
        //var colorDarkGreen : UIColor = UIColor(red: 0.1, green: 1.0, blue: 0.2, alpha: 0.80)
        letterLabel.fontColor = FontHUDWhite
        letterLabel.position = CGPoint(x: 0, y: -20)
        letterLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!

        tileNode.addChild(letterLabel)
        addChild(tileNode)
    }
    
    
    override func update(currentTime: NSTimeInterval) {
        if spriteTest!.position.y >= 180.0 {
            self.backgroundNode!.position = CGPointMake(self.backgroundNode!.position.x, -((self.spriteTest!.position.y-180.0)/8))
            foregroundNode!.position = CGPointMake(foregroundNode!.position.x, -((spriteTest!.position.y-180.0)/8))
        }
    }
    
    
//    override func didSimulatePhysics() {
//        self.spriteTest!.physicsBody!.velocity = CGVectorMake(self.xAxisAcceleration * 380.0, self.spriteTest!.physicsBody!.velocity.dy)
//        if spriteTest!.position.x < -(spriteTest!.size.width / 2) {
//            spriteTest!.position = CGPointMake(size.width - spriteTest!.size.width / 2, spriteTest!.position.y)
//        }
//        else if spriteTest!.position.x > self.size.width {
//            spriteTest!.position = CGPointMake(spriteTest!.size.width / 2, spriteTest!.position.y)
//        }
//    }
    
    
//    override func touchesMoved(touches: Set<NSObject>, withEvent event: UIEvent) {
//    //override func touchesMoved(    (touches: NSSet, withEvent event: UIEvent) {
//        println("Tetris touch moved")
//        
////        for touch: AnyObject in touches {
////            let location: CGPoint! = touch.locationInView(self.view)   // (<#view: UIView?#>)   //.locationInNode(self)
////            print("location \(location) .. ")
////            let nodeAtPoint = self.nodeAtPoint(location)
////            println("nodeAtPoint.location \(nodeAtPoint) .. ")
////            //self.nodeAtPoint(location) = location
////        }
//        
//        for touch : AnyObject in touches {
//            let location : CGPoint! = touch.locationInView(self.view) /// touch.locationInNode(self)
//            let touchedNode = nodeAtPoint(location)
//            touchedNode.position = location
//        }
//    }
    
    override func touchesMoved(touches: Set<NSObject>, withEvent event: UIEvent) {
        for touch in (touches as! Set<UITouch>) {
            let location = touch.locationInNode(self)
            let touchedNode = nodeAtPoint(location)
            touchedNode.position = location
        }
    }
    
    
    deinit {  // ??? for optimization of performance
        self.coreMotionManager.stopAccelerometerUpdates()
    }
    
    
    //    override func update(currentTime: CFTimeInterval) {
    //        /* Called before each frame is rendered */
    //    }
}


//class GameScene: SKScene {
//    override func didMoveToView(view: SKView) {
//        /* Setup your scene here */
//        let myLabel = SKLabelNode(fontNamed:"Chalkduster")
//        myLabel.text = "Hello, World!";
//        myLabel.fontSize = 65;
//        myLabel.position = CGPoint(x:CGRectGetMidX(self.frame), y:CGRectGetMidY(self.frame));
//        
//        self.addChild(myLabel)
//    }
//    
//    override func touchesBegan(touches: Set<NSObject>, withEvent event: UIEvent) {
//        /* Called when a touch begins */
//        
//        for touch in (touches as! Set<UITouch>) {
//            let location = touch.locationInNode(self)
//            
//            let sprite = SKSpriteNode(imageNamed:"Spaceship")
//            
//            sprite.xScale = 0.5
//            sprite.yScale = 0.5
//            sprite.position = location
//            
//            let action = SKAction.rotateByAngle(CGFloat(M_PI), duration:1)
//            
//            sprite.runAction(SKAction.repeatActionForever(action))
//            
//            self.addChild(sprite)
//        }
//    }
//   
//    override func update(currentTime: CFTimeInterval) {
//        /* Called before each frame is rendered */
//    }
//}
