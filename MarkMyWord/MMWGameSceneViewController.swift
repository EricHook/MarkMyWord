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
    var tileCollection : MMWTileBuilder
    var tilesPlayable : [MMWTile]
    var numPlayers : Int = 4
    var playerTurn :  Int = 1
    var minWordSize = 3
    var secondsPerTurn = 30
    var player0 : Player = Player(_playerID: 0, _playerName: "AI", _playerColor: 0) // used to add initial word ownership
    var player1 : Player = Player(_playerID: 1, _playerName: "Abe", _playerColor: 1)
    var player2 : Player = Player(_playerID: 2, _playerName: "Bart", _playerColor: 2)
    // player 3 and 4 objects created but only used in 3 or 4 player games
    var player3 : Player = Player(_playerID: 3, _playerName: "Charlie", _playerColor: 3)
    var player4 : Player = Player(_playerID: 4, _playerName: "Dan", _playerColor: 4)
    var playerArray : [Player]! // array of all players 0-3 for easier iteration of player turns
    var wordsLoaded4 = false
    var wordsLoaded5 = false
    var wordsLoaded6 = false
    var wordsLoaded7 = false
    var wordsLoaded8 = false
    var wordsLoaded9 = false
    var wordsLoaded10Plus = false

    var wordArray : [String] = [""]
    var wordArrayMod : [String.CharacterView] = ["".characters]
    
    var consequtivePasses = 0
    var lettersPlayedInTurn = 0

    var wordTrie : Trie<Character>?

    init (size: CGSize) {
        viewSize = size
        tileCollection = MMWTileBuilder()
        mmwGameScene = MMWGameScene(size: viewSize)
 
        if numPlayers == 2 {
            playerArray  = [player1, player2]
        }
        if numPlayers == 3 {
            playerArray  = [player1, player2, player3]
        }
        if numPlayers == 4 {
            playerArray  = [player1, player2, player3, player4]
        }
        
        tilesPlayable = tileCollection.mmwTileArray
        mmwGameScene.setViewController(self)
        mmwGameScene.setGrids() // sets tile grid positions, size of square, number of squares and position on screen for each grid possible
        mmwGameScene.buildGameView()
        setUpPlayers() // add player to view, match player to grid, fill grid with starter tiles and colorize to player color
        
        tileCollection.mmwGameSceneViewController = self
        tileCollection.mmwGameScene = self.mmwGameScene
        tileCollection.setViewControllerAndScene(self)
        
        
        loadWords()  //  send off threads to get sections of word list to build array and trei

//        buildWordArray("WordList1to3LetterNoDup")
//        buildTrie( buildWordArray("WordList1to3LetterNoDup") )
//        print("buildTrie() 1-3")
//        
//        buildWordArray("WordList4LetterNoDup")
//        insertTrie( buildWordArray("WordList4LetterNoDup") )
//        print("insertTrie() 4")
//        
//        buildWordArray("WordList5LetterNoDup")
//        insertTrie( buildWordArray("WordList5LetterNoDup") )
//        print("insertTrie() 5")
//        
////        buildWordArray("WordList6LetterNoDup")
////        insertTrie( buildWordArray("WordList6LetterNoDup") )
////        print("insertTrie() 6")

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
        player1.setPlayerView(mmwGameScene.player1View, mmwGameScene: self.mmwGameScene, mmwGameSceneViewController: self)
        
        player2.setPlayerTilesGrid(&mmwGameScene.mmwPlayer2Grid!)
        tileCollection.fillGridWithBlankTiles(&mmwGameScene.mmwPlayer2Grid!)
        player2.setPlayerView(mmwGameScene.player2View, mmwGameScene: self.mmwGameScene, mmwGameSceneViewController: self)
        
        tileCollection.fillGridWithBlankTiles(&mmwGameScene.mmwBoardGrid!)
    }
    
    func makeThreePlayers () {
        makeTwoPlayers()
        
        player3.setPlayerTilesGrid(&mmwGameScene.mmwPlayer3Grid!)
        tileCollection.fillGridWithBlankTiles(&mmwGameScene.mmwPlayer3Grid!)
        player3.setPlayerView(mmwGameScene.player3View, mmwGameScene: self.mmwGameScene, mmwGameSceneViewController: self)
    }
    
    func makeFourPlayers () {
        makeThreePlayers()

        player4.setPlayerTilesGrid(&mmwGameScene.mmwPlayer4Grid!)
        tileCollection.fillGridWithBlankTiles(&mmwGameScene.mmwPlayer4Grid!)
        player4.setPlayerView(mmwGameScene.player4View, mmwGameScene: self.mmwGameScene, mmwGameSceneViewController: self)
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
    
    func resetConsequtivePasses () {
        self.consequtivePasses = 0
    }
    
    func resetLettersPlayedInTurn () {
        self.lettersPlayedInTurn = 0
    }

    func getRandomWord() -> String {
        let wordToReturn : String
        if let aStreamReader = StreamReader(file: "5-LetterWords") { // "/Users/erichook/Desktop/testSmallUTF8.txt") {
            var numLines = 0
            while (aStreamReader.nextLine() != nil) {
                //print(line)
                ++numLines
            }
            
            let randomWordNum = Int(arc4random_uniform((UInt32(numLines-1))) )
            
            var lineNumber = 0
            
            aStreamReader.rewind()  // goback and get word selected on random line number
            
            while let line = aStreamReader.nextLine() {
                if lineNumber == randomWordNum {
                    wordToReturn = String(line)
                    //print("Random getFirstWord () Word: " + wordToReturn)
                    return String(wordToReturn)
                }
                ++lineNumber
            }
            // You can close the underlying file explicitly. Otherwise it will be
            // closed when the reader is deallocated.
            aStreamReader.close()
            //print("Final numLines getFirstWord (): " + String(numLines) )
        }
        return String("error - didn't find word in getRandomWord()")
    }
    
    func buildWordArray(wordList : String) -> [String] {
        // read the file
        wordArray = [""]  // clear wordArray so can add new chuncks of wordList
        
        if let aStreamReader = StreamReader(file: wordList) { // "/Users/erichook/Desktop/testSmallUTF8.txt") {
            
            while let line = aStreamReader.nextLine() {
                wordArray.append(line)
            }
            aStreamReader.close()
            print("FINISHED buildWordArray() with \(wordList)")
        }
        return wordArray
    }
    
    func buildTrie(stringArray: [String]) {
        wordArrayMod = stringArray.map{$0.characters}
        wordTrie = Trie(wordArrayMod)
    }
    
    func insertTrie(stringArray: [String]) {
        wordArrayMod = stringArray.map{$0.characters}
        print("InsertTrie store.count START: + \(wordTrie!.count) ")
//        print("InsertTrie store.contains jo: + \(wordTrie!.completions("jo".characters).count)")
//        let stringTest = "jox"
//        wordTrie!.insert(stringTest.characters) //    (wordArrayMod.map{$0.characters})
        for word in wordArray {
            wordTrie!.insert(word.characters)
        }
  
        print("FINISHED InsertTrie()")
    }

    /// func stringFromTileArr (tileArr: [MMWTile]) -> String
    /// given an array of tiles, get letter associated and return string
    func stringFromTileArr (tileArr: [MMWTile]) -> String {
        var stringFromArr = ""
        for tile in tileArr {
            stringFromArr = stringFromArr.stringByAppendingString( String(tile.tileSprite.letterLabel) )
        }
        return stringFromArr
    }
    
    /// checkPartialWordMatch(var wordToCheck: String) -> Bool
    /// -Parameters: var wordToCheck: String (var because converts to lower case for check)
    /// -Returns: Bool
    /// given a string, convert to lowercase, check for partial word in Trei (e.g. exists without trailing '!' character)
    func checkPartialWordMatch(var wordToCheck: String) -> Bool {
        wordToCheck = wordToCheck.lowercaseString
        if wordTrie!.contains("\(wordToCheck)".characters){
            return true
        }
        return false
    }
    
    /// checkPartialWordMatch(var wordToCheck: String) -> Bool
    /// given a string, convert to lowercase, check for whole word in Trei (e.g. exists withtrailing '!' character)
    func checkWholeWordMatch(var wordToCheck: String) -> Bool {
        wordToCheck = wordToCheck.lowercaseString
        if wordTrie!.contains("\(wordToCheck)!".characters){ // whole words have ! at end of string
            return true
        }
        return false
    }
    
    func AIPlayTiles (player: Player) {
        for tileColumn in self.tileCollection.mmwBoardTile2DArray! {
            for tile in tileColumn {
                print(" \(tile.tileOwner.hashValue) ")
            }
        }
    }

    
//    func checkWholeWordMatch(wordToCheck: String) -> Bool {
//        if let aStreamReader = StreamReader(file: "enable1WordList") { // "/Users/erichook/Desktop/enable1WordList.txt") {
//            var numLines = 0
//            while var line = aStreamReader.nextLine() {
//                line = line.stringByReplacingOccurrencesOfString("\r", withString: "") // remove /r character at end of line
//                if wordToCheck.caseInsensitiveCompare(line) == NSComparisonResult.OrderedSame && wordToCheck.characters.count >= self.minWordSize  {
//                    print("MATCH ... \(line) >> +3 points for playerArray[ \(playerArray[playerTurn - 1]) ]") // !!! NEED TO LOCK LETTERS HERE ??? currently lock in LetterTileSprite
//                    
//                    // 3 points for making a new complete word
//                    //playerArray[playerTurn - 1].playerScore += 3
//                    //player1.playerScore += 10
//
//                    return true
//                    
//                }
//                ++numLines
//            }
//       }
//        return false
//    }
    
//    func checkPartialWordMatch(wordToCheck: String) -> Bool {
//        //let wordToReturn : String
//        if let aStreamReader = StreamReader(file: "3-LetterWords") { // "/Users/erichook/Desktop/enable1WordList.txt") {
//            var numLines = 0
//            while var line = aStreamReader.nextLine() {
//                line = line.stringByReplacingOccurrencesOfString("\r", withString: "") // remove /r character at end of line
//                if wordToCheck.caseInsensitiveCompare(line) == NSComparisonResult.OrderedSame {
//                    print("MATCH ... \(line)") // !!! NEED TO LOCK LETTERS HERE ??? currently lock in LetterTileSprite
//                    return true
//                }
//                ++numLines
//            }
//        }
//        return false
//    }


    
    
    func checkTilesForWord (wordToCheck: String, letterTileArray: [MMWTile]) -> Bool {
        let string = wordToCheck // "dryad" //wordToCheck // can place test "STRING" in this value for testing purposed
        var lettersToPlay = [Int]()
        //var tilesToBoard = [MMWTile]()
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
                if String(letter).uppercaseString == tile.tileText.uppercaseString && !lettersToPlay.contains(tileArrayNumber){
                        print("Found letter: \(letter) in tiles \(tile.tileSprite.tileText)")
                        lettersToPlay.append(tileArrayNumber)
                        ++lettersToPlayCount
                        print( "LettersToPlay.count: \(lettersToPlay.count), lettersToPlayCount: \(lettersToPlayCount)" )
                        foundLetterInPass = true
                        break
                }
                tileArrayNumber++
                foundLetterInPass = false
            }
            if String(letter).uppercaseString == "\r" {break}  // return char exits loop
            if foundLetterInPass == false {
                print( "Tile \(String(letter).uppercaseString) doesn't exist to create word" )
                return false
            }
            tileArrayNumber = 0
        }
        
        print( "Letters to Play from letterTileArray: \(lettersToPlay)" )
        print( "LettersToPlay.count: \(lettersToPlay.count), wordToCheckArr.count: \(wordToCheckArrCount)" )
        
//        if lettersToPlay.count < wordToCheckArrCount{ // letters not availible return nil
//            print("FAIL!")
//            return false
//        }
        print( "PASS! Found \(string)! self.tileCollection.mmwTileArray.count: \(letterTileArray.count)" )
        return true
    }
    
    
    
    func returnTilesForWord (wordToCheck: String, inout letterTileArray: [MMWTile]) -> [MMWTile]? {
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
                if String(letter).uppercaseString == tile.tileText.uppercaseString && !lettersToPlay.contains(tileArrayNumber){
                    print("Found letter: \(letter) in tiles \(tile.tileSprite.tileText)")
                    lettersToPlay.append(tileArrayNumber)
                    ++lettersToPlayCount
                    print( "LettersToPlay.count: \(lettersToPlay.count), lettersToPlayCount: \(lettersToPlayCount)" )
                    foundLetterInPass = true
                    break
                }
                tileArrayNumber++
                foundLetterInPass = false
            }
            if String(letter).uppercaseString == "\r" {break}  // return char exits loop
            if foundLetterInPass == false {
                print( "Tile \(String(letter).uppercaseString) doesn't exist to create word" )
                break
            }
            tileArrayNumber = 0
        }
        
        print( "Letters to Play from letterTileArray: \(lettersToPlay)" )
        print( "LettersToPlay.count: \(lettersToPlay.count), wordToCheckArr.count: \(wordToCheckArrCount)" )
        
        if lettersToPlay.count < wordToCheckArrCount{ // letters not availible return nil
            print("FAIL!")
            return nil
        }
        print( "PASS! Found \(string)! self.tileCollection.mmwTileArray.count: \(letterTileArray.count)" )
        //return true
  
        ////////////////////////////////////////////
        for arrNum in lettersToPlay {
            tilesToBoard.append(letterTileArray[arrNum])
            //letterTileArray.removeAtIndex(arrNum)
        }
        ////////////////////////////////////////////
        
        tileArrayNumber = 0
        for letter in wordToCheckArr {
            for tile in letterTileArray {
                if String(letter).uppercaseString == tile.tileText.uppercaseString {
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

//func checkUndealtTilesForWord (wordToCheck: String, inout letterTileArray: [MMWTile]) -> [MMWTile]? {
//    let string = wordToCheck // "dryad" //wordToCheck // can place test "STRING" in this value for testing purposed
//    var lettersToPlay = [Int]()
//    var tilesToBoard = [MMWTile]()
//    let wordToCheckArr = Array(string.characters)
//    var wordToCheckArrCount = wordToCheckArr.count
//    var foundLetterInPass = false
//    var tileArrayNumber = 0
//    var lettersToPlayCount = 0
//    for letter in wordToCheckArr {
//        for tile in letterTileArray {
//            if String(letter).uppercaseString == "\r" {
//                print ("String(letter).uppercaseString == \r ...return char at \(letter)")
//                --wordToCheckArrCount
//                break
//            }  // return char exits loop
//            if String(letter).uppercaseString == tile.tileText.uppercaseString && !lettersToPlay.contains(tileArrayNumber){
//                print("Found letter: \(letter) in tiles \(tile.tileSprite.tileText)")
//                lettersToPlay.append(tileArrayNumber)
//                ++lettersToPlayCount
//                print( "LettersToPlay.count: \(lettersToPlay.count), lettersToPlayCount: \(lettersToPlayCount)" )
//                foundLetterInPass = true
//                break
//            }
//            tileArrayNumber++
//            foundLetterInPass = false
//        }
//        if String(letter).uppercaseString == "\r" {break}  // return char exits loop
//        if foundLetterInPass == false {
//            print( "Tile \(String(letter).uppercaseString) doesn't exist to create word" )
//            break
//        }
//        tileArrayNumber = 0
//    }
//    
//    print( "Letters to Play from letterTileArray: \(lettersToPlay)" )
//    print( "LettersToPlay.count: \(lettersToPlay.count), wordToCheckArr.count: \(wordToCheckArrCount)" )
//    
//    if lettersToPlay.count < wordToCheckArrCount{ // letters not availible return nil
//        print("FAIL!")
//        return false
//    }
//    print( "PASS! Found \(string)! self.tileCollection.mmwTileArray.count: \(letterTileArray.count)" )
//    return true
//}
//
//
//////////////////////////////////////////////
//for arrNum in lettersToPlay {
//    tilesToBoard.append(letterTileArray[arrNum])
//    //letterTileArray.removeAtIndex(arrNum)
//}
//////////////////////////////////////////////
//
//tileArrayNumber = 0
//for letter in wordToCheckArr {
//    for tile in letterTileArray {
//        if String(letter).uppercaseString == tile.tileText.uppercaseString {
//            print("REMOVE letter: \(letter) in tiles \(tile.tileSprite.tileText)")
//            letterTileArray.removeAtIndex(tileArrayNumber)
//            break
//        }
//        tileArrayNumber++
//    }
//    if String(letter).uppercaseString == "\r" {break}  // return char exits loop
//    tileArrayNumber = 0
//}
//print("After remove tiles: letterTileArray.count: \(letterTileArray.count)" )
//return tilesToBoard
//}






    func dealLetter (inout letterToPlace: MMWTile, gridToPlaceLetter: Grid, xSquare: Int, ySquare: Int) {
        
        let tileAtDropSpot : MMWTile = (gridToPlaceLetter.grid2DArr[xSquare][ySquare])
        //print("drop location info: state:\(tileAtDropSpot.tileOwner) letter:\(tileAtDropSpot.tileSprite.tileText)")
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
        let tileLocation = Grid.sendToGridSquare(gridToPlaceLetter, squareX: xSquare, squareY: ySquare)
        let tileLocX = tileLocation.x
        let tileLocY = tileLocation.y
        letterToPlace.tileSprite.position.x = tileLocX
        letterToPlace.tileSprite.position.y = tileLocY
    }
    
    
    func dealTestLetters () {
        //dealLetter(&<#T##letterToPlace: MMWTile##MMWTile#>, gridToPlaceLetter: <#T##Grid#>, xSquare: <#T##Int#>, ySquare: <#T##Int#>)
        
        
    }
    
    func placeWord (player: Player, startLocX: Int, startLocY: Int, direction: Character, wordToPlace: [MMWTile]){
        var tileToPlace = self.tilesPlayable[0]
        tileToPlace.tileSprite.color = tileColors[player.playerID]
        tileToPlace.tileSprite.tileLocation = CGPoint(x: 200, y: 200)
        tileToPlace.gridHome = mmwGameScene.mmwBoardGrid
        tileToPlace.gridEnd = mmwGameScene.mmwBoardGrid
        tileToPlace.tileGrid = mmwGameScene.mmwBoardGrid
        tileToPlace.gridX = startLocX
        tileToPlace.gridXEnd = startLocX
        tileToPlace.gridY = startLocY
        tileToPlace.gridYEnd = startLocY
        
        setTileOwner(&tileToPlace, player: player)
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
            if tileInWord.tileText == "\r" {break}
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
            tileInWord.tileSprite.zPosition = 0
            tileInWord.tileState = TileState.Locked
            tileInWord.tileSprite.hidden = false
            
            if IsHorizonal { ++xLoc }
            else { ++yLoc }
        }
        
        mmwGameScene.updateGridInScene(mmwGameScene.mmwBoardGrid)
        if player.playerLetterGrid != nil {
            self.mmwGameScene.updateGridInScene(player.playerLetterGrid)
        } 
    }

    func loadWords() {
        
        let priority = DISPATCH_QUEUE_PRIORITY_DEFAULT
        dispatch_async(dispatch_get_global_queue(priority, 0), { ()->() in

            print("gcd hello")
            print("hello from UI thread executed as dispatch 1")
            print("Currently dispatched asynchronously 0")
            
            self.buildWordArray("WordList1to3LetterNoDup")
            self.buildTrie( self.buildWordArray("WordList1to3LetterNoDup") )
            print("buildTrie() 1-3")
            print("Currently dispatched asynchronously 1-3")
            
            self.buildWordArray("WordList4LetterNoDup")
            self.insertTrie(self.buildWordArray("WordList4LetterNoDup") )
            self.wordsLoaded4 = true
            print("insertTrie() 4")
            print("Currently dispatched asynchronously 4")
            
            self.buildWordArray("WordList5LetterNoDup")
            self.insertTrie(self.buildWordArray("WordList5LetterNoDup") )
            self.wordsLoaded5 = true
            print("insertTrie() 5")
            print("Currently dispatched asynchronously 5")
            print("hello from UI thread executed as dispatch 2")
            
            self.buildWordArray("WordList6LetterNoDup")
            self.insertTrie(self.buildWordArray("WordList6LetterNoDup") )
            self.wordsLoaded4 = true
            print("insertTrie() 6")
            print("Currently dispatched asynchronously 6")
            
            dispatch_async(dispatch_get_main_queue(), {
                print("hello from UI thread executed as dispatch")

            })
        })

        print("hello from UI thread")
        
        
//        dispatch_async(dispatch_get_main_queue()) {
//            print("Currently dispatched asynchronously 0")
//            
//            self.buildWordArray("WordList1to3LetterNoDup")
//            self.buildTrie( self.buildWordArray("WordList1to3LetterNoDup") )
//            print("buildTrie() 1-3")
//            print("Currently dispatched asynchronously 1-3")
//            
//            self.buildWordArray("WordList4LetterNoDup")
//            self.insertTrie(self.buildWordArray("WordList4LetterNoDup") )
//            print("insertTrie() 4")
//            print("Currently dispatched asynchronously 4")
//
//            self.buildWordArray("WordList5LetterNoDup")
//            self.insertTrie(self.buildWordArray("WordList5LetterNoDup") )
//            print("insertTrie() 5")
//            print("Currently dispatched asynchronously 5")
//
//            self.buildWordArray("WordList6LetterNoDup")
//            self.insertTrie(self.buildWordArray("WordList6LetterNoDup") )
//            print("insertTrie() 6")
//            print("Currently dispatched asynchronously 6")

//        }
        
        
        
        
        
        
        
//        //let url = NSURL(string: urlString)
//        let semaphore = dispatch_semaphore_create(0) // 1
//        //let loadWords1to3 = buildWordArray("WordList1to3LetterNoDup")
//        
//        
////        buildWordArray("WordList1to3LetterNoDup")
////        buildTrie( buildWordArray("WordList1to3LetterNoDup") )
////        print("buildTrie() 1-3")
////
////        buildWordArray("WordList4LetterNoDup")
////        insertTrie( buildWordArray("WordList4LetterNoDup") )
////        print("insertTrie() 4")
////
////        buildWordArray("WordList5LetterNoDup")
////        insertTrie( buildWordArray("WordList5LetterNoDup") )
////        print("insertTrie() 5")
////
////        buildWordArray("WordList6LetterNoDup")
////        insertTrie( buildWordArray("WordList6LetterNoDup") )
////        print("insertTrie() 6")
//        
//        
//        
////        if let error = error {
////            XCTFail(" failed. \(error.localizedDescription)")
////        }
////                    image, error in
////                    if let error = error {
////                        XCTFail("\(urlString) failed. \(error.localizedDescription)")
////                    }
//         dispatch_semaphore_signal(semaphore) // 2
//                //}
//        
//        let timeout = dispatch_time(DISPATCH_TIME_NOW, 500)   // DefaultTimeoutLengthInNanoSeconds)
//        if dispatch_semaphore_wait(semaphore, timeout) != 0 { // 3
//            print("semaphore timed out")
//        }
    }


    
    
    
    
//                buildWordArray("WordList1to3LetterNoDup")
//                buildTrie( buildWordArray("WordList1to3LetterNoDup") )
//                print("buildTrie() 1-3")

//                buildWordArray("WordList4LetterNoDup")
//                insertTrie( buildWordArray("WordList4LetterNoDup") )
//                print("insertTrie() 4")
//        
//                buildWordArray("WordList5LetterNoDup")
//                insertTrie( buildWordArray("WordList5LetterNoDup") )
//                print("insertTrie() 5")
    
        //        buildWordArray("WordList6LetterNoDup")
        //        insertTrie( buildWordArray("WordList6LetterNoDup") )
        //        print("insertTrie() 6")
    
        
        
        
        
        
        
        
        
        
        
        
        
        //    override func viewDidLoad() {
        //        super.viewDidLoad()
        //        assert(image != nil, "Image not set; required to use view controller")
        //        photoImageView.image = image
        //
        //        // Resize if neccessary to ensure it's not pixelated
        //        if image.size.height <= photoImageView.bounds.size.height &&
        //            image.size.width <= photoImageView.bounds.size.width {
        //                photoImageView.contentMode = .Center
        //        }
        //
        //        dispatch_async(dispatch_get_global_queue(Int(QOS_CLASS_USER_INITIATED.value), 0)) { // 1
        //            let overlayImage = self.faceOverlayImageFromImage(self.image)
        //            dispatch_async(dispatch_get_main_queue()) { // 2
        //                self.fadeInNewImage(overlayImage) // 3
        //            }
        //        }
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

//    func fillTiles(inout tilesFrom: [MMWTile], inout tilesTo: [MMWTile], changeColorTo: UIColor) {
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




