//
//  AppDelegate.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/10/15.
//  Copyright (c) 2015 Hook Studios. All rights reserved.
//

import UIKit
import CoreMotion
import SpriteKit
import CoreData
import StoreKit

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {

    var window: UIWindow?
    
    var canPurchase:Bool = false
    
    func saveSettings () {
        
        let context = CDHelper.shared.context
        
        let request = NSFetchRequest(entityName: "Stats")
        do {
            try CDHelper.shared.context.executeFetchRequest(request)
        }
        catch {
            print("!!! Error in AppDeligate Demo() CDHelper.shared.context.executeFetchRequest(request)")
        }
        
//        let itemNames = ["Apples", "Milk", "Bread", "Cheese", "Sausages", "Butter", "Orange Juice", "Cereal", "Coffee", "Eggs", "Tomatoes", "Fish", "Fish"]
        
        
        
        //let mmwTile = MMWTile()
        
//        for itemName in itemNames {
//            let item:Item = NSEntityDescription.insertNewObjectForEntityForName("Item", inManagedObjectContext: CDHelper.shared.context) as! Item
//            item.name = itemName
//            print("Inserted New Managed Object for '\(item.name)'")
//        }
        
        var numLettersPlayed = mmwGameSceneViewController.numLettersPlayed
        var numWordsPlayed = mmwGameSceneViewController.numWordsMade
        var gamesWon = mmwGameSceneViewController.numGamesWon
        var gamesLost = mmwGameSceneViewController.numGamesLost
        
        do {
            if let stats = try CDHelper.shared.context.executeFetchRequest(request) as? [Stats] {
                for stat in stats {
                    if let player = stat.player {
                        print ("Deleting stats \(player)")
                        context.deleteObject(stat)
                    }
                }
            }
        } catch {
            print("Error deleting request")
        }
        
        
        
//        CDHelper.shared.context.reset()
//        NSEntityDescription.
        
        
    
        
        
        let stats : Stats = NSEntityDescription.insertNewObjectForEntityForName("Stats", inManagedObjectContext: CDHelper.shared.context) as! Stats
 
        
        stats.numLettersPlayed = numLettersPlayed
        print("Inserted New Managed Object for '\(stats.numWordsPlayed)'")
        stats.numWordsPlayed = numWordsPlayed
        print("Inserted New Managed Object for '\(stats.numWordsPlayed)'")
        stats.gamesWon = gamesWon
        print("Inserted New Managed Object for '\(stats.gamesWon)'")
        stats.gamesLost = gamesLost
        print("Inserted New Managed Object for '\(stats.gamesLost)'")
        stats.player = "playerTest2"
        print("Inserted New Managed Object for '\(stats.player)'")
        stats.purchasedDeluxe = deluxeVersionPurchased
        print("Inserted New Managed Object for '\(stats.purchasedDeluxe)'")
        
        //let item:Item = NSEntityDescription.
        
        //        let item:Item = NSEntityDescription.insertNewObjectForEntityForName("Item", inManagedObjectContext: CDHelper.shared.context) as! Item
        //        item.name = itemName
        //        print("Inserted New Managed Object for '\(item.name)'")
        
        CDHelper.saveSharedContext()
        
    }
    
    static func fetch () {
        
        
        let request = NSFetchRequest(entityName: "Stats")
        do {
            if let stats = try CDHelper.shared.context.executeFetchRequest(request) as? [Stats] {

                for stat in stats {
 
                    if let player = stat.player {
                        print("Fetched Manged Object = '\(player)'")
                        mmwGameSceneViewController.numLettersPlayed = Int(stat.numLettersPlayed!)
                        print("Inserted New Managed Object for \(stat.numWordsPlayed!)")
                        mmwGameSceneViewController.numWordsMade = Int(stat.numWordsPlayed!)
                        print("Inserted New Managed Object for \(stat.numWordsPlayed!)")
                        mmwGameSceneViewController.numGamesWon = Int(stat.gamesWon!)
                        print("Inserted New Managed Object for \(stat.gamesWon!)")
                        mmwGameSceneViewController.numGamesLost = Int(stat.gamesLost!)
                        print("Inserted New Managed Object for \(stat.gamesLost!)")
                        mmwGameSceneViewController.player1.playerName = (stat.player!)
                        print("Inserted New Managed Object for \( stat.player )")
                        if (Int(stat.purchasedDeluxe!)) > 0 {
                            deluxeVersionPurchased = 1
                        }
                        else {
                            deluxeVersionPurchased = 0
                        }
                        print("Inserted New Managed Object for \( stat.purchasedDeluxe )")
                    }
                }
            }  
        }
        catch {
            print("!!! Error in AppDeligate Demo() CDHelper.shared.context.executeFetchRequest(request)")
        }
        
//        let stats : Stats = NSEntityDescription.insertNewObjectForEntityForName("Stats", inManagedObjectContext: CDHelper.shared.context) as! Stats

        
        
//        let model = CDHelper.shared.model
//        if let template = model.fetchRequestTemplateForName("Stats"),
//            let request  = template.copy() as? NSFetchRequest {
//            
//            let sort = NSSortDescriptor(key: "player1", ascending: true)
//            request.sortDescriptors = [sort]
//            
//            do {
//                if let stats = try CDHelper.shared.context.executeFetchRequest(request) as? [Stats] {
//                    
//                    for stat in stats {
//                        if let gamesWon = stat.gamesWon {
//                            print("Fetched Managed Object = '\(gamesWon)'")
//                        }
//                    }
//                }
//            } catch {print("ERROR executing a fetch request: \(error)")}
//        } else {print("FAILED to prepare template")}
        
        
    }
    
//    func getStats () {
//        let request = NSFetchRequest(entityName: "Player1")
//        
//        do {
//            if let stats = try CDHelper.shared.context.executeRequest(request) as? [Stats] {
//                for stat in stats {
//                    if let stat = stat.player1 {
//                        print ("Fetched Managed Object =  '\(stat)' ")
//                    }
//                }
//                
//            } catch {
//                print("error on data fetch")
//            }
//        }
//    }


    func application(application: UIApplication, didFinishLaunchingWithOptions launchOptions: [NSObject: AnyObject]?) -> Bool {
        // Override point for customization after application launch.
        if debugMode == true { print("AppDelegate > application ...") }

        
        mmwGameSceneViewController.appPaused = false
        gameIsSuspended = false
        
        
        if SKPaymentQueue.canMakePayments(){
            self.canPurchase = true
            IAPManager.sharedInstance.setupInAppPurchases()
        }

        return true
    }

    
    func applicationWillResignActive(application: UIApplication) {
        // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
        // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
        if debugMode == true { print("AppDelegate > applicationWillResignActive ...") }
        if mmwGameSceneViewController.timerIsOn {
            mmwGameScene.stopTimer()
        }
        mmwGameSceneViewController.appPaused = true
        gameIsSuspended = true
        
        saveSettings()
        //CDHelper.shared
        //fetch()
        
    }

    
    func applicationDidEnterBackground(application: UIApplication) {
        // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
        // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
        
//        CDHelper.saveSharedContext()
        if mmwGameSceneViewController.timerIsOn {
            //mmwGameScene.stopTimer()
        }
        
        mmwGameSceneViewController.appPaused = true
        gameIsSuspended = true

        if debugMode == true { print("AppDelegate > application ...") }
    }

    
    func applicationWillEnterForeground(application: UIApplication) {
        // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
        if debugMode == true { print("AppDelegate > applicationWillEnterForeground ...") }
        if mmwGameSceneViewController.timerIsOn {
            mmwGameScene.startTimer(mmwGameScene.secondsLeft)
        }
        mmwGameSceneViewController.appPaused = false
        gameIsSuspended = false
    }

    
    func applicationDidBecomeActive(application: UIApplication) {
        // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
        
        //demo()

        if debugMode == true { print("AppDelegate > applicationDidBecomeActive ...") }
        //if mmwGameSceneViewController.timerIsOn {
            //mmwGameScene.startTimer(mmwGameScene.secondsLeft)
        //}
        mmwGameSceneViewController.appPaused = false
        gameIsSuspended = false
        
//        if deluxeVersionPurchased != 0 {
//            gameViewController.updateUIDeluxeVersion()
//        }
    }

    
    func applicationWillTerminate(application: UIApplication) {
        // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
        
        CDHelper.saveSharedContext()
        
        // Saves changes in the application's managed object context before the application terminates.
        //self.saveContext()
        
        
        saveSettings()
        //CDHelper.shared
        //fetch()
        
        
        mmwGameSceneViewController.appPaused = true
        gameIsSuspended = true
        if debugMode == true { print("AppDelegate > applicationWillTerminate...") }
    }
    
    
    
//    //////////////////////////////////
//    
//
//    // DEFAULT core data
//    
//    
//    // MARK: - Core Data stack
//    
//    lazy var applicationDocumentsDirectory: NSURL = {
//        // The directory the application uses to store the Core Data store file. This code uses a directory named "com.hookstudios.coreDataTest" in the application's documents Application Support directory.
//        let urls = NSFileManager.defaultManager().URLsForDirectory(.DocumentDirectory, inDomains: .UserDomainMask)
//        return urls[urls.count-1]
//    }()
//    
//    lazy var managedObjectModel: NSManagedObjectModel = {
//        // The managed object model for the application. This property is not optional. It is a fatal error for the application not to be able to find and load its model.
//        let modelURL = NSBundle.mainBundle().URLForResource("MarkMyWord+Stats", withExtension: "momd")!
//        return NSManagedObjectModel(contentsOfURL: modelURL)!
//    }()
//    
//    lazy var persistentStoreCoordinator: NSPersistentStoreCoordinator = {
//        // The persistent store coordinator for the application. This implementation creates and returns a coordinator, having added the store for the application to it. This property is optional since there are legitimate error conditions that could cause the creation of the store to fail.
//        // Create the coordinator and store
//        let coordinator = NSPersistentStoreCoordinator(managedObjectModel: self.managedObjectModel)
//        let url = self.applicationDocumentsDirectory.URLByAppendingPathComponent("SingleViewCoreData.sqlite")
//        var failureReason = "There was an error creating or loading the application's saved data."
//        do {
//            try coordinator.addPersistentStoreWithType(NSSQLiteStoreType, configuration: nil, URL: url, options: nil)
//        } catch {
//            // Report any error we got.
//            var dict = [String: AnyObject]()
//            dict[NSLocalizedDescriptionKey] = "Failed to initialize the application's saved data"
//            dict[NSLocalizedFailureReasonErrorKey] = failureReason
//            
//            dict[NSUnderlyingErrorKey] = error as NSError
//            let wrappedError = NSError(domain: "YOUR_ERROR_DOMAIN", code: 9999, userInfo: dict)
//            // Replace this with code to handle the error appropriately.
//            // abort() causes the application to generate a crash log and terminate. You should not use this function in a shipping application, although it may be useful during development.
//            NSLog("Unresolved error \(wrappedError), \(wrappedError.userInfo)")
//            abort()
//        }
//        
//        return coordinator
//    }()
//    
//    lazy var managedObjectContext: NSManagedObjectContext = {
//        // Returns the managed object context for the application (which is already bound to the persistent store coordinator for the application.) This property is optional since there are legitimate error conditions that could cause the creation of the context to fail.
//        let coordinator = self.persistentStoreCoordinator
//        var managedObjectContext = NSManagedObjectContext(concurrencyType: .MainQueueConcurrencyType)
//        managedObjectContext.persistentStoreCoordinator = coordinator
//        return managedObjectContext
//    }()
//    
//    // MARK: - Core Data Saving support
//    
//    func saveContext () {
//        if managedObjectContext.hasChanges {
//            do {
//                try managedObjectContext.save()
//            } catch {
//                // Replace this implementation with code to handle the error appropriately.
//                // abort() causes the application to generate a crash log and terminate. You should not use this function in a shipping application, although it may be useful during development.
//                let nserror = error as NSError
//                NSLog("Unresolved error \(nserror), \(nserror.userInfo)")
//                abort()
//            }
//        }
//    }
//
//    
//    var _managedObjectModel: NSManagedObjectModel? = nil
//    var _persistentStoreCoordinator: NSPersistentStoreCoordinator? = nil
//    
//    ////////////////////////////////
    
    
    
    
//    var _managedObjectContext: NSManagedObjectContext? = nil
//    
//    // Returns the managed object model for the application.
//    // If the model doesn't already exist, it is created from the application's model.
//    var managedObjectModel: NSManagedObjectModel {
//        if (_managedObjectModel == nil) {
//            let modelURL = NSBundle.mainBundle().URLForResource("iOSSwiftOpenGLCamera", withExtension: "momd")
//            _managedObjectModel = NSManagedObjectModel(contentsOfURL: modelURL!)
//        }
//        return _managedObjectModel!
//    }
//    var _managedObjectModel: NSManagedObjectModel? = nil
//    
//        lazy var persistentStoreCoordinator: NSPersistentStoreCoordinator = {
//            // The persistent store coordinator for the application. This implementation creates and returns a coordinator, having added the store for the application to it. This property is optional since there are legitimate error conditions that could cause the creation of the store to fail.
//            // Create the coordinator and store
//            let coordinator = NSPersistentStoreCoordinator(managedObjectModel: self.managedObjectModel)
//            let url = self.applicationDocumentsDirectory.URLByAppendingPathComponent("SingleViewCoreData.sqlite")
//            var failureReason = "There was an error creating or loading the application's saved data."
//            do {
//                try coordinator.addPersistentStoreWithType(NSSQLiteStoreType, configuration: nil, URL: url, options: nil)
//            } catch {
//                // Report any error we got.
//                var dict = [String: AnyObject]()
//                dict[NSLocalizedDescriptionKey] = "Failed to initialize the application's saved data"
//                dict[NSLocalizedFailureReasonErrorKey] = failureReason
//    
//                dict[NSUnderlyingErrorKey] = error as NSError
//                let wrappedError = NSError(domain: "YOUR_ERROR_DOMAIN", code: 9999, userInfo: dict)
//                // Replace this with code to handle the error appropriately.
//                // abort() causes the application to generate a crash log and terminate. You should not use this function in a shipping application, although it may be useful during development.
//                NSLog("Unresolved error \(wrappedError), \(wrappedError.userInfo)")
//                abort()
//            }
//            
//            return coordinator
//        }()
//    
//    var _persistentStoreCoordinator: NSPersistentStoreCoordinator? = nil
//    
//    // #pragma mark - Application's Documents directory
//    
//    // Returns the URL to the application's Documents directory.
//    var applicationDocumentsDirectory: NSURL {
//        let urls = NSFileManager.defaultManager().URLsForDirectory(.DocumentDirectory, inDomains: .UserDomainMask)
//        return urls[urls.endIndex-1] as NSURL
//    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
//    func saveContext () {
//        var error: NSError? = nil
//        let managedObjectContext = self.managedObjectContext
//        if managedObjectContext != nil {
//            if managedObjectContext.hasChanges && !managedObjectContext.save(&error) {
//                // Replace this implementation with code to handle the error appropriately.
//                // abort() causes the application to generate a crash log and terminate. You should not use this function in a shipping application, although it may be useful during development.
//                //println("Unresolved error \(error), \(error.userInfo)")
//                abort()
//            }
//        }
//    }
//    
//    // #pragma mark - Core Data stack
//    
//    // Returns the managed object context for the application.
//    // If the context doesn't already exist, it is created and bound to the persistent store coordinator for the application.
//    var managedObjectContext: NSManagedObjectContext {
//        if !_managedObjectContext {
//            let coordinator = self.persistentStoreCoordinator
//            if coordinator != nil {
//                _managedObjectContext = NSManagedObjectContext()
//                _managedObjectContext!.persistentStoreCoordinator = coordinator
//            }
//        }
//        return _managedObjectContext!
//    }
//    
//    var _managedObjectContext: NSManagedObjectContext? = nil
//    
//    // Returns the managed object model for the application.
//    // If the model doesn't already exist, it is created from the application's model.
//    var managedObjectModel: NSManagedObjectModel {
//        if !_managedObjectModel {
//            let modelURL = NSBundle.mainBundle().URLForResource("iOSSwiftOpenGLCamera", withExtension: "momd")
//            _managedObjectModel = NSManagedObjectModel(contentsOfURL: modelURL)
//        }
//        return _managedObjectModel!
//    }
//    var _managedObjectModel: NSManagedObjectModel? = nil
//    
//    // Returns the persistent store coordinator for the application.
//    // If the coordinator doesn't already exist, it is created and the application's store added to it.
//    var persistentStoreCoordinator: NSPersistentStoreCoordinator {
//        if !_persistentStoreCoordinator {
//            let storeURL = self.applicationDocumentsDirectory.URLByAppendingPathComponent("iOSSwiftOpenGLCamera.sqlite")
//            var error: NSError? = nil
//            _persistentStoreCoordinator = NSPersistentStoreCoordinator(managedObjectModel: self.managedObjectModel)
//            if _persistentStoreCoordinator!.addPersistentStoreWithType(NSSQLiteStoreType, configuration: nil, URL: storeURL, options: nil, error: &error) == nil {
//                /*
//                 Replace this implementation with code to handle the error appropriately.
//                 abort() causes the application to generate a crash log and terminate. You should not use this function in a shipping application, although it may be useful during development.
//                 Typical reasons for an error here include:
//                 * The persistent store is not accessible;
//                 * The schema for the persistent store is incompatible with current managed object model.
//                 Check the error message to determine what the actual problem was.
//                 If the persistent store is not accessible, there is typically something wrong with the file path. Often, a file URL is pointing into the application's resources directory instead of a writeable directory.
//                 If you encounter schema incompatibility errors during development, you can reduce their frequency by:
//                 * Simply deleting the existing store:
//                 NSFileManager.defaultManager().removeItemAtURL(storeURL, error: nil)
//                 * Performing automatic lightweight migration by passing the following dictionary as the options parameter:
//                 [NSMigratePersistentStoresAutomaticallyOption: true, NSInferMappingModelAutomaticallyOption: true}
//                 Lightweight migration will only work for a limited set of schema changes; consult "Core Data Model Versioning and Data Migration Programming Guide" for details.
//                 */
//                //println("Unresolved error \(error), \(error.userInfo)")
//                abort()
//            }
//        }
//        return _persistentStoreCoordinator!
//    }
//    var _persistentStoreCoordinator: NSPersistentStoreCoordinator? = nil
//    
//    // #pragma mark - Application's Documents directory
//    
//    // Returns the URL to the application's Documents directory.
//    var applicationDocumentsDirectory: NSURL {
//        let urls = NSFileManager.defaultManager().URLsForDirectory(.DocumentDirectory, inDomains: .UserDomainMask)
//        return urls[urls.endIndex-1] as NSURL
//    }
    
    
    
    
    
    
    
    

//    func playMenuScene() {
//        let transition = SKTransition.crossFadeWithDuration(2.0)
//        let menuScene = MenuScene(size: size,
//            gameResult: true,
//            score: 123)
//        view?.presentScene(menuScene, transition: transition)
//    }
    
    
    // MARK: - Core Data stack
    
    lazy var applicationDocumentsDirectory: NSURL = {
        // The directory the application uses to store the Core Data store file. This code uses a directory named "com.hookstudios.coreDataTests" in the application's documents Application Support directory.
        let urls = NSFileManager.defaultManager().URLsForDirectory(.DocumentDirectory, inDomains: .UserDomainMask)
        return urls[urls.count-1]
    }()
    
    lazy var managedObjectModel: NSManagedObjectModel = {
        // The managed object model for the application. This property is not optional. It is a fatal error for the application not to be able to find and load its model.
        let modelURL = NSBundle.mainBundle().URLForResource("Stats", withExtension: "momd")!
        return NSManagedObjectModel(contentsOfURL: modelURL)!
    }()
    
    lazy var persistentStoreCoordinator: NSPersistentStoreCoordinator = {
        // The persistent store coordinator for the application. This implementation creates and returns a coordinator, having added the store for the application to it. This property is optional since there are legitimate error conditions that could cause the creation of the store to fail.
        // Create the coordinator and store
        let coordinator = NSPersistentStoreCoordinator(managedObjectModel: self.managedObjectModel)
        let url = self.applicationDocumentsDirectory.URLByAppendingPathComponent("SingleViewCoreData.sqlite")
        var failureReason = "There was an error creating or loading the application's saved data."
        do {
            try coordinator.addPersistentStoreWithType(NSSQLiteStoreType, configuration: nil, URL: url, options: nil)
        } catch {
            // Report any error we got.
            var dict = [String: AnyObject]()
            dict[NSLocalizedDescriptionKey] = "Failed to initialize the application's saved data"
            dict[NSLocalizedFailureReasonErrorKey] = failureReason
            
            dict[NSUnderlyingErrorKey] = error as NSError
            let wrappedError = NSError(domain: "YOUR_ERROR_DOMAIN", code: 9999, userInfo: dict)
            // Replace this with code to handle the error appropriately.
            // abort() causes the application to generate a crash log and terminate. You should not use this function in a shipping application, although it may be useful during development.
            NSLog("Unresolved error \(wrappedError), \(wrappedError.userInfo)")
            abort()
        }
        
        return coordinator
    }()
    
    lazy var managedObjectContext: NSManagedObjectContext = {
        // Returns the managed object context for the application (which is already bound to the persistent store coordinator for the application.) This property is optional since there are legitimate error conditions that could cause the creation of the context to fail.
        let coordinator = self.persistentStoreCoordinator
        var managedObjectContext = NSManagedObjectContext(concurrencyType: .MainQueueConcurrencyType)
        managedObjectContext.persistentStoreCoordinator = coordinator
        return managedObjectContext
    }()
    
    // MARK: - Core Data Saving support
    
    func saveContext () {
        if managedObjectContext.hasChanges {
            do {
                try managedObjectContext.save()
            } catch {
                // Replace this implementation with code to handle the error appropriately.
                // abort() causes the application to generate a crash log and terminate. You should not use this function in a shipping application, although it may be useful during development.
                let nserror = error as NSError
                NSLog("Unresolved error \(nserror), \(nserror.userInfo)")
                abort()
            }
        }
    }
}

