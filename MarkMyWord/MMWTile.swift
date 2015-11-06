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
    case Undealt = 0, Player1, Player2, Player3, Player4, Played, Locked
}

enum TileOwner: Int {
    case None = 0, Player1, Player2, Player3, Player4, Discard
}

class MMWTile {
    var mmwGameSceneViewController : MMWGameSceneViewController?
    var mmwGameScene : MMWGameScene?
    var tileSprite : LetterTileSprite!
    var tileBuilder : MMWTileBuilder? = nil
    var tileType: TileType = TileType.Unknown
    var spritename: SpriteName = SpriteName.Blank
    var gridLocation: GridLocation = GridLocation.Undealt
    var gridLocationEnd: GridLocation = GridLocation.Undealt
    var gridHome: Grid? = nil
    var gridEnd: Grid? = nil
    var gridX: Int = -1
    var gridY: Int = -1
    var gridXEnd: Int = -1
    var gridYEnd: Int = -1

    var tileText: String = "?"
    var undealt = true
    var tileState: TileState = TileState.Undealt
    var tileOwner: TileOwner = TileOwner.None
    var tileGrid: Grid? = nil
    var description = ""
    
    var playableSpotsEachDirection : (left: Int, right: Int, up: Int, down: Int) = (0, 0, 0, 0)

    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    init () {  // creates blank placeholder tile
        self.tileType = TileType.Placeholder
        self.spritename = SpriteName.Blank
        //self.gridLocation = GridLocation[0]
        self.gridX = -1
        self.gridY = -1
        self.tileText = "!"
        description = "!"
        self.tileState = TileState.Undealt
        self.tileOwner = TileOwner.None
        self.tileSprite  = LetterTileSprite(tileStyle: LetterTileSprite.TileStyle.basic, withChar: tileText, withColor: UIColorGray, atPoint: CGPointMake(0.5, 0.5))
        self.tileSprite.position = CGPoint(x: 0.0, y: 0.0)
        self.tileSprite.zPosition = 1
        self.tileSprite.tileSpriteParent = self
        self.tileSprite.mmwGameSceneViewController = self.mmwGameSceneViewController
        self.tileSprite.mmwGameScene = self.mmwGameScene
        self.tileSprite.hidden = true
        tileSprite.alpha = 0.3
    }
    
    init (letterString : String) {
        self.tileType = TileType.Unknown
        self.spritename = SpriteName.Letter
        self.gridLocation = GridLocation.Undealt
        self.gridX = 0
        self.gridY = 0
        self.tileText = letterString
        description = "/(letterString)"
        self.tileState = TileState.Undealt
        self.tileOwner = TileOwner.None
        self.tileSprite  = LetterTileSprite(tileStyle: LetterTileSprite.TileStyle.basic, withChar: letterString, withColor: UIColor.blackColor(), atPoint: CGPointMake(0.0, 0.0))
        self.tileSprite.position = CGPoint(x: 0.0, y: 0.0)
        self.tileSprite.zPosition = 1
        self.tileSprite.tileSpriteParent = self
        self.tileSprite.mmwGameSceneViewController = self.mmwGameSceneViewController
        self.tileSprite.mmwGameScene = self.mmwGameScene
        self.tileSprite.hidden = true
    }
    
    
    
//    func setViewController (mmwGameSceneController: MMWGameSceneViewController) {
//        self.mmwGameSceneViewController = mmwGameSceneController
//    }

//    static func randomTileType() -> TileType {
//        return TileType(rawValue: Int(arc4random_uniform(4)) + 1)!
//    }
    
//    func updateTileGrid () {
//        self.gridEnd?.addToGridArray(self, xGrid: self.gridXEnd, yGrid: self.gridYEnd)
//    }
}
