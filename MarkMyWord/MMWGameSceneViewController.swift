//
//  GameViewController.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/10/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import Foundation
import UIKit
import SpriteKit
import CoreMotion

extension StreamReader : SequenceType {
    func generate() -> AnyGenerator<String> {
        return anyGenerator {
            return self.nextLine()
        }
    }
}

class MMWGameSceneViewController {
    
    var mmwGameScene: MMWGameScene!
    var viewSize : CGSize!
    var tileCollection : MMWTileBuilder = MMWTileBuilder()
    var tilesPlayable : [MMWTile]
    var numPlayers : Int = 3
    var playerTurn :  Int = 1
    
    var player1 : Player = Player(_playerID: 1, _playerName: "Abe", _playerColor: 1)
    var player2 : Player = Player(_playerID: 2, _playerName: "Bart", _playerColor: 2)
    // player 3 and 4 objects created but only used in 3 or 4 player games
    var player3 : Player = Player(_playerID: 3, _playerName: "Charlie", _playerColor: 3)
    var player4 : Player = Player(_playerID: 4, _playerName: "Dan", _playerColor: 4)
    
    let playerArray : [Player]! // array of all players 0-3 for easier iteration of player turns

    init (size: CGSize) {
        viewSize = size
        mmwGameScene = MMWGameScene(size: viewSize)
        playerArray  = [player1, player2, player3, player4]
        tilesPlayable = tileCollection.mmwTileArray
        mmwGameScene.setViewController(self)
        mmwGameScene.setGrids() // sets tile grid positions, size of square, number of squares and position on screen for each grid possible
        mmwGameScene.buildGameView()
        setUpPlayers() // add player to view, match player to grid, fill grid with starter tiles and colorize to player color
    }
 
    func setUpPlayers () {
        mmwGameScene.setViewController(self)
        if numPlayers == 2 {
            makeTwoPlayers()
        }
        if numPlayers == 3 {
            makeThreePlayers()
        }  
        if numPlayers == 4 {
            makeFourPlayers()
        }
    }
    
    func makeTwoPlayers () {
        player1.setPlayerTilesGrid(&mmwGameScene.mmwPlayer1Grid!)
        tileCollection.fillGridWithBlankTiles(&mmwGameScene.mmwPlayer1Grid!)
        player1.setPlayerView(mmwGameScene.player1View)
        
        player2.setPlayerTilesGrid(&mmwGameScene.mmwPlayer2Grid!)
        tileCollection.fillGridWithBlankTiles(&mmwGameScene.mmwPlayer2Grid!)
        player2.setPlayerView(mmwGameScene.player2View)
        
        tileCollection.fillGridWithBlankTiles(&mmwGameScene.mmwBoardGrid!)
    }
    
    func makeThreePlayers () {
        makeTwoPlayers()
        
        player3.setPlayerTilesGrid(&mmwGameScene.mmwPlayer3Grid!)
        tileCollection.fillGridWithBlankTiles(&mmwGameScene.mmwPlayer3Grid!)
        player3.setPlayerView(mmwGameScene.player3View)
    }
    
    func makeFourPlayers () {
        makeThreePlayers()

        player4.setPlayerTilesGrid(&mmwGameScene.mmwPlayer4Grid!)
        tileCollection.fillGridWithBlankTiles(&mmwGameScene.mmwPlayer4Grid!)
        player4.setPlayerView(mmwGameScene.player4View)
    }
    
    func setTileOwner (inout tileToSet: MMWTile, player: Player) {
        if player.playerID == 1 {
            tileToSet.tileOwner = TileOwner.Player1
        }
        else if player.playerID == 2 {
            tileToSet.tileOwner = TileOwner.Player2
        }
        else if player.playerID == 3 {
            tileToSet.tileOwner = TileOwner.Player3
        }
        else if player.playerID == 4 {
            tileToSet.tileOwner = TileOwner.Player4
        }
        else {
            tileToSet.tileOwner = TileOwner.None
        }
    }
    
    func getRandomWord() -> String {
        let wordToReturn : String
        if let aStreamReader = StreamReader(file: "5-LetterWords") { // "/Users/erichook/Desktop/testSmallUTF8.txt") {
            var numLines = 0
            while let line = aStreamReader.nextLine() {
                print(line)
                //                            if line == "be\r" {
                //                                break
                //                            }
                ++numLines
            }
            print("Number of Lines in Word List: " + String(numLines) )
            
            let randomWordNum : Int = Int(arc4random()) % (numLines - 1)
            
            print("Random Word Line Number: " + String(randomWordNum) )
            
            var lineNumber = 0
            
            aStreamReader.rewind()  // goback and get word selected on random line number
            
            while let line = aStreamReader.nextLine() {
                if lineNumber == randomWordNum {
                    wordToReturn = String(line)
                    print("Random getFirstWord () Word: " + wordToReturn)
                    
                    return String(wordToReturn)
                    //break
                }
                ++lineNumber
            }
            // You can close the underlying file explicitly. Otherwise it will be
            // closed when the reader is deallocated.
            aStreamReader.close()
            print("Final numLines getFirstWord (): " + String(numLines) )
        }
        return String("Random getFirstWord () Word: XYZ")
    }

    
    func getWordLetters () {
        
    }
    
    func checkUndealtTilesForWord (wordToCheck: String, inout letterTileArray: [MMWTile]) -> [MMWTile]? {
        let string = wordToCheck // "dryad" //wordToCheck // can place test "STRING" in this value for testing purposed
        var lettersToPlay = [Int]()
        var tilesToBoard = [MMWTile]()
        let wordToCheckArr = Array(string.characters)
        var wordToCheckArrCount = wordToCheckArr.count

        var foundLetterInPass = false
        var tileArrayNumber = 0
        var lettersToPlayCount = 0
        for letter in wordToCheckArr {
             for tile in letterTileArray {
                if String(letter).uppercaseString == "\r" {
                    print ("String(letter).uppercaseString == \r ...return char at \(letter)")
                    --wordToCheckArrCount
                    break
                }  // return char exits loop
                if String(letter).uppercaseString == tile.letterString.uppercaseString && !lettersToPlay.contains(tileArrayNumber){
                        print("Found letter: \(letter) in tiles \(tile.tileSprite.tileText)")
                        lettersToPlay.append(tileArrayNumber)
                        ++lettersToPlayCount
                        print("LettersToPlay.count: \(lettersToPlay.count), lettersToPlayCount: \(lettersToPlayCount)")
                        foundLetterInPass = true
                        break
                }
                tileArrayNumber++
                foundLetterInPass = false
            }
            if String(letter).uppercaseString == "\r" {break}  // return char exits loop
            if foundLetterInPass == false {
                print("Tile \(String(letter).uppercaseString) doesn't exist to create word")
                break
            }
            tileArrayNumber = 0
        }
        
        print("Letters to Play from letterTileArray: \(lettersToPlay)")
        print("LettersToPlay.count: \(lettersToPlay.count), wordToCheckArr.count: \(wordToCheckArrCount)")
        
        if lettersToPlay.count < wordToCheckArrCount{
            print("FAIL!")
            return nil
        }
        print("PASS! Found \(string)! self.tileCollection.mmwTileArray.count: \(letterTileArray.count)" )
        
        ////////////////////////////////////////////
        for arrNum in lettersToPlay {
            tilesToBoard.append(letterTileArray[arrNum])
            //letterTileArray.removeAtIndex(arrNum)
        }
        ////////////////////////////////////////////
        tileArrayNumber = 0
        for letter in wordToCheckArr {
            for tile in letterTileArray {
                if String(letter).uppercaseString == tile.letterString.uppercaseString {
                    print("REMOVE letter: \(letter) in tiles \(tile.tileSprite.tileText)")
                    letterTileArray.removeAtIndex(tileArrayNumber)
                    break
                }
                tileArrayNumber++
            }
            if String(letter).uppercaseString == "\r" {break}  // return char exits loop
            tileArrayNumber = 0
        }
        print("After remove tiles: letterTileArray.count: \(letterTileArray.count)" )
            return tilesToBoard
    }

    func dealLetter (inout letterToPlace: MMWTile, gridToPlaceLetter: Grid, xSquare: Int, ySquare: Int) {
        
        let tileAtDropSpot : MMWTile = (gridToPlaceLetter.grid2DArr[xSquare][ySquare])
        ////////////  TEST FOR TILE UNDER DROP SPOT
        if tileAtDropSpot.tileOwner == TileOwner.Player1 {
            //runAction(actionSound)
            letterToPlace.tileSprite.position = (tileAtDropSpot.gridHome?.sendToGridSquare(letterToPlace.gridHome!, squareX: letterToPlace.gridX , squareY: letterToPlace.gridY ))!
        }
        print("drop location info: state:\(tileAtDropSpot.tileOwner) letter:\(tileAtDropSpot.tileSprite.tileText)")
        
        letterToPlace.gridEnd = gridToPlaceLetter // set tileSprite parent (MMWTile) grid to grid snapped to
        letterToPlace.gridXEnd = xSquare
        letterToPlace.gridYEnd = ySquare
        letterToPlace.tileState = TileState.Played  // if put on valid board location set TileState to played
        
        // set basic placeholder tile settings to fit in void in grid - home grid and x and y values
        let replacementPlaceholderTile : MMWTile = MMWTile()
        replacementPlaceholderTile.gridHome = letterToPlace.gridHome
        replacementPlaceholderTile.gridX = letterToPlace.gridX
        replacementPlaceholderTile.gridY = letterToPlace.gridY
        letterToPlace.gridHome?.grid2DArr[letterToPlace.gridX][letterToPlace.gridY] = replacementPlaceholderTile
        
        // set value of snap results grid location to the MMWTile if valid location
        letterToPlace.gridHome? = letterToPlace.gridEnd!
        letterToPlace.gridHome?.grid2DArr[xSquare][ySquare] = letterToPlace
        letterToPlace.gridX = xSquare
        letterToPlace.gridY = ySquare
        // move tile to snap point
        //let tileSnapResults = gameGrid.getGridSquare(Float(xSquare), locY: Float(ySquare))
        let tileLocation = gridToPlaceLetter.sendToGridSquare(gridToPlaceLetter, squareX: xSquare, squareY: ySquare)
        let tileLocX = tileLocation.x
        let tileLocY = tileLocation.y
        letterToPlace.tileSprite.position.x = tileLocX
        letterToPlace.tileSprite.position.y = tileLocY
    }
    
    func placeWord (player: Player, startLocX: Int, startLocY: Int, direction: Character, wordToPlace: [MMWTile]){
        var tileToPlace = self.tilesPlayable[0]
        tileToPlace.tileSprite.color = tileToPlace.tileSprite.TileColors[player.playerID]
        tileToPlace.tileSprite.tileLocation = CGPoint(x: 200, y: 200)
        tileToPlace.gridHome = mmwGameScene.mmwBoardGrid
        tileToPlace.gridEnd = mmwGameScene.mmwBoardGrid
        tileToPlace.tileGrid = mmwGameScene.mmwBoardGrid
        tileToPlace.gridX = startLocX
        tileToPlace.gridXEnd = startLocX
        tileToPlace.gridY = startLocY
        tileToPlace.gridYEnd = startLocY
        
        setTileOwner(&tileToPlace, player: player)
        //tileToPlace.gridHome?.sendToGridSquare(tileToPlace.gridX, squareY: tileToPlace.gridY)
        tileToPlace.tileSprite.tileLocation = tileToPlace.gridHome!.sendToGridSquare(tileToPlace.gridX, squareY: tileToPlace.gridY)
        tileToPlace.tileState = TileState.Played
        tileToPlace.gridHome?.grid2DArr[tileToPlace.gridX][tileToPlace.gridY] = tileToPlace
        tileToPlace.tileSprite.zPosition = 11
        tileToPlace.tileSprite.hidden = false
    }
    
    func sendWordToBoard (inout letterTileArray: [MMWTile], gridToDisplay: Grid, xStartSquare: Int, yStartSquare: Int, IsHorizonal: Bool, player: Player) {
        //let wordToDisplayArray = Array(wordToDisplay.characters)
        var xLoc: Int = xStartSquare
        var yLoc: Int = yStartSquare
        for var tileInWord in letterTileArray {
            if tileInWord.letterString == "\r" {break}
            print("Letter tileInWord \(tileInWord) is at x:\(xLoc) y:\(yLoc)")
            setTileOwner(&tileInWord, player: player)
            tileInWord.gridHome = mmwGameScene.mmwBoardGrid
            tileInWord.tileGrid = mmwGameScene.mmwBoardGrid
            tileInWord.gridX = xLoc
            tileInWord.gridXEnd = xLoc
            tileInWord.gridY = yLoc
            tileInWord.gridYEnd = yLoc
            tileInWord.gridHome?.grid2DArr[xLoc][yLoc] = tileInWord
            tileInWord.tileState = TileState.Played
            tileInWord.tileSprite.zPosition = 11
            tileInWord.tileSprite.hidden = false
            
            if IsHorizonal { ++xLoc }
            else if !IsHorizonal { ++yLoc }
        }
        
        mmwGameScene.updateGridInScene(mmwGameScene.mmwBoardGrid)
        mmwGameScene.updateGridInScene(player.playerLetterGrid)
    }
    
    
//    func getWord (wordSize: Int = 5, letterDrawInput: [MMWTile] = tilesPlayable) {
//
//    }
    
//    func makeTwoPlayers () {
//        mmwGameScene.addPlayerView(1, playerView: PlayerView(mmwPlayer: player1))
//        player1.setPlayerTilesGrid(&mmwGameScene.mmwPlayer1Grid!)
//        tileCollection.fillGridWithBlankTiles(&mmwGameScene.mmwPlayer1Grid!, tilesFrom: &tileCollection.mmwTileArray, changeColorTo: 1)
//        mmwGameScene.addPlayerView(2, playerView: PlayerView(mmwPlayer: player2))
//        player2.setPlayerTilesGrid(&mmwGameScene.mmwPlayer2Grid!)
//        tileCollection.fillGridWithBlankTiles(&mmwGameScene.mmwPlayer2Grid!, tilesFrom: &tileCollection.mmwTileArray, changeColorTo: 2)
//        tileCollection.fillGridWithBlankTiles(&mmwGameScene.mmwBoardGrid!, tilesFrom: &tileCollection.mmwTileArray, changeColorTo: 0)
//    }
//    func makeThreePlayers () {
//        makeTwoPlayers()
//        mmwGameScene.addPlayerView(3, playerView: PlayerView(mmwPlayer: player3))
//        player3.setPlayerTilesGrid(&mmwGameScene.mmwPlayer3Grid!)
//        tileCollection.fillGridWithBlankTiles(&mmwGameScene.mmwPlayer3Grid!, tilesFrom: &tileCollection.mmwTileArray, changeColorTo: 3)
//    }
//    func makeFourPlayers () {
//        makeThreePlayers()
//        mmwGameScene.addPlayerView(4, playerView: PlayerView(mmwPlayer: player4))
//        player4.setPlayerTilesGrid(&mmwGameScene.mmwPlayer4Grid!)
//        tileCollection.fillGridWithBlankTiles(&mmwGameScene.mmwPlayer4Grid!, tilesFrom: &tileCollection.mmwTileArray, changeColorTo: 4)
//    }

//    func presentMMWScene() {
//
//        let transition = SKTransition.crossFadeWithDuration(2.0)
//        let MMWScene = MMWGameScene(size: view.bounds.size)
//        MMWScene.name = "MMWScene instance name made in GameScene"
//        //var currentScene = MMWScene
//        mmwGameScene.presentMenuScene()
//        //scene.presentScene(MMWScene, transition: transition)
//        print("presentMMWScene")
//        MMWScene.player1View?.playerNameLabel.text = "Fred"
//        
//        //MMWScene.player1View?.playerNameLabel.text = MMWScene.player1!.playerName
//        //MMWScene.changePlayerScore(MMWScene.player1View!, player: MMWScene.player1!, score: 22)
//        MMWScene.player1View?.playerScoreLabel.text = String(33)
//    }
//    
//    func presentMMWScene() {
//        
//        print("in GameViewController presentMMWScene 1")
//        let mmwScene = MMWGameScene(size: view.bounds.size)
//        
//        // Configure the view.
//        let skView = self.view as! SKView
//        skView.showsFPS = true
//        skView.showsNodeCount = true
//        
//        /* Sprite Kit applies additional optimizations to improve rendering performance */
//        skView.ignoresSiblingOrder = true
//        
//        /* Set the scale mode to scale to fit the window */
//        mmwScene.scaleMode = .AspectFill
//        
//        skView.presentScene(mmwScene)
//        print("in GameViewController presentMMWScene 2")
//        
//        //MMWGameSceneViewController()
//        //presentMMWScene()
//        print("in GameViewController presentMMWScene 3")
//        
//        mmwScene.changePlayerScore(mmwScene.player2View!, player: mmwScene.player2!, score: 2468)
//    }

    //func fillTiles(inout tilesFrom: [MMWTile], inout tilesTo: [MMWTile], changeColorTo: UIColor) {
//        let originalTilesGet = numTilesGet
//        while numTilesGet > 0 {
//            let numTiles : UInt32 = UInt32(tilesFrom.count - 1)
//            let tileInArr = arc4random_uniform( numTiles ) // select random tile in FROM array
//            let tileRemoved : MMWTile = tilesFrom.removeAtIndex( Int(tileInArr) )
//            tileRemoved.tileSprite.color = changeColorTo
//            tilesTo.insert(tileRemoved, atIndex: originalTilesGet - numTilesGet)
//            //tilesTo.append( tileRemoved )
//            numTilesGet--
//        }
//    }

//    override func didReceiveMemoryWarning() {
//        super.didReceiveMemoryWarning()
//        // Release any cached data, images, etc that aren't in use.
//    }
}




