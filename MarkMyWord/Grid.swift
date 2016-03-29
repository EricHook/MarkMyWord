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
    
    var gridUpperLeftX : Double = 0.0
    var gridUpperLeftY : Double = 0.0
    
    var gridSquareSizeX : Double = 0.0
    var gridSquareSizeY : Double = 0.0
    
    var gridNumSquaresX : Int = 0
    var gridNumSquaresY : Int = 0
    
    var gridNumSquaresXMin : Int = 3
    var gridNumSquaresYMin : Int = 2

    var grid2DArr = [[MMWTile]]()
    var gridName : String = ""
    var gridPlayer : Player? = nil
    var gridNum : Int = 0

    init (gridUpperLeftX: Double, gridUpperLeftY : Double, gridSquareSizeX : Double, gridSquareSizeY : Double, gridNumSquaresX : Int, gridNumSquaresY : Int, gridName : String){
        self.gridUpperLeftX = gridUpperLeftX
        self.gridUpperLeftY = gridUpperLeftY
        self.gridSquareSizeX = gridSquareSizeX
        self.gridSquareSizeY = gridSquareSizeY
        self.gridNumSquaresX = gridNumSquaresX
        self.gridNumSquaresY = gridNumSquaresY
        self.gridName = gridName
        grid2DArr = [[MMWTile]](count: gridNumSquaresX, repeatedValue: [MMWTile](count: gridNumSquaresY, repeatedValue: MMWTile() ))
    }
    
    
    func setGridPlayer (player : Player) {
        self.gridPlayer = player
    }
    
    
    func numLetterTilesInGrid () -> Int {
        var numLetterTiles = 0
        for y in 0...(self.gridNumSquaresY - 1) {   // fill letter tiles
            for x in 0...(self.gridNumSquaresX - 1) {
                if self.grid2DArr[x][y].tileType == TileType.Letter {
                    numLetterTiles++
                }  
            }
        }
        return numLetterTiles
    }
    
    
    func getArrayLetterTilesInGrid () -> [MMWTile] {
        var letterTilesInGrid = [MMWTile]()
        var numLetterTiles = 0
        for y in 0...(self.gridNumSquaresY - 1) {   // fill letter tiles
            for x in 0...(self.gridNumSquaresX - 1) {
                if self.grid2DArr[x][y].tileType == TileType.Letter {
                    numLetterTiles++
                    letterTilesInGrid.append(grid2DArr[x][y])
                }
            }
        }
        return letterTilesInGrid
    }
    
//    func sendLocationForPlayerScore () -> x: Int, y. {
//        var letterTilesInGrid = [MMWTile]()
//        var numLetterTiles = 0
//        for y in 0...(self.gridNumSquaresY - 1) {   // fill letter tiles
//            for x in 0...(self.gridNumSquaresX - 1) {
//                if self.grid2DArr[x][y].tileType == TileType.Letter {
//                    numLetterTiles++
//                    letterTilesInGrid.append(grid2DArr[x][y])
//                }
//            }
//        }
//        return letterTilesInGrid
//    }
    
    
    func getArrayLockedLetterTilesInGrid () -> [MMWTile] {
        var letterTilesInGrid = [MMWTile]()
        var randomizedTilesInGrid = [MMWTile]() 
        var numLetterTiles = 0

        for y in 0...(self.gridNumSquaresY - 1) {   // fill letter tiles
            for x in 0...(self.gridNumSquaresX - 1) {
                if self.grid2DArr[x][y].tileType == TileType.Letter && self.grid2DArr[x][y].tileState == TileState.Locked {
                    numLetterTiles++
                    letterTilesInGrid.append(grid2DArr[x][y])
                }
            }
        }
        
        for _tile in 0..<letterTilesInGrid.count {
            let randomTileNumber = Int(arc4random_uniform( UInt32(letterTilesInGrid.count - 1) ) )
            let tileToMove = letterTilesInGrid.removeAtIndex(randomTileNumber)
            randomizedTilesInGrid.append( tileToMove )
        }
        return randomizedTilesInGrid
    }
    
    
    func makeTilesInGridInteractive (isInteractive : Bool) {
        for y in 0...(self.gridNumSquaresY - 1) {   // fill letter tiles
            for x in 0...(self.gridNumSquaresX - 1) {
                self.grid2DArr[x][y].tileSprite.userInteractionEnabled = isInteractive
            }
        }
    }
    
    
    func getGridSquare (locX: Float, locY: Float) -> (GridSquareX: Int, GridSquareY: Int, GridSquareUpperLeftCornerX: Double, GridSquareUpperLeftCornerY: Double) {
        let locYmod = locY + Float((screenSize!).height) * 0.0455729167 // 35 (adj for flipped coordinates)
        var GridSquareX : Int = 0
        var GridSquareY : Int = 0
        var GridSquareUpperLeftCornerX : Double = 0.0
        var GridSquareUpperLeftCornerY : Double = 0.0
        
        GridSquareX = Int(( Double(locX)    - self.gridUpperLeftX ) / gridSquareSizeX ) // arrary [] x location
        GridSquareY = Int(( Double(locYmod) - self.gridUpperLeftY ) / gridSquareSizeY ) // arrary [] y location -> 0 is top left corner
        
        GridSquareUpperLeftCornerX = self.gridUpperLeftX + (Double(GridSquareX) * gridSquareSizeX)
        GridSquareUpperLeftCornerY = self.gridUpperLeftY + (Double(GridSquareY) * gridSquareSizeY)
        
        //print("<Grid> TEST Grid square[]: x[\(GridSquareX)], y[\(GridSquareY)] and UL corner: x\(GridSquareUpperLeftCornerX), y\(GridSquareUpperLeftCornerY), click: \(locX), locY: \(locY), locYmod: \(locYmod)")
        
        if GridSquareX < 0  { GridSquareX = 0  }
        if GridSquareX > 14 { GridSquareX = 14 }
        if GridSquareY < 0  { GridSquareY = 0  }
        if GridSquareY > 14 { GridSquareY = 14 }
            
        return (GridSquareX, GridSquareY, GridSquareUpperLeftCornerX, GridSquareUpperLeftCornerY)
    }
    
//    // given a grid and x, y func returns a CGPoint to place sprite
//    func sendToGridSquare (grid: Grid, squareX: Int, squareY: Int) -> (CGPoint) {
//        let tilePositionX = grid.gridUpperLeftX + 23.75 + ( Double(squareX) * grid.gridSquareSizeX )
//        let tilePositionY = 768 - grid.gridUpperLeftY - ( Double(squareY) * grid.gridSquareSizeY ) + 7
//        let tilePosition = CGPoint( x: tilePositionX, y: tilePositionY )
//        return tilePosition
//    }
    
    
    // given a grid and x, y func returns a CGPoint to place sprite
    class func sendToGridSquare (grid: Grid, squareX: Int, squareY: Int) -> (CGPoint) {
        let tilePositionX = grid.gridUpperLeftX + Double((screenSize!).width * 0.0232) + ( Double(squareX) * grid.gridSquareSizeX )
        //let tilePositionX = grid.gridUpperLeftX + 23.75 + ( Double(squareX) * grid.gridSquareSizeX )
        let tilePositionY = Double((screenSize!).height) - grid.gridUpperLeftY - ( Double(squareY) * grid.gridSquareSizeY ) + (Double((screenSize!).height) * 0.009)
        let tilePosition = CGPoint( x: tilePositionX, y: tilePositionY )
        return tilePosition
    }
    
    
    // given a grid and x, y func returns a CGPoint to place sprite
    func sendToGridSquare (squareX: Int, squareY: Int) -> (CGPoint) {
        let tilePositionX = self.gridUpperLeftX + Double((screenSize!).width * 0.0232) + ( Double(squareX) * self.gridSquareSizeX )
        let tilePositionY = Double((screenSize!).height) - self.gridUpperLeftY - ( Double(squareY) * self.gridSquareSizeY ) + (Double((screenSize!).height) * 0.009)
        let tilePosition = CGPoint( x: tilePositionX, y: tilePositionY )
        return tilePosition
    }
    
//    func addToGrid2DArray (tileToAdd: MMWTile, xGrid: Int, yGrid: Int) {
//        grid2DArr[xGrid].insert(tileToAdd, atIndex: [yGrid][xGrid])  // (tileToAdd, atIndex: xGrid)
//    }
    
    
    class func tileAtGridSquare (grid: Grid, squareX: Int, squareY: Int) -> (MMWTile?) {
        let tile : MMWTile? = (grid.grid2DArr[squareY][squareX] as MMWTile)
        return tile
    }
    
    
    func tileAtGridSquare (squareX: Int, squareY: Int) -> (MMWTile?) {
        let tile : MMWTile? = (self.grid2DArr[squareY][squareX] as MMWTile)
        return tile
    }
 
    
    func dealGridFromArrayRandom (inout arrayIn: [MMWTile], var numTilesToDeal: Int, playerNum: Int, clearGrid: Bool) {
        if debugMode == true { print("<Grid> fillGridFromArray(arrayIn: [MMWTile], gridToFill: Grid) \(self.gridName) ") }

        for y in 0...(self.gridNumSquaresY - 1) {   // fill letter tiles
            for x in 0...(self.gridNumSquaresX - 1) {
                
//                if clearGrid == true {
//                    tileCollection!.mmwDiscardedTileArray.append(self.grid2DArr[x][y])
//                    self.grid2DArr[x][y].tileSprite.hidden = true
//                }

                
                
                
                if arrayIn.count > 0 {
                    
                    if clearGrid {
                        //if self.grid2DArr[x][y].tileText != "!" {
                            tileCollection!.mmwDiscardedTileArray.append(self.grid2DArr[x][y])
                            self.grid2DArr[x][y].tileSprite.hidden = true
                            self.grid2DArr[x][y] = (tileCollection?.mmwTileArrayPlaceholder.removeFirst())!
                        //}
                    }

                    if numTilesToDeal > 0  && self.grid2DArr[x][y].tileText == "!"  {
                        
                        tileCollection!.mmwDiscardedTileArray.append(self.grid2DArr[x][y])
                        self.grid2DArr[x][y].tileSprite.hidden = true
 
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
                        
                        dealtTile.gridXTest = x
                        dealtTile.gridYTest = y
                        dealtTile.gridTest = self
                        
                        dealtTile.tileSprite.isMovable = true
                        dealtTile.tileSprite.userInteractionEnabled = false
                    }
                    numTilesToDeal--
                }
                else {
                    if debugMode == true { print("No More Tiles To Deal ... ") }
                }
            }
        }
    }
    
    
    /// func convert2DGridToArray (tileGrid : [[MMWTile]] ) () -> [MMWTile]
    /// converts a 2D player tile grid to a MMWTIle array
    func convert2DGridToArray (tileGrid : [[MMWTile]]) -> [MMWTile]? {
        var tileArr = [MMWTile]()
        for tileRow in tileGrid {
            for tile in tileRow {
                tileArr.append(tile)
            }
        }
        return tileArr
    }
    
    
    func dealGridFromArraySpecificTile (inout arrayIn: [MMWTile], tileArrayLocation: Int, playerNum: Int, squareX: Int, squareY: Int) {
        if debugMode == true { print("<Grid> refillGridFromArrayTile (inout arrayIn: [MMWTile], var tileArrayLocation: Int, playerNum: Int) \(self.gridName) ") }
        //var arrayInMarker = 0
//        for y in 0...(self.gridNumSquaresY - 1) {   // fill letter tiles
//            for x in 0...(self.gridNumSquaresX - 1) {
//                mmwGameScene.mmwGameSceneViewController.tileCollection.mmwDiscardedTileArray.append(self.grid2DArr[x][y])
//                self.grid2DArr[x][y].tileSprite.hidden = true
        if arrayIn.count > 0 {
                let dealtTile : MMWTile = arrayIn[Int(tileArrayLocation)]
                
                dealtTile.tileOwner = TileOwner(rawValue: playerNum)!
                dealtTile.tileState = TileState(rawValue: playerNum)!
                
                self.grid2DArr[squareX][squareY] = dealtTile
                arrayIn.removeAtIndex( Int(tileArrayLocation) )
                dealtTile.tileSprite.color =  gameColors[playerNum]
            
                dealtTile.gridX = squareX
                dealtTile.gridY = squareY
                dealtTile.gridHome = self
        }
        else {
            if debugMode == true { print("No More Tiles To Deal ... ") }
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
    
    
    
//    func checkAllNearLockedTiles (xGrid: Int, yGrid: Int) -> Bool{
//        var isLocked: Bool = false
//        if grid2DArr[xGrid+1][yGrid].tileState == TileState.Locked {
//            isLocked = true
//        }
//        return isLocked
//    }
    
    
    
    func printGrid () {
        if debugMode == true { print("<Grid> printGrid \(self.gridName) ") }
        //print(">>>mmwGameSceneViewController.mmwGameScene.mmwPlayer1Grid: ")
        for y in 0...(self.gridNumSquaresY - 1) {   // fill letter tiles
            for x in 0...(self.gridNumSquaresX - 1) {
                let gridArr = self.grid2DArr[x][y]
                if debugMode == true { print(" \( gridArr.tileSprite.tileText) -> [\(gridArr.tileSprite.tileSpriteParent.gridX)] [\(gridArr.tileSprite.tileSpriteParent.gridY)] -> \(gridArr.tileSprite.tileSpriteParent.tileOwner) ->  \(gridArr.tileSprite.tileSpriteParent.tileState) -> \(gridArr.tileSprite.tileSpriteParent.tileType) ") }
            }
        }
    }


    class func printGrid (gridToPrint: Grid ) {
        if debugMode == true { print("<Grid> printGrid \(gridToPrint.gridName) ") }
        for x in 0...(gridToPrint.gridNumSquaresX - 1) {   // fill letter tiles
            for y in 0...(gridToPrint.gridNumSquaresY - 1) {
                let gridArr = gridToPrint.grid2DArr[x][y]
                if debugMode == true { print(" \( gridArr.tileSprite.tileText) -> [\(gridArr.tileSprite.tileSpriteParent.gridX)] [\(gridArr.tileSprite.tileSpriteParent.gridY)] [\(gridArr.tileSprite.tileSpriteParent.gridHome)] state:[\(gridArr.tileSprite.tileSpriteParent.tileState)] owner:[\(gridArr.tileSprite.tileSpriteParent.tileOwner)] type:[\(gridArr.tileSprite.tileSpriteParent.tileType)]") }
            }
        }
    }
    

    func stringFromGridLetters () -> String {
        var stringFromGrid2DArr = ""
        for x in 0...(self.gridNumSquaresX - 1) {   // fill letter tiles
            for y in 0...(self.gridNumSquaresY - 1) {
                stringFromGrid2DArr = stringFromGrid2DArr.stringByAppendingString(self.grid2DArr[x][y].tileText)
            }
        }
        return stringFromGrid2DArr
    }
    
    
    func getTileWithLetter(letterToFind : String) -> MMWTile? {
        var tileToReturn = MMWTile()
        for tileRow in self.grid2DArr {
            for tile in tileRow {
                if tile.tileText == letterToFind && (tile.tileState != TileState.PlayedMadeWord) && (tile.tileState != TileState.Played) {
                    //tile.tileState = TileState.Played
                    tileToReturn = tile
                    
                }
            }
            
        }
        return tileToReturn
    }
    
    deinit {
        print("grid is being deinitialized")
    }
}