//
//  MMWTileBuilder.swift
//  MarkMyWord
//
//  Created by Eric Hook on 6/11/15.
//  Copyright © 2015 Hook Studios. All rights reserved.
//

import Foundation
import UIKit
import SpriteKit

class MMWTileBuilder {
    
//    var mmwGameSceneViewController : MMWGameSceneViewController?
//    var mmwGameScene: MMWGameScene?
    
    var mmwTileArray = [MMWTile]()
    
    var mmwTileArrayTemp = [MMWTile]()

    var mmwDiscardedTileArray = [MMWTile]()
    
    var mmwPlayer1LetterTile2DArray : [[MMWTile]]?
    var mmwPlayer2LetterTile2DArray : [[MMWTile]]?
    var mmwPlayer3LetterTile2DArray : [[MMWTile]]?
    var mmwPlayer4LetterTile2DArray : [[MMWTile]]?
    
    var mmwBoardTile2DArray : [[MMWTile]]?

    var tileA1 : MMWTile = MMWTile(letterString: "A")
    var tileA2 : MMWTile = MMWTile(letterString: "A")
    var tileA3 : MMWTile = MMWTile(letterString: "A")
    var tileA4 : MMWTile = MMWTile(letterString: "A")
    var tileA5 : MMWTile = MMWTile(letterString: "A")
    var tileA6 : MMWTile = MMWTile(letterString: "A")
    var tileA7 : MMWTile = MMWTile(letterString: "A")
    var tileA8 : MMWTile = MMWTile(letterString: "A")
    var tileA9 : MMWTile = MMWTile(letterString: "A")
    var tileA10 : MMWTile = MMWTile(letterString: "A")
    
    var tileB1 : MMWTile = MMWTile(letterString: "B")
    var tileB2 : MMWTile = MMWTile(letterString: "B")
    var tileB3 : MMWTile = MMWTile(letterString: "B")

    var tileC1 : MMWTile = MMWTile(letterString: "C")
    var tileC2 : MMWTile = MMWTile(letterString: "C")
    var tileC3 : MMWTile = MMWTile(letterString: "C")
    
    var tileD1 : MMWTile = MMWTile(letterString: "D")
    var tileD2 : MMWTile = MMWTile(letterString: "D")
    var tileD3 : MMWTile = MMWTile(letterString: "D")
    var tileD4 : MMWTile = MMWTile(letterString: "D")
    var tileD5 : MMWTile = MMWTile(letterString: "D")
    
    var tileE1 : MMWTile = MMWTile(letterString: "E")
    var tileE2 : MMWTile = MMWTile(letterString: "E")
    var tileE3 : MMWTile = MMWTile(letterString: "E")
    var tileE4 : MMWTile = MMWTile(letterString: "E")
    var tileE5 : MMWTile = MMWTile(letterString: "E")
    var tileE6 : MMWTile = MMWTile(letterString: "E")
    var tileE7 : MMWTile = MMWTile(letterString: "E")
    var tileE8 : MMWTile = MMWTile(letterString: "E")
    var tileE9 : MMWTile = MMWTile(letterString: "E")
    var tileE10 : MMWTile = MMWTile(letterString: "E")
    var tileE11 : MMWTile = MMWTile(letterString: "E")
    var tileE12 : MMWTile = MMWTile(letterString: "E")
    var tileE13 : MMWTile = MMWTile(letterString: "E")
    
    var tileF1 : MMWTile = MMWTile(letterString: "F")
    var tileF2 : MMWTile = MMWTile(letterString: "F")
    
    var tileG1 : MMWTile = MMWTile(letterString: "G")
    var tileG2 : MMWTile = MMWTile(letterString: "G")
    var tileG3 : MMWTile = MMWTile(letterString: "G")
    var tileG4 : MMWTile = MMWTile(letterString: "G")
    
    var tileH1 : MMWTile = MMWTile(letterString: "H")
    var tileH2 : MMWTile = MMWTile(letterString: "H")
    var tileH3 : MMWTile = MMWTile(letterString: "H")
    
    var tileI1 : MMWTile = MMWTile(letterString: "I")
    var tileI2 : MMWTile = MMWTile(letterString: "I")
    var tileI3 : MMWTile = MMWTile(letterString: "I")
    var tileI4 : MMWTile = MMWTile(letterString: "I")
    var tileI5 : MMWTile = MMWTile(letterString: "I")
    var tileI6 : MMWTile = MMWTile(letterString: "I")
    var tileI7 : MMWTile = MMWTile(letterString: "I")
    var tileI8 : MMWTile = MMWTile(letterString: "I")
    var tileI9 : MMWTile = MMWTile(letterString: "I")
    var tileI10 : MMWTile = MMWTile(letterString: "I")
    
    var tileJ1 : MMWTile = MMWTile(letterString: "J")
    
    var tileK1 : MMWTile = MMWTile(letterString: "K")
    var tileK2 : MMWTile = MMWTile(letterString: "K")
    
    var tileL1 : MMWTile = MMWTile(letterString: "L")
    var tileL2 : MMWTile = MMWTile(letterString: "L")
    var tileL3 : MMWTile = MMWTile(letterString: "L")
    var tileL4 : MMWTile = MMWTile(letterString: "L")
    
    var tileM1 : MMWTile = MMWTile(letterString: "M")
    var tileM2 : MMWTile = MMWTile(letterString: "M")
    var tileM3 : MMWTile = MMWTile(letterString: "M")
    var tileM4 : MMWTile = MMWTile(letterString: "M")
    
    var tileN1 : MMWTile = MMWTile(letterString: "N")
    var tileN2 : MMWTile = MMWTile(letterString: "N")
    var tileN3 : MMWTile = MMWTile(letterString: "N")
    var tileN4 : MMWTile = MMWTile(letterString: "N")
    var tileN5 : MMWTile = MMWTile(letterString: "N")
    var tileN6 : MMWTile = MMWTile(letterString: "N")
    
    var tileO1 : MMWTile = MMWTile(letterString: "O")
    var tileO2 : MMWTile = MMWTile(letterString: "O")
    var tileO3 : MMWTile = MMWTile(letterString: "O")
    var tileO4 : MMWTile = MMWTile(letterString: "O")
    var tileO5 : MMWTile = MMWTile(letterString: "O")
    var tileO6 : MMWTile = MMWTile(letterString: "O")
    var tileO7 : MMWTile = MMWTile(letterString: "O")
    var tileO8 : MMWTile = MMWTile(letterString: "O")
    var tileO9 : MMWTile = MMWTile(letterString: "O")
    
    var tileP1 : MMWTile = MMWTile(letterString: "P")
    var tileP2 : MMWTile = MMWTile(letterString: "P")
    var tileP3 : MMWTile = MMWTile(letterString: "P")

    var tileQ1 : MMWTile = MMWTile(letterString: "Q")

    var tileR1 : MMWTile = MMWTile(letterString: "R")
    var tileR2 : MMWTile = MMWTile(letterString: "R")
    var tileR3 : MMWTile = MMWTile(letterString: "R")
    var tileR4 : MMWTile = MMWTile(letterString: "R")
    var tileR5 : MMWTile = MMWTile(letterString: "R")
    var tileR6 : MMWTile = MMWTile(letterString: "R")
    var tileR7 : MMWTile = MMWTile(letterString: "R")

    var tileS1 : MMWTile = MMWTile(letterString: "S")
    var tileS2 : MMWTile = MMWTile(letterString: "S")
    var tileS3 : MMWTile = MMWTile(letterString: "S")
    var tileS4 : MMWTile = MMWTile(letterString: "S")
    var tileS5 : MMWTile = MMWTile(letterString: "S")
    var tileS6 : MMWTile = MMWTile(letterString: "S")

    var tileT1 : MMWTile = MMWTile(letterString: "T")
    var tileT2 : MMWTile = MMWTile(letterString: "T")
    var tileT3 : MMWTile = MMWTile(letterString: "T")
    var tileT4 : MMWTile = MMWTile(letterString: "T")
    var tileT5 : MMWTile = MMWTile(letterString: "T")
    var tileT6 : MMWTile = MMWTile(letterString: "T")
    var tileT7 : MMWTile = MMWTile(letterString: "T")

    var tileU1 : MMWTile = MMWTile(letterString: "U")
    var tileU2 : MMWTile = MMWTile(letterString: "U")
    var tileU3 : MMWTile = MMWTile(letterString: "U")
    var tileU4 : MMWTile = MMWTile(letterString: "U")

    var tileV1 : MMWTile = MMWTile(letterString: "V")
    var tileV2 : MMWTile = MMWTile(letterString: "V")

    var tileW1 : MMWTile = MMWTile(letterString: "W")
    var tileW2 : MMWTile = MMWTile(letterString: "W")

    var tileX1 : MMWTile = MMWTile(letterString: "X")

    var tileY1 : MMWTile = MMWTile(letterString: "Y")
    var tileY2 : MMWTile = MMWTile(letterString: "Y")
    var tileY3 : MMWTile = MMWTile(letterString: "Y")

    var tileZ1 : MMWTile = MMWTile(letterString: "Z")

//    var tileBlank1 : MMWTile = MMWTile(letterString: "?")
//    var tileBlank2 : MMWTile = MMWTile(letterString: "?")

    
    init () {
        
        //self.mmwGameSceneViewController = mmwGameSceneViewController

        mmwTileArray.append(tileA1) 
        mmwTileArray.append(tileA2)
        mmwTileArray.append(tileA3)
        mmwTileArray.append(tileA4)
        mmwTileArray.append(tileA5)
        mmwTileArray.append(tileA6)
        mmwTileArray.append(tileA7)
        mmwTileArray.append(tileA8)
        mmwTileArray.append(tileA9)
        
        mmwTileArray.append(tileB1)
        mmwTileArray.append(tileB2)
        
        mmwTileArray.append(tileC1)
        mmwTileArray.append(tileC2)
        
        mmwTileArray.append(tileD1)
        mmwTileArray.append(tileD2)
        mmwTileArray.append(tileD3)
        mmwTileArray.append(tileD4)
        
        mmwTileArray.append(tileE1)
        mmwTileArray.append(tileE2)
        mmwTileArray.append(tileE3)
        mmwTileArray.append(tileE4)
        mmwTileArray.append(tileE5)
        mmwTileArray.append(tileE6)
        mmwTileArray.append(tileE7)
        mmwTileArray.append(tileE8)
        mmwTileArray.append(tileE9)
        mmwTileArray.append(tileE10)
        mmwTileArray.append(tileE11)
        mmwTileArray.append(tileE12)
        
        mmwTileArray.append(tileF1)
        mmwTileArray.append(tileF2)
        
        mmwTileArray.append(tileG1)
        mmwTileArray.append(tileG2)
        mmwTileArray.append(tileG3)
        
        mmwTileArray.append(tileH1)
        mmwTileArray.append(tileH2)
        
        mmwTileArray.append(tileI1)
        mmwTileArray.append(tileI2)
        mmwTileArray.append(tileI3)
        mmwTileArray.append(tileI4)
        mmwTileArray.append(tileI5)
        mmwTileArray.append(tileI6)
        mmwTileArray.append(tileI7)
        mmwTileArray.append(tileI8)
        mmwTileArray.append(tileI9)
        
        mmwTileArray.append(tileJ1)
        
        mmwTileArray.append(tileK1)
        
        mmwTileArray.append(tileL1)
        mmwTileArray.append(tileL2)
        mmwTileArray.append(tileL3)
        mmwTileArray.append(tileL4)
        
        mmwTileArray.append(tileM1)
        mmwTileArray.append(tileM2)
        
        mmwTileArray.append(tileN1)
        mmwTileArray.append(tileN2)
        mmwTileArray.append(tileN3)
        mmwTileArray.append(tileN4)
        mmwTileArray.append(tileN5)
        mmwTileArray.append(tileN6)
        
        mmwTileArray.append(tileO1)
        mmwTileArray.append(tileO2)
        mmwTileArray.append(tileO3)
        mmwTileArray.append(tileO4)
        mmwTileArray.append(tileO5)
        mmwTileArray.append(tileO6)
        mmwTileArray.append(tileO7)
        mmwTileArray.append(tileO8)
        
        mmwTileArray.append(tileP1)
        mmwTileArray.append(tileP2)
        
        mmwTileArray.append(tileQ1)
        
        mmwTileArray.append(tileR1)
        mmwTileArray.append(tileR2)
        mmwTileArray.append(tileR3)
        mmwTileArray.append(tileR4)
        mmwTileArray.append(tileR5)
        mmwTileArray.append(tileR6)
        
        mmwTileArray.append(tileS1)
        mmwTileArray.append(tileS2)
        mmwTileArray.append(tileS3)
        mmwTileArray.append(tileS4)
        
        mmwTileArray.append(tileT1)
        mmwTileArray.append(tileT2)
        mmwTileArray.append(tileT3)
        mmwTileArray.append(tileT4)
        mmwTileArray.append(tileT5)
        mmwTileArray.append(tileT6)
        
        mmwTileArray.append(tileU1)
        mmwTileArray.append(tileU2)
        mmwTileArray.append(tileU3)
        mmwTileArray.append(tileU4)
        
        mmwTileArray.append(tileV1)
        mmwTileArray.append(tileV2)
        
        mmwTileArray.append(tileW1)
        mmwTileArray.append(tileW2)
        
        mmwTileArray.append(tileX1)
        
        mmwTileArray.append(tileY1)
        mmwTileArray.append(tileY2)
        
        mmwTileArray.append(tileZ1)
        
        for tile in mmwTileArray {
            tile.tileType = TileType.Letter
        }
        
//        mmwTileArray.append(tileBlank1)
//        mmwTileArray.append(tileBlank2)
        
        for tile in mmwTileArray {
            tile.tileState = TileState.Undealt
            tile.tileOwner = TileOwner.None
            tile.tileBuilder = self
            

            
        }
        

        
        
    }
    
    func setViewControllerAndScene (mmwGameSceneController: MMWGameSceneViewController) {
        //self.mmwGameSceneViewController = mmwGameSceneController
        for tile in mmwTileArray {
//            tile.mmwGameSceneViewController = self.mmwGameSceneViewController
//            tile.tileSprite.mmwGameSceneViewController = self.mmwGameSceneViewController
//            tile.mmwGameScene = self.mmwGameScene
//            tile.tileSprite.mmwGameScene = self.mmwGameScene
            
            tile.tileSprite.xScale = (screenSize!.width)/1024
            tile.tileSprite.yScale = (screenSize!.width)/1024
            //self.size( Double(mmwGameScene.viewSize.width)/1024 )
            
//            if mmwGame.deviceType == MMWGame.DeviceType.iPadPro {
//                //gameGrid = SKSpriteNode(imageNamed: "GameGrid@iPadPro.png")
//                tile.tileSprite.xScale *= 1.33
//                tile.tileSprite.yScale *= 1.33
//            }
            
//            if mmwGame.deviceType == MMWGame.DeviceType.iPhone5 {
//                //gameGrid = SKSpriteNode(imageNamed: "GameGrid@iPadPro.png")
//                tile.tileSprite.xScale *= 0.5
//                tile.tileSprite.yScale *= 0.5
//                //FontHUD = FontHUDiPhone
//            }
            
            if mmwGame.deviceType == MMWGame.DeviceType.iPhone6Plus || mmwGame.deviceType == MMWGame.DeviceType.iPhone5 {
                //gameGrid = SKSpriteNode(imageNamed: "GameGrid@iPadPro.png")
                tile.tileSprite.xScale *= 0.75
                tile.tileSprite.yScale *= 0.75
                //FontHUD = FontHUDiPhone
            }
        }
    }
    
    func displayTileArrayValues (tileArray: [MMWTile]) {
        print("\nMMWTileBuilder.displayTileArrayValues (tileArray: [MMWTile]) ... \(self)")
        for tile in tileArray {
            print("\(tile.tileText)")
        }
    }
    
    /// send/move num Tiles from one tile array to another tile array
    func getAllNewArrayTiles(inout tilesFrom: [MMWTile], inout tilesTo: [MMWTile], var numTilesGet: Int, changeColorTo: Int) {
        
        discardTiles(&tilesTo, numTilesDiscard: numTilesGet)
        
        let originalTilesGet = numTilesGet
        while numTilesGet > 0 {
            let numTiles : UInt32 = UInt32(tilesFrom.count - 1)
            let tileInArr = arc4random_uniform( numTiles ) // select random tile in FROM array
            let tileRemoved : MMWTile = tilesFrom.removeAtIndex( Int(tileInArr) )
            tilesTo.insert(tileRemoved, atIndex: originalTilesGet - numTilesGet)
            tilesTo[originalTilesGet - numTilesGet].tileSprite.color =  gameColors[changeColorTo]
            tilesTo[originalTilesGet - numTilesGet].tileSprite.hidden = false
            numTilesGet--
        }
    }
    
    /// send/move num Tiles from one tile array to another tile array
    func discardTiles(inout tilesFrom: [MMWTile], var numTilesDiscard: Int) {
        while numTilesDiscard > 0 {
            let tileRemoved : MMWTile = tilesFrom.removeAtIndex( Int(numTilesDiscard - 1) )
            tileRemoved.tileSprite.color =  gameColors[0]          //tilesTo.removeAtIndex(originalTilesGet - numTilesGet)
            tileRemoved.tileSprite.hidden = true
            mmwDiscardedTileArray.insert(tileRemoved, atIndex: mmwDiscardedTileArray.count)
            numTilesDiscard--
        }
    }
    
    /// send/move num Tiles from one tile array to another tile array
    func updateTiles(inout tilesFrom: [MMWTile], inout tilesTo: [MMWTile], var numTilesGet: Int, changeColorTo: Int) {
        let originalTilesGet = numTilesGet
        while numTilesGet > 0 {
            let numTiles : UInt32 = UInt32(tilesFrom.count - 1)
            let tileInArr = arc4random_uniform( numTiles ) // select random tile in FROM array
            let tileRemoved : MMWTile = tilesFrom.removeAtIndex( Int(tileInArr) )
            tileRemoved.tileSprite.color =  gameColors[changeColorTo]
            if (originalTilesGet - numTilesGet) < tilesTo.count {
                tilesTo[originalTilesGet - numTilesGet].tileSprite.hidden = false
                tilesTo.removeAtIndex(originalTilesGet - numTilesGet)
            }
            tilesTo.insert(tileRemoved, atIndex: originalTilesGet - numTilesGet)
            numTilesGet--
        }
    }
    
    func fillGridWithBlankTiles (inout gridToFill: Grid) {
        let gridNumSquaresX = gridToFill.gridNumSquaresX
        let gridNumSquaresY = gridToFill.gridNumSquaresY
        for y in 0...(gridNumSquaresY - 1) {   // fill letter tiles
            for x in 0...(gridNumSquaresX - 1) {
                let newTile = MMWTile()
                gridToFill.grid2DArr[x][y] = newTile
                newTile.gridHome = gridToFill
                newTile.gridX = x
                newTile.gridXEnd = x
                newTile.gridY = y
                newTile.gridYEnd = y
            }
        }
    }
}