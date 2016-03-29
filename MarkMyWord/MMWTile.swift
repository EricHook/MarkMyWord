//
//  Tile.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/17/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import SpriteKit

enum TileType: Int {
    case Unknown = 0, Letter, Blank, ColorBomb, DirectionBomb, Blocking, Placeholder
    }

enum SpriteName: Int {
    case Mystery = 0, Letter, Blank, ColorBovar, DirectionBomb, Blocking
}

enum GridLocation: Int {
    case Undealt = 0, Player1, Player2, Player3, Player4, GameBoard, Discard
}

//enum LetterValue: Int {
//    case unknown = 0, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z
//}

enum TileState: Int {
    case Undealt = 0, Player1, Player2, Player3, Player4, Played, PlayedMadeWord, Locked
}

enum PlayedMadeWord: Int {
    case None = 0, Vertical, Horizontal, Both
}

enum TileOwner: Int {
    case None = 0, Player1, Player2, Player3, Player4, Discard
}

class MMWTile : NSObject, NSCoding {
    var tileSprite : LetterTileSprite!
    //var tileBuilder : MMWTileBuilder? = nil
    var tileType: TileType = TileType.Unknown
    var spritename: SpriteName = SpriteName.Blank
    var gridLocation: GridLocation = GridLocation.Undealt
    var gridLocationEnd: GridLocation = GridLocation.Undealt
    var gridHome: Grid? = nil
    var gridEnd: Grid?  = nil
    var gridTest: Grid? = nil
    var gridX: Int = -1
    var gridY: Int = -1
    var gridXEnd: Int  = -1
    var gridYEnd: Int  = -1
    var gridXTest: Int = -1
    var gridYTest: Int = -1

    var tileText: String = "!"
    var undealt = true
    var tileState: TileState = TileState.Undealt
    var playedMadeWord       = PlayedMadeWord.None
    var tileOwner: TileOwner = TileOwner.None
    var tileGrid: Grid?      = nil
    var describe          = "!"
    
    var playableSpotsEachDirection : (left: Int, right: Int, up: Int, down: Int) = (0, 0, 0, 0)

//    required init?(coder aDecoder: NSCoder) {
//        fatalError("init(coder:) has not been implemented")
//    }
    
    required convenience init?(coder aDecoder: NSCoder) {
        //        let name = aDecoder.decodeObjectForKey(PropertyKey.nameKey) as! String
        //
        //        // Because photo is an optional property of Meal, use conditional cast.
        //        let photo = aDecoder.decodeObjectForKey(PropertyKey.photoKey) as? UIImage
        //
        //        let rating = aDecoder.decodeIntegerForKey(PropertyKey.ratingKey)
        //
        //        // Must call designated initializer.
//                self.init(name: name, photo: photo, rating: rating)
        
        self.init()
    }
    
    func resetTileValues () {
        self.tileSprite.removeActionForKey("tileAction")
        
        //if gridHome?.grid2DArr[gridX][gridY] = MMWTile()
        //gridHome?.grid2DArr[gridX][gridY] = MMWTile()
        
        if tileType == TileType.Letter {
            tileCollection?.mmwTileArray.append(self)
        }
        if tileType == TileType.Blank {
            tileCollection?.mmwTileArrayPlaceholder.append(self)
        }
        
        self.gridLocation = GridLocation.Undealt
        gridLocationEnd = GridLocation.Undealt
        gridHome = nil // mmwGameScene.mmwBoardGrid
        gridEnd = nil //mmwGameScene.mmwBoardGrid
        gridTest = nil //mmwGameScene.mmwBoardGrid
        gridX = -1
        gridY = -1
        gridXEnd = -1
        gridYEnd = -1
        gridXTest = -1
        gridYTest = -1
     
        tileSprite.position = CGPoint (x: -50, y: -50)
        tileSprite.zPosition = 2
        tileSprite.letterLabel.zPosition = 3
        undealt = true
        tileState = TileState.Undealt
        playedMadeWord = PlayedMadeWord.None
        tileOwner = TileOwner.None
        tileGrid = nil
        describe = ""
        
        let tileSize = CGSize(width: 47 , height: 47)
        tileSprite.size = tileSize
        if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
            tileSprite.size.width *= CGFloat(1.33)
            tileSprite.size.height *= CGFloat(1.33)
            tileSprite.tileShadow.xScale *= 0.75
            tileSprite.tileShadow.yScale *= 0.75
            tileSprite.tileGlow.xScale *= 0.75
            tileSprite.tileGlow.yScale *= 0.75
        }

        tileSprite.letterLabel.fontColor = UIColor(red: 1.0, green: 1.0, blue: 1.0, alpha: 0.98)
        tileSprite.color = UIColor(red: 0.0, green: 0.0, blue: 0.0, alpha: 1.00)
    }
    

    
    override init () {  // creates blank placeholder tile
        
        super.init()

        self.tileType = TileType.Placeholder
        self.spritename = SpriteName.Blank
        //self.gridLocation = GridLocation[0]
        self.gridX = -1
        self.gridY = -1
        //self.tileText = "!"
        //description = "!"
        //self.tileState = TileState.Undealt
        //self.tileOwner = TileOwner.None
        self.tileSprite  = LetterTileSprite(tileStyle: LetterTileSprite.TileStyle.basic, withChar: tileText, withColor: UIColorGray, atPoint: CGPointMake(0.0, 0.0))
        self.tileSprite.position = CGPoint(x: 0.0, y: 0.0)
        self.tileSprite.zPosition = 1
        self.tileSprite.tileSpriteParent = self
        self.tileSprite.hidden = true
        self.tileSprite.userInteractionEnabled = false
        tileSprite.alpha = 0.3
    }
    
    init (letterString : String) {
        
        super.init()
        
        self.tileType = TileType.Letter
        self.spritename = SpriteName.Letter
        self.gridLocation = GridLocation.Undealt
        self.gridX = 0
        self.gridY = 0
        self.tileText = letterString
        self.describe = "/(letterString)"
        self.tileState = TileState.Undealt
        self.tileOwner = TileOwner.None
        self.tileSprite  = LetterTileSprite(tileStyle: LetterTileSprite.TileStyle.basic, withChar: letterString, withColor: UIColor.blackColor(), atPoint: CGPointMake(0.0, 0.0))
        self.tileSprite.position = CGPoint(x: 0.0, y: 0.0)
        self.tileSprite.zPosition = 1
        self.tileSprite.tileSpriteParent = self
        self.tileSprite.hidden = true
        self.tileSprite.userInteractionEnabled = false
    }
    
    
    func encodeWithCoder(aCoder: NSCoder) {
        //        aCoder.encodeObject(name, forKey: PropertyKey.nameKey)
        //        aCoder.encodeObject(photo, forKey: PropertyKey.photoKey)
        //        aCoder.encodeInteger(rating, forKey: PropertyKey.ratingKey)
    }
    
    deinit {
        print("mmwtile is being deinitialized")
    }
    

}
