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

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {

    var window: UIWindow?
    
    func demo () {
        
        let request = NSFetchRequest(entityName: "Item")
        do {
            try CDHelper.shared.context.executeFetchRequest(request)
        }
        catch {
            print("!!! Error in AppDeligate Demo() CDHelper.shared.context.executeFetchRequest(request)")
        }
        
        let itemNames = ["Apples", "Milk", "Bread", "Cheese", "Sausages", "Butter", "Orange Juice", "Cereal", "Coffee", "Eggs", "Tomatoes", "Fish", "Fish"]
        
        //let mmwTile = MMWTile()
        
        for itemName in itemNames {
            let item:Item = NSEntityDescription.insertNewObjectForEntityForName("Item", inManagedObjectContext: CDHelper.shared.context) as! Item
            item.name = itemName
            print("Inserted New Managed Object for '\(item.name)'")
        }
        
//        let item:Item = NSEntityDescription.insertNewObjectForEntityForName("Item", inManagedObjectContext: CDHelper.shared.context) as! Item
//        item.name = itemName
//        print("Inserted New Managed Object for '\(item.name)'")

        CDHelper.saveSharedContext()

    }
    
    func fetch () {
        let model = CDHelper.shared.model
        if let template = model.fetchRequestTemplateForName("Test"),
            let request  = template.copy() as? NSFetchRequest {
                
                let sort = NSSortDescriptor(key: "name", ascending: true)
                request.sortDescriptors = [sort]
                
                do {
                    if let items = try CDHelper.shared.context.executeFetchRequest(request) as? [Item] {
                        
                        for item in items {
                            if let name = item.name {
                                print("Fetched Managed Object = '\(name)'")
                            }
                        }
                    }
                } catch {print("ERROR executing a fetch request: \(error)")}
        } else {print("FAILED to prepare template")}
    }
    

    func application(application: UIApplication, didFinishLaunchingWithOptions launchOptions: [NSObject: AnyObject]?) -> Bool {
        // Override point for customization after application launch.
        print("AppDelegate > application ...")
        
        //demo()
        CDHelper.shared
        fetch()

        return true
    }

    func applicationWillResignActive(application: UIApplication) {
        // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
        // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
        print("AppDelegate > applicationWillResignActive ...")
        if mmwGameSceneViewController.timerIsOn {
            mmwGameScene.stopTimer()
        }
    }

    func applicationDidEnterBackground(application: UIApplication) {
        // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
        // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
        
        CDHelper.saveSharedContext()
        if mmwGameSceneViewController.timerIsOn {
            mmwGameScene.stopTimer()
        }

        print("AppDelegate > application ...")
    }

    func applicationWillEnterForeground(application: UIApplication) {
        // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
        print("AppDelegate > applicationWillEnterForeground ...")
        if mmwGameSceneViewController.timerIsOn {
            mmwGameScene.startTimer(mmwGameScene.secondsLeft)
        }
    }

    func applicationDidBecomeActive(application: UIApplication) {
        // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
        
        //demo()

        print("AppDelegate > applicationDidBecomeActive ...")
        if mmwGameSceneViewController.timerIsOn {
            mmwGameScene.startTimer(mmwGameScene.secondsLeft)
        }
    }

    func applicationWillTerminate(application: UIApplication) {
        // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
        
        CDHelper.saveSharedContext()

        print("AppDelegate > applicationWillTerminate...")
    }
    

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
        let modelURL = NSBundle.mainBundle().URLForResource("coreDataTests", withExtension: "momd")!
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

