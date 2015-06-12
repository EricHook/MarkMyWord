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

enum spriteName: Int {
    case Mystery = 0, Letter, Blank, ColorBomb, DirectionBomb, Blocking
}

enum gridLocation: Int {
    case undealt = 0, player1, player2, player3, player4, gameBoard
}

enum letterValue: Int {
    case Unknown = 0, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z
}

enum tileState: Int {
    case black = 0, player1, player3, player4, gray, highlight
}

enum TileOwner: Int {
    case None = 0, Player1, Player2, Player3, Player4
}

class MMWTile {

    var highlightedSpriteName = ("\(spriteName.Blank) -Highlighted")
    
    static func random() -> TileType {
        return TileType(rawValue: Int(arc4random_uniform(4)) + 1)!
    }
    
    
//    var gridLocation : gridLocation[0]
//    var column: Int
//    var row: Int
//    let tileType: TileType
    
    //var sprite: SKSpriteNode?
    
    init(column: Int, row: Int, tileType: TileType) {
//        self.tileType = tileType
//        self.column = column
//        self.row = row
        
    }
}
