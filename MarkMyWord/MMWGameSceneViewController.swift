
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

extension StreamReader : SequenceType {
    func generate() -> AnyGenerator<String> {
        return AnyGenerator {
            return self.nextLine()
        }
    }
}


class MMWGameSceneViewController : UIViewController {

    var numGamesWon = 0
    var numGamesTied = 0
    var numGamesLost = 0
    var numWordsMade = 0
    var numLettersPlayed = 0
    
    var viewSize = screenSize
    var tilesPlayable = [MMWTile]()
    var numPlayers   = 3
    var playerTurn   = 1
    var numStarterWords = 2
    var minWordSize  = 3
    var audioOn = true
    var secondsPerTurn = 45
    var timerIsOn = true
    var appPaused = false
    var playerFinishedTurn = false
    var allowOffensiveWords = false
    var backgroundNumber = 0

    var player0 : Player = Player(_playerID: 0, _playerName: "AI", _playerColor: 0) // used for initial word ownership
    var player1 : Player = Player(_playerID: 1, _playerName: "Player 1", _playerColor: 1)
    var player2 : Player = Player(_playerID: 2, _playerName: "Bart",  _playerColor: 2)
    var player3 : Player = Player(_playerID: 3, _playerName: "Charlie", _playerColor: 3)
    var player4 : Player = Player(_playerID: 4, _playerName: "Dan", _playerColor: 4)
    
//    var playerArray : [Player]! //  = [player1, player2, player3, player4]
    var playerArray : [Player]! // = [Player(_playerID: 0, _playerName: "AI", _playerColor: 0)]   // [player1, player2, player3, player4]

    //    var wordArray : [String] = [""]
    //    var wordArrayMod : [String.CharacterView] = ["".characters]
    
    var consecutivePasses = 0
    var lettersPlayedInTurn = 0
 
    var wordSet : WordSet?
    let wordSetPrecomputedSize : Int = 1253231;

    override func viewDidLoad() {
        if debugMode == true { print("---   in viewDidLoad mmwGameSceneViewController") }
        if debugMode == true { print("before MMWGameSceneViewController viewDidLoad() setUpGame() ") }
        setUpGame()
        if debugMode == true { print("after MMWGameSceneViewController viewDidLoad() setUpGame() ") }
        //CDHelper.
        super.viewDidLoad()
    }

//    override init(nibName nibNameOrNil: String?, bundle nibBundleOrNil: NSBundle?) {
//        currentDetailViewController = UIViewController()
//        super.init(nibName: nibNameOrNil, bundle: nibBundleOrNil)
//    }
//    
//    
////    // this is a convenient way to create this view controller without a imageURL
    convenience init() {
        self.init(nibName: nil, bundle: nil)
        self.playerArray = [self.player1, self.player2, self.player3, self.player4]
    }
//
//    
//    required init?(coder aDecoder: NSCoder) {
//        playerArray = [player1, player2, player3, player4]
//        
//        //fatalError("init(coder:) has not been implemented")
//    }
//    
////        init () {
////            super.init()
////            //viewSize = size/Users/erichook/Downloads/VSToolsForWindows1C.exe
////            //timer = NSTimer.scheduledTimerWithTimeInterval(1, target:mmwGameScene, selector: Selector("updateCounter"), userInfo: nil, repeats: true)
////        }
    

    func setUpGame () {
        if debugMode == true { print("---   in setUpGame mmwGameSceneViewController") }
        loadWordSet()
        mmwGameScene.buildGameView()
        setUpPlayers() // add player to view, match player to grid, fill grid with starter tiles and colorize to player color
    }
    
    
    func setUpPlayers () {
        if debugMode == true { print("---   in setUpPlayers mmwGameSceneViewController") }
        player1.setPlayerTilesGrid(&mmwGameScene.mmwPlayer1Grid!)
        player1.setPlayerView(&mmwGameScene.player1View!)
        player2.setPlayerTilesGrid(&mmwGameScene.mmwPlayer2Grid!)
        player2.setPlayerView(&mmwGameScene.player2View!)
        player3.setPlayerTilesGrid(&mmwGameScene.mmwPlayer3Grid!)
        player3.setPlayerView(&mmwGameScene.player3View!)
        player4.setPlayerTilesGrid(&mmwGameScene.mmwPlayer4Grid!)
        player4.setPlayerView(&mmwGameScene.player4View!)
    }
    
    
    func loadWordSet() {
        if debugMode == true { print("---   in loadWordSet mmwGameSceneViewController") }
        
        if mmwGameSceneViewController.allowOffensiveWords == true {
            do {
                if let path = NSBundle.mainBundle().pathForResource("WordListChoppedNoDupRaw", ofType: "txt") {
                    wordSet = WordSet(filePath: path, precomputedSize: wordSetPrecomputedSize)
                    try wordSet?.load()
                }
            } catch {
                // alert message to user here
            }
        }
        else {
            do {
                if let path = NSBundle.mainBundle().pathForResource("WordListChoppedNoDupClean", ofType: "txt") {
                    wordSet = WordSet(filePath: path, precomputedSize: wordSetPrecomputedSize)
                    try wordSet?.load()
                }
            } catch {
                // alert message to user here
            }
        }
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
        consecutivePasses = 0
    }
    
    
    func resetLettersPlayedInTurn () {
        lettersPlayedInTurn = 0
    }
    
    
    func getRandomWord() -> String {
        let wordToReturn : String
        if let aStreamReader = StreamReader(file: "5-LetterWords") { // "/Users/erichook/Desktop/testSmallUTF8.txt") {
            var numLines = 0
            while (aStreamReader.nextLine() != nil) {
                //print(line)
                numLines += 1
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
                lineNumber += 1
            }
            // You can close the underlying file explicitly. Otherwise it will be
            // closed when the reader is deallocated.
            aStreamReader.close()
            if debugMode == true { print("Final numLines getFirstWord (): " + String(numLines) ) }
        }
        return String("error - didn't find word in getRandomWord()")
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
    func checkPartialWordMatch(wordToCheckIn: String) -> Bool {
        let wordToCheck = wordToCheckIn.lowercaseString
        
        if (wordSet!.contains(wordToCheck)) {
            return true
        }
        
        return false
    }
    
    
    /// checkPartialWordMatch(var wordToCheck: String) -> Bool
    /// given a string, convert to lowercase, check for whole word in Trei (e.g. exists withtrailing '!' character)
    func checkWholeWordMatch(wordToCheckIn: String) -> Bool {
        
        let wordToCheck = wordToCheckIn.lowercaseString
        
        if (wordSet!.contains(wordToCheck + "!")) {
            return true
        }
        return false
    }
    
    
    func checkTilesForWord (wordToCheck: String, letterTileArray: [MMWTile]) -> Bool {
        let string = wordToCheck // "dryad" //wordToCheck // can place test "STRING" in this value for testing purposed
        var lettersToPlay = [Int]()
        let wordToCheckArr = Array(string.characters)
        var wordToCheckArrCount = wordToCheckArr.count
        var foundLetterInPass = false
        var tileArrayNumber = 0
        var lettersToPlayCount = 0
        for letter in wordToCheckArr {
            for tile in letterTileArray {
                if String(letter).uppercaseString == "\r" {
                    //print ("String(letter).uppercaseString == \r ...return char at \(letter)")
                    wordToCheckArrCount -= 1
                    break
                }  // return char exits loop
                if String(letter).uppercaseString == tile.tileText.uppercaseString && !lettersToPlay.contains(tileArrayNumber){
                    if debugMode == true { print("Found letter: \(letter) in tiles \(tile.tileSprite.tileText)") }
                    lettersToPlay.append(tileArrayNumber)
                    lettersToPlayCount += 1
                    //print( "LettersToPlay.count: \(lettersToPlay.count), lettersToPlayCount: \(lettersToPlayCount)" )
                    foundLetterInPass = true
                    break
                }
                tileArrayNumber += 1
                foundLetterInPass = false
            }
            if String(letter).uppercaseString == "\r" {break}  // return char exits loop
            if foundLetterInPass == false {
                if debugMode == true { print( "Tile \(String(letter).uppercaseString) doesn't exist to create word" ) }
                return false
            }
            tileArrayNumber = 0
        }
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
                    if debugMode == true { print ("String(letter).uppercaseString == \r ...return char at \(letter)") }
                    wordToCheckArrCount -= 1
                    break
                }  // return char exits loop
                if String(letter).uppercaseString == tile.tileText.uppercaseString && !lettersToPlay.contains(tileArrayNumber){
                    if debugMode == true { print("Found letter: \(letter) in tiles \(tile.tileSprite.tileText)") }
                    lettersToPlay.append(tileArrayNumber)
                    lettersToPlayCount += 1
                    if debugMode == true { print( "LettersToPlay.count: \(lettersToPlay.count), lettersToPlayCount: \(lettersToPlayCount)" ) }
                    foundLetterInPass = true
                    break
                }
                tileArrayNumber += 1
                foundLetterInPass = false
            }
            if String(letter).uppercaseString == "\r" {break}  // return char exits loop
            if foundLetterInPass == false {
                if debugMode == true { print( "Tile \(String(letter).uppercaseString) doesn't exist to create word" ) }
                break
            }
            tileArrayNumber = 0
        }
        
        if debugMode == true { print( "Letters to Play from letterTileArray: \(lettersToPlay)" ) }
        if debugMode == true { print( "LettersToPlay.count: \(lettersToPlay.count), wordToCheckArr.count: \(wordToCheckArrCount)" ) }
        
        if lettersToPlay.count < wordToCheckArrCount{ // letters not availible return nil
            if debugMode == true { print("FAIL!") }
            return nil
        }
        if debugMode == true { print( "PASS! Found \(string)! self.tileCollection.mmwTileArray.count: \(letterTileArray.count)" ) }

        for arrNum in lettersToPlay {
            tilesToBoard.append(letterTileArray[arrNum])
        }
        
        tileArrayNumber = 0
        for letter in wordToCheckArr {
            for tile in letterTileArray {
                if String(letter).uppercaseString == tile.tileText.uppercaseString {
                    if debugMode == true { print("REMOVE letter: \(letter) in tiles \(tile.tileSprite.tileText)") }
                    letterTileArray.removeAtIndex(tileArrayNumber) 
                    break
                }
                tileArrayNumber += 1
            }
            if String(letter).uppercaseString == "\r" {break}  // return char exits loop
            tileArrayNumber = 0
        }
        if debugMode == true { print("After remove tiles: letterTileArray.count: \(letterTileArray.count)" ) }
        return tilesToBoard
    }

    
    func dealLetter (inout letterToPlace: MMWTile, gridToPlaceLetter: Grid, xSquare: Int, ySquare: Int) {

        letterToPlace.gridEnd = gridToPlaceLetter // set tileSprite parent (MMWTile) grid to grid snapped to
        letterToPlace.gridXEnd = xSquare
        letterToPlace.gridYEnd = ySquare
        letterToPlace.tileState = TileState.Played  // if put on valid board location set TileState to played
        
        letterToPlace.gridHome? = letterToPlace.gridEnd!
        letterToPlace.gridX = xSquare
        letterToPlace.gridY = ySquare
        letterToPlace.gridHome?.grid2DArr[xSquare][ySquare] = letterToPlace

        // move tile to snap point
        let tileLocation = Grid.sendToGridSquare(gridToPlaceLetter, squareX: xSquare, squareY: ySquare)
        let tileLocX = tileLocation.x
        let tileLocY = tileLocation.y
        letterToPlace.tileSprite.position.x = tileLocX
        letterToPlace.tileSprite.position.y = tileLocY
        
        letterToPlace.gridTest? = gridToPlaceLetter
        letterToPlace.gridXTest = xSquare
        letterToPlace.gridYTest = ySquare
    }
    
    
    func placeWord (player: Player, startLocX: Int, startLocY: Int, direction: Character, wordToPlace: [MMWTile]){
        weak var tileToPlace = self.tilesPlayable[0]
        tileToPlace!.tileSprite.color = tileColors[player.playerID]
        tileToPlace!.tileSprite.tileLocation = CGPoint(x: 200, y: 200)
        tileToPlace!.gridHome = mmwGameScene.mmwBoardGrid
        tileToPlace!.gridEnd = mmwGameScene.mmwBoardGrid
        tileToPlace!.tileGrid = mmwGameScene.mmwBoardGrid
        tileToPlace!.gridX = startLocX
        tileToPlace!.gridXEnd = startLocX
        tileToPlace!.gridY = startLocY
        tileToPlace!.gridYEnd = startLocY
        
        setTileOwner(&tileToPlace!, player: player)
        tileToPlace!.tileSprite.tileLocation = tileToPlace!.gridHome!.sendToGridSquare(tileToPlace!.gridX, squareY: tileToPlace!.gridY)
        tileToPlace!.tileState = TileState.Played
        tileToPlace!.gridHome?.grid2DArr[tileToPlace!.gridX][tileToPlace!.gridY] = tileToPlace!
        tileToPlace!.tileSprite.zPosition = 11
        tileToPlace!.tileSprite.hidden = false
    }
    
    
    func sendWordToBoard (inout letterTileArray: [MMWTile], gridToDisplay: Grid, xStartSquare: Int, yStartSquare: Int, IsHorizonal: Bool, player: Player) {
        var xLoc: Int = xStartSquare
        var yLoc: Int = yStartSquare
        for var tileInWord in letterTileArray {
            if tileInWord.tileText == "\r" {break}
            if debugMode == true { print("Letter tileInWord \(tileInWord) is at x:\(xLoc) y:\(yLoc)") }
            setTileOwner(&tileInWord, player: player)
            tileInWord.gridHome = mmwGameScene.mmwBoardGrid
            tileInWord.tileGrid = mmwGameScene.mmwBoardGrid
            tileInWord.gridX = xLoc
            tileInWord.gridXEnd = xLoc
            tileInWord.gridY = yLoc
            tileInWord.gridYEnd = yLoc
            
            tileCollection?.mmwTileArrayPlaceholder.append(mmwGameScene.mmwBoardGrid.grid2DArr[xLoc][yLoc])
            
            tileInWord.gridHome?.grid2DArr[xLoc][yLoc] = tileInWord
            tileInWord.tileState = TileState.Played
            tileInWord.tileSprite.zPosition = 0
            tileInWord.tileState = TileState.Locked
            tileInWord.tileSprite.hidden = false
            
            if IsHorizonal { xLoc += 1 }
            else { yLoc += 1 }
        }
        
        mmwGameScene.updateGridInScene(mmwGameScene.mmwBoardGrid)
        if player.playerLetterGrid != nil {
            mmwGameScene.updateGridInScene(player.playerLetterGrid)
        }
    }
}

