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


var tileCollection : MMWTileBuilder? = MMWTileBuilder() // : MMWTileBuilder

class MMWTileBuilder {

    var mmwTileArray = [MMWTile]()
    
    var mmwTileArrayPlaceholder = [MMWTile](count: 250, repeatedValue: MMWTile())

    var mmwDiscardedTileArray = [MMWTile]()
    
    //var mmwBlankTileArray = [MMWTile]()
    
    var mmwPlayer1LetterTile2DArray : [[MMWTile?]]?
    var mmwPlayer2LetterTile2DArray : [[MMWTile?]]?
    var mmwPlayer3LetterTile2DArray : [[MMWTile?]]?
    var mmwPlayer4LetterTile2DArray : [[MMWTile?]]?
    
    var mmwBoardTile2DArray : [[MMWTile?]]?
    var arr = [3,4,5]

//    func returnTilesToStartArray () {
////        for tileRow in mmwPlayer2LetterTile2DArray! {
////            for tile in tileRow {
////                tile?.resetTileValues()
////            }
////        }
//    }

    weak var tileA1 = MMWTile(letterString: "A")
    weak var tileA2 = MMWTile(letterString: "A")
    weak var tileA3 = MMWTile(letterString: "A")
    weak var tileA4 = MMWTile(letterString: "A")
    weak var tileA5 = MMWTile(letterString: "A")
    weak var tileA6 = MMWTile(letterString: "A")
    weak var tileA7 = MMWTile(letterString: "A")
    weak var tileA8 = MMWTile(letterString: "A")
    weak var tileA9 = MMWTile(letterString: "A")
    weak var tileA10 = MMWTile(letterString: "A")
    
    weak var tileB1 = MMWTile(letterString: "B")
    weak var tileB2 = MMWTile(letterString: "B")
    weak var tileB3 = MMWTile(letterString: "B")

    weak var tileC1 = MMWTile(letterString: "C")
    weak var tileC2 = MMWTile(letterString: "C")
    weak var tileC3 = MMWTile(letterString: "C")
    
    weak var tileD1 = MMWTile(letterString: "D")
    weak var tileD2 = MMWTile(letterString: "D")
    weak var tileD3 = MMWTile(letterString: "D")
    weak var tileD4 = MMWTile(letterString: "D")
    weak var tileD5 = MMWTile(letterString: "D")
    
    weak var tileE1 = MMWTile(letterString: "E")
    weak var tileE2 = MMWTile(letterString: "E")
    weak var tileE3 = MMWTile(letterString: "E")
    weak var tileE4 = MMWTile(letterString: "E")
    weak var tileE5 = MMWTile(letterString: "E")
    weak var tileE6 = MMWTile(letterString: "E")
    weak var tileE7 = MMWTile(letterString: "E")
    weak var tileE8 = MMWTile(letterString: "E")
    weak var tileE9 = MMWTile(letterString: "E")
    weak var tileE10 = MMWTile(letterString: "E")
    weak var tileE11 = MMWTile(letterString: "E")
    weak var tileE12 = MMWTile(letterString: "E")
    weak var tileE13 = MMWTile(letterString: "E")
    
    weak var tileF1 = MMWTile(letterString: "F")
    weak var tileF2 = MMWTile(letterString: "F")
    
    weak var tileG1 = MMWTile(letterString: "G")
    weak var tileG2 = MMWTile(letterString: "G")
    weak var tileG3 = MMWTile(letterString: "G")
    weak var tileG4 = MMWTile(letterString: "G")
    
    weak var tileH1 = MMWTile(letterString: "H")
    weak var tileH2 = MMWTile(letterString: "H")
    weak var tileH3 = MMWTile(letterString: "H")
    
    weak var tileI1 = MMWTile(letterString: "I")
    weak var tileI2 = MMWTile(letterString: "I")
    weak var tileI3 = MMWTile(letterString: "I")
    weak var tileI4 = MMWTile(letterString: "I")
    weak var tileI5 = MMWTile(letterString: "I")
    weak var tileI6 = MMWTile(letterString: "I")
    weak var tileI7 = MMWTile(letterString: "I")
    weak var tileI8 = MMWTile(letterString: "I")
    weak var tileI9 = MMWTile(letterString: "I")
    weak var tileI10 = MMWTile(letterString: "I")
    
    weak var tileJ1 = MMWTile(letterString: "J")
    
    weak var tileK1 = MMWTile(letterString: "K")
    weak var tileK2 = MMWTile(letterString: "K")
    
    weak var tileL1 = MMWTile(letterString: "L")
    weak var tileL2 = MMWTile(letterString: "L")
    weak var tileL3 = MMWTile(letterString: "L")
    weak var tileL4 = MMWTile(letterString: "L")
    
    weak var tileM1 = MMWTile(letterString: "M")
    weak var tileM2 = MMWTile(letterString: "M")
    weak var tileM3 = MMWTile(letterString: "M")
    weak var tileM4 = MMWTile(letterString: "M")
    
    weak var tileN1 = MMWTile(letterString: "N")
    weak var tileN2 = MMWTile(letterString: "N")
    weak var tileN3 = MMWTile(letterString: "N")
    weak var tileN4 = MMWTile(letterString: "N")
    weak var tileN5 = MMWTile(letterString: "N")
    weak var tileN6 = MMWTile(letterString: "N")
    
    weak var tileO1 = MMWTile(letterString: "O")
    weak var tileO2 = MMWTile(letterString: "O")
    weak var tileO3 = MMWTile(letterString: "O")
    weak var tileO4 = MMWTile(letterString: "O")
    weak var tileO5 = MMWTile(letterString: "O")
    weak var tileO6 = MMWTile(letterString: "O")
    weak var tileO7 = MMWTile(letterString: "O")
    weak var tileO8 = MMWTile(letterString: "O")
    weak var tileO9 = MMWTile(letterString: "O")
    
    weak var tileP1 = MMWTile(letterString: "P")
    weak var tileP2 = MMWTile(letterString: "P")
    weak var tileP3 = MMWTile(letterString: "P")

    weak var tileQ1 = MMWTile(letterString: "Q")

    weak var tileR1 = MMWTile(letterString: "R")
    weak var tileR2 = MMWTile(letterString: "R")
    weak var tileR3 = MMWTile(letterString: "R")
    weak var tileR4 = MMWTile(letterString: "R")
    weak var tileR5 = MMWTile(letterString: "R")
    weak var tileR6 = MMWTile(letterString: "R")
    weak var tileR7 = MMWTile(letterString: "R")

    weak var tileS1 = MMWTile(letterString: "S")
    weak var tileS2 = MMWTile(letterString: "S")
    weak var tileS3 = MMWTile(letterString: "S")
    weak var tileS4 = MMWTile(letterString: "S")
    weak var tileS5 = MMWTile(letterString: "S")
    weak var tileS6 = MMWTile(letterString: "S")

    weak var tileT1 = MMWTile(letterString: "T")
    weak var tileT2 = MMWTile(letterString: "T")
    weak var tileT3 = MMWTile(letterString: "T")
    weak var tileT4 = MMWTile(letterString: "T")
    weak var tileT5 = MMWTile(letterString: "T")
    weak var tileT6 = MMWTile(letterString: "T")
    weak var tileT7 = MMWTile(letterString: "T")

    weak var tileU1 = MMWTile(letterString: "U")
    weak var tileU2 = MMWTile(letterString: "U")
    weak var tileU3 = MMWTile(letterString: "U")
    weak var tileU4 = MMWTile(letterString: "U")

    weak var tileV1 = MMWTile(letterString: "V")
    weak var tileV2 = MMWTile(letterString: "V")

    weak var tileW1 = MMWTile(letterString: "W")
    weak var tileW2 = MMWTile(letterString: "W")

    weak var tileX1 = MMWTile(letterString: "X")

    weak var tileY1 = MMWTile(letterString: "Y")
    weak var tileY2 = MMWTile(letterString: "Y")
    weak var tileY3 = MMWTile(letterString: "Y")

    weak var tileZ1 = MMWTile(letterString: "Z")

//    var tileBlank1 : MMWTile = MMWTile(letterString: "?")
//    var tileBlank2 : MMWTile = MMWTile(letterString: "?")

    
    init () {

        mmwTileArray.append(tileA1!) 
        mmwTileArray.append(tileA2!)
        mmwTileArray.append(tileA3!)
        mmwTileArray.append(tileA4!)
        mmwTileArray.append(tileA5!)
        mmwTileArray.append(tileA6!)
        mmwTileArray.append(tileA7!)
        mmwTileArray.append(tileA8!)
        mmwTileArray.append(tileA9!)
        
        mmwTileArray.append(tileB1!)
        mmwTileArray.append(tileB2!)
        
        mmwTileArray.append(tileC1!)
        mmwTileArray.append(tileC2!)
        
        mmwTileArray.append(tileD1!)
        mmwTileArray.append(tileD2!)
        mmwTileArray.append(tileD3!)
        mmwTileArray.append(tileD4!)
        
        mmwTileArray.append(tileE1!)
        mmwTileArray.append(tileE2!)
        mmwTileArray.append(tileE3!)
        mmwTileArray.append(tileE4!)
        mmwTileArray.append(tileE5!)
        mmwTileArray.append(tileE6!)
        mmwTileArray.append(tileE7!)
        mmwTileArray.append(tileE8!)
        mmwTileArray.append(tileE9!)
        mmwTileArray.append(tileE10!)
        mmwTileArray.append(tileE11!)
        mmwTileArray.append(tileE12!)
        
        mmwTileArray.append(tileF1!)
        mmwTileArray.append(tileF2!)
        
        mmwTileArray.append(tileG1!)
        mmwTileArray.append(tileG2!)
        mmwTileArray.append(tileG3!)
        
        mmwTileArray.append(tileH1!)
        mmwTileArray.append(tileH2!)
        
        mmwTileArray.append(tileI1!)
        mmwTileArray.append(tileI2!)
        mmwTileArray.append(tileI3!)
        mmwTileArray.append(tileI4!)
        mmwTileArray.append(tileI5!)
        mmwTileArray.append(tileI6!)
        mmwTileArray.append(tileI7!)
        mmwTileArray.append(tileI8!)
        mmwTileArray.append(tileI9!)
        
        mmwTileArray.append(tileJ1!)
        
        mmwTileArray.append(tileK1!)
        
        mmwTileArray.append(tileL1!)
        mmwTileArray.append(tileL2!)
        mmwTileArray.append(tileL3!)
        mmwTileArray.append(tileL4!)
        
        mmwTileArray.append(tileM1!)
        mmwTileArray.append(tileM2!)
        
        mmwTileArray.append(tileN1!)
        mmwTileArray.append(tileN2!)
        mmwTileArray.append(tileN3!)
        mmwTileArray.append(tileN4!)
        mmwTileArray.append(tileN5!)
        mmwTileArray.append(tileN6!)
        
        mmwTileArray.append(tileO1!)
        mmwTileArray.append(tileO2!)
        mmwTileArray.append(tileO3!)
        mmwTileArray.append(tileO4!)
        mmwTileArray.append(tileO5!)
        mmwTileArray.append(tileO6!)
        mmwTileArray.append(tileO7!)
        mmwTileArray.append(tileO8!)
        
        mmwTileArray.append(tileP1!)
        mmwTileArray.append(tileP2!)
        
        mmwTileArray.append(tileQ1!)
        
        mmwTileArray.append(tileR1!)
        mmwTileArray.append(tileR2!)
        mmwTileArray.append(tileR3!)
        mmwTileArray.append(tileR4!)
        mmwTileArray.append(tileR5!)
        mmwTileArray.append(tileR6!)
        
        mmwTileArray.append(tileS1!)
        mmwTileArray.append(tileS2!)
        mmwTileArray.append(tileS3!)
        mmwTileArray.append(tileS4!)
        
        mmwTileArray.append(tileT1!)
        mmwTileArray.append(tileT2!)
        mmwTileArray.append(tileT3!)
        mmwTileArray.append(tileT4!)
        mmwTileArray.append(tileT5!)
        mmwTileArray.append(tileT6!)
        
        mmwTileArray.append(tileU1!)
        mmwTileArray.append(tileU2!)
        mmwTileArray.append(tileU3!)
        mmwTileArray.append(tileU4!)
        
        mmwTileArray.append(tileV1!)
        mmwTileArray.append(tileV2!)
        
        mmwTileArray.append(tileW1!)
        mmwTileArray.append(tileW2!)
        
        mmwTileArray.append(tileX1!)
        
        mmwTileArray.append(tileY1!)
        mmwTileArray.append(tileY2!)
        
        mmwTileArray.append(tileZ1!)
        
        for tile in mmwTileArray {
            tile.tileType = TileType.Letter
        }
        
//        mmwTileArray.append(tileBlank1)
//        mmwTileArray.append(tileBlank2)
        
        for tile in mmwTileArray {
            tile.tileState = TileState.Undealt
            tile.tileOwner = TileOwner.None
            //tile.tileBuilder = self
            tile.tileSprite.xScale = (screenSize!.width)/1024
            tile.tileSprite.yScale = (screenSize!.width)/1024
        }
        
//        for _ in 0..<500 {
//            mmwTileArrayPlaceholder.append(MMWTile())
//        }
        
//        for _ in 0..<250 {
//            mmwBlankTileArray.append(MMWTile())
//        }
    }

    
    func displayTileArrayValues (tileArray: [MMWTile]) {
        if debugMode == true { print("\nMMWTileBuilder.displayTileArrayValues (tileArray: [MMWTile]) ... \(tileArray) count:\(tileArray.count)") }
        for tile in tileArray {
            if debugMode == true { print("\(tile.tileText)") }
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
            numTilesGet -= 1
        }
    }
    
    
    /// send/move num Tiles from one tile array to another tile array
    func discardTiles(inout tilesFrom: [MMWTile], var numTilesDiscard: Int) {
        while numTilesDiscard > 0 {
            let tileRemoved : MMWTile = tilesFrom.removeAtIndex( Int(numTilesDiscard - 1) )
            tileRemoved.tileSprite.color =  gameColors[0]          //tilesTo.removeAtIndex(originalTilesGet - numTilesGet)
            tileRemoved.tileSprite.hidden = true
            mmwDiscardedTileArray.insert(tileRemoved, atIndex: mmwDiscardedTileArray.count)
            numTilesDiscard -= 1
        }
    }
    
    
    func getNewLetterTiles() {
        mmwTileArray.removeAll()
        
        mmwTileArray.append(tileA1!)
        mmwTileArray.append(tileA2!)
        mmwTileArray.append(tileA3!)
        mmwTileArray.append(tileA4!)
        mmwTileArray.append(tileA5!)
        mmwTileArray.append(tileA6!)
        mmwTileArray.append(tileA7!)
        mmwTileArray.append(tileA8!)
        mmwTileArray.append(tileA9!)
        
        mmwTileArray.append(tileB1!)
        mmwTileArray.append(tileB2!)
        
        mmwTileArray.append(tileC1!)
        mmwTileArray.append(tileC2!)
        
        mmwTileArray.append(tileD1!)
        mmwTileArray.append(tileD2!)
        mmwTileArray.append(tileD3!)
        mmwTileArray.append(tileD4!)
        
        mmwTileArray.append(tileE1!)
        mmwTileArray.append(tileE2!)
        mmwTileArray.append(tileE3!)
        mmwTileArray.append(tileE4!)
        mmwTileArray.append(tileE5!)
        mmwTileArray.append(tileE6!)
        mmwTileArray.append(tileE7!)
        mmwTileArray.append(tileE8!)
        mmwTileArray.append(tileE9!)
        mmwTileArray.append(tileE10!)
        mmwTileArray.append(tileE11!)
        mmwTileArray.append(tileE12!)
        
        mmwTileArray.append(tileF1!)
        mmwTileArray.append(tileF2!)
        
        mmwTileArray.append(tileG1!)
        mmwTileArray.append(tileG2!)
        mmwTileArray.append(tileG3!)
        
        mmwTileArray.append(tileH1!)
        mmwTileArray.append(tileH2!)
        
        mmwTileArray.append(tileI1!)
        mmwTileArray.append(tileI2!)
        mmwTileArray.append(tileI3!)
        mmwTileArray.append(tileI4!)
        mmwTileArray.append(tileI5!)
        mmwTileArray.append(tileI6!)
        mmwTileArray.append(tileI7!)
        mmwTileArray.append(tileI8!)
        mmwTileArray.append(tileI9!)
        
        mmwTileArray.append(tileJ1!)
        
        mmwTileArray.append(tileK1!)
        
        mmwTileArray.append(tileL1!)
        mmwTileArray.append(tileL2!)
        mmwTileArray.append(tileL3!)
        mmwTileArray.append(tileL4!)
        
        mmwTileArray.append(tileM1!)
        mmwTileArray.append(tileM2!)
        
        mmwTileArray.append(tileN1!)
        mmwTileArray.append(tileN2!)
        mmwTileArray.append(tileN3!)
        mmwTileArray.append(tileN4!)
        mmwTileArray.append(tileN5!)
        mmwTileArray.append(tileN6!)
        
        mmwTileArray.append(tileO1!)
        mmwTileArray.append(tileO2!)
        mmwTileArray.append(tileO3!)
        mmwTileArray.append(tileO4!)
        mmwTileArray.append(tileO5!)
        mmwTileArray.append(tileO6!)
        mmwTileArray.append(tileO7!)
        mmwTileArray.append(tileO8!)
        
        mmwTileArray.append(tileP1!)
        mmwTileArray.append(tileP2!)
        
        mmwTileArray.append(tileQ1!)
        
        mmwTileArray.append(tileR1!)
        mmwTileArray.append(tileR2!)
        mmwTileArray.append(tileR3!)
        mmwTileArray.append(tileR4!)
        mmwTileArray.append(tileR5!)
        mmwTileArray.append(tileR6!)
        
        mmwTileArray.append(tileS1!)
        mmwTileArray.append(tileS2!)
        mmwTileArray.append(tileS3!)
        mmwTileArray.append(tileS4!)
        
        mmwTileArray.append(tileT1!)
        mmwTileArray.append(tileT2!)
        mmwTileArray.append(tileT3!)
        mmwTileArray.append(tileT4!)
        mmwTileArray.append(tileT5!)
        mmwTileArray.append(tileT6!)
        
        mmwTileArray.append(tileU1!)
        mmwTileArray.append(tileU2!)
        mmwTileArray.append(tileU3!)
        mmwTileArray.append(tileU4!)
        
        mmwTileArray.append(tileV1!)
        mmwTileArray.append(tileV2!)
        
        mmwTileArray.append(tileW1!)
        mmwTileArray.append(tileW2!)
        
        mmwTileArray.append(tileX1!)
        
        mmwTileArray.append(tileY1!)
        mmwTileArray.append(tileY2!)
        
        mmwTileArray.append(tileZ1!)
        
        for tile in mmwTileArray {
            tile.tileType = TileType.Letter
        }
        
        //        mmwTileArray.append(tileBlank1)
        //        mmwTileArray.append(tileBlank2)
        
        for tile in mmwTileArray {
            tile.tileState = TileState.Undealt
            tile.tileOwner = TileOwner.None
            tile.tileSprite.xScale = (screenSize!.width)/1024
            tile.tileSprite.yScale = (screenSize!.width)/1024
        }

    }
    
    
    /// send/move num Tiles from one tile array to another tile array
    func resetAllTiles() {

        LetterTileSprite.removeAllTileHighlights()
        
        let gameGrids = [ mmwGameScene.mmwBoardGrid, mmwGameScene.mmwPlayer1Grid, mmwGameScene.mmwPlayer2Grid, mmwGameScene.mmwPlayer3Grid, mmwGameScene.mmwPlayer4Grid ]
        // remove all actions from tiles e.g. moving, scaling, etc
        for grid in gameGrids {
            // move letter tiles back to mmwTileArray and replace with placeholder tiles
            for tileRow in 0..<grid.grid2DArr.count {  //self.mmwBoardTile2DArray! {
                for tileCol in 0..<grid.grid2DArr[tileRow].count {
                    let tile = grid.grid2DArr[tileRow][tileCol]
                    if tile.tileType == TileType.Letter {
                        tile.tileSprite.removeAllActions()
                    }
                }
            }
        }
        // reset tiles for all letters eg grid, zposition, scale, etc
        for grid in gameGrids {
            // move letter tiles back to mmwTileArray and replace with placeholder tiles
            for tileRow in 0..<grid.grid2DArr.count {  //self.mmwBoardTile2DArray! {
                for tileCol in 0..<grid.grid2DArr[tileRow].count {
                    let tile = grid.grid2DArr[tileRow][tileCol]
                    if tile.tileType == TileType.Letter {
                        grid.grid2DArr[tileRow][tileCol] = self.mmwTileArrayPlaceholder.removeFirst()
                    }
                    
                    if tile.tileType == TileType.Placeholder {

                    }
                    tile.resetTileValues()
                }
            }
        }
        
        // return letter tiles and placeholders to respective arrays
        for tile in self.mmwDiscardedTileArray {
            if tile.tileType == TileType.Placeholder {
                tileCollection?.mmwTileArrayPlaceholder.append(mmwDiscardedTileArray.removeFirst())
            }
            if tile.tileType == TileType.Letter {
                tileCollection?.mmwTileArray.append(mmwDiscardedTileArray.removeFirst())
            }
            tile.resetTileValues()
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
            numTilesGet -= 1
        }
    }
    
    deinit {
        print("mmwtilebuilder is being deinitialized")
    }
    
    
//    func fillGridWithBlankTiles (inout gridToFill: Grid) {
//        let gridNumSquaresX = gridToFill.gridNumSquaresX
//        let gridNumSquaresY = gridToFill.gridNumSquaresY
//        for y in 0...(gridNumSquaresY - 1) {   // fill letter tiles
//            for x in 0...(gridNumSquaresX - 1) {
//                let newTile = mmwBlankTileArray.popLast() // MMWTile()
//                gridToFill.grid2DArr[x][y] = newTile!
//                newTile!.gridHome = gridToFill
//                newTile!.gridX = x
//                newTile!.gridXEnd = x
//                newTile!.gridY = y
//                newTile!.gridYEnd = y
//            }
//        }
//    }
    
}