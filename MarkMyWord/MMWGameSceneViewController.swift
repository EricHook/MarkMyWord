
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
import CoreData

extension StreamReader : SequenceType {
    func generate() -> AnyGenerator<String> {
        return anyGenerator {
            return self.nextLine()
        }
    }
}

struct GameSceneSettings {
    
    
}


class MMWGameSceneViewController : UIViewController {

    var numGamesWon = 0
    var numGamesTied = 0
    var numGamesLost = 0
    var numWordsMade = 0
    var numLettersPlayed = 0
    
    var viewSize = screenSize
    // var tileCollection : MMWTileBuilder? = MMWTileBuilder() // : MMWTileBuilder
    var tilesPlayable = [MMWTile]()
    //let tilesPlayableOriginal = [MMWTile]
    var numPlayers   = 2
    var playerTurn   = 1
    var numStarterWords = 1
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
    
    var playerArray : [Player]!

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
    
    //    required init?(coder aDecoder: NSCoder) {
    //        fatalError("init(coder:) has not been implemented")
    //    }
    //
    //    init () {
    //        //super.init(aDecoder)
    //        //viewSize = size/Users/erichook/Downloads/VSToolsForWindows1C.exe
    //        //timer = NSTimer.scheduledTimerWithTimeInterval(1, target:mmwGameScene, selector: Selector("updateCounter"), userInfo: nil, repeats: true)
    //    }
    
    
    
    func setUpGame () {
        if debugMode == true { print("---   in setUpGame mmwGameSceneViewController") }
        loadWordSet()
        
        playerArray = [player1, player2, player3, player4]  // array of all players 0-3 for easier iteration of player turns

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
        
        //if wordTrie!.contains("\(wordToCheck)".characters){
        //    return true
        //}
        
        if (wordSet!.contains(wordToCheck)) {
            return true
        }
        
        return false
    }
    
    
    /// checkPartialWordMatch(var wordToCheck: String) -> Bool
    /// given a string, convert to lowercase, check for whole word in Trei (e.g. exists withtrailing '!' character)
    func checkWholeWordMatch(var wordToCheck: String) -> Bool {
        wordToCheck = wordToCheck.lowercaseString
        
        if (wordSet!.contains(wordToCheck + "!")) {
            return true
        }
        
        //if wordTrie!.contains("\(wordToCheck)!".characters){ // whole words have ! at end of string
        //    return true
        //}
        
        return false
    }
    
    
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
                    //print ("String(letter).uppercaseString == \r ...return char at \(letter)")
                    --wordToCheckArrCount
                    break
                }  // return char exits loop
                if String(letter).uppercaseString == tile.tileText.uppercaseString && !lettersToPlay.contains(tileArrayNumber){
                    if debugMode == true { print("Found letter: \(letter) in tiles \(tile.tileSprite.tileText)") }
                    lettersToPlay.append(tileArrayNumber)
                    ++lettersToPlayCount
                    //print( "LettersToPlay.count: \(lettersToPlay.count), lettersToPlayCount: \(lettersToPlayCount)" )
                    foundLetterInPass = true
                    break
                }
                tileArrayNumber++
                foundLetterInPass = false
            }
            if String(letter).uppercaseString == "\r" {break}  // return char exits loop
            if foundLetterInPass == false {
                //print( "Tile \(String(letter).uppercaseString) doesn't exist to create word" )
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
                    --wordToCheckArrCount
                    break
                }  // return char exits loop
                if String(letter).uppercaseString == tile.tileText.uppercaseString && !lettersToPlay.contains(tileArrayNumber){
                    if debugMode == true { print("Found letter: \(letter) in tiles \(tile.tileSprite.tileText)") }
                    lettersToPlay.append(tileArrayNumber)
                    ++lettersToPlayCount
                    if debugMode == true { print( "LettersToPlay.count: \(lettersToPlay.count), lettersToPlayCount: \(lettersToPlayCount)" ) }
                    foundLetterInPass = true
                    break
                }
                tileArrayNumber++
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
        //return true
        
        ////////////////////////////////////////////
        for arrNum in lettersToPlay {
            tilesToBoard.append(letterTileArray[arrNum])
            //letterTileArray.removeAtIndex(arrNum)
        }
        
        tileArrayNumber = 0
        for letter in wordToCheckArr {
            for tile in letterTileArray {
                if String(letter).uppercaseString == tile.tileText.uppercaseString {
                    if debugMode == true { print("REMOVE letter: \(letter) in tiles \(tile.tileSprite.tileText)") }
                    letterTileArray.removeAtIndex(tileArrayNumber) 
                    break
                }
                tileArrayNumber++
            }
            if String(letter).uppercaseString == "\r" {break}  // return char exits loop
            tileArrayNumber = 0
        }
        if debugMode == true { print("After remove tiles: letterTileArray.count: \(letterTileArray.count)" ) }
        return tilesToBoard
    }

    
    func dealLetter (inout letterToPlace: MMWTile, gridToPlaceLetter: Grid, xSquare: Int, ySquare: Int) {
        
        //let tileAtDropSpot : MMWTile = (gridToPlaceLetter.grid2DArr[xSquare][ySquare])
        //print("drop location info: state:\(tileAtDropSpot.tileOwner) letter:\(tileAtDropSpot.tileSprite.tileText)")
        letterToPlace.gridEnd = gridToPlaceLetter // set tileSprite parent (MMWTile) grid to grid snapped to
        letterToPlace.gridXEnd = xSquare
        letterToPlace.gridYEnd = ySquare
        letterToPlace.tileState = TileState.Played  // if put on valid board location set TileState to played
        
        // recycle placeholder tile
        //        let replacementPlaceholderTile : MMWTile = mmwGameSceneViewController.tileCollection?.mmwBlankTileArray.append(gridToPlaceLetter.grid2DArr[xSquare][ySquare]) as MMWTile
        
        // set basic placeholder tile settings to fit in void in grid - home grid and x and y values
        
        // fill in emptied slot in tile grid with blank
        //let replacementPlaceholderTile : MMWTile = MMWTile()
        
        
        //        let replacementPlaceholderTile = mmwGameSceneViewController.tileCollection?.mmwBlankTileArray.popLast()
        
        
        //        replacementPlaceholderTile!.gridHome = letterToPlace.gridHome
        //        replacementPlaceholderTile!.gridX = letterToPlace.gridX
        //        replacementPlaceholderTile!.gridY = letterToPlace.gridY
        //        letterToPlace.gridHome?.grid2DArr[letterToPlace.gridX][letterToPlace.gridY] = replacementPlaceholderTile!
        
        //        mmwGameSceneViewController.tileCollection?.mmwBlankTileArray.append(gridToPlaceLetter.grid2DArr[xSquare][ySquare])
        
        // set value of snap results grid location to the MMWTile if valid location
        
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
    
    
    //    func dealTestLetters () {
    //        //dealLetter(&<#T##letterToPlace: MMWTile##MMWTile#>, gridToPlaceLetter: <#T##Grid#>, xSquare: <#T##Int#>, ySquare: <#T##Int#>)
    //    }
    
    
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
        //let wordToDisplayArray = Array(wordToDisplay.characters)
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
            
            if IsHorizonal { ++xLoc }
            else { ++yLoc }
        }
        
        mmwGameScene.updateGridInScene(mmwGameScene.mmwBoardGrid)
        if player.playerLetterGrid != nil {
            mmwGameScene.updateGridInScene(player.playerLetterGrid)
        }
    }
    
    
    func loadWords() {
        //        let qualityOfServiceClass = QOS_CLASS_BACKGROUND
        //        let backgroundQueue = dispatch_get_global_queue(qualityOfServiceClass, 0)
        //        dispatch_async(backgroundQueue, {
        //            print("This is run on the background queue")
        //
        //            //dispatch_async(dispatch_get_main_queue()) {
        //
        //                //self.buildWordArray("WordList1to3LetterNoDup")
        //                self.buildTrie( self.buildWordArray("WordList1to3LetterNoDup") )
        //                self.wordsLoaded1to3 = true
        //                print("buildTrie() 1-3")
        //                print("Currently dispatched asynchronously 1-3")
        //                print("I got dispatched1!")
        //
        //            //}
        //
        //            //dispatch_async(dispatch_get_main_queue()) {
        //
        //                //self.buildWordArray("WordList4LetterNoDup")
        //                self.insertTrie(self.buildWordArray("WordList4LetterNoDup") )
        //                self.wordsLoaded4 = true
        //                print("insertTrie() 4")
        //                print("Currently dispatched asynchronously 4")
        //                print("I got dispatched2!")
        //            //}
        //
        //            //dispatch_async(dispatch_get_main_queue()) {
        //
        //                //self.buildWordArray("WordList5LetterNoDup")
        //                self.insertTrie(self.buildWordArray("WordList5LetterNoDup") )
        //                self.wordsLoaded5 = true
        //                print("insertTrie() 5")
        //                print("Currently dispatched asynchronously 5")
        //                //print("hello from UI thread executed as dispatch 2")
        //                print("I got dispatched3!")
        //            //}
        //
        //            //dispatch_async(dispatch_get_main_queue()) {
        //
        //                //self.buildWordArray("WordList6LetterNoDup")
        //                self.insertTrie(self.buildWordArray("WordList6LetterNoDup") )
        //                self.wordsLoaded6 = true
        //                print("insertTrie() 6")
        //                print("Currently dispatched asynchronously 6")
        //
        //                print("I got dispatched4!")
        //            //}
        //
        //            dispatch_async(dispatch_get_main_queue()) {
        //
        //                //self.buildWordArray("WordList6LetterNoDup")
        //                self.insertTrie(self.buildWordArray("WordList6to7LetterNoDup") )
        //                self.wordsLoaded7 = true
        //                print("insertTrie() 6 to 7")
        //                print("Currently dispatched asynchronously 7")
        //                print("I got dispatched5!")
        //
        //            }
        //
        //            dispatch_async(dispatch_get_main_queue()) {
        //
        //                //self.buildWordArray("WordList6LetterNoDup")
        //                self.insertTrie(self.buildWordArray("WordList8to10LetterNoDup") )
        //                self.wordsLoaded8 = true
        //                print("insertTrie() 8to10")
        //                print("Currently dispatched asynchronously 8to10")
        //                print("I got dispatched6!")
        //
        //            }
        //
        //
        //            dispatch_async(dispatch_get_main_queue(), { () -> Void in
        //                print("This is run on the main queue, after the previous code in outer block")
        //            })
        //        })
        
        
        
        //        //let priority = DISPATCH_QUEUE_PRIORITY_DEFAULT
        //        let priority = DISPATCH_QUEUE_PRIORITY_HIGH
        //
        //        dispatch_async(dispatch_get_global_queue(priority, 0), { ()->() in
        //
        //            print("gcd hello")
        //            print("hello from UI thread executed as dispatch 1")
        //            print("Currently dispatched asynchronously 0")
        //
        //            //self.buildWordArray("WordList1to3LetterNoDup")
        //            self.buildTrie(self.buildWordArray("WordList1to3LetterNoDup") )
        //            self.wordsLoaded1to3 = true
        //            print("buildTrie() 1-3")
        //            print("Currently dispatched asynchronously 1-3")
        //
        //            //self.buildWordArray("WordList4LetterNoDup")
        //            self.insertTrie(self.buildWordArray("WordList4LetterNoDup") )
        //            self.wordsLoaded4 = true
        //            print("insertTrie() 4")
        //            print("Currently dispatched asynchronously 4")
        //
        ////            //self.buildWordArray("WordList5LetterNoDup")
        ////            self.insertTrie(self.buildWordArray("WordList5LetterNoDup") )
        ////            self.wordsLoaded5 = true
        ////            print("insertTrie() 5")
        ////            print("Currently dispatched asynchronously 5")
        ////            //print("hello from UI thread executed as dispatch 2")
        ////
        ////            //self.buildWordArray("WordList6LetterNoDup")
        ////            self.insertTrie(self.buildWordArray("WordList6LetterNoDup") )
        ////            self.wordsLoaded6 = true
        ////            print("insertTrie() 6")
        ////            print("Currently dispatched asynchronously 6")
        ////
        ////            /////////////////////////
        ////
        ////            //self.buildWordArray("WordList6to7LetterNoDup")
        ////            self.insertTrie(self.buildWordArray("WordList6to7LetterNoDup") )
        ////            self.wordsLoaded7 = true
        ////            print("insertTrie() 6 to 7")
        ////            print("Currently dispatched asynchronously 7")
        ////
        ////            //self.buildWordArray("WordList6LetterNoDup")
        ////            self.insertTrie(self.buildWordArray("WordList8to10LetterNoDup") )
        ////            self.wordsLoaded8 = true
        ////            print("insertTrie() 8to10")
        ////            print("Currently dispatched asynchronously 8to10")
        //
        //            ////////////////////////
        //
        ////            //self.buildWordArray("WordList11to16LetterNoDup")
        ////            self.insertTrie(self.buildWordArray("WordList11to16LetterNoDup") )
        ////            self.wordsLoaded9 = true
        ////            print("insertTrie() 11to16")
        ////            print("Currently dispatched asynchronously 11to16")
        //
        //
        //
        //
        //
        //
        ////            //??? INSERT SAVED TREI HERE ???
        ////            //aCoder.encodeObject(name, forKey: PropertyKey.nameKey)
        ////
        ////            let trieData: TrieData = NSEntityDescription.insertNewObjectForEntityForName("TrieData", inManagedObjectContext: CDHelper.shared.context) as! TrieData
        ////
        ////
        ////            //store you class object into NSUserDefaults.
        ////            let theData = NSKeyedArchiver.archivedDataWithRootObject(trieData)
        ////            NSUserDefaults().setObject(theData, forKey: "theData")
        ////            print(theData)
        ////
        ////            //get your object from NSUserDefaults.
        ////            if let loadedData = NSUserDefaults().dataForKey("theData") {
        ////
        ////                if let loadedData = NSKeyedUnarchiver.unarchiveObjectWithData(loadedData) as? NSData {
        ////
        ////                    trieData.prebuiltTrie = loadedData
        ////                }
        ////            }
        //
        //
        //
        //
        //
        ////            dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0), { ()->() in
        ////
        ////                })
        //
        //            dispatch_async(dispatch_get_main_queue(), {
        //
        //                print("hello from UI thread executed as dispatch")
        //
        //            })
        //        })
        //
        //        print("hello from UI thread")
        
        
        //        // ??? INSERT SAVED TREI HERE ???
        ////        let item: Item = NSEntityDescription.insertNewObjectForEntityForName("Item", inManagedObjectContext: CDHelper.shared.context) as! Item
        ////        item.name = itemName
        ////        print("Inserted New Managed Object for '\(item.name)'")
        ////        CDHelper.saveSharedContext()
        //
        //
        //
        //        //var wordTrie : Trie<Character>?
        //
        //
        //         //??? INSERT SAVED TREI HERE ???
        //
        //                let trieData: TrieData = NSEntityDescription.insertNewObjectForEntityForName("TrieData", inManagedObjectContext: CDHelper.shared.context) as! TrieData
        //        //let theTrie = NSData(      (wordTrie)
        //
        //
        //        //store you class object into NSUserDefaults.
        //        let theData = NSKeyedArchiver.archivedDataWithRootObject(trieData)
        //        NSUserDefaults().setObject(theData, forKey: "theData")
        //
        //        //get your object from NSUserDefaults.
        //        if let loadedData = NSUserDefaults().dataForKey("theData") {
        //
        //            if let loadedData = NSKeyedUnarchiver.unarchiveObjectWithData(loadedData) as? NSData {
        //
        //                trieData.prebuiltTrie = loadedData
        //
        ////                trieData.prebuiltTrie
        ////                loadedPerson[0].name   //"Leo"
        ////                loadedPerson[0].age    //45
        //            }
        //        }
        //
        ////        var theData : NSData = NSData(wordTrie)
        ////                trieData.prebuiltTrie = wordTrie
        ////                //print("Inserted New Managed Object for '\(item.name)'")
        ////                CDHelper.saveSharedContext()
        
    }
}

