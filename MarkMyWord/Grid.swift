//
//  GridTools.swift
//  MarkMyWord
//
//  Created by Eric Hook on 5/5/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation
import SpriteKit

class Grid {
    
    var gridUpperLeftX : Double = 1.0
    var gridUpperLeftY : Double = 1.0
    
    var gridSquareSizeX : Double = 1.0
    var gridSquareSizeY : Double = 1.0
    
    var gridNumSquaresX : Int = 1
    var gridNumSquaresY : Int = 1
    
    var gridNumSquaresXMin : Int = 3
    var gridNumSquaresYMin : Int = 2

    var grid2DArr = [[MMWTile]]()
    var gridName : String = ""
    var gridPlayer : Player? = nil
    var gridNum : Int = 0
    
    var mmwGameScene : MMWGameScene

    init (gridUpperLeftX: Double, gridUpperLeftY : Double, gridSquareSizeX : Double, gridSquareSizeY : Double, gridNumSquaresX : Int, gridNumSquaresY : Int, gridName : String, mmwGameScene: MMWGameScene){
        self.gridUpperLeftX = gridUpperLeftX
        self.gridUpperLeftY = gridUpperLeftY
        self.gridSquareSizeX = gridSquareSizeX
        self.gridSquareSizeY = gridSquareSizeY
        self.gridNumSquaresX = gridNumSquaresX
        self.gridNumSquaresY = gridNumSquaresY
        self.gridName = gridName
        self.mmwGameScene = mmwGameScene
        //self.grid1DArr = gridArr
        grid2DArr = [[MMWTile]](count: gridNumSquaresX, repeatedValue: [MMWTile](count: gridNumSquaresY, repeatedValue: MMWTile() ))
    }
    
    func setGridPlayer (player : Player) {
        self.gridPlayer = player
    }
    
    func makeTilesInGridInteractive (isInteractive : Bool) {
        //for y in gridToInteract.grid2DArr
        for y in 0...(self.gridNumSquaresY - 1) {   // fill letter tiles
            for x in 0...(self.gridNumSquaresX - 1) {
                self.grid2DArr[x][y].tileSprite.userInteractionEnabled = isInteractive
            }
        }
    }
    
    func getGridSquare (locX: Float, locY: Float) -> (GridSquareX: Int, GridSquareY: Int, GridSquareUpperLeftCornerX: Double, GridSquareUpperLeftCornerY: Double) {
        let locYmod = locY + 35// (adj for flipped coordinates)
        var GridSquareX : Int = 0
        var GridSquareY : Int = 0
        var GridSquareUpperLeftCornerX : Double = 0.0
        var GridSquareUpperLeftCornerY : Double = 0.0
        
        GridSquareX = Int(( Double(locX)    - self.gridUpperLeftX ) / gridSquareSizeX ) // arrary [] x location
        GridSquareY = Int(( Double(locYmod) - self.gridUpperLeftY ) / gridSquareSizeY ) // arrary [] y location -> 0 is top left corner
        
        GridSquareUpperLeftCornerX = self.gridUpperLeftX + (Double(GridSquareX) * gridSquareSizeX)
        GridSquareUpperLeftCornerY = self.gridUpperLeftY + (Double(GridSquareY) * gridSquareSizeY)
        
        print("<Grid> TEST Grid square[]: x[\(GridSquareX)], y[\(GridSquareY)] and UL corner: x\(GridSquareUpperLeftCornerX), y\(GridSquareUpperLeftCornerY), click: \(locX), locY: \(locY), locYmod: \(locYmod)")
        
        return (GridSquareX, GridSquareY, GridSquareUpperLeftCornerX, GridSquareUpperLeftCornerY )
    }
    
    // given a grid and x, y func returns a CGPoint to place sprite
    func sendToGridSquare (grid: Grid, squareX: Int, squareY: Int) -> (CGPoint) {
        let tilePositionX = grid.gridUpperLeftX + 23.75 + ( Double(squareX) * grid.gridSquareSizeX )
        let tilePositionY = 768 - grid.gridUpperLeftY - ( Double(squareY) * grid.gridSquareSizeY ) + 7
        let tilePosition = CGPoint( x: tilePositionX, y: tilePositionY )
        return tilePosition
    }
    
    // given a grid and x, y func returns a CGPoint to place sprite
    func sendToGridSquare (squareX: Int, squareY: Int) -> (CGPoint) {
        let tilePositionX = self.gridUpperLeftX + 23.75 + ( Double(squareX) * self.gridSquareSizeX )
        let tilePositionY = 768 - self.gridUpperLeftY - ( Double(squareY) * self.gridSquareSizeY ) + 7
        let tilePosition = CGPoint( x: tilePositionX, y: tilePositionY )
        return tilePosition
    }
    
//    func addToGrid2DArray (tileToAdd: MMWTile, xGrid: Int, yGrid: Int) {
//        grid2DArr[xGrid].insert(tileToAdd, atIndex: [yGrid][xGrid])  // (tileToAdd, atIndex: xGrid)
//    }
    
    func tileAtGridSquare (grid: Grid, squareX: Int, squareY: Int) -> (MMWTile?) {
        let tile : MMWTile? = (grid.grid2DArr[squareY][squareX] as MMWTile)
        return tile
    }
    
    func tileAtGridSquare (squareX: Int, squareY: Int) -> (MMWTile?) {
        let tile : MMWTile? = (self.grid2DArr[squareY][squareX] as MMWTile)
        return tile
    }
    
    func dealGridFromArrayRandom (inout arrayIn: [MMWTile], var numTilesToDeal: Int, playerNum: Int) {
        print("<Grid> fillGridFromArray(arrayIn: [MMWTile], gridToFill: Grid) \(self.gridName) ")
        //var arrayInMarker = 0
        for y in 0...(self.gridNumSquaresY - 1) {   // fill letter tiles
            for x in 0...(self.gridNumSquaresX - 1) {
            mmwGameScene.mmwGameSceneViewController.tileCollection.mmwDiscardedTileArray.append( self.grid2DArr[x][y])
                    self.grid2DArr[x][y].tileSprite.hidden = true
                if arrayIn.count > 0 {
                    if numTilesToDeal > 0 {
                        let numTiles : UInt32 = UInt32(arrayIn.count)
                        let randomTileNumber = arc4random_uniform(numTiles) // select random tile in FROM array
                        let dealtTile : MMWTile = arrayIn[Int(randomTileNumber)]
                        
                        dealtTile.tileOwner = TileOwner(rawValue: playerNum)!
                        dealtTile.tileState = TileState(rawValue: playerNum)!
                        
                        self.grid2DArr[x][y] = dealtTile
                        arrayIn.removeAtIndex( Int(randomTileNumber) )
                        dealtTile.tileSprite.color =  gameColors[playerNum]
                        dealtTile.gridX = x
                        dealtTile.gridY = y
                        dealtTile.gridHome = self
                        numTilesToDeal--
                    }
                }
                else {
                    print("No More Tiles To Deal ... ")
                }
            }
        }
    }
    
    func dealGridFromArraySpecificTile (inout arrayIn: [MMWTile], tileArrayLocation: Int, playerNum: Int, squareX: Int, squareY: Int) {
        print("<Grid> dealGridFromArrayTile (inout arrayIn: [MMWTile], var tileArrayLocation: Int, playerNum: Int) \(self.gridName) ")
        //var arrayInMarker = 0
//        for y in 0...(self.gridNumSquaresY - 1) {   // fill letter tiles
//            for x in 0...(self.gridNumSquaresX - 1) {
//                mmwGameScene.mmwGameSceneViewController.tileCollection.mmwDiscardedTileArray.append(self.grid2DArr[x][y])
//                self.grid2DArr[x][y].tileSprite.hidden = true
                if arrayIn.count > 0 {
//                    if numTilesToDeal > 0 {
//                        let numTiles : UInt32 = UInt32(arrayIn.count)
//                        let randomTileNumber = arc4random_uniform(numTiles) // select random tile in FROM array
                        let dealtTile : MMWTile = arrayIn[Int(tileArrayLocation)]
                        
                        dealtTile.tileOwner = TileOwner(rawValue: playerNum)!
                        dealtTile.tileState = TileState(rawValue: playerNum)!
                        
                        self.grid2DArr[squareX][squareY] = dealtTile
                        arrayIn.removeAtIndex( Int(tileArrayLocation) )
                        dealtTile.tileSprite.color =  gameColors[playerNum]
                        dealtTile.gridX = squareX
                        dealtTile.gridY = squareY
                        dealtTile.gridHome = self
//                        numTilesToDeal--
                }
                else {
                    print("No More Tiles To Deal ... ")
                }
    }
    
//    // send/move num Tiles from one tile array to another tile array
//    func getAllNewGridTiles(inout tilesFromArray: [MMWTile], var numTilesToDeal: Int, changeColorTo: Int) {
//        let originalNumTilesToDeal = numTilesToDeal
//        for y in 0...(self.gridNumSquaresY - 1) {   // fill letter tiles
//            for x in 0...(self.gridNumSquaresX - 1) {
//                if numTilesToDeal > 0 {
//                    
//                    self.grid2DArr[x][y]
//                    let numTiles : UInt32 = UInt32(tilesFromArray.count - 1)
//                    let tileInArr = arc4random_uniform( numTiles ) // select random tile in FROM array
//                    let tileRemoved : MMWTile = tilesFrom.removeAtIndex( Int(tileInArr) )
//                    self.grid2DArr[x][y] = tileRemoved
//                    tilesTo[originalNumTilesToDeal - numTilesToDeal].tileSprite.color =  gameColors[changeColorTo]
//                    tilesTo[originalNumTilesToDeal - numTilesToDeal].tileSprite.hidden = false
//                    numTilesGet--
//        }
//    }
    
//    func fillArrayFromGrid (arrayToFill: [MMWTile], gridOut: Grid ) {
//        var arrayInMarker = 0
//        for y in 0...(gridOut.gridNumSquaresY - 1) {   // fill Player letter tiles
//            for x in 0...(gridOut.gridNumSquaresX - 1) {
//                if arrayInMarker < x * y {
//                    gridOut.grid2DArr[y][x] = arrayToFill[arrayInMarker] as MMWTile
//                    arrayInMarker++
//                }
//            }
//        }
//    }
    
//    func fillArrayFromGrid () {
//        var arrayInMarker = 0
//        for y in 0...(self.gridNumSquaresY - 1) {   // fill Player letter tiles
//            for x in 0...(self.gridNumSquaresX - 1) {
//                if arrayInMarker < x * y {
//                    self.grid1DArr[x] = self.grid1DArr[arrayInMarker] as MMWTile
//                    arrayInMarker++
//                }
//            }
//        }
//    }
//    
//    func fillGridFromArray () {
//        print("<Grid> fillGridFromArray() ")
//        var arrayInMarker = 0
//        for y in 0...(self.gridNumSquaresY - 1) {   // fill letter tiles
//            for x in 0...(self.gridNumSquaresX - 1) {
//                if arrayInMarker < (self.gridNumSquaresY) * (self.gridNumSquaresX) {
//                    let tileUpdated : MMWTile = grid1DArr[arrayInMarker]
//                    self.grid2DArr[y][x] = tileUpdated
//                    tileUpdated.gridX = x
//                    tileUpdated.gridY = y
//                    //print("tile: \((arrayIn[arrayInMarker] as MMWTile).tileSprite.tileText) [\(tileUpdated.gridX)] [\(tileUpdated.gridY)] ")
//                    arrayInMarker++
//                }
//            }
//        }
//        printGrid(self)
//    }
    
    func printGrid () {
        print("<Grid> printGrid \(self.gridName) ")
        for y in 0...(self.gridNumSquaresY - 1) {   // fill letter tiles
            for x in 0...(self.gridNumSquaresX - 1) {
                let gridArr = self.grid2DArr[x][y]
                print(" \( gridArr.tileSprite.tileText) -> [\(gridArr.tileSprite.tileSpriteParent.gridX)] [\(gridArr.tileSprite.tileSpriteParent.gridY)] ")
            }
        }
    }

    func printGrid (gridToPrint: Grid ) {
        print("<Grid> printGrid \(gridToPrint.gridName) ")
        for x in 0...(gridToPrint.gridNumSquaresX - 1) {   // fill letter tiles
            for y in 0...(gridToPrint.gridNumSquaresY - 1) {
                let gridArr = gridToPrint.grid2DArr[x][y]
                print(" \( gridArr.tileSprite.tileText) -> [\(gridArr.tileSprite.tileSpriteParent.gridX)] [\(gridArr.tileSprite.tileSpriteParent.gridY)] state:[\(gridArr.tileSprite.tileSpriteParent.tileState)] owner:[\(gridArr.tileSprite.tileSpriteParent.tileOwner)] type:[\(gridArr.tileSprite.tileSpriteParent.tileType)]")
            }
        }
    }
}