//
//  GameViewController.swift
//
//  Created by Eric Hook on 4/29/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import UIKit
import SpriteKit

extension SKNode {
    class func unarchiveFromFile(file : String) -> SKNode? {
        if let path = NSBundle.mainBundle().pathForResource(file, ofType: "sks") {
            let sceneData = NSData(contentsOfFile: path)!
            let archiver = NSKeyedUnarchiver(forReadingWithData: sceneData)

            archiver.setClass(self.classForKeyedUnarchiver(), forClassName: "SKScene")
            let scene1 = archiver.decodeObjectForKey(NSKeyedArchiveRootObjectKey) as! MainMenuScene
            
            scene1.name = "scene in GameViewController"
            print("scene is: \(scene1.description)")
            
            archiver.finishDecoding()
            return scene1
        }
        return nil
    }
}

//var endGameSceneController = EndGameSceneController()

//var mmwEndGameScene = MMWEndGameScene()
var mmwGameSceneViewController : MMWGameSceneViewController! = MMWGameSceneViewController()
var gameViewController : GameViewController! = GameViewController()

var humanAvatarPrefixString  = "avatar00"
var meyamaAvatarPrefixString = "meyama00"
var selectedHumanAvatar  = 0
var selectedMeyamaAvatar = 0

//var avatar000 =

//var humanAvatarImages = [SKimage]
//var meyamaAvatarImages = [SKimage]

class GameViewController : UIViewController, UITextFieldDelegate {
    
    //@IBOutlet weak var menuView: UIView?
    //@IBOutlet weak var playButton: UIButton?

    //var tempSecondsPerTurn : Int!
    
    var tempAudioOn : Bool!
    
    var tempMinWordSize : Int!
    var tempNumStarterWords : Int!
    var tempNumPlayers : Int!
    
    var tempPlayer1Name : String!
    var tempPlayer2Name : String!
    var tempPlayer3Name : String!
    var tempPlayer4Name : String!
    
    var tempPlayer1IsHuman : Bool!
    var tempPlayer2IsHuman : Bool!
    var tempPlayer3IsHuman : Bool!
    var tempPlayer4IsHuman : Bool!
    
    var tempPlayer1SkillLevel : Int!
    var tempPlayer2SkillLevel : Int!
    var tempPlayer3SkillLevel : Int!
    var tempPlayer4SkillLevel : Int!
    
    var tempPlayer1AvatarNumber = 0
    var tempPlayer2AvatarNumber = 0
    var tempPlayer3AvatarNumber = 0
    var tempPlayer4AvatarNumber = 0
    
    var tempPlayer1MeyamaNumber = 0
    var tempPlayer2MeyamaNumber = 0
    var tempPlayer3MeyamaNumber = 0
    var tempPlayer4MeyamaNumber = 0
    
    var playerImageArray = ["avatar000.png", "avatar001.png", "avatar002.png", "avatar003.png", "avatar004.png", "avatar005.png"]
    
    var meyamaImageArray = ["meyama000.png", "meyama001.png", "meyama002.png", "meyama003.png", "meyama004.png", "meyama003.png"]
    
    var playerAvatarNames = ["Player1a", "Player1b", "Player1c", "Player1d", "Player1e", "Player1f"]
    
    var meyamaAvatarNames = ["Alice", "Bongo", "Cocoa", "Dizzy", "Ethota", "Fadama"]
    
    func updateGameSettings() {
//        tempSecondsPerTurn = mmwGameSceneViewController.secondsPerTurn
//        tempAudioOn = mmwGameSceneViewController.audioOn
        
//        // seconds per turn UI
//        switch (mmwGameSceneViewController.secondsPerTurn) {
//        case 20:
//            gameViewController.secondPerTurnSwitchOutlet.selectedSegmentIndex = 0
//            print("UI secondsPerTurn = 20")
//        case 30:
//            gameViewController.secondPerTurnSwitchOutlet.selectedSegmentIndex = 1
//            print("UI secondsPerTurn = 30")
//        case 45:
//            gameViewController.secondPerTurnSwitchOutlet.selectedSegmentIndex = 2
//            print("UI secondsPerTurn = 45")
//        case 60:
//            gameViewController.secondPerTurnSwitchOutlet.selectedSegmentIndex = 3
//            print("UI secondsPerTurn = 60")
//        case 999:
//            gameViewController.secondPerTurnSwitchOutlet.selectedSegmentIndex = 4
//            print("UI secondsPerTurn = 999 / OFF")
//        default:
//            break;
//        }
        
        // audio UI
        if mmwGameSceneViewController.audioOn == true {
            gameViewController.audioSettingOutlet.selectedSegmentIndex = 1
        }
        else {
            gameViewController.audioSettingOutlet.selectedSegmentIndex = 0
        }
        print("UI audioOn = \(mmwGameSceneViewController.audioOn)")
        
        // minWordSize UI
        gameViewController.minWordLengthSwitchOutlet.selectedSegmentIndex = mmwGameSceneViewController.minWordSize - 2
        print("UI minWordSize = \(mmwGameSceneViewController.minWordSize)")
        
        // number starter words UI
        gameViewController.numStarterWordsSwitchOutlet.selectedSegmentIndex = mmwGameSceneViewController.numStarterWords - 1
        print("UI numStarterWords = \(mmwGameSceneViewController.numStarterWords)")
        
        // number of players UI
        gameViewController.numPlayersSwitchOutlet.selectedSegmentIndex = mmwGameSceneViewController.numPlayers - 2
        switch (mmwGameSceneViewController.numPlayers - 2){
        case 0:
            mmwGameSceneViewController.numPlayers = 2
            print("UI numPlayers 2")
            gameViewController.ViewPlayer3UI.alpha = 0.2
            gameViewController.ViewPlayer3UI.userInteractionEnabled = false
            gameViewController.ViewPlayer4UI.alpha = 0.2
            gameViewController.ViewPlayer4UI.userInteractionEnabled = false
        case 1:
            mmwGameSceneViewController.numPlayers = 3
            print("UI numPlayers 3")
            gameViewController.ViewPlayer3UI.alpha = 1.0
            gameViewController.ViewPlayer3UI.userInteractionEnabled = true
            gameViewController.ViewPlayer4UI.alpha = 0.2
            gameViewController.ViewPlayer4UI.userInteractionEnabled = false
        case 2:
            mmwGameSceneViewController.numPlayers = 4
            print("UI numPlayers 4")
            gameViewController.ViewPlayer3UI.alpha = 1.0
            gameViewController.ViewPlayer3UI.userInteractionEnabled = true
            gameViewController.ViewPlayer4UI.alpha = 1.0
            gameViewController.ViewPlayer4UI.userInteractionEnabled = true
        default:
            break;
        }
        
//        ////////////////////
//        
//        if mmwGameSceneViewController.playerArray[1].isHuman == false {
//            gameViewController.isHumanPlayer2Outlet.selectedSegmentIndex = 0
//        }
//        else {
//            gameViewController.isHumanPlayer2Outlet.selectedSegmentIndex = 1
//        }
//        
//        if mmwGameSceneViewController.playerArray[2].isHuman == false {
//            gameViewController.isHumanPlayer3Outlet.selectedSegmentIndex = 0
//        }
//        else {
//            gameViewController.isHumanPlayer3Outlet.selectedSegmentIndex = 1
//        }
//        
//        if mmwGameSceneViewController.playerArray[3].isHuman == false {
//            gameViewController.isHumanPlayer4Outlet.selectedSegmentIndex = 0
//        }
//        else {
//            gameViewController.isHumanPlayer4Outlet.selectedSegmentIndex = 1
//        }
//        
//        ////////////////////

        tempAudioOn = mmwGameSceneViewController.audioOn
        tempMinWordSize = mmwGameSceneViewController.minWordSize
        tempNumStarterWords = mmwGameSceneViewController.numStarterWords
        tempNumPlayers = mmwGameSceneViewController.numPlayers
        
        tempPlayer1Name = mmwGameSceneViewController.playerArray[0].playerName
        tempPlayer2Name = mmwGameSceneViewController.playerArray[1].playerName
        tempPlayer3Name = mmwGameSceneViewController.playerArray[2].playerName
        tempPlayer4Name = mmwGameSceneViewController.playerArray[3].playerName
        
        tempPlayer1IsHuman = mmwGameSceneViewController.playerArray[0].isHuman
        tempPlayer2IsHuman = mmwGameSceneViewController.playerArray[1].isHuman
        tempPlayer3IsHuman = mmwGameSceneViewController.playerArray[2].isHuman
        tempPlayer4IsHuman = mmwGameSceneViewController.playerArray[3].isHuman
        
        tempPlayer1SkillLevel = mmwGameSceneViewController.playerArray[0].playerSkillLevel
        tempPlayer2SkillLevel = mmwGameSceneViewController.playerArray[1].playerSkillLevel
        tempPlayer3SkillLevel = mmwGameSceneViewController.playerArray[2].playerSkillLevel
        tempPlayer4SkillLevel = mmwGameSceneViewController.playerArray[3].playerSkillLevel
        
        tempPlayer1AvatarNumber = mmwGameSceneViewController.playerArray[0].playerAvatarNumber
        tempPlayer2AvatarNumber = mmwGameSceneViewController.playerArray[1].playerAvatarNumber
        tempPlayer3AvatarNumber = mmwGameSceneViewController.playerArray[2].playerAvatarNumber
        tempPlayer4AvatarNumber = mmwGameSceneViewController.playerArray[3].playerAvatarNumber
        
        tempPlayer1MeyamaNumber = mmwGameSceneViewController.playerArray[0].playerMeyamaNumber
        tempPlayer2MeyamaNumber = mmwGameSceneViewController.playerArray[1].playerMeyamaNumber
        tempPlayer3MeyamaNumber = mmwGameSceneViewController.playerArray[2].playerMeyamaNumber
        tempPlayer4MeyamaNumber = mmwGameSceneViewController.playerArray[3].playerMeyamaNumber
        
        // Player 1 always human
        player1NameTextFieldOutlet.hidden = false
        player1NameLabel.hidden = true
        

        if tempPlayer2IsHuman == false {
            isHumanPlayer2Outlet.selectedSegmentIndex = 0
            player2NameLabel.text = meyamaAvatarNames[tempPlayer2MeyamaNumber]
            player2ImageOutlet.image = UIImage(named: meyamaImageArray[tempPlayer2MeyamaNumber ])
            avatarStepperPlayer02.value = Double(tempPlayer2MeyamaNumber)
            player2NameTextFieldOutlet.hidden = true
            player2NameLabel.hidden = false
            player2SkillLevelOutlet.hidden = false
            
        }
        else { // human player tempPlayer2IsHuman = true
            isHumanPlayer2Outlet.selectedSegmentIndex = 1
            player2NameLabel.text = playerAvatarNames[tempPlayer2AvatarNumber]
            player2ImageOutlet.image = UIImage(named: playerImageArray[tempPlayer2AvatarNumber ])
            avatarStepperPlayer02.value = Double(tempPlayer2AvatarNumber)
            player2NameTextFieldOutlet.hidden = false
            player2NameLabel.hidden = true
            player2SkillLevelOutlet.hidden = true
        }
        
        if tempPlayer3IsHuman == false {
            isHumanPlayer3Outlet.selectedSegmentIndex = 0
            player3NameLabel.text = meyamaAvatarNames[tempPlayer3MeyamaNumber]
            player3ImageOutlet.image = UIImage(named: meyamaImageArray[tempPlayer3MeyamaNumber ])
            avatarStepperPlayer03.value = Double(tempPlayer3MeyamaNumber)
            player3NameTextFieldOutlet.hidden = true
            player3NameLabel.hidden = false
            player3SkillLevelOutlet.hidden = false
            
        }
        else { // human player tempPlayer3IsHuman = true
            isHumanPlayer3Outlet.selectedSegmentIndex = 1
            player3NameLabel.text = playerAvatarNames[tempPlayer3AvatarNumber]
            player3ImageOutlet.image = UIImage(named: playerImageArray[tempPlayer3AvatarNumber ])
            avatarStepperPlayer03.value = Double(tempPlayer3AvatarNumber)
            player3NameTextFieldOutlet.hidden = false
            player3NameLabel.hidden = true
            player3SkillLevelOutlet.hidden = true
        }

        if tempPlayer4IsHuman == false {
            isHumanPlayer4Outlet.selectedSegmentIndex = 0
            player4NameLabel.text = meyamaAvatarNames[tempPlayer4MeyamaNumber]
            player4ImageOutlet.image = UIImage(named: meyamaImageArray[tempPlayer4MeyamaNumber ])
            avatarStepperPlayer04.value = Double(tempPlayer4MeyamaNumber)
            player4NameTextFieldOutlet.hidden = true
            player4NameLabel.hidden = false
            player4SkillLevelOutlet.hidden = false
        }
        else { // human player tempPlayer4IsHuman = true
            isHumanPlayer4Outlet.selectedSegmentIndex = 1
            player4NameLabel.text = playerAvatarNames[tempPlayer4AvatarNumber]
            player4ImageOutlet.image = UIImage(named: playerImageArray[tempPlayer4AvatarNumber ])
            avatarStepperPlayer04.value = Double(tempPlayer4AvatarNumber)
            player4NameTextFieldOutlet.hidden = false
            player4NameLabel.hidden = true
            player4SkillLevelOutlet.hidden = true
        }
    }

    @IBOutlet var GameViewControllerUI: SKView!
    
    
    @IBOutlet weak var ViewOptionsUI: UIView!
    @IBOutlet weak var ViewAllOptionsUI: UIView!
    @IBOutlet weak var ViewEndGameUI: UIView!
    @IBOutlet weak var ViewResultsScreenUI: UIView!

    
    @IBOutlet weak var ViewPlayer1UI: UIView!
    @IBOutlet weak var ViewPlayer2UI: UIView!
    @IBOutlet weak var ViewPlayer3UI: UIView!
    @IBOutlet weak var ViewPlayer4UI: UIView!
    @IBOutlet weak var numberPlayersUI: UISegmentedControl!
    
    
    @IBOutlet weak var viewStatsContainer: UIView!
    @IBOutlet weak var viewRulesContainer: UIView!

    @IBOutlet weak var OptionsSwitchOutlet: UISegmentedControl!

    
    @IBAction func OptionsSwitchAction(sender: AnyObject) {
//        if sender.selectedSegmentIndex == 2 {
//            print("stats selected")
//            gameViewController.ViewAllOptionsUI.hidden = false
//            gameViewController.ViewOptionsUI.hidden = true
//            gameViewController.viewRulesContainer.hidden = true
//            gameViewController.viewStatsContainer.hidden = false
//            gameViewController.ViewEndGameUI.hidden = true
//        }
        
        switch (self.OptionsSwitchOutlet.selectedSegmentIndex){
        case 0:
            print("outlet 0 Options selected")
            ViewOptionsUI.userInteractionEnabled = true
            gameViewController.ViewAllOptionsUI.hidden = false
            gameViewController.ViewOptionsUI.hidden = false
            gameViewController.viewRulesContainer.hidden = true
            gameViewController.viewStatsContainer.hidden = true
            gameViewController.ViewEndGameUI.hidden = true
        case 1:
            print("outlet 1 Rules selected")
            gameViewController.ViewAllOptionsUI.userInteractionEnabled = true
            gameViewController.ViewAllOptionsUI.hidden = false
            gameViewController.ViewOptionsUI.hidden = true
            gameViewController.viewRulesContainer.hidden = false
            gameViewController.viewStatsContainer.hidden = true
            gameViewController.ViewEndGameUI.hidden = true
        case 2:
            print("outlet 2 selected")
            gameViewController.ViewAllOptionsUI.userInteractionEnabled = true
            gameViewController.ViewAllOptionsUI.hidden = false
            gameViewController.ViewOptionsUI.hidden = true
            gameViewController.viewRulesContainer.hidden = true
            gameViewController.viewStatsContainer.hidden = false
            gameViewController.ViewEndGameUI.hidden = true
            
            toStatViewSettings()
//            self.viewRulesContainer.hidden = true
//            self.viewStatsContainer.hidden = false
            
//            toStatView.sendActionsForControlEvents(.TouchUpInside)
            
        default:
            break;
        }
        
        print("testSwitch worked")
//        button.enabled = false
//        button.hidden = true
        //testSwitch.tintColor = UIColorApplePurple
        
    }


    
    //    @IBAction func OptionsSwitchAction(sender: AnyObject) {
    //
    //       //    }
    
    
   
//    @IBOutlet weak var toStatViewOutlet: UIButton!
//    
//    
//    @IBAction func toStatView(sender: AnyObject) {
//
//        
//    }
    
    func toStatViewSettings() {
        
        gamesWonOutlet.text = String(mmwGameSceneViewController.numGamesWon)
        
        gamesLostOutlet.text = String(mmwGameSceneViewController.numGamesLost)
        
        wordsPlayedOutlet.text = String(mmwGameSceneViewController.numWordsMade)
        
        lettersPlayedOutlet.text = String(mmwGameSceneViewController.numLettersPlayed)
        
    }

    
    @IBOutlet weak var secondPerTurnSwitchOutlet: UISegmentedControl!
    
    
    @IBAction func secondsPerTurnAction(sender: AnyObject) {
//        switch (sender.selectedSegmentIndex){
//        case 0:
//            tempSecondsPerTurn = 20
//            print("tempSecondsPerTurn = 20")
//        case 1:
//            tempSecondsPerTurn = 30
//            print("tempSecondsPerTurn = 30")
//        case 2:
//            tempSecondsPerTurn = 45
//            print("tempSecondsPerTurn = 45")
//        case 3:
//            tempSecondsPerTurn = 60
//            print("tempSecondsPerTurn = 60")
//        case 4:
//            tempSecondsPerTurn = 999
//            print("tempSecondsPerTurn = 999")
//        default:
//            break;
//        }
    }

    
    @IBOutlet weak var audioSettingOutlet: UISegmentedControl!
    
    
    @IBAction func audioSettingAction(sender: AnyObject) {
        switch (sender.selectedSegmentIndex){
        case 0:
            tempAudioOn = false
            print("tempAudioOn turned to off")
            
        case 1:
            tempAudioOn = true
            print("tempAudioOn turned to on")
        default:
            break;
        }
    }
    
    
    @IBOutlet weak var minWordLengthSwitchOutlet: UISegmentedControl!
    
    
    @IBAction func minWordLengthSwitchAction(sender: AnyObject) {
        switch (sender.selectedSegmentIndex){
        case 0:
            tempMinWordSize = 2
            print("tempMinWordSize set to 2")
        case 1:
            tempMinWordSize = 3
            print("tempMinWordSize set to 3")
        case 2:
            tempMinWordSize = 3
            print("tempMinWordSize set to 4")
        default:
            break;
        }
    }
    
    
    @IBOutlet weak var numStarterWordsSwitchOutlet: UISegmentedControl!
    
    
    @IBAction func numStarterWordsSwitchAction(sender: AnyObject) {
        switch (sender.selectedSegmentIndex){
        case 0:
            tempNumStarterWords = 1
            print("tempNumStarterWords = 1")
        case 1:
            tempNumStarterWords = 2
            print("tempNumStarterWords = 2")
        case 2:
            tempNumStarterWords = 3
            print("tempNumStarterWords = 3")
        default:
            break;
        }
    }

    
    @IBOutlet weak var numPlayersSwitchOutlet: UISegmentedControl!

    @IBAction func numPlayersSwitchAction(sender: AnyObject) {
        switch (sender.selectedSegmentIndex){
        case 0:
            tempNumPlayers = 2
            print("tempNumPlayers 2")
            ViewPlayer3UI.alpha = 0.5
            ViewPlayer3UI.userInteractionEnabled = false
            ViewPlayer4UI.alpha = 0.5
            ViewPlayer4UI.userInteractionEnabled = false
        case 1:
            tempNumPlayers = 3
            print("tempNumPlayers 3")
            ViewPlayer3UI.alpha = 1.0
            ViewPlayer3UI.userInteractionEnabled = true
            ViewPlayer4UI.alpha = 0.5
            ViewPlayer4UI.userInteractionEnabled = false
        case 2:
            tempNumPlayers = 4
            print("tempNumPlayers 4")
            ViewPlayer3UI.alpha = 1.0
            ViewPlayer3UI.userInteractionEnabled = true
            ViewPlayer4UI.alpha = 1.0
            ViewPlayer4UI.userInteractionEnabled = true
        default:
            break;
        }
    }
    
    ///////////////////////////////////////
    
    @IBOutlet weak var player1NameLabel: UILabel!
    
    @IBOutlet weak var player2NameLabel: UILabel!
    
    @IBOutlet weak var player3NameLabel: UILabel!
    
    @IBOutlet weak var player4NameLabel: UILabel!

    ///////////////////////////////////////
    
    // No isHumanPlayer1Outlet / Player 1 always human
    @IBOutlet weak var isHumanPlayer2Outlet: UISegmentedControl!
    @IBOutlet weak var isHumanPlayer3Outlet: UISegmentedControl!
    @IBOutlet weak var isHumanPlayer4Outlet: UISegmentedControl!
    
    ///////////////////////////////////////
    
    @IBAction func isHumanPlayer2Action(sender: AnyObject) {
        switch (sender.selectedSegmentIndex){
        case 0: // AI Meyama
            tempPlayer2IsHuman = false
            player2ImageOutlet.image = UIImage(named: meyamaImageArray[tempPlayer2MeyamaNumber ])
            player2NameLabel.text = meyamaAvatarNames[tempPlayer2MeyamaNumber]
            avatarStepperPlayer02.value = Double(tempPlayer2MeyamaNumber)
            player2NameTextFieldOutlet.hidden = true
            player2NameLabel.hidden = false
            player2SkillLevelOutlet.hidden = false
            
        case 1: // human player
            tempPlayer2IsHuman = true
            player2NameLabel.text = playerAvatarNames[tempPlayer2AvatarNumber]
            player2ImageOutlet.image = UIImage(named: playerImageArray[tempPlayer2AvatarNumber ])
            avatarStepperPlayer02.value = Double(tempPlayer2AvatarNumber)
            player2NameTextFieldOutlet.hidden = false
            player2NameLabel.hidden = true
            player2SkillLevelOutlet.hidden = true
        default:
            break
        }
    }

    @IBAction func isHumanPlayer3Action(sender: AnyObject) {
        switch (sender.selectedSegmentIndex){
        case 0: // AI Meyama
            tempPlayer3IsHuman = false
            player3ImageOutlet.image = UIImage(named: meyamaImageArray[tempPlayer3MeyamaNumber ])
            player3NameLabel.text = meyamaAvatarNames[tempPlayer3MeyamaNumber]
            avatarStepperPlayer03.value = Double(tempPlayer3MeyamaNumber)
            player3NameTextFieldOutlet.hidden = true
            player3NameLabel.hidden = false
            player3SkillLevelOutlet.hidden = false
        
        case 1: // human player
            tempPlayer3IsHuman = true
            player3NameLabel.text = playerAvatarNames[tempPlayer3AvatarNumber]
            player3ImageOutlet.image = UIImage(named: playerImageArray[tempPlayer3AvatarNumber ])
            avatarStepperPlayer03.value = Double(tempPlayer3AvatarNumber)
            player3NameTextFieldOutlet.hidden = false
            player3NameLabel.hidden = true
            player3SkillLevelOutlet.hidden = true
        default:
            break
        }
    }

    @IBAction func isHumanPlayer4Action(sender: AnyObject) {
        switch (sender.selectedSegmentIndex){
        case 0: // AI Meyama
            tempPlayer4IsHuman = false
            player4ImageOutlet.image = UIImage(named: meyamaImageArray[tempPlayer4MeyamaNumber ])
            player4NameLabel.text = meyamaAvatarNames[tempPlayer4MeyamaNumber]
            avatarStepperPlayer03.value = Double(tempPlayer4MeyamaNumber)
            player4NameTextFieldOutlet.hidden = true
            player4NameLabel.hidden = false
            player4SkillLevelOutlet.hidden = false
        case 1: // human player
            tempPlayer4IsHuman = true
            player4NameLabel.text = playerAvatarNames[tempPlayer4AvatarNumber]
            player4ImageOutlet.image = UIImage(named: playerImageArray[tempPlayer4AvatarNumber ])
            avatarStepperPlayer04.value = Double(tempPlayer4AvatarNumber)
            player4NameTextFieldOutlet.hidden = false
            player4NameLabel.hidden = true
            player4SkillLevelOutlet.hidden = true
        default:
            break
        }
    }

    ///////////////////////////////////////

    @IBOutlet weak var avatarStepperPlayer02: UIStepper!
    @IBOutlet weak var avatarStepperPlayer03: UIStepper!
    @IBOutlet weak var avatarStepperPlayer04: UIStepper!

   ///////////////////////////////////////
    
    @IBAction func avatarStepperPlayer01(sender: UIStepper) {
        
        tempPlayer1AvatarNumber = Int(sender.value)
        player1ImageOutlet.image = UIImage(named: playerImageArray[tempPlayer1AvatarNumber ])
        if tempPlayer1IsHuman == true {
            player1NameLabel.text = playerAvatarNames[tempPlayer1AvatarNumber]
//            player1NameTextFieldOutlet.hidden = false
//            player1NameLabel.hidden = true
        } else {
            player1NameLabel.text = playerAvatarNames[tempPlayer1AvatarNumber]
//            player1NameTextFieldOutlet.hidden = true
//            player1NameLabel.hidden = false
        }
    }

    @IBAction func avatarStepperPlayer02(sender: UIStepper) {
        
        if tempPlayer2IsHuman == false { // an AI Meyama
            tempPlayer2MeyamaNumber = Int(sender.value)
            player2NameLabel.text = meyamaAvatarNames[tempPlayer2MeyamaNumber]
            player2ImageOutlet.image = UIImage(named: meyamaImageArray[tempPlayer2MeyamaNumber])
            player2NameLabel.text = meyamaAvatarNames[tempPlayer2MeyamaNumber]
//            player2NameTextFieldOutlet.hidden = true
//            player2NameLabel.hidden = false
        } else { // human
            tempPlayer2AvatarNumber = Int(sender.value)
            player2NameLabel.text = playerAvatarNames[tempPlayer2AvatarNumber]
            player2ImageOutlet.image = UIImage(named: playerImageArray[tempPlayer2AvatarNumber])
            player2NameLabel.text = playerAvatarNames[tempPlayer2AvatarNumber]
//            player2NameTextFieldOutlet.hidden = false
//            player2NameLabel.hidden = true
        }
    }

    @IBAction func avatarStepperPlayer03(sender: UIStepper) {
        if tempPlayer3IsHuman == false{ // an AI Meyama
            tempPlayer3MeyamaNumber = Int(sender.value)
            player3NameLabel.text = meyamaAvatarNames[tempPlayer3MeyamaNumber]
            player3ImageOutlet.image = UIImage(named: meyamaImageArray[tempPlayer3MeyamaNumber])
            player3NameLabel.text = meyamaAvatarNames[tempPlayer3MeyamaNumber]
//            player3NameTextFieldOutlet.hidden = true
//            player3NameLabel.hidden = false
        } else { // human
            tempPlayer3AvatarNumber = Int(sender.value)
            player3NameLabel.text = playerAvatarNames[tempPlayer3AvatarNumber]
            player3ImageOutlet.image = UIImage(named: playerImageArray[tempPlayer3AvatarNumber])
            player3NameLabel.text = playerAvatarNames[tempPlayer3AvatarNumber]
//            player3NameTextFieldOutlet.hidden = false
//            player3NameLabel.hidden = true
        }
    }

    @IBAction func avatarStepperPlayer04(sender: UIStepper) {
        if tempPlayer4IsHuman == false{ // an AI Meyama
            tempPlayer4MeyamaNumber = Int(sender.value)
            player4NameLabel.text = meyamaAvatarNames[tempPlayer4MeyamaNumber]
            player4ImageOutlet.image = UIImage(named: meyamaImageArray[tempPlayer4MeyamaNumber])
            player4NameLabel.text = meyamaAvatarNames[tempPlayer4MeyamaNumber]
//            player4NameTextFieldOutlet.hidden = true
//            player4NameLabel.hidden = false
        } else { // human
            tempPlayer4AvatarNumber = Int(sender.value)
            player4NameLabel.text = playerAvatarNames[tempPlayer4AvatarNumber]
            player4ImageOutlet.image = UIImage(named: playerImageArray[tempPlayer4AvatarNumber])
            player4NameLabel.text = playerAvatarNames[tempPlayer4AvatarNumber]
//            player4NameTextFieldOutlet.hidden = false
//            player4NameLabel.hidden = true
        }
    }

    ///////////////////////////////////////
    
    @IBOutlet weak var player1ImageOutlet: UIImageView!

    @IBOutlet weak var player2ImageOutlet: UIImageView!

    @IBOutlet weak var player3ImageOutlet: UIImageView!
    
    @IBOutlet weak var player4ImageOutlet: UIImageView!

    ///////////////////////////////////////
    
    // switch (sender.selectedSegmentIndex){

    @IBOutlet weak var player2SkillLevelOutlet: UISegmentedControl!
    
    @IBAction func player2SkillLevelAction(sender: AnyObject) {
        
        let skillLevelNum = sender.selectedSegmentIndex

        if tempPlayer2IsHuman == false { // an AI Meyama
            player2SkillLevelOutlet.hidden = false
            tempPlayer2SkillLevel = skillLevelNum

        } else { // human
            player2SkillLevelOutlet.hidden = true
        }
    }


    @IBOutlet weak var player3SkillLevelOutlet: UISegmentedControl!
    
    @IBAction func player3SkillLevelAction(sender: AnyObject) {
        let skillLevelNum = sender.selectedSegmentIndex
        
        if tempPlayer3IsHuman == false { // an AI Meyama
            player3SkillLevelOutlet.hidden = false
            tempPlayer3SkillLevel = skillLevelNum
            
        } else { // human
            player3SkillLevelOutlet.hidden = true
        }
    }


    @IBOutlet weak var player4SkillLevelOutlet: UISegmentedControl!
    
    @IBAction func player4SkillLevelAction(sender: AnyObject) {
        let skillLevelNum = sender.selectedSegmentIndex
        
        if tempPlayer4IsHuman == false { // an AI Meyama
            player4SkillLevelOutlet.hidden = false
            tempPlayer4SkillLevel = skillLevelNum
            
        } else { // human
            player4SkillLevelOutlet.hidden = true
        }
    }


    func startNewGame() {
        
        //mmwGameSceneViewController.secondsPerTurn = tempSecondsPerTurn
        
        mmwGameSceneViewController.audioOn = tempAudioOn
        
        mmwGameSceneViewController.minWordSize = tempMinWordSize
        mmwGameSceneViewController.numStarterWords = tempNumStarterWords
        mmwGameSceneViewController.numPlayers = tempNumPlayers
        
        tempPlayer1Name = player1NameTextFieldOutlet.text!
        tempPlayer2Name = player2NameTextFieldOutlet.text!
        tempPlayer3Name = player3NameTextFieldOutlet.text!
        tempPlayer4Name = player4NameTextFieldOutlet.text!
        
        mmwGameSceneViewController.playerArray[0].playerName = tempPlayer1Name
        mmwGameSceneViewController.playerArray[1].playerName = tempPlayer2Name
        mmwGameSceneViewController.playerArray[2].playerName = tempPlayer3Name
        mmwGameSceneViewController.playerArray[3].playerName = tempPlayer4Name
        
        mmwGameSceneViewController.playerArray[0].isHuman = tempPlayer1IsHuman
        mmwGameSceneViewController.playerArray[1].isHuman = tempPlayer2IsHuman
        mmwGameSceneViewController.playerArray[2].isHuman = tempPlayer3IsHuman
        mmwGameSceneViewController.playerArray[3].isHuman = tempPlayer4IsHuman
        
        mmwGameSceneViewController.playerArray[0].playerSkillLevel = tempPlayer1SkillLevel
        mmwGameSceneViewController.playerArray[1].playerSkillLevel = tempPlayer2SkillLevel
        mmwGameSceneViewController.playerArray[2].playerSkillLevel = tempPlayer3SkillLevel
        mmwGameSceneViewController.playerArray[3].playerSkillLevel = tempPlayer4SkillLevel
        
        mmwGameSceneViewController.playerArray[0].playerAvatarNumber = tempPlayer1AvatarNumber
        mmwGameSceneViewController.playerArray[1].playerAvatarNumber = tempPlayer2AvatarNumber
        mmwGameSceneViewController.playerArray[2].playerAvatarNumber = tempPlayer3AvatarNumber
        mmwGameSceneViewController.playerArray[3].playerAvatarNumber = tempPlayer4AvatarNumber
        
        mmwGameSceneViewController.playerArray[0].playerMeyamaNumber = tempPlayer1MeyamaNumber
        mmwGameSceneViewController.playerArray[1].playerMeyamaNumber = tempPlayer2MeyamaNumber
        mmwGameSceneViewController.playerArray[2].playerMeyamaNumber = tempPlayer3MeyamaNumber
        mmwGameSceneViewController.playerArray[3].playerMeyamaNumber = tempPlayer4MeyamaNumber
        
        mmwOptionScreen.newGameScene()

    }

    @IBAction func startNewGameButton(sender: AnyObject) {
        // update game settings to those in UI
        startNewGame()
    }
    
    
    @IBOutlet weak var returnToGameButtonOutlet: UIButton!

    @IBAction func returnToGameButton(sender: AnyObject) {
        // returns to game without any settings changes
        mmwOptionScreen.returnToGameScene()
    }

    
    let button   = UIButton(type: UIButtonType.System) as UIButton

    override func viewDidLoad() {
        super.viewDidLoad()
        print("in GameViewController viewDidLoad")
        screenSize = view.bounds.size
        let mainMenuScene = MainMenuScene(size: screenSize!)
        
        switch Int(screenSize!.width) {
            case 1024: // [1024x768]
                mmwGame.setDeviceType(MMWGame.DeviceType.iPad)
            case 2048: //  [2048 x 1536]
                mmwGame.setDeviceType(MMWGame.DeviceType.iPadRetina)
            case 1366: // [2732 x 2048] --> Width 1366!
                mmwGame.setDeviceType(MMWGame.DeviceType.iPadPro)
            case 568: // [568.0, 320.0]
                mmwGame.setDeviceType(MMWGame.DeviceType.iPhone5)
            case 2732: // [2732 x 2048]
                mmwGame.setDeviceType(MMWGame.DeviceType.iPhone6)
            case 736: // [2732 x 2048]
               mmwGame.setDeviceType(MMWGame.DeviceType.iPhone6Plus)
            default:
                mmwGame.setDeviceType(MMWGame.DeviceType.iPad)
            print("Screen width:\(screenSize!.width) , device type: \(mmwGame.deviceType) ")
        }

        let skView = self.view as! SKView
        skView.showsFPS = false
        skView.showsNodeCount = false
        
        /* Sprite Kit applies additional optimizations to improve rendering performance */
        skView.ignoresSiblingOrder = true

        /* Set the scale mode to scale to fit the window */
        mainMenuScene.scaleMode = .AspectFill
        
/////////////////
//        NSNotificationCenter.defaultCenter().addObserver(self, selector: "presentView", name: "showController", object: nil)
/////////////////////////////////
        
        //scene.parentController = self
        initializeTextFields()
        

        skView.presentScene(mainMenuScene)


        //var cornerView = UIView()
        
        //self.view.insertSubview(cornerView, atIndex: 2)
        
//        button.frame = CGRectMake(100, 100, 100, 50)
//        button.backgroundColor = UIColor.greenColor()
//        button.setTitle("Test Button", forState: UIControlState.Normal)
//        button.addTarget(self, action: "buttonAction:", forControlEvents: UIControlEvents.TouchUpInside)
        
        //cornerView.addSubview(button)

        gameViewController = self as GameViewController
        //gameViewController.buttonAction()
        
        //self.view.addSubview(cornerView)
    }


//    func buttonAction(sender:UIButton!)
//    {
//        print("Manual Button tapped")
//        
//    }
    
    func buttonAction()
    {
        print("TEST Manual Button tapped")
    }

    // MARK: END GAME SCREEN

    @IBAction func EndGameYesButton(sender: AnyObject) {
        // returns to game without any settings changes
        
        print("MMWOptionScreen didMoveToView")
        gameViewController.ViewAllOptionsUI.userInteractionEnabled = true
        gameViewController.ViewAllOptionsUI.hidden = false
        gameViewController.OptionsSwitchOutlet.hidden = true
        gameViewController.ViewOptionsUI.hidden = true
        gameViewController.viewRulesContainer.hidden = true
        gameViewController.viewStatsContainer.hidden = true
        gameViewController.ViewEndGameUI.hidden = true
        gameViewController.ViewResultsScreenUI.hidden = false
        goToResults()
        //startNewGame()
    }
    
    func goToResults() {
        var otherHighestScore = -10000
        for playerNum in 1..<mmwGameSceneViewController.playerArray.count {
            if mmwGameSceneViewController.playerArray[playerNum].playerScore > otherHighestScore {
                otherHighestScore = mmwGameSceneViewController.playerArray[playerNum].playerScore
            }
        }

        if mmwGameSceneViewController.playerArray[0].playerScore > otherHighestScore {
            gameResultsLabel.text = "Congratulations! You Won!"
            mmwGameSceneViewController.numGamesWon++
        }
        else if mmwGameSceneViewController.playerArray[0].playerScore == otherHighestScore {
            gameResultsLabel.text = "Not bad. You Tied."
            mmwGameSceneViewController.numGamesTied++
        }
        else if mmwGameSceneViewController.playerArray[0].playerScore < otherHighestScore {
            gameResultsLabel.text = "You Lost."
            mmwGameSceneViewController.numGamesLost++
        }
    }


    @IBAction func EndGameNoButton(sender: AnyObject) {
        // returns to game without any settings changes
        mmwGameSceneViewController.consecutivePasses = mmwGameSceneViewController.numPlayers - 2
        mmwOptionScreen.returnToGameScene()
    }

    
    // MARK: RESULTS SCREEN
    
    
    @IBOutlet weak var gameResultsLabel: UILabel!
    
    
    @IBAction func gameResultsNewGameButton(sender: AnyObject) {
        
        //mmwGameSceneViewController.secondsPerTurn = tempSecondsPerTurn
        
        mmwGameSceneViewController.audioOn = tempAudioOn
        
        mmwGameSceneViewController.minWordSize = tempMinWordSize
        mmwGameSceneViewController.numStarterWords = tempNumStarterWords
        mmwGameSceneViewController.numPlayers = tempNumPlayers
        
        mmwGameSceneViewController.playerArray[0].playerName = player1NameTextFieldOutlet.text!
        mmwGameSceneViewController.playerArray[1].playerName = player2NameTextFieldOutlet.text!
        mmwGameSceneViewController.playerArray[2].playerName = player3NameTextFieldOutlet.text!
        mmwGameSceneViewController.playerArray[3].playerName = player4NameTextFieldOutlet.text!

        mmwGameSceneViewController.playerArray[0].playerName = tempPlayer1Name
        mmwGameSceneViewController.playerArray[1].playerName = tempPlayer2Name
        mmwGameSceneViewController.playerArray[2].playerName = tempPlayer3Name
        mmwGameSceneViewController.playerArray[3].playerName = tempPlayer4Name
        
//        mmwGameSceneViewController.playerArray[0].playerName = player1NameTextFieldOutlet.text!
//        mmwGameSceneViewController.playerArray[1].playerName = player2NameTextFieldOutlet.text!
//        mmwGameSceneViewController.playerArray[2].playerName = player3NameTextFieldOutlet.text!
//        mmwGameSceneViewController.playerArray[3].playerName = player4NameTextFieldOutlet.text!
        
        mmwGameSceneViewController.playerArray[0].isHuman = tempPlayer1IsHuman
        mmwGameSceneViewController.playerArray[1].isHuman = tempPlayer2IsHuman
        mmwGameSceneViewController.playerArray[2].isHuman = tempPlayer3IsHuman
        mmwGameSceneViewController.playerArray[3].isHuman = tempPlayer4IsHuman
        
        mmwGameSceneViewController.playerArray[0].playerSkillLevel = tempPlayer1SkillLevel
        mmwGameSceneViewController.playerArray[1].playerSkillLevel = tempPlayer2SkillLevel
        mmwGameSceneViewController.playerArray[2].playerSkillLevel = tempPlayer3SkillLevel
        mmwGameSceneViewController.playerArray[3].playerSkillLevel = tempPlayer4SkillLevel
        
        mmwGameSceneViewController.playerArray[0].playerAvatarNumber = tempPlayer1AvatarNumber
        mmwGameSceneViewController.playerArray[1].playerAvatarNumber = tempPlayer2AvatarNumber
        mmwGameSceneViewController.playerArray[2].playerAvatarNumber = tempPlayer3AvatarNumber
        mmwGameSceneViewController.playerArray[3].playerAvatarNumber = tempPlayer4AvatarNumber
        
        mmwGameSceneViewController.playerArray[0].playerMeyamaNumber = tempPlayer1MeyamaNumber
        mmwGameSceneViewController.playerArray[1].playerMeyamaNumber = tempPlayer2MeyamaNumber
        mmwGameSceneViewController.playerArray[2].playerMeyamaNumber = tempPlayer3MeyamaNumber
        mmwGameSceneViewController.playerArray[3].playerMeyamaNumber = tempPlayer4MeyamaNumber
        
        mmwOptionScreen.newGameScene()
        //startNewGame()
    }
    
    @IBAction func gameResultsChangeOptionsButton(sender: AnyObject) {
        print("options button pressed")
        gameViewController.ViewAllOptionsUI.userInteractionEnabled = true
        gameViewController.ViewAllOptionsUI.hidden = false
        gameViewController.OptionsSwitchOutlet.hidden = false
        gameViewController.ViewOptionsUI.hidden = false
        gameViewController.returnToGameButtonOutlet.alpha = 0.25
        gameViewController.returnToGameButtonOutlet.userInteractionEnabled = false
        gameViewController.viewRulesContainer.hidden = true
        gameViewController.viewStatsContainer.hidden = true
        gameViewController.ViewEndGameUI.hidden = true
        gameViewController.ViewResultsScreenUI.hidden = true

        //view?.presentScene(mmwOptionScreen)

        print("back to  mmwGameScene from optionsButton")
        
    }

    
    // MARK: STATS SCREEN
    

    @IBOutlet weak var gamesWonOutlet: UILabel!
    
    @IBOutlet weak var gamesLostOutlet: UILabel!
    
    @IBOutlet weak var wordsPlayedOutlet: UILabel!

    @IBOutlet weak var lettersPlayedOutlet: UILabel!
    
    override func shouldAutorotate() -> Bool {
        return true
    }
    
    
    override func supportedInterfaceOrientations() -> UIInterfaceOrientationMask {
        if UIDevice.currentDevice().userInterfaceIdiom == .Phone {
            return UIInterfaceOrientationMask.AllButUpsideDown
        } else {
            return UIInterfaceOrientationMask.All
        }
    }

    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Release any cached data, images, etc that aren't in use.
    }

    
    override func prefersStatusBarHidden() -> Bool {
        return true
    }
    
    // Tap outside a text field to dismiss the keyboard
    // ------------------------------------------------
    // By changing the underlying class of the view from UIView to UIControl,
    // the view can respond to events, including Touch Down, which is
    // wired to this method.
    @IBAction func userTappedBackground(sender: AnyObject) {
        view.endEditing(true)
    }
    
    @IBOutlet weak var player1NameTextFieldOutlet: UITextField!
    
    @IBOutlet weak var player2NameTextFieldOutlet: UITextField!
    
    @IBOutlet weak var player3NameTextFieldOutlet: UITextField!
    
    @IBOutlet weak var player4NameTextFieldOutlet: UITextField!

    
//    @IBAction func player1TextFieldNameAction(sender: AnyObject) {
////        textField(Pl, shouldChangeCharactersInRange: <#T##NSRange#>, replacementString: <#T##String#>)
//    }
    
    func initializeTextFields() {
        player1NameTextFieldOutlet.delegate = self
        player2NameTextFieldOutlet.delegate = self
        player3NameTextFieldOutlet.delegate = self
        player4NameTextFieldOutlet.delegate = self
        
//        player1NameTextFieldOutlet.keyboardType = UIKeyboardType.Alphabet
//        player1NameTextFieldOutlet.keyboardType = UIKeyboardType.Alphabet
//        player1NameTextFieldOutlet.keyboardType = UIKeyboardType.Alphabet
//        player1NameTextFieldOutlet.keyboardType = UIKeyboardType.Alphabet
        
//        noVowelsTextField.delegate = self
//        noVowelsTextField.keyboardType = UIKeyboardType.ASCIICapable
//        
//        digitsOnlyTextField.delegate = self
//        digitsOnlyTextField.keyboardType = UIKeyboardType.NumberPad
//        
//        numericOnlyTextField.delegate = self
//        numericOnlyTextField.keyboardType = UIKeyboardType.NumbersAndPunctuation
//        
//        positiveIntegersOnlyTextField.delegate = self
//        positiveIntegersOnlyTextField.keyboardType = UIKeyboardType.DecimalPad
    }
    
    
    func textField(textField: UITextField,
        shouldChangeCharactersInRange range: NSRange,
        replacementString string: String)
        -> Bool
    {
        // We ignore any change that doesn't add characters to the text field.
        // These changes are things like character deletions and cuts, as well
        // as moving the insertion point.
        //
        // We still return true to allow the change to take place.
        if string.characters.count == 0 {
            return true
        }
        
        // Check to see if the text field's contents still fit the constraints
        // with the new content added to it.
        // If the contents still fit the constraints, allow the change
        // by returning true; otherwise disallow the change by returning false.
        let currentText = textField.text ?? ""
        let prospectiveText = (currentText as NSString).stringByReplacingCharactersInRange(range, withString: string)
        
        print("player name = \(prospectiveText)")
        
        return

            prospectiveText.characters.count <= 12
        
        switch textField {
            
            // Allow only upper- and lower-case vowels in this field,
            // and limit its contents to a maximum of 6 characters.
        case  player1NameTextFieldOutlet:
            return
                prospectiveText.characters.count <= 6
            
//            // Allow any characters EXCEPT upper- and lower-case vowels in this field,
//            // and limit its contents to a maximum of 8 characters.
//        case noVowelsTextField:
//            return prospectiveText.doesNotContainCharactersIn("aeiouAEIOU") &&
//                prospectiveText.characters.count <= 8
//            
//            // Allow only digits in this field,
//            // and limit its contents to a maximum of 3 characters.
//        case digitsOnlyTextField:
//            return prospectiveText.containsOnlyCharactersIn("0123456789") &&
//                prospectiveText.characters.count <= 3
//            
//            // Allow only values that evaluate to proper numeric values in this field,
//            // and limit its contents to a maximum of 7 characters.
//        case numericOnlyTextField:
//            return prospectiveText.isNumeric() &&
//                prospectiveText.characters.count <= 7
//            
//            // In this field, allow only values that evalulate to proper numeric values and
//            // do not contain the "-" and "e" characters, nor the decimal separator character
//            // for the current locale. Limit its contents to a maximum of 5 characters.
//        case positiveIntegersOnlyTextField:
//            let decimalSeparator = NSLocale.currentLocale().objectForKey(NSLocaleDecimalSeparator) as! String
//            return prospectiveText.isNumeric() &&
//                prospectiveText.doesNotContainCharactersIn("-e" + decimalSeparator) &&
//                prospectiveText.characters.count <= 5
            
            // Do not put constraints on any other text field in this view
            // that uses this class as its delegate.
        default:
            return true
        }
        
    }

}
