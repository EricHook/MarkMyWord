//
//  Tile.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/17/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//


import SpriteKit

enum TileType: Int {
    case Unknown = 0, Letter, Blank, ColorBomb, DirectionBomb, Blocking
    }

enum SpriteName: Int {
    case Mystery = 0, Letter, Blank, ColorBomb, DirectionBomb, Blocking
}

enum GridLocation: Int {
    case undealt = 0, player1, player2, player3, player4, gameBoard
}

//enum LetterValue: Int {
//    case unknown = 0, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z
//}

enum TileState: Int {
    case black = 0, player1, player3, player4, gray, highlight
}

enum TileOwner: Int {
    case None = 0, Player1, Player2, Player3, Player4
}

class MMWTile {
    var tileSprite : LetterTileSprite!
    var tileType: TileType = TileType.Unknown
    var spritename: SpriteName = SpriteName.Blank
    var gridLocation: GridLocation = GridLocation.undealt
    var gridLocationEnd: GridLocation = GridLocation.undealt
    var gridHome: Grid? = nil
    var gridEnd: Grid? = nil
    var gridX: Int = -1
    var gridY: Int = -1
    var gridXEnd: Int = -1
    var gridYEnd: Int = -1
    var letterString: String = "?"
    var tileState: TileState = TileState.black
    var tileOwner: TileOwner = TileOwner.None
    var tileGrid: Grid? = nil
    
    //var tileObjectParent : MMWTile
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    init () {
        self.tileType = TileType.Unknown
        self.spritename = SpriteName.Blank
        //self.gridLocation = GridLocation[0]
        self.gridX = 0
        self.gridY = 0
        self.letterString = "!"
        self.tileState = TileState.black
        self.tileOwner = TileOwner.None
        self.tileSprite  = LetterTileSprite(tileStyle: LetterTileSprite.TileStyle.basic, withChar: letterString, withColor: UIColorGray, atPoint: CGPointMake(0.5, 0.5))
        //self.tileSprite.alpha = 0.5
        self.tileSprite.zPosition = 1
        self.tileSprite.tileObjectParent = self
    }
    
    init (letterString : String) {
        self.tileType = TileType.Unknown
        self.spritename = SpriteName.Blank
        self.gridLocation = GridLocation.undealt
        self.gridX = 0
        self.gridY = 0
        self.letterString = letterString
        self.tileState = TileState.black
        self.tileOwner = TileOwner.None
        self.tileSprite  = LetterTileSprite(tileStyle: LetterTileSprite.TileStyle.basic, withChar: letterString, withColor: UIColorGray, atPoint: CGPointMake(0.5, 0.5))
        //self.tileSprite.alpha = 0.5
        self.tileSprite.zPosition = 1
        self.tileSprite.tileObjectParent = self
    }

    static func randomTileType() -> TileType {
        return TileType(rawValue: Int(arc4random_uniform(4)) + 1)!
    }
    
//    var gridLocation : gridLocation[0]
//    var column: Int
//    var row: Int
//    let tileType: TileType
    
    //var sprite: SKSpriteNode?
    

}
