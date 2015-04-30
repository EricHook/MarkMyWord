//
//  TileView.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/30/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation
import SpriteKit

class LetterTile : SKSpriteNode {
    
    var isMovable : Bool = true
    var faceUp : Bool = true
    
    var frontTexture: SKTexture
    var backTexture: SKTexture
    var largeTexture: SKTexture?
    var largeTextureFilename: String
    var tileLocation : CGPoint = CGPointMake(0, 0)
    
    enum TileStyle : Int {
        case basic = 0,
        metal,
        pastel,
        wood,
        neon
    }
    
    required init(coder aDecoder: NSCoder) {
        fatalError("NSCoding not supported")
    }
    
    init(imageNamed: String) {
        // initialize properties
        backTexture = SKTexture(imageNamed: "TileBackTest90x90")
        frontTexture = SKTexture(imageNamed: "LetterTest90x90")
        largeTextureFilename = "OrangeLetterA"
        let cardTexture = SKTexture(imageNamed: imageNamed)
        super.init(texture: cardTexture, color: nil, size: cardTexture.size())
        self.userInteractionEnabled = true
    }
    
    init(tileStyle: TileStyle, withChar : String, withColor : SKColor, atPoint: CGPoint) {  //  // add letter, name, colorize?,
        
        
        backTexture = SKTexture(imageNamed: "TileBackTest90x90")
        switch tileStyle {
        case .basic:
            frontTexture = SKTexture(imageNamed: "LetterTest90x90")
            largeTextureFilename = "OrangeLetterA"
            
        case .metal:
            frontTexture = SKTexture(imageNamed: "LetterTest90x90")
            largeTextureFilename = "OrangeLetterA"
            
        default:
            frontTexture = SKTexture(imageNamed: "LetterTest90x90")
            largeTextureFilename = "OrangeLetterA"
        }
        
        // call designated initializer on super
        super.init(texture: frontTexture, color: nil, size: frontTexture.size())

        
        // initialize properties

        

        
        //var tileNode = SKSpriteNode(   (frontTexture)  //  (imageNamed: "LetterTest90x90")
        
        
//        var frame = CGRect(x: 0, y: 0, width: frontTexture.size().width , height: frontTexture.size().height)
//            //add a letter(s) on top
//            //let letterLabel = SKLabelNode(     (frame: frame.bounds)
//            letterLabel.textAlignment = NSTextAlignment.Center
//            letterLabel.textColor = UIColor.whiteColor()
//            letterLabel.center = CGPointMake(0, 0)
//            // letterLabel.anchorPoint
//            // letterLabel.backgroundColor = UIColor.clearColor() // original textured
//            let colorDarkGreen : UIColor = UIColor(red: 0.1, green: 1.0, blue: 0.2, alpha: 0.80)
//            letterLabel.backgroundColor =  colorDarkGreen
//            //letterLabel.text = String(letter).uppercaseString
//            letterLabel.font =  UIFont(name: "HelveticaNeue-Bold", size:  25.0)
//            self.addSubview(letterLabel)
        
        
        
//        func addTile (atPoint: CGPoint, withChar : String, withColor : SKColor){   // add letter, name, colorize?,
            //var letter : String = withChar
            self.name = "tile"
            self.position = atPoint
            self.anchorPoint = CGPointMake(0, 0)
            
            self.color = withColor
            self.colorBlendFactor = 1.0
            
            var letterLabel = SKLabelNode(fontNamed: FontHUDName)
            letterLabel.text = withChar
            letterLabel.fontSize = 40 // FontHUDSize
            //var colorDarkGreen : UIColor = UIColor(red: 0.1, green: 1.0, blue: 0.2, alpha: 0.80)
            letterLabel.fontColor = FontHUDWhite
            letterLabel.position = CGPointMake(22.5, 11)
            letterLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 0)!
            self.addChild(letterLabel)
        
            var letterValueLabel = SKLabelNode(fontNamed: FontHUDName)
            letterValueLabel.text = "10"
            letterValueLabel.fontSize = 10
            letterValueLabel.fontColor = FontHUDWhite
            letterValueLabel.position = CGPointMake(1, 1)
            letterValueLabel.horizontalAlignmentMode = SKLabelHorizontalAlignmentMode(rawValue: 1)!
        
            self.addChild(letterValueLabel)
            //foregroundNode!.addChild(tileNode)
//        }

        
        

        

        
        // set properties defined in super
        userInteractionEnabled = true
    }
    
//    //5 create a new tile for a given letter
//    init(letter:Character, sideLength:CGFloat) {
//        self.letter = letter
//        
//        //the tile background
//        let image = UIImage(named: "OrangeLetterA")!
//        
//        //superclass initializer
//        //references to superview's "self" must take place after super.init
//        super.init(image:image)
//        
//        //    let LetterAImage : UIImage = UIImage(named: "OrangeLetterA45x45")!
//        //    var imageV : UIImageView = UIImageView(image: LetterAImage)
//        //    imageV.frame = CGRectMake(0, 0, LetterAImage.size.width, LetterAImage.size.height)
//        //    imageV.center = CGPointMake(-45, -45)
//        //    self.addSubview(imageV)
//        
//        //6 resize the tile
//        let scale = sideLength / image.size.width
//        self.frame = CGRect(x: 0, y: 0, width: image.size.width * scale, height: image.size.height * scale)
//        //add a letter(s) on top
//        let letterLabel = UILabel(frame: self.bounds)
//        letterLabel.textAlignment = NSTextAlignment.Center
//        letterLabel.textColor = UIColor.whiteColor()
//        letterLabel.center = CGPointMake(0, 0)
//        // letterLabel.anchorPoint
//        // letterLabel.backgroundColor = UIColor.clearColor() // original textured
//        let colorDarkGreen : UIColor = UIColor(red: 0.1, green: 1.0, blue: 0.2, alpha: 0.80)
//        letterLabel.backgroundColor =  colorDarkGreen
//        letterLabel.text = String(letter).uppercaseString
//        letterLabel.font =  UIFont(name: "HelveticaNeue-Bold", size:  75.0*scale)
//        self.addSubview(letterLabel)
//        
//        //    ////////
//        //    //add a letter value number on top
//        //    let letterLabelNum = UILabel(frame: self.bounds)
//        //    letterLabelNum.textAlignment = NSTextAlignment.Natural
//        //    letterLabelNum.textColor = UIColor.whiteColor()
//        //    letterLabelNum.text = "      222\n333"  // ???
//        //    letterLabelNum.font = UIFont(name: "Verdana-Bold", size: 15.0*scale)
//        //    self.addSubview(letterLabelNum)
//        //    /////////
//        
//        self.userInteractionEnabled = true
//        
//        //create the tile shadow
//        self.layer.shadowColor = UIColor.blackColor().CGColor
//        self.layer.shadowOpacity = 0
//        self.layer.shadowOffset = CGSizeMake(10.0, 10.0)
//        self.layer.shadowRadius = 15.0
//        self.layer.masksToBounds = false
//        
//        let path = UIBezierPath(rect: self.bounds)
//        self.layer.shadowPath = path.CGPath
//        
//    }
    
    func flip() {
        if faceUp {
            self.texture = self.backTexture
            if let damageLabel = self.childNodeWithName("damageLabel") {
                damageLabel.hidden = true
            }
            self.faceUp = false
        } else {
            self.texture = self.frontTexture
            if let damageLabel = self.childNodeWithName("damageLabel") {
                damageLabel.hidden = false
            }
            self.faceUp = true
        }
    }
    
    var enlarged = false
    var savedPosition = CGPointZero
    
    func enlarge() {
        if enlarged {
            let slide = SKAction.moveTo(savedPosition, duration:0.3)
            let scaleDown = SKAction.scaleTo(1.0, duration:0.3)
            texture = backTexture
            runAction(SKAction.group([slide, scaleDown])) {
                self.enlarged = false
                self.zPosition = 0
            }
        } else {
            enlarged = true
            savedPosition = position
            
            if largeTexture != nil {
                texture = largeTexture
            } else {
                largeTexture = SKTexture(imageNamed: largeTextureFilename)
                texture = largeTexture
            }
            
            zPosition = 20
            
            let newPosition = CGPointMake(CGRectGetMidX(parent!.frame), CGRectGetMidY(parent!.frame))
            removeAllActions()
            
            let slide = SKAction.moveTo(newPosition, duration:0.3)
            let scaleUp = SKAction.scaleTo(5.0, duration:0.3)
            runAction(SKAction.group([slide, scaleUp]))
        }
    }
    
    override func touchesBegan(touches: Set<NSObject>, withEvent event: UIEvent) {
        /* Called when a touch begins */
        let actionSound = SKAction.playSoundFileNamed("37Bronk.mp3", waitForCompletion: true)
        runAction(actionSound)
        
        for touch in (touches as! Set<UITouch>) {
            //            if touch.tapCount > 1 {
            //                flip()
            //            }
            if touch.tapCount > 1 {
                enlarge()
            }
            
            if enlarged { return }
            //            let location = touch.locationInNode(self)
            //            let touchedNode = nodeAtPoint(location)
            zPosition = 15
            let liftUp = SKAction.scaleTo(1.2, duration: 0.1)
            let smallSlide = SKAction.moveByX(-6, y: -6, duration: 0.1)
//            runAction(liftUp, withKey: "pickup")
            runAction(SKAction.group([liftUp, smallSlide]))

        }
    }
    
    override func touchesMoved(touches: Set<NSObject>, withEvent event: UIEvent) {
        if enlarged { return }
        for touch in (touches as! Set<UITouch>) {
            if isMovable {
                let location = touch.locationInNode(scene)
                let touchedNode = nodeAtPoint(location)
                touchedNode.position = location
            }
        }
    }
    
    override func touchesEnded(touches: Set<NSObject>, withEvent event: UIEvent) {
        if enlarged { return }
        for touch in (touches as! Set<UITouch>) {
            //            let location = touch.locationInNode(self)
            //            let touchedNode = nodeAtPoint(location)
            zPosition = 0
            let dropDown = SKAction.scaleTo(1.0, duration: 0.1)
            let smallSlide = SKAction.moveByX(6, y: 6, duration: 0.1)
//            runAction(dropDown, withKey: "drop")
//            runAction(smallSlide, withKey: "slideBack")
            runAction(SKAction.group([dropDown, smallSlide]))
        }
    }
    
}